#pragma once

#ifdef _WIN32
#include <Windows.h>
#endif

#include <stdio.h>

// Set to 1 to enable optional debug messages
// In memory.c & others
#define DEBUG 1 
void gc_MessageBox(const char* message, int type);
