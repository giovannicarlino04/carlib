#include "common.h"

// Windows related functions
#ifdef _WIN32

#include <Windows.h>
#include <Dbghelp.h>
#pragma comment(lib, "Dbghelp.lib")

typedef struct gc_hook {
    void* target_function; // Function to hook
    void* hook_function;   // Function to call instead of original
    void* original_function; // Pointer to store the original function address
} gc_hook_t;

DLLEXPORT const char** gc_list_functions(HANDLE hModule) {
    if (!hModule) return NULL;

    ULONG size = 0;
    PIMAGE_EXPORT_DIRECTORY exportDir = (PIMAGE_EXPORT_DIRECTORY)ImageDirectoryEntryToData(
        hModule, TRUE, IMAGE_DIRECTORY_ENTRY_EXPORT, &size);

    if (!exportDir || exportDir->NumberOfNames == 0) return NULL;

    const char** names = (const char**)malloc((exportDir->NumberOfNames + 1) * sizeof(char*));
    if (!names) return NULL;

    DWORD* nameRVAs = (DWORD*)((BYTE*)hModule + exportDir->AddressOfNames);
    for (DWORD i = 0; i < exportDir->NumberOfNames; ++i) {
        const char* funcName = (const char*)hModule + nameRVAs[i];
        names[i] = _strdup(funcName); // Duplicate string for caller to free
    }
    names[exportDir->NumberOfNames] = NULL; // Null-terminate

    return names;
}

// Free the list of function names returned by gc_list_functions
DLLEXPORT void gc_free_function_list(const char** names) {
    if (!names) return;
    for (int i = 0; names[i]; ++i) {
        free((void*)names[i]);
    }
    free((void*)names);
}

// Now maybe a generic Win32 function search function
// that can be used to find functions in DLLs, like GetProcAddress but more generic?
// This could be useful for dynamically finding functions to hook or call.
// For example, we could use it to find a function in a specific DLL by name.
// This function could take a DLL name and a function name, and return the address of the function.
DLLEXPORT void* gc_find_function(const char* dll_name, const char* function_name) {
    HMODULE hModule = LoadLibraryA(dll_name);
    if (hModule) {
        return GetProcAddress(hModule, function_name);
    }
    return NULL; // Return NULL if the DLL or function is not found
}

DLLEXPORT BOOL gc_hook_create(HMODULE hModule, void* target_function, void* hook_function){
    DWORD oldProtect;
    gc_hook_t* hook = (gc_hook_t*)malloc(sizeof(gc_hook_t));

    // Use hModule for context if needed (not strictly required for VirtualProtect, but included for API style)
    if (!VirtualProtect((LPVOID)target_function, sizeof(LPVOID), PAGE_EXECUTE_READWRITE, &oldProtect)) {
        printf("failed to change memprotect");
        return FALSE;
    }
    if (hook) {
        hook->target_function = target_function;
        hook->hook_function = hook_function;
        hook->original_function = NULL; // Initialize original function pointer to NULL
    }
    else{
        printf("failed to hook function");
        return FALSE;
    }
    if (!VirtualProtect((LPVOID)target_function, sizeof(LPVOID), oldProtect, &oldProtect)) {
        printf("failed to restore memprotect");
        return FALSE;  // Error: Failed to restore memory protection
    }

    return TRUE;
}

DLLEXPORT BOOL gc_hook_free(gc_hook_t* hook){
    if (hook) {
        free(hook);
    }
    return TRUE; // Return TRUE to indicate the hook has been freed
}
#endif

