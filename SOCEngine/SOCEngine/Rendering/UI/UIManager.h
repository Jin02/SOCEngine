#pragma once

#include "SimpleImage2D.h"
#include "Structure.h"
#include "UICamera.h"

namespace UI
{
	namespace Manager
	{
		class UIManager : public Structure::Vector<std::string, UIObject>
		{
		private:
			std::vector<UI::UIObject*>		_rootUIObjects;

		public:
			UIManager();
			~UIManager();

		public:
			void AddUpdateQueue(UI::UIObject* uiObject);
			UIObject* AddRenderQueue(std::string key, UI::UIObject* uiObject, bool copy = false);
			void DepthSort();

		public:
			GET_ACCESSOR(RootUIObjects, const std::vector<UI::UIObject*>&, _rootUIObjects);
		};
	}
}