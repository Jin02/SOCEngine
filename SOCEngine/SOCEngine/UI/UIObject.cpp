#include "UIObject.h"

using namespace UI;
using namespace Core;
using namespace Math;
using namespace Rendering::Buffer;

#define _child _vector

UIObject::UIObject(const std::string& name, Object* parent) 
	: Object(name, parent), _transformCB(nullptr), _depth(0)
{

}

UIObject::~UIObject()
{
	SAFE_DELETE(_transformCB);
}

void UIObject::InitConstBuffer()
{
	_transformCB = new ConstBuffer;
	_transformCB->Initialize(sizeof(Math::Matrix));
}

void UIObject::Add(const std::string& key, UIObject* object)
{
	Core::Object* obj = dynamic_cast<Object*>(object);
	Object::Add(key, obj);
}

UIObject* UIObject::Find(const std::string& key)
{
	return dynamic_cast<UIObject*>( *Object::Find(key) );
}

UIObject* UIObject::Get(uint index)
{
	return dynamic_cast<UIObject*>( Object::Get(index) );
}

void UIObject::Delete(const std::string& key, bool dealloc)
{
	if(dealloc)
	{
		Object* obj = Find(key);
		SAFE_DELETE(obj);
	}

	Object::Delete(key);
}

void UIObject::DeleteAll(bool dealloc)
{
	if(dealloc)
	{
		auto childs = GetVector();
		for(auto iter = childs.begin(); iter != childs.end(); ++iter)
			SAFE_DELETE(*iter);
	}

	Object::DeleteAll();
}

void UIObject::Update(float delta)
{
	if(_use == false)
		return;
	
	for(auto iter = _child.begin(); iter != _child.end(); ++iter)
		(*iter)->Update(delta);
}

void UIObject::UpdateTransform(ID3D11DeviceContext* context, const Matrix& viewProj)
{
	if(_use == false)
		return;

	Matrix world;
	_transform->FetchWorldMatrix(world);

	Matrix worldViewProj = world * viewProj;
	Matrix::Transpose(worldViewProj, worldViewProj);

	_transformCB->UpdateSubResource(context, &worldViewProj);

	for(auto iter = _child.begin(); iter != _child.end(); ++iter)
	{
		UIObject* uiObj = dynamic_cast<UIObject*>((*iter));
		uiObj->UpdateTransform(context, viewProj);
	}
}