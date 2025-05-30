#include "gc_avi.h"
#include <windows.h>
#include <mfapi.h>
#include <mfplay.h>
#include <mfreadwrite.h>
#include <mftransform.h>
#include <mfobjects.h>
#include <shlwapi.h>
#include <strmif.h>
#include <initguid.h>

#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "mf.lib")
#pragma comment(lib, "shlwapi.lib")

struct gc_avi {
    IMFSourceReader* reader;
    IMFMediaType* outputType;
};

HRESULT MFGetAttributeSize(
    IMFAttributes *pAttributes,
    REFGUID guidKey,
    UINT32 *punWidth,
    UINT32 *punHeight
) {
    UINT64 unSize = 0;
    HRESULT hr = pAttributes->lpVtbl->GetUINT64(pAttributes, guidKey, &unSize);
    if (SUCCEEDED(hr)) {
        *punWidth  = (UINT32)(unSize >> 32);       // width nei 32 bit alti
        *punHeight = (UINT32)(unSize & 0xFFFFFFFF); // height nei 32 bit bassi
    }
    return hr;
}

static void gc_initialize_media_foundation() {
    static int initialized = 0;
    if (!initialized) {
        if (FAILED(MFStartup(MF_VERSION, MFSTARTUP_LITE))) {
            MessageBox(NULL, "MFStartup failed", "Error", MB_ICONERROR);
        } else {
            //MessageBox(NULL, "MFStartup succeeded", "Info", MB_OK);
            initialized = 1;
        }
    }
}
void DumpMediaTypes(IMFSourceReader* reader) {
    DWORD i = 0;
    HRESULT hr;
    IMFMediaType* pType = NULL;
    CHAR* guidString = NULL;

    while (SUCCEEDED(hr = reader->lpVtbl->GetNativeMediaType(reader, MF_SOURCE_READER_FIRST_VIDEO_STREAM, i, &pType))) {
        hr = (HRESULT )pType->lpVtbl->GetGUID(pType, &MF_MT_SUBTYPE, &guidString);
        pType->lpVtbl->Release(pType);
        pType = NULL;
        i++;
    }
}

gc_avi* gc_open(const char* path) {
    gc_initialize_media_foundation();

    gc_avi* avi = (gc_avi*)calloc(1, sizeof(gc_avi));
    if (!avi) {
        MessageBoxA(NULL, "Failed to allocate memory for gc_avi.", "Error", MB_ICONERROR);
        return NULL;
    }

    WCHAR wpath[MAX_PATH];
    if (MultiByteToWideChar(CP_UTF8, 0, path, -1, wpath, MAX_PATH) == 0) {
        MessageBoxA(NULL, "Failed to convert path to wide string.", "Error", MB_ICONERROR);
        free(avi);
        return NULL;
    }

    IMFAttributes* pAttributes = NULL;
    HRESULT hr = MFCreateAttributes(&pAttributes, 1);
    if (FAILED(hr)) {
        MessageBoxA(NULL, "Failed to create IMFAttributes.", "Error", MB_ICONERROR);
        free(avi);
        return NULL;
    }

    hr = MFCreateSourceReaderFromURL(wpath, pAttributes, &avi->reader);
    pAttributes->lpVtbl->Release(pAttributes);
    if (FAILED(hr)) {
        MessageBoxA(NULL, "Failed to create source reader from URL.", "Error", MB_ICONERROR);
        free(avi);
        return NULL;
    }

    hr = MFCreateMediaType(&avi->outputType);
    if (FAILED(hr)) {
        MessageBoxA(NULL, "Failed to create media type.", "Error", MB_ICONERROR);
        avi->reader->lpVtbl->Release(avi->reader);
        free(avi);
        return NULL;
    }

    hr = avi->outputType->lpVtbl->SetGUID(avi->outputType, &MF_MT_MAJOR_TYPE, &MFMediaType_Video);
    if (FAILED(hr)) {
        MessageBoxA(NULL, "Failed to set major media type GUID.", "Error", MB_ICONERROR);
        avi->outputType->lpVtbl->Release(avi->outputType);
        avi->reader->lpVtbl->Release(avi->reader);
        free(avi);
        return NULL;
    }

    hr = avi->outputType->lpVtbl->SetGUID(avi->outputType, &MF_MT_SUBTYPE, &MFVideoFormat_RGB32);
    if (FAILED(hr)) {
        MessageBoxA(NULL, "Failed to set subtype GUID to RGB32.", "Error", MB_ICONERROR);
        avi->outputType->lpVtbl->Release(avi->outputType);
        avi->reader->lpVtbl->Release(avi->reader);
        free(avi);
        return NULL;
    }
    hr = avi->reader->lpVtbl->SetCurrentMediaType(
        avi->reader,
        MF_SOURCE_READER_FIRST_VIDEO_STREAM,
        NULL,
        avi->outputType
    );
    DumpMediaTypes(avi->reader);

    return avi;
}

static void print_guid(const GUID* guid) {
    char buf[64];
    wsprintfA(buf, "{%08lX-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
        guid->Data1, guid->Data2, guid->Data3,
        guid->Data4[0], guid->Data4[1], guid->Data4[2], guid->Data4[3],
        guid->Data4[4], guid->Data4[5], guid->Data4[6], guid->Data4[7]);
    //MessageBoxA(NULL, buf, "Video Format GUID", MB_OK);
}


