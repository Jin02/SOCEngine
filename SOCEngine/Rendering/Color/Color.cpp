#include "Color.h"

#include "Vector3.h"
#include "Vector4.h"

namespace Rendering
{
	Color::Color()
	{
		r = g = b = a = 1.0f;
	}

	Color::Color(float r, float g, float b)
	{
		this->r = r;
		this->g = g;
		this->b = b;
	}

	Color::Color(float r, float g, float b, float a)
	{
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}

	Color::Color(double r, double g, double b)
	{
		this->r = (float)r;
		this->g = (float)g;
		this->b = (float)b;
		this->a = 1.0f;
	}

	Color::Color(double r, double g, double b, double a)
	{
		this->r = (float)r;
		this->g = (float)g;
		this->b = (float)b;
		this->a = (float)a;
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
		Color color = Color( r * c.r, g * c.g, b * c.b, a * c.a );
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
		if(color->r > 1.0f) color->r = 1.0f;
		else if(color->r < 0.0f) color->r = 0.0f;

		if(color->g > 1.0f) color->g = 1.0f;
		else if(color->g < 0.0f) color->g = 0.0f;

		if(color->b > 1.0f) color->b = 1.0f;
		else if(color->b < 0.0f) color->b = 0.0f;

		if(color->a > 1.0f) color->a = 1.0f;
		else if(color->a < 0.0f) color->a = 0.0f;
	}

	Color Color::black()
	{
		return Color(0.0f, 0.0f, 0.0f, 0.0f);
	}

	Color Color::blue()
	{
		return Color(0.0f, 0.0f, 1.0f, 1.0f);
	}

	Color Color::clear()
	{
		return Color(0.0f, 0.0f, 0.0f, 0.0f);
	}

	Color Color::cyan()
	{
		return Color(0, 100.0f/255.0f, 100.0f/255.0f);
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
		return Color(0.0f, 1.0f, 0.0f);
	}

	Color Color::magenta()
	{
		return Color(1.0f, 0.0f, 1.0f);
	}

	Color Color::red()
	{
		return Color(1.0f, 0.0f, 0.0f);
	}

	Color Color::white()
	{
		return Color(1.0f, 1.0f, 1.0f);
	}

	Color Color::yellow()
	{
		return Color(1.0f, 1.0f, 0.0f);
	}

	Color Color::Lerp(Color a, Color b, float t)
	{
#define LERP(a, b, t)\
	a - (a * t) + (b * t)

		float _a = LERP(a.a, b.a, t);
		float _r = LERP(a.r, b.r, t);
		float _g = LERP(a.g, b.g, t);
		float _b = LERP(a.b, b.b, t);		

		return Color(_r,_g,_b,_a);
	}
}