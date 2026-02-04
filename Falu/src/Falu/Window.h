/*****************************************************************//**
 * \file   Window.h
 * \brief  
 * 
 * \author tsunn
 * \date   2026/02/04
 *********************************************************************/
#pragma once

#include <Windows.h>
#include <string>

namespace Falu
{
	class Window
	{
	public:
		Window();
		~Window();

		bool Create(HINSTANCE hinstance, const std::wstring& title, int width, int height);
		void Destroy();
		bool ProcessMessage();

		HWND GetHandle() const { return m_hWnd; }
		int GetWidth() const { return m_width; }
		int GetHeight() const { return m_height;}
		float GetAspectRatio() const { return (float)m_width / (float)m_height; }

		void SetTitle(const std::wstring& title);
		void Resize(int width, int height);

	private:
		static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		LRESULT HandleMessage(UINT message, WPARAM wParam, LPARAM lParam);

	private:
		HWND m_hWnd;
		HINSTANCE m_hInstance;
		std::wstring m_title;
		int m_width;
		int m_height;
		bool m_isResizing;

	};
}
