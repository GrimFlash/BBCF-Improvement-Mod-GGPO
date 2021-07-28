#include "IWindow.h"

void IWindow::Update()
{
	if (!m_windowOpen)
	{
		return;
	}

	BeforeDraw();

	ImGui::Begin(m_windowTitle.c_str(), GetWindowOpenPointer(), m_windowFlags);
	Draw();
	ImGui::End();

	AfterDraw();
}

void IWindow::Open()
{
	m_windowOpen = true;
}

void IWindow::Close()
{
	m_windowOpen = false;
}

void IWindow::ToggleOpen()
{
	m_windowOpen = !m_windowOpen;
}

bool IWindow::IsOpen() const
{
	return m_windowOpen;
}

void IWindow::SetWindowFlag(ImGuiWindowFlags flag)
{
	m_windowFlags |= flag;
}

void IWindow::ClearWindowFlag(ImGuiWindowFlags flag)
{
	m_windowFlags &= ~flag;
}

bool * IWindow::GetWindowOpenPointer()
{
	if (m_windowClosable)
	{
		return &m_windowOpen;
	}

	return nullptr;
}
