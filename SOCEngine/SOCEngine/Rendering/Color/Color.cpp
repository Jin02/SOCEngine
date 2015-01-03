#include "Color.h"

#include "Vector3.h"
#include "Vector4.h"

#include "Utility.h"

namespace Rendering
{
	Color::Color() : r(1.0f), g(1.0f), b(1.0f), a(1.0f)
	{
	}

	Color::Color(float _r, float _g, float _b, float _a) :
		r(_r), g(_g), b(_b), a(_a)
	{
	}

	Color Color::operator -(Color c)
	{
		Color color = (*this);

		color.r -= c.r;
		color.g -= c.g;
		color.b -= c.b;
		color.a -= c.a;

		CalculateColor(&color);
		return color;
	}

	bool Color::operator !=(Color c)
	{
		return (r != c.r) || (g != c.g) || (b != c.b) || (a != c.a);
	}

	Color Color::operator *(Color c)
	{
		Color color = Color( r * c.r, g * c.g, b * c.b, a * c.a);
		CalculateColor(&color);
		return color;
	}

	Color Color::operator *(float f)
	{
		return Color(r * f, g * f, b * f, a * f);
	}

	Color Color::operator /(float f)
	{
		return Color(r / f, g / f, b / f, a / f);
	}

	Color Color::operator +(Color c)
	{
		Color color = (*this);

		color.r += c.r;
		color.g += c.g;
		color.b += c.b;
		color.a += c.a;

		CalculateColor(&color);
		return color;
	}

	bool Color::operator ==(Color c)
	{
		return (r == c.r) && (g == c.g) && (b == c.b) && (a == c.a);
	}

	void Color::CalculateColor(Color *color)
	{
		auto Work=[](float& color)
		{
			if(color > 1.0f)
				color = 1.0f;
			else if(color < 0.0f)
				color = 0.0f;
		};
		Work(color->r);
		Work(color->g);
		Work(color->b);
		Work(color->a);
	}

	Color Color::black()
	{
		return Color(0.0f, 0.0f, 0.0f, 1.0f);
	}

	Color Color::blue()
	{
		return Color(0.0f, 0.0f, 1.0f, 1.0f);
	}

	Color Color::clear()
	{
		return Color(0.0f, 0.0f, 0.0f, 1.0f);
	}

	Color Color::cyan()
	{
		return Color(0, 100.0f/255.0f, 100.0f/255.0f, 1.0f);
	}

	Color Color::gray()
	{
		return Color(0.5f, 0.5f, 0.5f, 1.0f);
	}

	float Color::grayscale()
	{
		return (this->r * 0.3f) + (this->g * 0.59f) + (this->b * 0.11f);
	}

	Color Color::green()
	{
		return Color(0.0f, 1.0f, 0.0f, 1.0f);
	}

	Color Color::magenta()
	{
		return Color(1.0f, 0.0f, 1.0f, 1.0f);
	}

	Color Color::red()
	{
		return Color(1.0f, 0.0f, 0.0f, 1.0f);
	}

	Color Color::white()
	{
		return Color(1.0f, 1.0f, 1.0f, 1.0f);
	}

	Color Color::yellow()
	{
		return Color(1.0f, 1.0f, 0.0f, 1.0f);
	}

	Color Color::Lerp(Color a, Color b, float t)
	{
#define LERP(a, b, t)\
	a - (a * t) + (b * t)

		float _r = LERP(a.r, b.r, t);
		float _g = LERP(a.g, b.g, t);
		float _b = LERP(a.b, b.b, t);		
		float _a = LERP(a.a, b.a, t);		

		return Color(_r, _g, _b, _a);
	}

	void Color::SetColor(const float* color)
	{
		r = color[0];
		g = color[1];
		b = color[2];
		a = color[3];
	}

	unsigned long Color::Get32BitUintColor()
	{
		unsigned long _r = static_cast<unsigned long>(r * 255.0f + 0.5);
		unsigned long _g = static_cast<unsigned long>(g * 255.0f + 0.5);
		unsigned long _b = static_cast<unsigned long>(b * 255.0f + 0.5);
		unsigned long _a = static_cast<unsigned long>(a * 255.0f + 0.5);

		return (_a << 24) | (_b << 16) | (_a << 8) | _r;
	}

	void Color::Get16BitFloat4Color(std::array<unsigned short, 4>& outArray)
	{
		outArray[0] = Utility::Common::ConvertF32ToF16(r);
		outArray[1] = Utility::Common::ConvertF32ToF16(g);
		outArray[2] = Utility::Common::ConvertF32ToF16(b);
		outArray[3] = Utility::Common::ConvertF32ToF16(a);
	}
}