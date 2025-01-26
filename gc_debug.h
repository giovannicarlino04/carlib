#pragma once

#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#endif 

// Set to 1 to enable optional debug messages
// In memory.c & others
#define DEBUG 1 

#ifdef _WIN32
void gc_MessageBox(const char* message, int type);
#else

#define MB_ICONERROR 0x00000010L
#define MB_ICONWARNING 0x00000030L
#define MB_ICONINFORMATION 0x00000040L

void gc_MessageBox(const char* message,  int type);  //type unused
#endif

// For now let's provide an alternative
// Just to get some sort of error to the console in Unix
