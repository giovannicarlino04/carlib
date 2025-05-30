#include "gc_mp4.h"
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

struct gc_mp4 {
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
        *punWidth = (UINT32)(unSize & 0xFFFFFFFF);
        *punHeight = (UINT32)(unSize >> 32);
    }
    return hr;
}

static void gc_initialize_media_foundation() {
    static int initialized = 0;
    if (!initialized) {
        if (FAILED(MFStartup(MF_VERSION, MFSTARTUP_LITE))) {
            //MessageBox(NULL, "MFStartup failed", "Error", MB_ICONERROR);
        } else {
            //MessageBox(NULL, "MFStartup succeeded", "Info", MB_OK);
            initialized = 1;
        }
    }
}

gc_mp4* gc_open(const char* path) {
    gc_initialize_media_foundation();

    gc_mp4* mp4 = (gc_mp4*)calloc(1, sizeof(gc_mp4));
    if (!mp4) {
        //MessageBox(NULL, "Failed to allocate gc_mp4", "Error", MB_ICONERROR);
        return NULL;
    }

    WCHAR wpath[MAX_PATH];
    MultiByteToWideChar(CP_UTF8, 0, path, -1, wpath, MAX_PATH);
    //MessageBoxA(NULL, (LPCSTR)wpath, (LPCSTR)"Opening file", MB_OK);

    IMFAttributes* pAttributes = NULL;
    HRESULT hr = MFCreateAttributes(&pAttributes, 1);
    if (FAILED(hr)) {
        //MessageBox(NULL, "MFCreateAttributes failed", "Error", MB_ICONERROR);
        free(mp4);
        return NULL;
    }
    //MessageBoxA(NULL, "MFCreateAttributes succeeded", "Info", MB_OK);

    hr = MFCreateSourceReaderFromURL(wpath, pAttributes, &mp4->reader);
    if (FAILED(hr)) {
        //MessageBox(NULL, "MFCreateSourceReaderFromURL failed", "Error", MB_ICONERROR);
        pAttributes->lpVtbl->Release(pAttributes);
        free(mp4);
        return NULL;
    }
    //MessageBox(NULL, "MFCreateSourceReaderFromURL succeeded", "Info", MB_OK);

    pAttributes->lpVtbl->Release(pAttributes);

    hr = MFCreateMediaType(&mp4->outputType);
    if (FAILED(hr)) {
        //MessageBox(NULL, "MFCreateMediaType failed", "Error", MB_ICONERROR);
        mp4->reader->lpVtbl->Release(mp4->reader);
        free(mp4);
        return NULL;
    }
    //MessageBox(NULL, "MFCreateMediaType succeeded", "Info", MB_OK);

    hr = mp4->outputType->lpVtbl->SetGUID(mp4->outputType, &MF_MT_MAJOR_TYPE, &MFMediaType_Video);
    if (FAILED(hr)) {
        //MessageBox(NULL, "SetGUID MF_MT_MAJOR_TYPE failed", "Error", MB_ICONERROR);
    }

    hr = mp4->outputType->lpVtbl->SetGUID(mp4->outputType, &MF_MT_SUBTYPE, &MFVideoFormat_RGB32);
    if (FAILED(hr)) {
        //MessageBox(NULL, "SetGUID MF_MT_SUBTYPE failed", "Error", MB_ICONERROR);
    }

    mp4->outputType->lpVtbl->SetGUID(mp4->outputType, &MF_MT_MAJOR_TYPE, &MFMediaType_Video);
    mp4->outputType->lpVtbl->SetGUID(mp4->outputType, &MF_MT_SUBTYPE, &MFVideoFormat_NV12);


    //MessageBox(NULL, "SetCurrentMediaType succeeded", "Info", MB_OK);

    return mp4;
}

