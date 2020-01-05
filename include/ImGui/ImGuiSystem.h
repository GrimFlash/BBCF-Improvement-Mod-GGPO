#pragma once
#include "../stdafx.h"

// reminder to myself, pitfall of using imgui in your project:
// ALWAYS ADD THE IMGUI SOURCE FILES TO YOUR PROJECT OR YOU WILL GET UNRESOLVED EXTERNAL SYMBOL ERRORS

struct ImGuiLog;

class ImGuiSystem
{
public:
	static bool Initialized;
	static bool Init(void *hwnd, IDirect3DDevice9 *device);
	static void Render();
	static void Shutdown();
	static void InvalidateDeviceObjects();
	static void CreateDeviceObjects();
	static void HandleImGuiWindows();
	static void AddLog(const char* message, ...);
	static void AddLogSeparator();
	static void WriteLogToFile();
	static void SetNotification(const char *text, float timeToShowInSec = 5.0, bool showNotificationWindow = false);
	// start with a type of message: "[system]", "[info]", "[warning]", "[error]", "[fatal]", "[notice]", "[log]"
	static bool IsUpdateAvailable;
	static bool DoLogging;
	static bool DrawHitbox;
private:
	static void SetMainWindowTitle(const char *text = 0);
	static void HandleNotification();
	static void ShowDebugWindow(bool* p_open);
	static void ShowCustomPalettesMenu();
	static void ShowNotificationWindow();
	static void ShowLogWindow(bool* p_open);
	static void ShowUpdateWindow();
	static void ShowPaletteEditorWindow(bool* p_open);
	static void ShowGameplaySettingsMenu();
	static ImGuiLog Log;
};