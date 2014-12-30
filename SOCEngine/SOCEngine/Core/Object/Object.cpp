#include "Object.h"
#include "Math.h"

using namespace std;
using namespace Intersection;
using namespace Rendering;
using namespace Math;

namespace Core
{
#define _child _vector

	using namespace Rendering::Light;

	Object::Object(Object* parent /* = NULL */) :
		_culled(false), _parent(parent), _use(true), _hasMesh(false)
	{
		_transform = new Transform( this );		
		_root = parent ? parent->_root : this;
	}

	Object::~Object(void)
	{
		SAFE_DELETE(_transform);

		DeleteAll(true);
		DeleteAllComponent();
	}

	Object* Object::AddObject(Object *child, bool copy/* = false */)
	{		
		Object *c =  Add(child->_name, child, copy);
		child->_parent = this;
		return c;
	}

	bool Object::CompareIsChildOfParent(Object *parent)
	{
		return (parent == _parent); 
	}

	bool Object::Culling(Camera::Frustum *frustum)
	{
		Vector3 wp;
		_transform->WorldPosition(wp);
		_culled = frustum->In(wp, _transform->GetRadius());

		if(_culled == false)
		{
			for(auto iter = _child.begin(); iter != _child.end(); ++iter)
				GET_CONTENT_FROM_ITERATOR(iter)->Culling(frustum);
		}

		return _culled;
	}

	void Object::Update(float delta)
	{
		if(_use == false)
			return;

		for(auto iter = _components.begin(); iter != _components.end(); ++iter)
			(*iter)->Update(delta);

		for(auto iter = _child.begin(); iter != _child.end(); ++iter)
			GET_CONTENT_FROM_ITERATOR(iter)->Update(delta);
	}

	void Object::UpdateTransformCBAndCheckRender(TransformPipelineParam& transformParam)
	{
		if(_use == false || _culled)
			return;

		_transform->WorldMatrix(transformParam.worldMat);

		TransformPipelineShaderInput transposeTransform;
		{
			Matrix::Transpose(transposeTransform.worldMat, transformParam.worldMat);

			Matrix worldView = transformParam.worldMat * transformParam.viewMat;
			Matrix::Transpose(transposeTransform.worldViewMat, worldView);

			Matrix worldViewProj = worldView * transformParam.projMat;
			Matrix::Transpose(transposeTransform.worldViewProjMat, worldViewProj);
		}

		for(auto iter = _components.begin(); iter != _components.end(); ++iter)
			(*iter)->UpdateConstBuffer(transposeTransform);

		for(auto iter = _child.begin(); iter != _child.end(); ++iter)
			GET_CONTENT_FROM_ITERATOR(iter)->UpdateTransformCBAndCheckRender(transformParam);
	}

	bool Object::Intersects(Intersection::Sphere &sphere)
	{
		Vector3 wp;
		_transform->WorldPosition(wp);
		return sphere.Intersects(wp, _transform->GetRadius());
	}

	void Object::DeleteComponent(Component *component)
	{
		for(auto iter = _components.begin(); iter != _components.end(); ++iter)
		{
			if((*iter) == component)
			{
				(*iter)->Destroy();
				_components.erase(iter);
				delete (*iter);
				return;
			}
		}

	}

	Object* Object::Copy(const Object *obj)
	{
		return new Object(*obj);
	}

	void Object::DeleteAllComponent()
	{
		for(auto iter = _components.begin(); iter != _components.end(); ++iter)
			delete (*iter);

		_components.clear();
	}
}