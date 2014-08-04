#pragma once

#define SET_ACCESSOR(n, x, y)\
	inline void Set##n(x t)	{ y = t; }
#define GET_ACCESSOR(n, x, y)\
	inline x Get##n() const	{ return y; }
#define GET_SET_ACCESSOR(n, x, y)\
	SET_ACCESSOR(n, x, y) GET_ACCESSOR(n, x, y)

#define SAFE_DELETE(x) { if(x){ delete x; x = nullptr; } }
#define SAFE_DELETE_ARRAY(x) { if(x){ delete[] x; x = nullptr; } }
#define SAFE_RELEASE(x) { if(x){ x->Release(); x = nullptr; } }

#define ENGINE_INDEX_TYPE unsigned short