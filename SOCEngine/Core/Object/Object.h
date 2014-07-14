#pragma once

//#include <stdarg.h>
#include <vector>
#include <string>

#include "Platform.h"
#include "Sphere.h"

#include "Container.h"

#include "Mesh.h"
#include "Light.h"
#include "Camera.h"

//#include "Component.h"

//#include "Frustum.h"
//#include "Transform.h"

namespace Rendering
{
	class Object : public Container<Object>
	{
	protected:
		bool use;
		bool culled;
		bool hasMesh;

	protected:
		Object *parent;
		Object *root;
		Transform *transform;

	protected:
		std::vector<Component*> components;

	public:
		Object(Object* parent = NULL);
		virtual ~Object(void);

	public:
		void Update(float delta);
		void Render(std::vector<Light::LightForm*> *lights, SOC_Matrix *viewMat, SOC_Matrix *projMat, SOC_Matrix *viewProjMat);

		bool Intersect(Intersection::Sphere &sphere);

	public:
		Object* AddObject(Object *child, bool copy = false);
		Object* AddObject(Object *child, int renderQueueOrder, bool copy = false);

		template<typename ComponentType>
		ComponentType* AddComponent()
		{
			if( ComponentType::ComponentType < Component::Type::User )
			{
				typename std::vector<Component*>::iterator iter;
				for(iter = components.begin(); iter != components.end(); ++iter)
				{
					ComponentType *compareComponent = dynamic_cast<ComponentType*>(*iter);

					if( compareComponent->ComponentType == ComponentType::ComponentType )
						return compareComponent;
				}
			}

			ComponentType *compo = new ComponentType;
			compo->SetOwner(this);

			if(compo->ComponentType == Component::Type::Mesh)
				hasMesh = true;

			//오직 유저 컴포넌트만 중복 가능
			compo->Initialize();
			components.push_back(compo);

			return compo;
		}

		template<class ComponentType>
		ComponentType* GetComponent()
		{
			typename std::vector<Component*>::iterator iter;
			for(iter = components.begin(); iter != components.end(); ++iter)
			{
				ComponentType *compareComponent = dynamic_cast<ComponentType*>(*iter);

				if(compareComponent->ComponentType == ComponentType::ComponentType)
					return compareComponent;
			}

			return nullptr;
		}

		template<class ComponentType>
		std::vector<ComponentType*> GetComponents()
		{
			std::vector<Component*> v;
			typename std::vector<Component*>::iterator iter;
			for(iter = components.begin(); iter != components.end(); ++iter)
			{
				if((*iter)->ComponentType == ComponentType::ComponentType)
					v.push_back((*iter));
			}

			return v;
		}

		void DeleteComponent(Component *component);
		void DeleteAllComponent();

		bool Culling(Frustum *frustum);
		bool IsChildOf(Object *parent);

		void SetUse(bool is);
		bool GetUse();
		bool Culled();

	public:
		void UpdateChild(float delta);

	public:
		static Object* Copy(Object *obj);
		Transform *GetTransform();
	};

}