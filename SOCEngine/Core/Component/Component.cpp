#include "Component.h"
#include "Object.h"

Component::Component(void)
{
}

Component::~Component(void)
{
}

void Component::Update(float deltaTime)
{
	//null
}

void Component::Render(Rendering::TransformParameters *transform, std::vector<Rendering::Light::LightParameters> *lights, Vector4 &viewPos)
{
	//null
}

void Component::SetOwner(Rendering::Object *object)
{
	this->owner = object;
	this->ownerTransform = object->GetTransform();
}

Rendering::Object* Component::GetOwner()
{
	return owner;
}