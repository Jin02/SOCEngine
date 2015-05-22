#include "UIManager.h"
#include "Director.h"

using namespace UI::Manager;
using namespace Device;
using namespace Rendering::Camera;

#define ParentClass Structure::Vector<std::string, UIObject>

UIManager::UIManager()
{
}

UIManager::~UIManager()
{
}

void UIManager::DepthSort()
{
	auto depthSort = [](const Type& left, const Type& right)
	{
		int leftDepth  = left.second.second->GetDepth();
		int rightDepth= right.second.second->GetDepth();

		return leftDepth < rightDepth;
	};
	std::sort(_vector.begin(), _vector.end(), depthSort);
}

UI::UIObject* UIManager::AddRenderQueue(std::string key, UI::UIObject* component, bool copy)
{
	ParentClass::Add(key, component, copy);

	auto iter = _vector.back();
	UI::UIObject* backComponent = iter.second.second;
	if(component->GetDepth() < backComponent->GetDepth())
		DepthSort();

	return component;
}

void UIManager::AddUpdateQueue(UI::UIObject* uiObject)
{
	_rootUIObjects.push_back(uiObject);
}