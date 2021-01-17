#pragma once
#include "WindowType.h"
#include "Overlay/Window/IWindow.h"

#include <map>

typedef std::map<WindowType_, IWindow*> WindowMap;

class WindowContainer
{
public:
	WindowContainer();
	~WindowContainer()
	{
		for (const auto& window : m_windows)
		{
			delete window.second;
		}
	}

	template <class T>
	T* GetWindow(WindowType_ type) { return (T*)m_windows[type]; }
	IWindow* GetWindow(WindowType_ type) { return m_windows[type]; }
	const WindowMap& GetWindows() const { return m_windows; }
private:
	void AddWindow(WindowType_ type, IWindow* pWindow) { m_windows[type] = pWindow; }

	WindowMap m_windows;
};
