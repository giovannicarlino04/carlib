# Carlib

**Carlib** is a modular C library and DLL for Windows, providing a wide range of utilities for file I/O, graphics, cryptography, networking, patching, and more. It is designed for extensibility and ease of integration into other C/C++ projects.

---

## Table of Contents

- [Features](#features)
- [Directory Structure](#directory-structure)
- [Building](#building)
- [Usage](#usage)
- [Modules Overview](#modules-overview)
- [Export/Import Macros](#exportimport-macros)
- [Contributing](#contributing)
- [License](#license)

---

## Features

- **File I/O**: Read, write, copy, move, and delete files.
- **Graphics**: Simple Win32 GUI controls, drawing primitives, and bitmap handling.
- **Cryptography**: SHA-256, HMAC-SHA256, AES, and secure random bytes.
- **Debugging**: Message boxes and logging utilities.
- **Memory Management**: Custom memory pool allocator.
- **Networking**: Simple TCP server and file serving.
- **Patching**: Pattern scanning and memory patching.
- **String Utilities**: String duplication, formatting, and manipulation.
- **SWF/WAV/IGGY**: Parsing and handling of SWF, WAV, and IGGY file formats.
- **Timing**: High-resolution timers and sleep utilities.
- **DLL Export**: All public APIs are exported for use from other applications.

---

## Directory Structure

```
carlib/
├── build.bat           # Build script for Windows (MSVC cl.exe)
├── src/                # All source files (.c)
│   ├── dllmain.c
│   ├── gc_avi.c
│   ├── gc_binpatch.c
│   ├── gc_common.c
│   ├── gc_crypto.c
│   ├── gc_debug.c
│   ├── gc_file.c
│   ├── gc_graphics.c
│   ├── gc_hook.c
│   ├── gc_iggy.c
│   ├── gc_input.c
│   ├── gc_memory.c
│   ├── gc_network.c
│   ├── gc_patch.c
│   ├── gc_string.c
│   ├── gc_swf.c
│   ├── gc_time.c
│   ├── gc_wav.c
│   └── gc_xenoverse_cms.c
├── include/            # (Optional) Header files for public API
├── build/              # Output directory for DLL and import library
│   ├── dll/
│   └── lib/
├── obj/                # Intermediate object files
├── .gitignore
└── README.md           # This documentation
```

---

## Building

### Prerequisites

- **Windows** with Microsoft Visual Studio (cl.exe, link.exe)
- Windows SDK (for headers and system libraries)
- (Optional) Media Foundation SDK for video support

### Build Steps

1. Open a **Developer Command Prompt** for Visual Studio.
2. Run the build script:

   ```
   build.bat
   ```

   This will:
   - Compile all `.c` files in `src/` to `.obj` files in `obj/`
   - Link them into `build\dll\carlib.dll` and `build\lib\carlib.lib`
   - Output files are placed in the `build/` directory

### Linking Notes

- The build script links against required system libraries: `user32.lib`, `gdi32.lib`, `ws2_32.lib`, `advapi32.lib`, etc.
- Media Foundation libraries are linked via `#pragma comment(lib, ...)` in the source.

---

## Usage

### As a DLL

- Include the appropriate headers from `include/` in your project.
- Link against `carlib.lib` and ensure `carlib.dll` is available at runtime.
- All public functions are exported with `__declspec(dllexport)` and use the `DLLEXPORT` macro.

### Example

```c
#include "gc_file.h"

int main() {
    size_t size;
    unsigned char* data = gc_read_file("example.txt", &size);
    if (data) {
        // Use data...
        gc_free(data);
    }
    return 0;
}
```

---

## Modules Overview

### File I/O (`gc_file.c`)
- `gc_read_file`, `gc_copy_file`, `gc_move_file`, `gc_delete_file`, etc.

### Graphics (`gc_graphics.c`)
- Win32 window and control creation, drawing primitives, double-buffering.

### Cryptography (`gc_crypto.c`)
- `gc_sha256`, `gc_hmac_sha256`, `gc_crypto_random`, AES routines.

### Debugging (`gc_debug.c`)
- `gc_MessageBox` for displaying message boxes.

### Memory (`gc_memory.c`)
- `gc_malloc`, `gc_free`: custom memory pool allocator.

### Networking (`gc_network.c`)
- TCP server, file serving, initialization and cleanup.

### Patching (`gc_patch.c`)
- Pattern scanning, patching, and restoring memory.

### String Utilities (`gc_string.c`)
- `gc_strndup`, `gc_snprintf`, `gc_ends_with`, etc.

### SWF/WAV/IGGY (`gc_swf.c`, `gc_wav.c`, `gc_iggy.c`)
- Parsing and handling of SWF, WAV, and IGGY file formats.

### Timing (`gc_time.c`)
- High-resolution timers, sleep functions.

### DLL Entry (`dllmain.c`)
- Standard `DllMain` for DLL initialization.

---

## Export/Import Macros

- **DLLEXPORT**: Used for all public API functions, defined as `__declspec(dllexport)` in `common.h`.
- **internal**: Used for file-local (static) functions, defined as `static` in `common.h`.

---

## Contributing

- Fork the repository and submit pull requests.
- Follow the code style and modular structure.
- Add documentation for new modules or functions.

---

## License

This project is licensed under the MIT License or as specified in the repository.

---

## Troubleshooting

- **Linker errors for Windows API functions**: Ensure you are linking against the required system libraries.
- **socklen_t undefined**: The code defines `socklen_t` as `int` on Windows for compatibility.
- **Media Foundation errors**: Ensure you have the Media Foundation SDK and required DLLs on your system.
- **Linux Support** The function implementations mostly rely on the Windows API, but as people start using the library I will probably extend support.


---

## Contact

For questions or contributions, open an issue or contact the maintainer.

