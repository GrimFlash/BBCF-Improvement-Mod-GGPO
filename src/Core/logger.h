#pragma once
#include "Settings.h"

#include "D3D9EXWrapper/d3d9.h"

#define DEBUG_LOG_LEVEL	5 //0 = highest, 7 = lowest priority
#define FORCE_LOGGING 0

#if defined(_DEBUG) || FORCE_LOGGING == 1
#define ENABLE_LOGGING 1
#endif

#ifdef ENABLE_LOGGING
#define LOG(_level, _str, ...) { \
	if (DEBUG_LOG_LEVEL >= _level) { logger(_str, __VA_ARGS__); }}

//Use this to log in naked asm functions
#define LOG_ASM(_level, _str, ...) { \
	__asm{__asm pushad }; \
	if (DEBUG_LOG_LEVEL >= _level) { {logger(_str, __VA_ARGS__);} } \
	__asm{__asm popad }; }
#else
#define LOG(_level, _str, ...) {}

//Use this to log in naked asm functions
#define LOG_ASM(_level, _str, ...) {}
#endif

inline void logger(const char* message, ...);
void openLogger();
void closeLogger();
//free it after usage!!
char* getFullDate();
void logSettingsIni();
bool hookSucceeded(PBYTE addr, const char* funcName);
void logD3DPParams(D3DPRESENT_PARAMETERS* pPresentationParameters, bool isOriginalSettings = true);