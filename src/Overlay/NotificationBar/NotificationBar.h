#pragma once

#include "Overlay/Window/DebugWindow.h"

#include <string>
#include <queue>

class NotificationBar;

extern NotificationBar* g_notificationBar;

class NotificationBar
{
public:
	void DrawNotifications();
	void AddNotification(const char* text, ...);
	void ClearNotifications();

private:
	void CalculateTextStartingOffset(const std::string& text);

	std::queue<std::string> m_notifications;

	bool m_isTextOffsetInitialized = false;
	float m_textOffset = 0;
	float m_textWidth = 0;
	float m_screenWidth = 0;
	float m_secondsToSlide = 7;

	// For debug purposes
	friend class DebugWindow;
};
