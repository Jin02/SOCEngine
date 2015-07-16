#pragma once

#include "SimpleImage2D.h"
#include "Structure.h"
#include "UICamera.h"

namespace UI
{
	namespace Manager
	{
		class UIManager
		{
		private:
			std::vector<UI::UIObject*>		_rootObjects;
			std::vector<UI::UIObject*>		_renderQueue;

		public:
			UIManager();
			~UIManager();

		public:
			void AddUpdateQueue(UI::UIObject* uiObject);
			UIObject* AddRenderQueue(std::string key, UI::UIObject* uiObject);
			void DepthSort();

		public:
			GET_ACCESSOR(RootUIObjects, const std::vector<UI::UIObject*>&, _rootObjects);
			GET_ACCESSOR(RenderQueue, const std::vector<UI::UIObject*>&, _renderQueue);
		};
	}
}