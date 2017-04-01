#pragma once

#include "Common.h"
#include <array>
#include "Half.h"

struct Color
{
public:
	Color(float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f);
	Color(uint uintColor);

public:
	const Color operator -(const Color& c);
	bool operator !=(const Color& c);
	const Color operator *(const Color& c);
	const Color operator *(float f);
	const Color operator /(float f);
	const Color operator +(const Color& c);
	bool operator ==(const Color& c);

public:
	static const Color Black();
	static const Color Blue();
	static const Color Clear();
	static const Color Cyan();
	static const Color Gray();
	static const Color Green();
	static const Color Magenta();
	static const Color Red();
	static const Color White();
	static const Color Yellow();

	float Grayscale();

	static const Color Lerp(const Color& a, const Color& b, float t);
	void SetColor(const float* color);

	unsigned long Get32BitUintColor() const;
	static unsigned long Get32BitUintColor(float r, float g, float b, float a);

	const std::array<Half, 4> Get16BitFloat4Color() const;

	const Color Normalized();

private:
	void CalculateColor(Color *color);

public:
	union
	{
		struct
		{
			float r, g, b, a;
		};

		float color[4];
	};
};