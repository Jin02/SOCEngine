#include "UIManager.h"
#include "Director.h"

using namespace UI::Manager;
using namespace Device;
using namespace Rendering::Camera;

UIManager::UIManager()
{
}

UIManager::~UIManager()
{
}

void UIManager::DepthSort()
{
	auto depthSort = [](const UI::UIObject* left, const UI::UIObject* right)
	{
		int leftDepth  = left->GetDepth();
		int rightDepth= right->GetDepth();

		return leftDepth < rightDepth;
	};

	std::sort(_renderQueue.begin(), _renderQueue.end(), depthSort);
}

UI::UIObject* UIManager::AddRenderQueue(std::string key, UI::UIObject* component)
{
	_renderQueue.push_back(component);

	auto iter = _renderQueue.back();
	UI::UIObject* backComponent = iter;
	if(component->GetDepth() < backComponent->GetDepth())
		DepthSort();

	return component;
}

void UIManager::AddUpdateQueue(UI::UIObject* uiObject)
{
	_rootObjects.push_back(uiObject);
}