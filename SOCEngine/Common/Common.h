#pragma once

#define SET_ACCESSOR(n, x, y)\
	inline void Set##n(x t)	{ y = t; }
#define GET_ACCESSOR(n, x, y)\
	inline x Get##n() const	{ return y; }
#define GET_SET_ACCESSOR(n, x, y)\
	SET_ACCESSOR(n, x, y) GET_ACCESSOR(n, x, y)

#define SETP_ACCESSOR(n, x, y)\
	inline void Set##n(x* t) { y = *t; }
#define GETP_ACCESSOR(n, x, y)\
	inline x* Get##n() { return &y; }
#define GETP_SETP_ACCESSOR(n, x, y)\
	SETP_ACCESSOR(n, x, y) GETP_ACCESSOR(n, x, y)

#define SAFE_DELETE(x) { if(x){ delete x; x = nullptr; } }
#define SAFE_DELETE_ARRAY(x) { if(x){ delete[] x; x = nullptr; } }
#define SAFE_RELEASE(x) { if(x){ x->Release(); x = nullptr; } }

#define MIN(A, B) A < B ? A : B
#define MAX(A, B) A > B ? A : B