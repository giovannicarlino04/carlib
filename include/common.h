
#ifndef COMMON_H
#define COMMON_H

#include <Windows.h>
#include <mfapi.h>
#include <mfplay.h>
#include <mfreadwrite.h>
#include <mftransform.h>
#include <mfobjects.h>
#include <shlwapi.h>
#include <strmif.h>
#include <initguid.h>
#include <stdint.h>
#include <sys/stat.h>
#include <stdio.h>

#ifndef DLLEXPORT
#define DLLEXPORT __declspec(dllexport)
#endif

#define internal static

#endif