@echo off
setlocal
set SRC_DIR=.\src
set INCLUDE_DIR=.\include
set OUT_DIR=.\build
set OBJ_DIR=.\obj
set LIB_DIR=.\build\lib
set DLL_DIR=.\build\dll

set LIBS=user32.lib gdi32.lib ws2_32.lib advapi32.lib

if not exist "%OUT_DIR%" mkdir "%OUT_DIR%"
if not exist "%OBJ_DIR%" mkdir "%OBJ_DIR%"
if not exist "%LIB_DIR%" mkdir "%LIB_DIR%"
if not exist "%DLL_DIR%" mkdir "%DLL_DIR%"

:: Compile all .c files to .obj
for %%f in (
    "%SRC_DIR%\dllmain.c"
    "%SRC_DIR%\gc_avi.c"
    "%SRC_DIR%\gc_binpatch.c"
    "%SRC_DIR%\gc_common.c"
    "%SRC_DIR%\gc_crypto.c"
    "%SRC_DIR%\gc_debug.c"
    "%SRC_DIR%\gc_file.c"
    "%SRC_DIR%\gc_graphics.c"
    "%SRC_DIR%\gc_hook.c"
    "%SRC_DIR%\gc_iggy.c"
    "%SRC_DIR%\gc_input.c"
    "%SRC_DIR%\gc_memory.c"
    "%SRC_DIR%\gc_network.c"
    "%SRC_DIR%\gc_patch.c"
    "%SRC_DIR%\gc_string.c"
    "%SRC_DIR%\gc_swf.c"
    "%SRC_DIR%\gc_time.c"
    "%SRC_DIR%\gc_wav.c"
    "%SRC_DIR%\gc_xenoverse_cms.c"
) do (
    cl /c /Fo"%OBJ_DIR%\%%~nf.obj" /I"%SRC_DIR%" /I"%INCLUDE_DIR%" %%f
)

:: Link all .obj files into a DLL, add required system libraries
link /DLL /OUT:"%DLL_DIR%\carlib.dll" /IMPLIB:"%LIB_DIR%\carlib.lib" ^
    "%OBJ_DIR%\dllmain.obj" ^
    "%OBJ_DIR%\gc_avi.obj" ^
    "%OBJ_DIR%\gc_binpatch.obj" ^
    "%OBJ_DIR%\gc_common.obj" ^
    "%OBJ_DIR%\gc_crypto.obj" ^
    "%OBJ_DIR%\gc_debug.obj" ^
    "%OBJ_DIR%\gc_file.obj" ^
    "%OBJ_DIR%\gc_graphics.obj" ^
    "%OBJ_DIR%\gc_hook.obj" ^
    "%OBJ_DIR%\gc_iggy.obj" ^
    "%OBJ_DIR%\gc_input.obj" ^
    "%OBJ_DIR%\gc_memory.obj" ^
    "%OBJ_DIR%\gc_network.obj" ^
    "%OBJ_DIR%\gc_patch.obj" ^
    "%OBJ_DIR%\gc_string.obj" ^
    "%OBJ_DIR%\gc_swf.obj" ^
    "%OBJ_DIR%\gc_time.obj" ^
    "%OBJ_DIR%\gc_wav.obj" ^
    "%OBJ_DIR%\gc_xenoverse_cms.obj" ^
    %LIBS%

endlocal
