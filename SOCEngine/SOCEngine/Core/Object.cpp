#include "Object.h"
#include "Transform.h"
#include <assert.h>
#include "ObjectManager.h"

using namespace Core;

void Object::Initialize(ObjectID id)
{
	_id = id;
	CoreConnector::SharedInstance()->GetTransformPool()->Add(id);
}

void Object::Destroy()
{
	CoreConnector::SharedInstance()->GetTransformPool()->Delete(_id.Literal());
}

void Object::AddChild(Object& child)
{
	ObjectID childID = child.GetObjectID();
	auto foundtf = CoreConnector::SharedInstance()->GetTransformPool()->Find(childID.Literal());
	assert(foundtf);

	CoreConnector::SharedInstance()->GetTransformPool()->Find(_id.Literal())->AddChild(*foundtf);
}

bool Object::HasChild(const Object& child) const
{
	auto childID = child.GetObjectID();
	return CoreConnector::SharedInstance()->GetTransformPool()->Find(_id.Literal())->HasChild(childID);
}

void Object::DeleteChild(Object& child)
{
	CoreConnector::SharedInstance()->GetTransformPool()->Find(_id.Literal())->DeleteChild(child.GetObjectID());
}

auto Object::FindChildUsingIndex(uint index)
{
	auto thisTF = CoreConnector::SharedInstance()->GetTransformPool()->Find(_id.Literal());

	ObjectID id = thisTF->GetChild(index);
	return CoreConnector::SharedInstance()->GetTransformPool()->Find(id.Literal());
}

uint Object::GetChildCount() const
{
	return CoreConnector::SharedInstance()->GetTransformPool()->Find(_id.Literal())->GetChildCount();
}

Transform& Object::FetchTransform()
{
	Transform* transform = CoreConnector::SharedInstance()->GetTransformPool()->Find(_id.Literal());
	assert(transform);

	return *transform;
}