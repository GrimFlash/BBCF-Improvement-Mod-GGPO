#include "WindowContainer.h"

#include "Overlay/Window/DebugWindow.h"
#include "Overlay/Window/HitboxOverlay.h"
#include "Overlay/Window/LogWindow.h"
#include "Overlay/Window/MainWindow.h"
#include "Overlay/Window/PaletteEditorWindow.h"
#include "Overlay/Window/RoomWindow.h"
#include "Overlay/Window/SaveLoadWindow.h"
#include "Overlay/Window/UpdateNotifierWindow.h"

#include "Core/info.h"
#include "Core/logger.h"
#include "Core/Settings.h"

WindowContainer::WindowContainer()
{
	AddWindow(WindowType_Main,
		new MainWindow(MOD_WINDOW_TITLE, false, *this, ImGuiWindowFlags_AlwaysAutoResize));

	AddWindow(WindowType_Log,
		new LogWindow("Log", true, *(ImGuiLogger*)g_imGuiLogger,
			ImGuiWindowFlags_NoCollapse));

	AddWindow(WindowType_Debug,
		new DebugWindow("DEBUG", true));

	AddWindow(WindowType_UpdateNotifier,
		new UpdateNotifierWindow("Update available", true,
			ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse));

	AddWindow(WindowType_PaletteEditor,
		new PaletteEditorWindow("Palette Editor", true));

	AddWindow(WindowType_HitboxOverlay,
		new HitboxOverlay("##HitboxOverlay", false, ImGuiWindowFlags_NoCollapse));

	AddWindow(WindowType_Room,
		new RoomWindow("Online###Room", true, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse));

	AddWindow(WindowType_SaveLoadState,
		new SaveLoadWindow("Save/Load State", true));
}
