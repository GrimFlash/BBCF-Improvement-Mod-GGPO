#include "NotificationBar.h"

#include "Core/Settings.h"

#include <imgui.h>

NotificationBar* g_notificationBar = new NotificationBar();

void NotificationBar::DrawNotifications()
{
	if (m_notifications.empty())
		return;

	if (ImGui::BeginMainMenuBar())
	{
		m_screenWidth = ImGui::GetWindowWidth();

		if (!m_isTextOffsetInitialized)
		{
			CalculateTextStartingOffset(m_notifications.front().c_str());
			m_isTextOffsetInitialized = true;
		}

		ImGui::SetCursorPosX(m_textOffset);
		ImGui::TextUnformatted(m_notifications.front().c_str());

		ImGui::EndMainMenuBar();
	}

	m_textOffset -= ImGui::GetIO().DeltaTime * (m_screenWidth / m_secondsToSlide);

	bool isTextOffscreen = m_textOffset < -m_textWidth;
	if (isTextOffscreen)
	{
		m_notifications.pop();
		m_isTextOffsetInitialized = false;
	}
}

void NotificationBar::AddNotification(const char* text, ...)
{
	if (!Settings::settingsIni.notifications)
		return;

	va_list args;
	va_start(args, text);

	const int size = std::vsnprintf(nullptr, 0, text, args) + 1;
	std::string msg(size, ' ');

	vsprintf(&msg[0], text, args);
	va_end(args);

	m_notifications.push(msg);
}

void NotificationBar::ClearNotifications()
{
	m_notifications = {};
	m_isTextOffsetInitialized = false;
}

void NotificationBar::CalculateTextStartingOffset(const std::string& text)
{
	ImGui::TextColored(ImVec4(0, 0, 0, 0), text.c_str());
	m_textWidth = ImGui::GetItemRectSize().x;
	m_textOffset = m_screenWidth;
}
