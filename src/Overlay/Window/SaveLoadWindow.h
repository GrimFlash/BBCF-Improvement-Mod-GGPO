#pragma once
#include "IWindow.h"

class SaveLoadWindow : public IWindow
{
public:
	SaveLoadWindow(const std::string& windowTitle, bool windowClosable,
		ImGuiWindowFlags windowFlags = 0)
		: IWindow(windowTitle, windowClosable, windowFlags) {}
	~SaveLoadWindow() override = default;

protected:
	void Draw() override;

private:
	void DrawSaveLoadStateWindow();
};