#pragma once

template <typename Type>
class TemporaryPtr
{
public:
	TemporaryPtr() = default;
	explicit TemporaryPtr(Type* type) : _type(type) {	}

	const Type* operator->() const	{ return _type; }
	Type* operator->()				{ return _type; }

	bool IsNull() const { return _type != nullptr; }

private:
	Type* _type = nullptr;
};