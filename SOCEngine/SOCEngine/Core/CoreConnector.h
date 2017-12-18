#pragma once

#include "Common.h"
#include "Singleton.h"

#include "ComponentSystem.h"
#include "ObjectManager.h"
#include "Transform.h"

namespace Core
{
	class CoreConnector final : private Singleton<CoreConnector>
	{
	public:
		friend class Object;
		using Singleton<CoreConnector>::SharedInstance;

		void Connect(TransformPool* tf,  TransformPool* cantUseTF, ObjectManager* obj, ComponentSystem* cs)
		{
			_tfPool			= tf;
			_cantUseTFPool	= cantUseTF;
			_objMgr			= obj;
			_compoSystem	= cs;
		}

	private:
		GET_ACCESSOR_PTR(TransformPool,			TransformPool,		*_tfPool);
		GET_ACCESSOR_PTR(CantUseTransformPool,	TransformPool,		*_cantUseTFPool);
		GET_ACCESSOR_PTR(ObjectManager,			ObjectManager,		*_objMgr);
		GET_ACCESSOR_PTR(ComponentSystem,		ComponentSystem,	*_compoSystem);

	private:
		TransformPool*			_tfPool			= nullptr;
		TransformPool*			_cantUseTFPool	= nullptr;

		ObjectManager*			_objMgr			= nullptr;
		ComponentSystem*		_compoSystem	= nullptr;
	};
}