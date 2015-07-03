#include "UIObject.h"

using namespace UI;
using namespace Core;
using namespace Math;
using namespace Rendering::Buffer;

#define _child _vector

UIObject::UIObject(const std::string& name, const Object* parent) 
	: Object(name, parent), _transformCB(nullptr), _depth(0)
{

}

UIObject::~UIObject()
{

}

void UIObject::InitConstBuffer()
{
	_transformCB = new ConstBuffer;
	_transformCB->Initialize(sizeof(Math::Matrix));
}

void UIObject::Add(const std::string& key, UIObject* object, bool copy)
{
	Object::Add(key, object,copy);
}

UIObject* UIObject::Find(const std::string& key)
{
	return dynamic_cast<UIObject*>( Object::Find(key) );
}

UIObject* UIObject::Get(uint index)
{
	return dynamic_cast<UIObject*>( Object::Get(index) );
}

void UIObject::Delete(const std::string& key, bool contentRemove)
{
	Object::Delete(key, contentRemove);
}

void UIObject::DeleteAll(bool contentRemove)
{
	Object::DeleteAll(contentRemove);
}

void UIObject::Update(float delta)
{
	if(_use == false)
		return;
	
	for(auto iter = _child.begin(); iter != _child.end(); ++iter)
		GET_CONTENT_FROM_ITERATOR(iter)->Update(delta);
}

void UIObject::UpdateTransform(ID3D11DeviceContext* context, const Matrix& viewProj)
{
	if(_use == false)
		return;

	Matrix world;
	_transform->FetchWorldMatrix(world);

	Matrix worldViewProj = world * viewProj;
	Matrix::Transpose(worldViewProj, worldViewProj);

	_transformCB->Update(context, &worldViewProj);

	for(auto iter = _child.begin(); iter != _child.end(); ++iter)
	{
		UIObject* uiObj = dynamic_cast<UIObject*>(GET_CONTENT_FROM_ITERATOR(iter));
		uiObj->UpdateTransform(context, viewProj);
	}
}