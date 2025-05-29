@echo off
setlocal

REM Set output binary name
mkdir build
set EXE_NAME=.\build\test.exe
set OBJECTS=gc_graphics.c gc_input.c main.c


REM Clean previous build
if exist %EXE_NAME% del %EXE_NAME%

REM Compile
cl /nologo /O2 /GL /Gy /favor:INTEL64 /arch:AVX2 /Oi /Ot /Oy /GS- /DNDEBUG /Fe%EXE_NAME% %OBJECTS% user32.lib gdi32.lib /link /LTCG /INCREMENTAL:NO

REM Run if successful
if exist %EXE_NAME% (
    echo Build succeeded.
    %EXE_NAME%
) else (
    echo Build failed.
)

endlocal
pause