int gc_decode_next_frame(gc_avi* avi, gc_frame_t* out) {
    if (!avi || !out) {
        return -1;
    }

    IMFSample* sample = NULL;
    DWORD streamIndex = 0, flags = 0;
    LONGLONG timestamp = 0;

    HRESULT hr = avi->reader->lpVtbl->ReadSample(avi->reader,
        MF_SOURCE_READER_FIRST_VIDEO_STREAM,
        0, &streamIndex, &flags, &timestamp, &sample);

    if (FAILED(hr)) {
        if (sample) sample->lpVtbl->Release(sample);
        return -1;
    }

    if (flags & MF_SOURCE_READERF_ENDOFSTREAM) {
        if (sample) sample->lpVtbl->Release(sample);
        return 0; // End of stream
    }

    if (!sample) {
        return -1; // No sample available
    }

    IMFMediaBuffer* buffer = NULL;
    hr = sample->lpVtbl->ConvertToContiguousBuffer(sample, &buffer);
    if (FAILED(hr) || !buffer) {
        sample->lpVtbl->Release(sample);
        return -1;
    }

    BYTE* data = NULL;
    DWORD maxLen = 0, currentLen = 0;
    hr = buffer->lpVtbl->Lock(buffer, &data, &maxLen, &currentLen);
    if (FAILED(hr)) {
        buffer->lpVtbl->Release(buffer);
        sample->lpVtbl->Release(sample);
        return -1;
    }

    IMFMediaType* type = NULL;
    hr = avi->reader->lpVtbl->GetCurrentMediaType(avi->reader, MF_SOURCE_READER_FIRST_VIDEO_STREAM, &type);
    if (FAILED(hr) || !type) {
        buffer->lpVtbl->Unlock(buffer);
        buffer->lpVtbl->Release(buffer);
        sample->lpVtbl->Release(sample);
        return -1;
    }

    UINT32 width = 0, height = 0;
    hr = MFGetAttributeSize((IMFAttributes*)type, &MF_MT_FRAME_SIZE, &width, &height);
    if (FAILED(hr) || width == 0 || height == 0) {
        type->lpVtbl->Release(type);
        buffer->lpVtbl->Unlock(buffer);
        buffer->lpVtbl->Release(buffer);
        sample->lpVtbl->Release(sample);
        return -1;
    }

    // Determine stride based on subtype
    GUID subtype = {0};
    hr = type->lpVtbl->GetGUID(type, &MF_MT_SUBTYPE, &subtype);
    print_guid(&subtype);

    if (memcmp(&subtype, &MFVideoFormat_RGB32, sizeof(GUID)) != 0) {
        MessageBoxA(NULL,
            "Questo file video non può essere decodificato da Media Foundation.\n"
            "Prova con un altro file o installa i codec H.264.",
            "Formato non supportato", MB_ICONERROR);
        type->lpVtbl->Release(type);
        buffer->lpVtbl->Unlock(buffer);
        buffer->lpVtbl->Release(buffer);
        sample->lpVtbl->Release(sample);
        //free
        if (data) {
            HeapFree(GetProcessHeap(), 0, data);
        }
        return -1;
    }

    UINT32 stride = 0;
    if (SUCCEEDED(hr)) {
            stride = width * 4;
    } else {
        stride = width * 4;
    }

    LONG mf_stride = 0;
    hr = type->lpVtbl->GetUINT32(type, &MF_MT_DEFAULT_STRIDE, (UINT32*)&mf_stride);
    if (FAILED(hr) || mf_stride == 0) {
        mf_stride = width * 4; // fallback
    }

    SIZE_T needed_size = (SIZE_T)height * width * 4;
    uint8_t* output = (uint8_t*)HeapAlloc(GetProcessHeap(), 0, needed_size);

    if (!output) {
        type->lpVtbl->Release(type);
        buffer->lpVtbl->Unlock(buffer);
        buffer->lpVtbl->Release(buffer);
        sample->lpVtbl->Release(sample);
        return -1;
    }

    // Copia solo i dati disponibili, azzera il resto
    memset(output, 0, needed_size);
    for (UINT32 y = 0; y < height; ++y) {
        SIZE_T src_off = y * mf_stride;
        SIZE_T dst_off = y * width * 4;
        if (src_off + width * 4 <= currentLen && dst_off + width * 4 <= needed_size) {
            memcpy(output + dst_off, data + src_off, width * 4);
        }
    }
    out->data = output;
    out->width = width;
    out->height = height;
    out->stride = width * 4;

    type->lpVtbl->Release(type);
    buffer->lpVtbl->Unlock(buffer);
    buffer->lpVtbl->Release(buffer);
    sample->lpVtbl->Release(sample);

    return 1;
}

void gc_free_frame(gc_frame_t* frame) {
    if (!frame)
        return;

    if (frame->data) {
        HeapFree(GetProcessHeap(), 0, frame->data);
        frame->data = NULL;
    }

    frame->width = 0;
    frame->height = 0;
    frame->stride = 0;
}


void gc_close(gc_avi* avi) {
    if (!avi) return;
    if (avi->outputType) {
        avi->outputType->lpVtbl->Release(avi->outputType);
        //MessageBox(NULL, "Released outputType", "Info", MB_OK);
    }
    if (avi->reader) {
        avi->reader->lpVtbl->Release(avi->reader);
        //MessageBox(NULL, "Released reader", "Info", MB_OK);
    }
    free(avi);
    //MessageBox(NULL, "gc_avi closed and memory freed", "Info", MB_OK);
}
