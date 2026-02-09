/*****************************************************************//**
 * \file   InputManager.cpp
 * \brief  インプットマネージャー実装
 * 
 * \author tsunn
 * \date   2026/02/08
 *********************************************************************/
#include "InputManager.h"

namespace Falu
{
	InputManager::InputManager()
		: m_previousMouseX(0)
		, m_previousMouseY(0)
	{
		m_mouseState.x = 0;
		m_mouseState.y = 0;
		m_mouseState.deltaX = 0;
		m_mouseState.deltaY = 0;
		m_mouseState.wheelDelta = 0;
	}

	InputManager::~InputManager()
	{
	}

	void InputManager::Update()
	{
		// 前フレームの状態を保存
		m_previousKeyStates = m_keyStates;
		m_previousMouseButtonStates = m_mouseButtonStates;

		// キーの状態の更新
		for (auto& pair : m_keyStates)
		{
			if (pair.second == KeyState::Pressed)
			{
				pair.second = KeyState::Held;
			}
			else if (pair.second == KeyState::Released)
			{
				pair.second = KeyState::None;
			}
		}

		// マウスボタン状態の更新
		for (auto& pair : m_mouseButtonStates)
		{
			if (pair.second == KeyState::Pressed)
			{
				pair.second = KeyState::Held;
			}
			else if(pair.second == KeyState::Released)
			{
				pair.second = KeyState::None;
			}
		}

		// マウスデルタの更新
		m_mouseState.deltaX = m_mouseState.x - m_previousMouseX;
		m_mouseState.deltaY = m_mouseState.y - m_previousMouseY;
		m_previousMouseX = m_mouseState.x;
		m_previousMouseY = m_mouseState.y;

		// マウスホイールのリセット
		m_mouseState.wheelDelta = 0;
	}

	void InputManager::ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		{
			int key = static_cast<int>(wParam);
			if (m_keyStates[key] != KeyState::Held)
			{
				m_keyStates[key] = KeyState::Pressed;
			}
		}
		break;

		case WM_KEYUP:
		case WM_SYSKEYUP:
		{
			int key = static_cast<int>(wParam);
			m_keyStates[key] = KeyState::Released;
		}
		break;

		case WM_LBUTTONDOWN:
			m_mouseButtonStates[static_cast<int>(MouseButton::Left)] = KeyState::Pressed;
			break;

		case WM_LBUTTONUP:
			m_mouseButtonStates[static_cast<int>(MouseButton::Left)] = KeyState::Released;
			break;

		case WM_RBUTTONDOWN:
			m_mouseButtonStates[static_cast<int>(MouseButton::Right)] = KeyState::Pressed;
			break;

		case WM_RBUTTONUP:
			m_mouseButtonStates[static_cast<int>(MouseButton::Right)] = KeyState::Released;
			break;
	
		case WM_MBUTTONDOWN:
			m_mouseButtonStates[static_cast<int>(MouseButton::Middle)] = KeyState::Pressed;
			break;

		case WM_MBUTTONUP:
			m_mouseButtonStates[static_cast<int>(MouseButton::Middle)] = KeyState::Released;
			break;

		case WM_MOUSEMOVE:
			m_mouseState.x = LOWORD(lParam);
			m_mouseState.y = HIWORD(lParam);
			break;

		case WM_MOUSEWHEEL:
			m_mouseState.wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
			break;
		}
	}

	bool InputManager::IsKeyPressed(KeyCode key) const
	{
		auto it = m_keyStates.find(static_cast<int>(key));
		if (it != m_keyStates.end())
		{
			return it->second == KeyState::Pressed;
		}
		return false;
	}

	bool InputManager::IsKeyHeld(KeyCode key) const
	{
		auto it = m_keyStates.find(static_cast<int>(key));
		if (it != m_keyStates.end())
		{
			return it->second == KeyState::Held || it->second == KeyState::Pressed;
		}
		return false;
	}

	bool InputManager::IsKeyReleased(KeyCode key) const
	{
		auto it = m_keyStates.find(static_cast<int>(key));
		if (it != m_keyStates.end())
		{
			return it->second == KeyState::Released;
		}
		return false;
	}

	bool InputManager::IsMouseButtonPressed(MouseButton button) const
	{
		auto it = m_mouseButtonStates.find(static_cast<int>(button));
		if (it != m_mouseButtonStates.end())
		{
			return it->second == KeyState::Pressed;
		}
		return false;
	}

	bool InputManager::IsMouseButtonHeld(MouseButton button) const
	{
		auto it = m_mouseButtonStates.find(static_cast<int>(button));
		if (it != m_mouseButtonStates.end())
		{
			return it->second == KeyState::Held ||it->second == KeyState::Pressed;
		}
		return false;
	}

	bool InputManager::IsMouseButtonReleased(MouseButton button) const
	{
		auto it = m_mouseButtonStates.find(static_cast<int>(button));
		if (it != m_mouseButtonStates.end())
		{
			return it->second == KeyState::Released;
		}
		return false;
	}

	void InputManager::ShowCursor(bool show)
	{
		::ShowCursor(show ? TRUE : FALSE);
	}

	void InputManager::SetCursorPosition(int x, int y)
	{
		POINT pt = { x,y };
		ClientToScreen(GetActiveWindow(),&pt);
		SetCursorPos(pt.x, pt.y);
	}

}
