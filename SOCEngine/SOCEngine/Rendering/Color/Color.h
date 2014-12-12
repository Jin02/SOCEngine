#pragma once

#include "Common.h"

namespace Math
{
	class Vector3;
	class Vector4;
}

namespace Rendering
{
	struct Color
	{
	public:
		union
		{
			struct
			{
				float        r,g,b;
			};

			float color[3];
		};

	public:
		Color();
		Color(float r, float g, float b);

	public:
		Color operator -(Color c);
		bool operator !=(Color c);
		Color operator *(Color c);
		Color operator *(float f);
		Color operator /(float f);
		Color operator +(Color c);
		bool operator ==(Color c);

	public:
		static Color black();
		static Color blue();
		static Color clear();
		static Color cyan();

		static Color gray();

		float grayscale();

		static Color green();		
		static Color magenta();		
		static Color red();		
		static Color white();		
		static Color yellow();

		static Color Lerp(Color a, Color b, float t);
		void SetColor(const float* color);

	private:
		void CalculateColor(Color *color);
	};

}