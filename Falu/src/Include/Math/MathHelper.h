/*****************************************************************//**
 * \file   MethHelper.h
 * \brief  êîäwÉwÉãÉpÅ[
 * 
 * \author tsunn
 * \date   February 2026
 *********************************************************************/
#pragma once

#include <DirectXMath.h>

namespace Falu
{
	namespace Math
	{
		using namespace DirectX;

		//==== ã§í ÇÃíËêî ====
		constexpr float PI = 3.14159265358979323846f;
		constexpr float TWO_PI = 6.28318530717958647692f;
		constexpr float PI_DEV_2 = 1.57079632679489661923f;
		constexpr float PI_DEV_4 = 0.78539816339744830962f;

		inline float ToRadians(float degrees)
		{
			return degrees * (PI / 180.0f);
		}

		inline float ToDegrees(float radians)
		{
			return radians * (180.0f / PI);
		}

		inline float Lerp(float a, float b, float t)
		{
			return a + (b - a) * t;
		}

		inline XMVECTOR LerpVector(XMVECTOR a, XMVECTOR b, float t)
		{
			return XMVectorLerp(a, b, t);
		}

		inline float Clamp(float value, float min, float max)
		{
			if (value < min)return min;
			if (value > max) return max;
			return value;
		}

		struct Vector3
		{
			float x, y, z;

			Vector3() :x(0.0f), y(0.0f), z(0.0f) {}
			Vector3(float x,float y,float z) :x(x), y(y), z(z) {}

			XMFLOAT3 ToXMFLOAT3() const { return XMFLOAT3(x, y, z); }
			XMVECTOR ToXMVECTOR() const { 
				XMFLOAT3 stack = ToXMFLOAT3();
				return XMLoadFloat3(&stack); 
			}

			static Vector3 Zero()		{ return Vector3(0.0f, 0.0f, 0.0f); }
			static Vector3 One()		{ return Vector3(1.0f, 1.0f, 1.0f); }
			static Vector3 Up()			{ return Vector3(0.0f,1.0f, 0.0f); }
			static Vector3 Forward()	{ return Vector3(0.0f, 0.0f, 1.0f); }
			static Vector3 Right()		{ return Vector3(1.0f, 0.0f, 0.0f); }
		};

		struct Vector4
		{
			float x, y, z,w;

			Vector4() :x(0.0f), y(0.0f), z(0.0f) {}
			Vector4(float x, float y, float z,float w) :x(x), y(y), z(z),w(w) {}

			XMFLOAT4 ToXMFLOAT4() const { return XMFLOAT4(x, y, z,w); }
			XMVECTOR ToXMVECTOR() const { 
				XMFLOAT4 stack = ToXMFLOAT4();
				return XMLoadFloat4(&stack); }
		};


		struct Color
		{
			float r, g, b, a;

			Color() :r(1.0f), g(1.0f), b(1.0f), a(1.0f) {}
			Color(float r, float g, float b, float a) :r(r), g(g), b(b), a(a) {}

			XMFLOAT4 ToFloat4() const { return XMFLOAT4(r, g, b, a); }

			static Color White()	{ return Color(1.0f, 1.0f, 1.0f, 1.0f); }
			static Color Black()	{ return Color(0.0f, 0.0f, 0.0f, 1.0f); }
			static Color Red()		{ return Color(1.0f, 0.0f, 0.0f, 1.0f); }
			static Color Green()	{ return Color(0.0f, 1.0f, 0.0f, 1.0f); }
			static Color Blue()		{ return Color(0.0f, 0.0f, 1.0f, 1.0f); }
		};
	}
}