int gc_decode_next_frame(gc_mp4* mp4, gc_frame_t* out) {
    if (!mp4 || !out) {
        //MessageBox(NULL, "Invalid arguments to gc_decode_next_frame", "Error", MB_ICONERROR);
        return -1;
    }

    IMFSample* sample = NULL;
    DWORD streamIndex = 0, flags = 0;
    LONGLONG timestamp = 0;

    HRESULT hr = mp4->reader->lpVtbl->ReadSample(mp4->reader,
        MF_SOURCE_READER_FIRST_VIDEO_STREAM,
        0, &streamIndex, &flags, &timestamp, &sample);

    if (FAILED(hr)) {
        //MessageBox(NULL, "ReadSample failed", "Error", MB_ICONERROR);
        if (sample) sample->lpVtbl->Release(sample);
        return -1;
    }

    if (flags & MF_SOURCE_READERF_ENDOFSTREAM) {
        //MessageBox(NULL, "End of stream reached", "Info", MB_OK);
        if (sample) sample->lpVtbl->Release(sample);
        return 0;
    }

    if (!sample) {
        //MessageBox(NULL, "ReadSample returned no sample", "Error", MB_ICONERROR);
        return -1;
    }

    IMFMediaBuffer* buffer = NULL;
    hr = sample->lpVtbl->ConvertToContiguousBuffer(sample, &buffer);
    if (FAILED(hr) || !buffer) {
        //MessageBox(NULL, "ConvertToContiguousBuffer failed", "Error", MB_ICONERROR);
        sample->lpVtbl->Release(sample);
        return -1;
    }

    BYTE* data = NULL;
    DWORD maxLen = 0, currentLen = 0;
    hr = buffer->lpVtbl->Lock(buffer, &data, &maxLen, &currentLen);
    if (FAILED(hr)) {
        //MessageBox(NULL, "Buffer lock failed", "Error", MB_ICONERROR);
        buffer->lpVtbl->Release(buffer);
        sample->lpVtbl->Release(sample);
        return -1;
    }

    IMFMediaType* type = NULL;
    hr = mp4->reader->lpVtbl->GetCurrentMediaType(mp4->reader, MF_SOURCE_READER_FIRST_VIDEO_STREAM, &type);
    if (FAILED(hr) || !type) {
        //MessageBox(NULL, "GetCurrentMediaType failed", "Error", MB_ICONERROR);
        buffer->lpVtbl->Unlock(buffer);
        buffer->lpVtbl->Release(buffer);
        sample->lpVtbl->Release(sample);
        return -1;
    }

    UINT32 width = 0, height = 0;
    hr = MFGetAttributeSize((IMFAttributes*)type, &MF_MT_FRAME_SIZE, &width, &height);
    if (FAILED(hr) || width == 0 || height == 0) {
        //MessageBoxA(NULL, "MFGetAttributeSize failed or invalid size", "Error", MB_ICONERROR);
        type->lpVtbl->Release(type);
        buffer->lpVtbl->Unlock(buffer);
        buffer->lpVtbl->Release(buffer);
        sample->lpVtbl->Release(sample);
        return -1;
    }
    wchar_t buf[256];
    wsprintf( (LPSTR)buf,  (LPCSTR)"Frame size: %u x %u\nBuffer length: %u bytes", width, height, currentLen);
    //MessageBoxA(NULL,  (LPCSTR)buf, "Info", MB_OK);

    uint8_t* output = (uint8_t*)malloc(currentLen);
    if (!output) {
        //MessageBox(NULL, "Failed to allocate output buffer", "Error", MB_ICONERROR);
        type->lpVtbl->Release(type);
        buffer->lpVtbl->Unlock(buffer);
        buffer->lpVtbl->Release(buffer);
        sample->lpVtbl->Release(sample);
        return -1;
    }

    memcpy(output, data, currentLen);

    out->data = output;
    out->width = width;
    out->height = height;
    out->stride = width * 4;

    type->lpVtbl->Release(type);
    buffer->lpVtbl->Unlock(buffer);
    buffer->lpVtbl->Release(buffer);
    sample->lpVtbl->Release(sample);

    //MessageBox(NULL, "Frame decoded successfully", "Info", MB_OK);

    return 1;
}

void gc_free_frame(gc_frame_t* frame) {
    if (frame && frame->data) {
        free(frame->data);
        frame->data = NULL;
        //MessageBox(NULL, "Frame memory freed", "Info", MB_OK);
    }
}

void gc_close(gc_mp4* mp4) {
    if (!mp4) return;
    if (mp4->outputType) {
        mp4->outputType->lpVtbl->Release(mp4->outputType);
        //MessageBox(NULL, "Released outputType", "Info", MB_OK);
    }
    if (mp4->reader) {
        mp4->reader->lpVtbl->Release(mp4->reader);
        //MessageBox(NULL, "Released reader", "Info", MB_OK);
    }
    free(mp4);
    //MessageBox(NULL, "gc_mp4 closed and memory freed", "Info", MB_OK);
}
