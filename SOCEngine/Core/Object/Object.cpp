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
		_transform = new Transform( parent ? parent->_transform : nullptr);		
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
		Object *c =  Vector::Add(child->_name, child, copy);
		_transform->CalcRadius(c->_transform);

		return c;
	}

	void Object::UpdateChild(float delta)
	{
		if(_use == false)
			return;

		for(auto iter = _child.begin(); iter != _child.end(); ++iter)
			GET_CONTENT_FROM_ITERATOR(iter)->Update(delta);
	}

	bool Object::CompareIsChildOfParent(Object *parent)
	{
		return (parent == _parent); 
	}

	bool Object::Culling(Frustum *frustum)
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
		for(auto iter = _components.begin(); iter != _components.end(); ++iter)
			(*iter)->Update(delta);
	}

	void Object::Render(const std::vector<Rendering::Light::LightForm*>& lights,
						const Math::Matrix& viewMat, const Math::Matrix& projMat,
						const Math::Matrix& viewProjMat)
	{
		if(_culled)	return;

		Vector3 wp;
		_transform->WorldPosition(wp);

		Sphere thisObject(wp, _transform->GetRadius());
		std::vector<LightForm*> intersectLights;

		for(auto iter = lights.begin(); iter != lights.end(); ++iter)
		{
			if((*iter)->Intersects(thisObject))
				intersectLights.push_back((*iter));
		}
		
		Matrix worldMat, worldViewProjMat;

		_transform->WorldMatrix(worldMat);
		worldViewProjMat = worldMat * viewProjMat;

		Matrix worldViewInvTns;
		worldViewInvTns = worldMat * viewMat;
		worldViewInvTns.Inverse();
		worldViewInvTns.Transpose();

		TransformParameters transformParam(&worldMat, &viewMat, &projMat, &viewProjMat, &worldViewProjMat, &worldViewInvTns);

		Vector4 viewPos = Vector4(viewMat._41, viewMat._42, viewMat._43, 1.0f);

		for(auto iter = _components.begin(); iter != _components.end(); ++iter)
			(*iter)->Render(&transformParam, &intersectLights, viewPos);

		for(auto iter = _child.begin(); iter != _child.end(); ++iter)
			GET_CONTENT_FROM_ITERATOR(iter)->Render(lights, viewMat, projMat, viewProjMat);
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

	Object* Object::Copy(Object *obj)
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