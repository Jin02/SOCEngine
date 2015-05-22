#pragma once

#include "SimpleImage2D.h"
#include "Structure.h"

namespace UI
{
	namespace Manager
	{
		class UIManager : public Structure::Vector<std::string, UIObject>
		{
		public:
			UIManager();
			~UIManager();

		public:
			UIObject* Add(std::string key, UI::UIObject* component, bool copy = false);
			void DepthSort();
		};
	}
}