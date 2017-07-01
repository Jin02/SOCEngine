#include "Object.h"
#include "Transform.h"
#include <assert.h>

using namespace Core;

Object::Object(ObjectId id, ComponentSystem* compoSystem, TransformPool* tfPool)
	: _id(id), _tfPool(tfPool), _compoSystem(compoSystem)
{
	_tfPool->Add(id);
}

Object::~Object()
{
	_tfPool->Delete(_id.Literal());
}

void Object::AddChild(Object& child)
{
	ObjectId childId = child.GetObjectId();
	auto foundtf = _tfPool->Find(childId.Literal());
	assert(foundtf);

	_tfPool->Find(_id.Literal())->AddChild(*foundtf);
}

bool Object::HasChild(const Object& child) const
{
	auto childId = child.GetObjectId();
	return _tfPool->Find(_id.Literal())->HasChild(childId);
}

void Object::DeleteChild(Object& child)
{
	_tfPool->Find(_id.Literal())->DeleteChild(child.GetObjectId());
}

auto Object::FindChildUsingIndex(uint index)
{
	auto thisTF = _tfPool->Find(_id.Literal());

	ObjectId id = thisTF->GetChild(index);
	return _tfPool->Find(id.Literal());
}

uint Object::GetChildCount() const
{
	return _tfPool->Find(_id.Literal())->GetChildCount();
}
