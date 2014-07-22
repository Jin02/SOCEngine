#pragma once

#include "EngineMath.h"
#include "Structure.h"
#include "Transform.h"
#include "Component.h"
#include "Light.h"
#include "Frustum.h"

namespace Core
{
	class Object : public Container<Object>
	{
	protected:
		bool _use;
		bool _culled;
		bool _hasMesh;

	protected:
		Object *_parent;
		Object *_root;
		Transform *_transform;

	protected:
		std::vector<Component*> _components;

	public:
		Object(Object* parent = NULL);
		virtual ~Object(void);

	public:
		void Update(float delta);
		void Render(
			const std::vector<Rendering::Light::LightForm*>& lights,
			const Math::Matrix& viewMat, const Math::Matrix& projMat,
			const Math::Matrix& viewProjMat);

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

		bool Culling(Rendering::Frustum *frustum);
		bool CompareIsChildOfParent(Object *parent);

		GET_SET_ACCESSOR(Use, bool, _use);
		GET_ACCESSOR(Culled, bool, _culled);
		GET_ACCESSOR(HasMesh, bool, _hasMesh);

	public:
		void UpdateChild(float delta);

	public:
		static Object* Copy(Object *obj);
		GET_ACCESSOR(Transform, Transform*, _transform);
	};

}