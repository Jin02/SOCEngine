#pragma once

#define SET_ACCESSOR(name, type, variable)			inline void Set##name(type t)	{ variable = t; }
#define GET_CONST_ACCESSOR(name, type, variable)	inline type Get##name() const	{ return variable; }
#define GET_ACCESSOR(name, type, variable)			inline type Get##name() 		{ return variable; }
#define GET_SET_ACCESSOR(name, type, variable)\
	SET_ACCESSOR(name, type, variable) GET_CONST_ACCESSOR(name, type, variable)

#define SAFE_DELETE(x) { if(x){ delete x; x = nullptr; } }
#define SAFE_DELETE_ARRAY(x) { if(x){ delete[] x; x = nullptr; } }
#define SAFE_RELEASE(x) { if(x){ x->Release(); x = nullptr; } }
#define SAFE_DESTROY_DELTE(x) { if(x){ x->Destroy(); delete x; x = nullptr; } }

#define ENGINE_INDEX_TYPE unsigned int

#define DISALLOW_COPY_CONSTRUCTOR(CLASS) CLASS(const CLASS&) = delete
#define DISALLOW_ASSIGN(CLASS) CLASS& operator=(const CLASS&) = delete
#define DISALLOW_ASSIGN_COPY(CLASS) DISALLOW_COPY_CONSTRUCTOR(CLASS); DISALLOW_ASSIGN(CLASS);

#define MATH_PI 3.141592654f
#define RAD_TO_DEG(a) (180.0f / MATH_PI * a)
#define DEG_TO_RAD(a) (MATH_PI / 180.0f * a)
#define ASSERT_SUCCEEDED(x) assert(SUCCEEDED(x))

#define IN
#define OUT

typedef unsigned int		uint;
typedef unsigned short int	ushort;
typedef unsigned char		uchar;
