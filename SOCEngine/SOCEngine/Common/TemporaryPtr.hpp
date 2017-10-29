#pragma once

template <typename Type>
class TemporaryPtr
{
public:
	explicit TemporaryPtr(Type* type) : _type(type) {	}

	const Type* operator->() const	{ return _type; }
	Type* operator->()				{ return _type; }

private:
	Type* _type;
};