#pragma once

#include "EngineMath.h"
#include "Structure.h"
#include "Transform.h"
#include "Component.h"
#include "Frustum.h"
#include "TransformPipelineParam.h"
#include "Mesh.h"

namespace Core
{
	class Object : protected Structure::VectorMap<std::string, Object*>
	{
	protected:
		bool					_use;
		bool					_culled;
		bool					_hasMesh;

		std::string				_name;

		const Object*			_parent;
		const Object*			_root;
		Transform*				_transform;

		std::vector<Component*> _components;

	public:
		Object(const std::string& name, const Object* parent = NULL);
		virtual ~Object(void);

	protected:
		void DeleteAllChild();

	public:
		void Update(float delta);

		bool Culling(const Rendering::Camera::Frustum *frustum);
		void RenderPreviewWithUpdateTransformCB(TransformPipelineParam& transformParam);

		bool Intersects(Intersection::Sphere &sphere);

	public:
		void AddChild(Object *child);
		inline Object* FindChild(const std::string& key)	{ return *Find(key); }
		inline Object* GetChild(uint index)					{ return Get(index); }

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
			compo->OnInitialize();
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

		bool CompareIsChildOfParent(Object *parent);
		Object* Clone() const;

	public:
		GET_SET_ACCESSOR(Use,		bool,				_use);
		GET_SET_ACCESSOR(Parent,	const Object*,		_parent);
		GET_ACCESSOR(Culled,		bool,				_culled);
		GET_ACCESSOR(HasMesh,		bool,				_hasMesh);
		GET_ACCESSOR(Name,			const std::string&,	_name);
		GET_ACCESSOR(Transform,		Transform*,			_transform);
	};
}