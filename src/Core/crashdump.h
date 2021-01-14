#pragma once
#include <Windows.h>

//Custom exception handler to generate memory dump upon crash
LONG WINAPI UnhandledExFilter(PEXCEPTION_POINTERS ExPtr);