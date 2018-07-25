#pragma once
#include <windows.h>
#include <tlhelp32.h>

static bool GetPidByName(const char *exeFileName, int& pid);
HANDLE OpenGameProcess(const char *exeFileName);