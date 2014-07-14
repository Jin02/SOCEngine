#include "Object.h"
#include "Math.h"
#include "Utility.h"

using namespace std;
using namespace Intersection;

namespace Rendering
{
	using namespace Light;

	Object::Object(Object* parent /* = NULL */) : Container()
	{
		culled = false;
		transform = new Transform( parent ? parent->transform : nullptr);
		
		this->parent = parent;
		this->root = parent ? parent->root : this;
		this->use = true;
		this->hasMesh = false;
	}

	Object::~Object(void)
	{
		if(transform)
			delete transform;

		DeleteAll(true);
		DeleteAllComponent();
	}

	Object* Object::AddObject(Object *child, int renderQueue, bool copy/* = false */)
	{
		Object *c =  Container::Add(child, renderQueue, copy);
		transform->CalcRadius(c->transform);

		return c;
	}

	Object* Object::AddObject(Object *child, bool copy/* = false */)
	{
		Object *c =  Container::Add(child, copy);
		transform->CalcRadius(c->transform);

		return c;
	}

	void Object::UpdateChild(float delta)
	{
		if(use == false)
			return;

		vector<Object*>::iterator iter;

		for(iter = objects.begin(); iter != objects.end(); ++iter)
			(*iter)->Update(delta);
	}

	void Object::SetUse(bool is)
	{
		use = is;
	}

	bool Object::IsChildOf(Object *parent)
	{
		return (parent == this->parent); 
	}

	bool Object::Culling(Frustum *frustum)
	{
		culled = frustum->In(transform->GetWorldPosition(), transform->GetRadius());

		if(culled == false)
		{
			for(vector<Object*>::iterator iter = objects.begin(); iter != objects.end(); ++iter)
				(*iter)->Culling(frustum);
		}

		return culled;
	}

	bool Object::GetUse()
	{
		return use;
	}

	bool Object::Culled()
	{
		return culled;
	}

	void Object::Update(float delta)
	{
		for(std::vector<Component*>::iterator iter = components.begin(); iter != components.end(); ++iter)
			(*iter)->Update(delta);
	}

	void Object::Render(std::vector<LightForm*> *lights, SOC_Matrix *viewMat, SOC_Matrix *projMat, SOC_Matrix *viewProjMat)
	{
		if(culled)	return;

		Sphere thisObject(transform->GetWorldPosition(), transform->GetRadius());
		std::vector<LightForm*> intersectLights;

		for(std::vector<LightForm*>::iterator iter = lights->begin(); iter != lights->end(); ++iter)
		{
			if((*iter)->Intersect(thisObject))
				intersectLights.push_back((*iter));
		}
		
		SOC_Matrix worldMat, worldViewProjMat;

		transform->GetWorldMatrix(&worldMat);
		SOCMatrixMultiply(&worldViewProjMat, &worldMat, viewProjMat);

		if(this->components.size() != 0)
		{
			//debug break point
			int a= 5;
			a=3;
		}

		SOC_Matrix worldViewInvTns;
		SOCMatrixMultiply(&worldViewInvTns, &worldMat, viewMat);
		SOCMatrixInverse(&worldViewInvTns, nullptr, &worldViewInvTns);
		SOCMatrixTranspose(&worldViewInvTns, &worldViewInvTns);

		TransformParameters transformParam;
		transformParam.SetMatrix(&worldMat, viewMat, projMat, viewProjMat, &worldViewProjMat, &worldViewInvTns);

		vector<LightParameters> lightParam;
		SOC_Vector4 viewPos = SOC_Vector4(viewMat->_41, viewMat->_42, viewMat->_43, 1.0f);

		if(hasMesh)
		{
			int intersectLightCount = intersectLights.size();
			int count = intersectLightCount > MAX_LIGHT ? MAX_LIGHT : intersectLightCount;

			for(int i=0; i < count; ++i)
			{
				LightForm *light = intersectLights[i];
				SOC_Vector3 w = light->GetWorldPosition();
				SOC_Vector3 d = light->GetDirection();

				LightParameters lp;
				lp.SetData(light->ambient.GetVector3(),
					light->diffuse.GetVector3(),
					light->specular.GetVector3(), 
					light->range, 
					w,
					d,
					LightForm::SPOT == light->GetType() ? static_cast<SpotLight*>(light)->spotAngle : 1.0f,
					(int)light->GetType());

				lightParam.push_back(lp);
			}
		}

		for(std::vector<Component*>::iterator iter = components.begin(); iter != components.end(); ++iter)
			(*iter)->Render(&transformParam, &lightParam, viewPos);

		for(std::vector<Object*>::iterator iter = objects.begin(); iter != objects.end(); ++iter)
			(*iter)->Render(lights, viewMat, projMat, viewProjMat);
	}

	bool Object::Intersect(Intersection::Sphere &sphere)
	{
		return sphere.Intersection(transform->GetWorldPosition(), transform->GetRadius());
	}

	//void Object::_Render(std::vector<LightForm*> *lights, SOC_Matrix *viewMat, SOC_Matrix *projMat, SOC_Matrix *viewProjMat)
	//{
	//	//null
	//}

	void Object::DeleteComponent(Component *component)
	{
		for(std::vector<Component*>::iterator iter = components.begin(); iter != components.end(); ++iter)
		{
			if((*iter) == component)
			{
				(*iter)->Destroy();
				components.erase(iter);
				delete (*iter);
				return;
			}
		}

	}

	Object* Object::Copy(Object *obj)
	{
		return new Object(*obj);
	}

	Transform* Object::GetTransform()
	{
		return transform;
	}

	void Object::DeleteAllComponent()
	{
		for(std::vector<Component*>::iterator iter = components.begin(); iter != components.end(); ++iter)
			delete (*iter);

		components.clear();
	}
}