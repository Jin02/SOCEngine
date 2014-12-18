#pragma once

#include "TransformPipelineParam.h"
#include "Common.h"
#include <vector>

namespace Rendering
{
	namespace Light
	{
		class LightForm;
	};
};

namespace Core
{
	class Object;
	class Transform;

	class Component
	{
	public:
		enum Type{
			Mesh,
			Camera,
			Light,
			User
		};

	protected:
		Object	*_owner;

	public:
		Component();
		virtual ~Component(void);

	public:
		virtual void Initialize() = 0;
		virtual void Update(float deltaTime);
		virtual void UpdateConstBuffer(const TransformPipelineParam& transpose_Transform);
		virtual void Render();
		virtual void Destroy() = 0;

	public:
		GET_SET_ACCESSOR(Owner, Object*, _owner);
	};
}