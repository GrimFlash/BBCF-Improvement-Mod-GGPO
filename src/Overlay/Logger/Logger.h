#pragma once
#include <cstdio>

enum LogLevel_
{
	LogLevel_System,
	LogLevel_Info,
	LogLevel_Warning,
	LogLevel_Error,
	LogLevel_Fatal,
	LogLevel_Notice,
	LogLevel_Log
};

class Logger
{
public:
	virtual ~Logger() = default;
	virtual void Log(LogLevel_ logLevel, const char* fmt, ...) = 0;
	virtual void Log(const char* fmt, ...) = 0;
	virtual void LogSeparator() = 0;
	virtual void Clear() = 0;
	virtual void ToFile(FILE* file) const = 0;
	virtual void EnableLog(bool value) = 0;
	virtual bool IsLogEnabled() const = 0;
	virtual const void* GetBuffer() const = 0;
};
