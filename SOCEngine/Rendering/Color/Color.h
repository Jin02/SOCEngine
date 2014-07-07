#pragma once

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
		float a;
		float b;
		float g;
		float r;

	public:
		Color();
		Color(float r, float g, float b);
		Color(float r, float g, float b, float a);
		Color(double r, double g, double b);
		Color(double r, double g, double b, double a);


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

	public:
		Color& GetColor();
		Math::Vector3 GetVector3();
		Math::Vector4 GetVector4();

	public:
		void SetColor(Color &color);

	private:
		void CalculateColor(Color *color);
	};

}