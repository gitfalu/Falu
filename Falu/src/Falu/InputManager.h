/*****************************************************************//**
 * \file   InputManager.h
 * \brief  インプット
 * 
 * \author tsunn
 * \date   2026/02/04
 *********************************************************************/
#pragma once

#include <Windows.h>
#include <unordered_map>

namespace Falu
{
	enum class KeyCode
	{
		A = 'A', B = 'B', C = 'C', D = 'D', E = 'E', F = 'F', G = 'G',
		H = 'H', I = 'I', J = 'J', K = 'K', L = 'L', M = 'M', N = 'N',
		O = 'O', P = 'P', Q = 'Q', R = 'R', S = 'S', T = 'T', U = 'U',
		V = 'V', W = 'W', X = 'X', Y = 'Y', Z = 'Z',

		Num0 = '0', Num1 = '1',Num2 = '2', Num3 = '3', Num4 = '4',
		Num5 = '5', Num6 = '6', Num7 = '7',Num8 = '8', Num9 = '9',

		Space = VK_SPACE,
		Enter = VK_RETURN,
		Escape = VK_ESCAPE,
		Tab = VK_TAB,
		Shift = VK_SHIFT,
		Control = VK_CONTROL,
		Alt = VK_MENU,

		Left = VK_LEFT,
		Right = VK_RIGHT,
		Up = VK_UP,
		Down = VK_DOWN,

		F1 = VK_F1, F2 = VK_F2, F3 = VK_F3, F4 = VK_F4, F5 = VK_F5,
		F6 = VK_F6, F7 = VK_F7, F8 = VK_F8, F9 = VK_F9, F10 = VK_F10,
		F11 = VK_F11, F12 = VK_F12
	};

	enum class MouseButton
	{
		Left = 0,
		Right,
		Middle,
	};

	struct MouseState
	{
		int x;
		int y;
		int deltaX;
		int deltaY;
		int wheelDelta;
	};

	class InputManager
	{
	public:
		InputManager();
		~InputManager();

		void Update();
		void ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam);

		//=== Keyboard ===
		bool IsKeyPressed(KeyCode key)const;
		bool IsKeyHeld(KeyCode key)const;
		bool IsKeyReleased(KeyCode key)const;

		//=== Mouse ===
		bool IsMouseButtonPressed(MouseButton button)const;
		bool IsMouseButtonHold(MouseButton button)const;
		bool IsMouseButtonReleased(MouseButton button)const;

		const MouseState& GetMouseState() const { return m_mouseState; }
		int GetMouseX() const { return m_mouseState.x; }
		int GetMouseY() const { return m_mouseState.y; }
		int GetMouseDeltaX() const { return m_mouseState.deltaX; }
		int GetMouseDeltaY() const { return m_mouseState.deltaY; }
		int GetMouseWheelDelta() const { return m_mouseState.wheelDelta; }

		void ShowCursor(bool show);
		void SetCursorPosition(int x, int y);

	private:
		enum class KeyState
		{
			None,
			Pressed,
			Held,
			Released
		};
	private:
		std::unordered_map<int, KeyState> m_keyStates;
		std::unordered_map<int, KeyState> m_previousKeyStates;

		std::unordered_map<int, KeyState> m_mouseButtonStates;
		std::unordered_map<int, KeyState> m_previousMouseButtonStates;

		MouseState m_mouseState;
		int m_previousMouseX;
		int m_previousMouseY;
	};
}
