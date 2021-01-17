#pragma once
#include "IWindow.h"
#include "Overlay/Logger/ImGuiLogger.h"

class LogWindow : public IWindow
{
public:
	LogWindow(const std::string& windowTitle, bool windowClosable, ImGuiLogger& logger,
		ImGuiWindowFlags windowFlags = 0)
		: IWindow(windowTitle, windowClosable, windowFlags), m_logger(logger) {}
	~LogWindow() override = default;
protected:
	void BeforeDraw() override;
	void Draw() override;
private:
	ImGuiLogger&    m_logger;
	ImGuiTextFilter m_filter;
	float           m_prevScrollMaxY = 0;
};
