#pragma once
#include "Logger.h"

#include <imgui.h>

#include <string>

extern Logger* g_imGuiLogger;

struct ImGuiLogBuffer
{
	ImGuiTextBuffer textBuffer;
	ImVector<int>   lineOffsets; // Index to lines offset
};

class ImGuiLogger : public Logger
{
public:
	ImGuiLogger() : m_textBuffer(m_buffer.textBuffer), m_lineOffsets(m_buffer.lineOffsets) {}
	~ImGuiLogger() override = default;
	void Log(LogLevel_ logLevel, const char* fmt, ...) override;
	void Log(const char* fmt, ...) override;
	void LogSeparator() override;
	void Clear() override;
	void ToFile(FILE* file) const override { fprintf(file, "%s", m_buffer.textBuffer.begin()); }
	void EnableLog(bool value) override { m_loggingEnabled = value; }
	bool IsLogEnabled() const override { return m_loggingEnabled; }
	const void* GetBuffer() const override { return &m_buffer; }

private:
	const std::string GetTime();
	const std::string GetDate();

	bool            m_loggingEnabled = true;
	ImGuiLogBuffer  m_buffer;

	ImGuiTextBuffer& m_textBuffer;
	ImVector<int>& m_lineOffsets;
};
