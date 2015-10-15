#include "Component.h"
#include "Object.h"
#include "LightForm.h"

namespace Core
{

	Component::Component(void)
		:_owner(nullptr)
	{
	}

	Component::~Component(void)
	{
	}

	void Component::OnUpdate(float deltaTime)
	{
		//null
	}

	void Component::OnUpdateTransformCB(const Device::DirectX*& dx, const Math::Matrix& transposedWorldMatrix)
	{
		//null
	}

	void Component::OnRenderPreview()
	{
		//null
	}

}