/*****************************************************************//**
 * \file   Window.cpp
 * \brief  ウィンドウ処理
 * 
 * \author tsunn
 * \date   2026/02/06
 *********************************************************************/
#include "Window.h"

// ImGui include
#include "imgui.h"
#include "imgui_impl_win32.h"

#include "Engine.h"
#include "InputManager.h"
#include "../Renderer/Renderer.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Falu
{
	Window::Window()
		:m_hWnd(nullptr),m_hInstance(nullptr),m_width(0),m_height(0),m_isResizing(false)
	{

	}
	Window::~Window()
	{
		Destroy();
	}
	bool Window::Create(HINSTANCE hInstance, const std::wstring& title, int width, int height)
	{
		m_hInstance = hInstance;
		m_title = title;
		m_width = width;
		m_height = height;

		// ウィンドウクラスの登録
		WNDCLASSEXW wc = {};
		wc.cbSize = sizeof(WNDCLASSEXW);
		wc.style = CS_HREDRAW | CS_VREDRAW; // 全体再描画
		wc.lpfnWndProc = WindowProc;
		wc.hInstance = hInstance;
		wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wc.lpszClassName = L"FaluEngineWindowClass";

		if (!RegisterClassExW(&wc))
		{
			return false;
		}

		// ウィンドウサイズの調整
		RECT rect = { 0,0,width,height };
		AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW,FALSE);

		//ウィンドウの作成
		m_hWnd = CreateWindowExW(
			0,
			L"FaluEngineWindowClass",
			title.c_str(),
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT,
			rect.right - rect.left,
			rect.bottom - rect.top,
			nullptr,
			nullptr,
			hInstance,
			this
		);

		if (!m_hWnd)
		{
			return false;
		}

		ShowWindow(m_hWnd, SW_SHOW);
		UpdateWindow(m_hWnd);

		return true;
	}
	void Window::Destroy()
	{
		if (m_hWnd)
		{
			DestroyWindow(m_hWnd);
			m_hWnd = nullptr;
		}
	}
	bool Window::ProcessMessage()
	{
		MSG msg = {};
		while (PeekMessage(&msg,nullptr,0,0,PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				return false;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		return true;
	}
	void Window::SetTitle(const std::wstring& title)
	{
		m_title = title;
		if (m_hWnd)
		{
			SetWindowTextW(m_hWnd,title.c_str());
		}
	}
	void Window::Resize(int width, int height)
	{
		m_width = width;
		m_height = height;

		if (Engine::GetInstance().GetRenderer())
		{
			Engine::GetInstance().GetRenderer()->OnResize(width,height);
		}
	}
	
	LRESULT Window::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		Window* window = nullptr;

		if (message == WM_NCCREATE)
		{
			CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
			// reinterpret_cast<>() : ほかのポインタに変換されることを許可する
			window = reinterpret_cast<Window*>(pCreate->lpCreateParams);
			SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
		}
		else
		{
			window = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
		}

		if (window)
		{
			return window->HandleMessage(hWnd,message, wParam, lParam);
		}

		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	LRESULT Window::HandleMessage(HWND hWnd,UINT message, WPARAM wParam, LPARAM lParam)
	{
		// ImGuiにメッセージを送る
		if (ImGui_ImplWin32_WndProcHandler(m_hWnd, message, wParam, lParam))
			return true;

		// 入力管理にメッセージを渡す
		if (Engine::GetInstance().GetInputManager())
		{
			Engine::GetInstance().GetInputManager()->ProcessMessage(message,wParam,lParam);
		}

		// 入力管理にメッセージを飛ばす
		if (Engine::GetInstance().GetInputManager())
		{
			Engine::GetInstance().GetInputManager()->ProcessMessage(message, wParam, lParam);
		}

		switch (message)
		{
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

		case WM_SIZE:
			{
				int width = LOWORD(lParam);
				int height = HIWORD(lParam);

				if (width != m_width || height != m_height)
				{
					Resize(width, height);
				}
			}
			return 0;

		case WM_KEYDOWN:
			if (wParam == VK_ESCAPE)
			{
				Engine::GetInstance().Quit();
			}
			return 0;

		case WM_ENTERSIZEMOVE:
			m_isResizing = true;
			return 0;

		case WM_EXITSIZEMOVE:
			m_isResizing = false;
			return 0;
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}
