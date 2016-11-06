#pragma once

#include "EngineMath.h"
#include "Structure.h"
#include "Transform.h"
#include "Component.h"
#include "Frustum.h"
#include "Mesh.h"
#include "BoundBox.h"

namespace Core
{
	class Object : protected Structure::VectorMap<std::string, Object*>
	{
	protected:
		bool					_use;
		bool					_culled;
		bool					_hasMesh;

		std::string				_name;

		Object*					_parent;
		Object*					_root;
		Transform*				_transform;

		std::vector<Component*>			_components;

		float					_radius;
		Intersection::BoundBox			_boundBox;

		Math::Matrix				_prevWorldMat;
		TransformCB::ChangeState		_tfChangeState;

	public:
		Object(const std::string& name, Object* parent = NULL);
		virtual ~Object(void);

	public:
		void Update(float delta);

		void Culling(const Intersection::Frustum* frustum);
		void UpdateTransformCB_With_ComputeSceneMinMaxPos(const Device::DirectX*& dx, Math::Vector3& refWorldPosMin, Math::Vector3& refWorldPosMax, const Math::Matrix& parentWorldMat);

		bool Intersects(Intersection::Sphere &sphere);

	public:
		void			AddChild(Object *child);
		Object*			FindChild(const std::string& key);
		inline Object*	GetChild(uint index) { return Get(index); }

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
		void DeleteAllChild();

		bool CompareIsChildOfParent(Object *parent);
		Object* Clone() const;

	public:
		GET_ACCESSOR(Use,			bool,							_use);
		void SetUse(bool b);

		GET_SET_ACCESSOR(Parent,	Object*,						_parent);
		GET_SET_ACCESSOR(Radius,	float,							_radius);
		GET_SET_ACCESSOR(Name,		const std::string&,				_name);

		GET_ACCESSOR(Culled,		bool,							_culled);
		GET_ACCESSOR(HasMesh,		bool,							_hasMesh);
		GET_ACCESSOR(Transform,		Transform*,						_transform);

		SET_ACCESSOR(BoundBox,		const Intersection::BoundBox&,	_boundBox);
	};
}
