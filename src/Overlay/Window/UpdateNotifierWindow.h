#pragma once
#include "IWindow.h"

class UpdateNotifierWindow : public IWindow
{
public:
	UpdateNotifierWindow(const std::string& windowTitle, bool windowClosable,
		ImGuiWindowFlags windowFlags)
		: IWindow(windowTitle, windowClosable, windowFlags) {}
	~UpdateNotifierWindow() override = default;
protected:
	void BeforeDraw() override;
	void Draw() override;
};
