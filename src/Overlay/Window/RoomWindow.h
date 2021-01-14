#pragma once
#include "IWindow.h"

#include <steamclientpublic.h>

class RoomWindow : public IWindow
{
public:
	RoomWindow(const std::string& windowTitle, bool windowClosable,
		ImGuiWindowFlags windowFlags = 0)
		: IWindow(windowTitle, windowClosable, windowFlags), m_origWindowTitle(windowTitle)
	{
	}

	~RoomWindow() override = default;

protected:
	void BeforeDraw() override;
	void Draw() override;

private:
	void SetWindowTitleRoomType(const std::string& roomTypeName);
	void ShowClickableSteamUser(const char* playerName, const CSteamID& steamId) const;
	void DrawRoomImPlayers();
	void DrawMatchImPlayers();

	const std::string m_origWindowTitle;
};
