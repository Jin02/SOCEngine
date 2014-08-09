#pragma once

#include "EngineMath.h"
#include "Structure.h"
#include "Transform.h"
#include "Component.h"
#include "Light.h"
#include "Frustum.h"
#include "TransformPipelineParam.h"

namespace Core
{
	class Object : public Structure::Vector<Object>
	{
	protected:
		bool _use;
		bool _culled;
		bool _hasMesh;

	protected:
		std::string _name;

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
		void Render(const std::vector<Rendering::Light::LightForm*>& lights, TransformPipelineParam& transformParam);

		bool Intersects(Intersection::Sphere &sphere);

	public:
		Object* AddObject(Object *child, bool copy = false);

		template<typename ComponentType>
		ComponentType* AddComponent()
		{
			if( ComponentType::GetComponentType() < Component::Type::User )
			{
				typename std::vector<Component*>::iterator iter;
				for(iter = _components.begin(); iter != _components.end(); ++iter)
				{
					ComponentType *compareComponent = dynamic_cast<ComponentType*>(*iter);

					if( compareComponent->GetComponentType() == ComponentType::GetComponentType() )
						return compareComponent;
				}
			}

			ComponentType *compo = new ComponentType;
			compo->SetOwner(this);

			if(compo->GetComponentType() == Component::Type::Mesh)
				_hasMesh = true;

			//오직 유저 컴포넌트만 중복 가능
			compo->Initialize();
			_components.push_back(compo);

			return compo;
		}

		template<class ComponentType>
		ComponentType* GetComponent()
		{
			typename std::vector<Component*>::iterator iter;
			for(iter = _components.begin(); iter != _components.end(); ++iter)
			{
				ComponentType *compareComponent = dynamic_cast<ComponentType*>(*iter);

				if(compareComponent->GetComponentType() == ComponentType::GetComponentType())
					return compareComponent;
			}

			return nullptr;
		}

		template<class ComponentType>
		std::vector<Component*> GetComponents()
		{
			std::vector<Component*> v;
			typename std::vector<Component*>::iterator iter;
			for(iter = _components.begin(); iter != _components.end(); ++iter)
			{
				ComponentType *compareComponent = dynamic_cast<ComponentType*>(*iter);

				if(compareComponent->GetComponentType() == ComponentType::GetComponentType())
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
		GET_SET_ACCESSOR(Name, const std::string&, _name);

	public:
		void UpdateChild(float delta);

	public:
		static Object* Copy(Object *obj);
		GET_ACCESSOR(Transform, Transform*, _transform);
	};

}