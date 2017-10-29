#pragma once

#include "Common.h"
#include "Singleton.h"

#include "ComponentSystem.hpp"
#include "ObjectManager.h"
#include "Transform.h"

namespace Core
{
	class CoreConnector final : private Singleton<CoreConnector>
	{
	public:
		friend class Object;
		using Singleton<CoreConnector>::SharedInstance;

		void Connect(TransformPool* tf, ObjectManager* obj, ComponentSystem* cs)
		{
			_tfPool			= tf;
			_objMgr			= obj;
			_compoSystem	= cs;
		}

	private:
		GET_ACCESSOR(TransformPool,		auto,	_tfPool);
		GET_ACCESSOR(ObjectManager,		auto,	_objMgr);
		GET_ACCESSOR(ComponentSystem,	auto,	_compoSystem);

	private:
		TransformPool*			_tfPool			= nullptr;
		ObjectManager*			_objMgr			= nullptr;
		ComponentSystem*		_compoSystem	= nullptr;
	};
}