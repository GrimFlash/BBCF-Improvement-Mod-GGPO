#pragma once
#include "IWindow.h"

#include "Overlay/WindowContainer/WindowContainer.h"

class MainWindow : public IWindow
{
public:
	MainWindow(const std::string& windowTitle, bool windowClosable,
		WindowContainer& windowContainer, ImGuiWindowFlags windowFlags = 0);

	~MainWindow() override = default;

protected:
	void BeforeDraw() override;
	void Draw() override;

private:
	void DrawUtilButtons() const;
	void DrawCurrentPlayersCount() const;
	void DrawLinkButtons() const;
	void DrawLoadedSettingsValuesSection() const;
	void DrawCustomPalettesSection() const;
	void DrawHitboxOverlaySection() const;
	void DrawGameplaySettingSection() const;
	void DrawAvatarSection() const;

	const ImVec2 BTN_SIZE = ImVec2(60, 20);
	WindowContainer* m_pWindowContainer = nullptr;
};
