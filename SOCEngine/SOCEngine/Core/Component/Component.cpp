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

	void Component::UpdateConstBuffer(const TransformPipelineParam& transpose_Transform)
	{
		//null
	}

	void Component::Render()
	{
		//null
	}

}