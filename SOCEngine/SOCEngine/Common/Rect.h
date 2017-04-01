#pragma once

template <typename Type>
struct Size
{
public:
	Size(Type _w, Type _h) : w(_w), h(_h) {}

public:
	Type w, h;
};

template <typename Type>
struct Rect
{
public:
	Rect() : x(0), y(0), size(0, 0) { }
	Rect(Type _x, Type _y, Type _w, Type _h) : x(_x), y(_y), size(_w, _h) { }

public:
	Type x, y;
	Size<Type> size;
};