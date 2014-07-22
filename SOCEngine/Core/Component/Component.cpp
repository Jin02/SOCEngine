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

	void Component::Update(float deltaTime)
	{
		//null
	}

	void Component::Render(TransformParameters *transform, std::vector<Rendering::Light::LightForm*> *lights, const Math::Vector4& viewPos)
	{
		//null
	}

}