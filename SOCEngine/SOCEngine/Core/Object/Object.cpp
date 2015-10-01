#include "Object.h"
#include "Math.h"

using namespace std;
using namespace Intersection;
using namespace Rendering;
using namespace Math;
using namespace Rendering::Light;
using namespace Core; 

#define _child _vector

Object::Object(const std::string& name, Object* parent /* = NULL */) :
	_culled(false), _parent(parent), _use(true), _hasMesh(false), _name(name)
{
	_transform = new Transform( this );		
	_root = parent ? parent->_root : this;

	if(parent)
		parent->AddChild(this);
}

Object::~Object(void)
{
	SAFE_DELETE(_transform);

	DeleteAllChild();
	DeleteAllComponent();
}

void Object::DeleteAllChild()
{
	for(auto iter = _vector.begin(); iter != _vector.end(); ++iter)
		SAFE_DELETE((*iter));

	DeleteAll();
}

void Object::AddChild(Object *child)
{
	ASSERT_COND_MSG( (child->_parent == nullptr) || (child->_parent == this), 
		"Error, Invalid parent");

	if(FindChild(child->GetName()) == nullptr)
	{
		Add(child->_name, child);
		child->_parent = this;
	}
	else
	{
		ASSERT_MSG("Duplicated child. plz check your code.");
	}
}

Object* Object::FindChild(const std::string& key)
{
	Object** ret = Find(key, nullptr);
	return ret ? (*ret) : nullptr;
}

bool Object::CompareIsChildOfParent(Object *parent)
{
	return (parent == _parent); 
}

bool Object::Culling(const Camera::Frustum *frustum)
{
	Vector3 wp;
	_transform->FetchWorldPosition(wp);
	_culled = frustum->In(wp, _transform->GetRadius());

	if(_culled == false)
	{
		for(auto iter = _child.begin(); iter != _child.end(); ++iter)
			(*iter)->Culling(frustum);
	}

	return _culled;
}

void Object::Update(float delta)
{
	if(_use == false)
		return;

	for(auto iter = _components.begin(); iter != _components.end(); ++iter)
		(*iter)->OnUpdate(delta);

	for(auto iter = _child.begin(); iter != _child.end(); ++iter)
		(*iter)->Update(delta);
}

void Object::UpdateTransformCB(const Math::Matrix& viewMat, const Math::Matrix& projMat)
{
	if(_use == false)
		return;

	Math::Matrix worldMat;
	_transform->FetchWorldMatrix(worldMat);

	TransformPipelineShaderInput transposeTransform;
	{
		Matrix::Transpose(transposeTransform.worldMat, worldMat);

		Matrix worldView = worldMat * viewMat;
		Matrix::Transpose(transposeTransform.worldViewMat, worldView);

		Matrix worldViewProj = worldView * projMat;
		Matrix::Transpose(transposeTransform.worldViewProjMat, worldViewProj);
	}

	for(auto iter = _components.begin(); iter != _components.end(); ++iter)
	{
		(*iter)->OnUpdateTransformCB(transposeTransform);
		(*iter)->OnRenderPreview();
	}

	for(auto iter = _child.begin(); iter != _child.end(); ++iter)
		(*iter)->UpdateTransformCB(viewMat, projMat);
}

bool Object::Intersects(Intersection::Sphere &sphere)
{
	Vector3 wp;
	_transform->FetchWorldPosition(wp);
	return sphere.Intersects(wp, _transform->GetRadius());
}

void Object::DeleteComponent(Component *component)
{
	for(auto iter = _components.begin(); iter != _components.end(); ++iter)
	{
		if((*iter) == component)
		{
			(*iter)->OnDestroy();
			_components.erase(iter);
			delete (*iter);
			return;
		}
	}

}

void Object::DeleteAllComponent()
{
	for(auto iter = _components.begin(); iter != _components.end(); ++iter)
		delete (*iter);

	_components.clear();
}

Object* Object::Clone() const
{
	Object* newObject = new Object(_name + "-Clone", _parent);
	newObject->_hasMesh = _hasMesh;
	newObject->_transform->UpdateTransform(*_transform);

	for(auto iter = _components.begin(); iter != _components.end(); ++iter)
		newObject->_components.push_back( (*iter)->Clone() );
	
	return newObject;
}
