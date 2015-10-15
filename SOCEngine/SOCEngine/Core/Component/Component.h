#pragma once

#include "Matrix.h"
#include "Common.h"
#include <vector>

namespace Device
{
	class DirectX;
};

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
		enum class Type{
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
		virtual void OnInitialize() = 0;
		virtual void OnUpdate(float deltaTime);
		virtual void OnUpdateTransformCB(const Device::DirectX*& dx, const Math::Matrix& transposedWorldMatrix);
		virtual void OnRenderPreview();
		virtual void OnDestroy() = 0;

	public:
		virtual Component* Clone() const = 0;

	public:
		GET_SET_ACCESSOR(Owner, Object*, _owner);
	};
}