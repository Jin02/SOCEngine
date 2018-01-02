#include "Color.h"

Color::Color(float _r, float _g, float _b, float _a) :
	r(_r), g(_g), b(_b), a(_a)
{
}

Color::Color(uint uintColor)
{
	a = static_cast<float>( (uintColor & 0b11111111000000000000000000000000) >> 24 ) / 255.0f;
	b = static_cast<float>( (uintColor & 0b00000000111111110000000000000000) >> 16 ) / 255.0f;
	g = static_cast<float>( (uintColor & 0b00000000000000001111111100000000) >>  8 ) / 255.0f;
	r = static_cast<float>( (uintColor & 0b00000000000000000000000011111111) >>  0 ) / 255.0f;
}

const Color Color::operator -(const Color& c)
{
	Color color = (*this);

	color.r -= c.r;
	color.g -= c.g;
	color.b -= c.b;
	color.a -= c.a;

	CalculateColor(&color);
	return color;
}

bool Color::operator !=(const Color& c)
{
	return (r != c.r) | (g != c.g) | (b != c.b) | (a != c.a);
}

const Color Color::operator *(const Color& c)
{
	Color color = Color(r * c.r, g * c.g, b * c.b, a * c.a);
	CalculateColor(&color);
	return color;
}

const Color Color::operator *(float f)
{
	return Color(r * f, g * f, b * f, a * f);
}

const Color Color::operator /(float f)
{
	return Color(r / f, g / f, b / f, a / f);
}

const Color Color::operator +(const Color& c)
{
	Color color = (*this);

	color.r += c.r;
	color.g += c.g;
	color.b += c.b;
	color.a += c.a;

	return color;
}

bool Color::operator ==(const Color& c)
{
	return (r == c.r) & (g == c.g) & (b == c.b) & (a == c.a);
}

void Color::CalculateColor(Color *color)
{
	auto Work = [](float& color)
	{
		if (color > 1.0f)
			color = 1.0f;
		else if (color < 0.0f)
			color = 0.0f;
	};
	Work(color->r);
	Work(color->g);
	Work(color->b);
	Work(color->a);
}

const Color Color::Black()
{
	return Color(0.0f, 0.0f, 0.0f, 1.0f);
}

const Color Color::Blue()
{
	return Color(0.0f, 0.0f, 1.0f, 1.0f);
}

const Color Color::Clear()
{
	return Color(0.0f, 0.0f, 0.0f, 0.0f);
}

const Color Color::Cyan()
{
	return Color(0, 100.0f / 255.0f, 100.0f / 255.0f, 1.0f);
}

const Color Color::Gray()
{
	return Color(0.5f, 0.5f, 0.5f, 1.0f);
}

float Color::Grayscale()
{
	return (this->r * 0.3f) + (this->g * 0.59f) + (this->b * 0.11f);
}

const Color Color::Green()
{
	return Color(0.0f, 1.0f, 0.0f, 1.0f);
}

const Color Color::Magenta()
{
	return Color(1.0f, 0.0f, 1.0f, 1.0f);
}

const Color Color::Red()
{
	return Color(1.0f, 0.0f, 0.0f, 1.0f);
}

const Color Color::White()
{
	return Color(1.0f, 1.0f, 1.0f, 1.0f);
}

const Color Color::Yellow()
{
	return Color(1.0f, 1.0f, 0.0f, 1.0f);
}

const Color Color::Lerp(const Color& a, const Color& b, float t)
{
#define LERP(a, b, t)\
	a - (a * t) + (b * t)

	float _r = LERP(a.r, b.r, t);
	float _g = LERP(a.g, b.g, t);
	float _b = LERP(a.b, b.b, t);
	float _a = LERP(a.a, b.a, t);
#undef LERP

	return Color(_r, _g, _b, _a);
}

void Color::SetColor(const float* color)
{
	r = color[0];
	g = color[1];
	b = color[2];
	a = color[3];
}

unsigned long Color::Get32BitUintColor() const
{
	unsigned long _r = static_cast<unsigned long>(r * 255.0f + 0.5);
	unsigned long _g = static_cast<unsigned long>(g * 255.0f + 0.5);
	unsigned long _b = static_cast<unsigned long>(b * 255.0f + 0.5);
	unsigned long _a = static_cast<unsigned long>(a * 255.0f + 0.5);

	return (_a << 24) | (_b << 16) | (_g << 8) | _r;
}

unsigned long Color::Get32BitUintColor(float r, float g, float b, float a)
{
	Color color(r, g, b, a);
	return color.Get32BitUintColor();
}

const std::array<Half, 4> Color::Get16BitFloat4Color() const
{
	return{ Half(r), Half(g), Half(b), Half(a) };
}

const Color Color::Normalized()
{
	Color thisColor = (*this);
	CalculateColor(&thisColor);

	return thisColor;
}
