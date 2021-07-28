#pragma once
#include "Logger/Logger.h"
#include "WindowContainer/WindowContainer.h"

#include <d3d9.h>

class WindowManager
{
public:
	static WindowManager& GetInstance();
	WindowContainer* GetWindowContainer() const { return m_windowContainer; }
	bool Initialize(void *hwnd, IDirect3DDevice9 *device);
	void Shutdown();
	void Render();
	void InvalidateDeviceObjects();
	void CreateDeviceObjects();
	bool IsInitialized() const { return m_initialized; }

private:
	WindowManager() = default;
	void HandleButtons();
	void DrawAllWindows() const;

	static WindowManager* m_instance;
	bool m_initialized = false;
	WindowContainer* m_windowContainer = nullptr;
	Logger* m_pLogger = nullptr;
};
