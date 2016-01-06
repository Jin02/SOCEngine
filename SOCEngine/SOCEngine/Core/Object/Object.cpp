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
	_culled(false), _parent(parent), _use(true), _hasMesh(false), _name(name),
	_radius(0.0f)
{
	_boundBox.SetMinMax(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f));

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

void Object::Culling(const Intersection::Frustum *frustum)
{
	Vector3 wp;
	_transform->FetchWorldPosition(wp);
	_culled = !frustum->In(wp, _radius);

	for(auto iter = _child.begin(); iter != _child.end(); ++iter)
		(*iter)->Culling(frustum);
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

void Object::UpdateTransformCB_With_ComputeSceneMinMaxPos(
	const Device::DirectX*& dx, Math::Vector3& refWorldPosMin, Math::Vector3& refWorldPosMax,
	const Math::Matrix& parentWorldMat)
{
	if(_use == false)
		return;

	Math::Matrix localMat;
	_transform->FetchLocalMatrix(localMat);

	Math::Matrix worldMat = localMat * parentWorldMat;

	if(_hasMesh)
	{
		Vector3 extents		= _boundBox.GetExtents();
		Vector3 boxCenter	= _boundBox.GetCenter(); 

		Vector3 worldPos = Vector3(worldMat._41, worldMat._42, worldMat._43) + boxCenter;

		Vector3 worldScale;
		Transform::FetchWorldScale(worldScale, worldMat);

		Vector3 minPos = (worldPos - extents) * worldScale;
		Vector3 maxPos = (worldPos + extents) * worldScale;

		if(refWorldPosMin.x > minPos.x) refWorldPosMin.x = minPos.x;
		if(refWorldPosMin.y > minPos.y) refWorldPosMin.y = minPos.y;
		if(refWorldPosMin.z > minPos.z) refWorldPosMin.z = minPos.z;

		if(refWorldPosMax.x < maxPos.x) refWorldPosMax.x = maxPos.x;
		if(refWorldPosMax.y < maxPos.y) refWorldPosMax.y = maxPos.y;
		if(refWorldPosMax.z < maxPos.z) refWorldPosMax.z = maxPos.z;
	}

	Matrix transposedWM;
	Matrix::Transpose(transposedWM, worldMat);

	bool changedWorldMat = memcmp(&_prevWorldMat, &worldMat, sizeof(Math::Matrix)) != 0;
	if(changedWorldMat)
		_prevWorldMat = worldMat;

	for(auto iter = _components.begin(); iter != _components.end(); ++iter)
	{
		if(changedWorldMat)
			(*iter)->OnUpdateTransformCB(dx, transposedWM);

		(*iter)->OnRenderPreview();
	}

	for(auto iter = _child.begin(); iter != _child.end(); ++iter)
		(*iter)->UpdateTransformCB_With_ComputeSceneMinMaxPos(dx, refWorldPosMin, refWorldPosMax, worldMat);
}

bool Object::Intersects(Intersection::Sphere &sphere)
{
	Vector3 wp;
	_transform->FetchWorldPosition(wp);
	return sphere.Intersects(wp, _radius);
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
	Object* newObject = new Object(_name + "-Clone", nullptr);
	newObject->_transform->UpdateTransform(*_transform);
	newObject->_hasMesh		= _hasMesh;
	newObject->_use			= _use;
	newObject->_radius		= _radius;
	newObject->_boundBox	= _boundBox;

	for(auto iter = _components.begin(); iter != _components.end(); ++iter)
		newObject->_components.push_back( (*iter)->Clone() );

	for(auto iter = _child.begin(); iter != _child.end(); ++iter)
		newObject->AddChild( (*iter)->Clone() );
	
	return newObject;
}
