#pragma once

#include "GlobalDefine.h"
#include "Sphere.h"
#include "Color.h"
#include "Common.h"
#include "BoundBox.h"
#include "ObjectID.hpp"
#include <functional>
#include "VectorIndexer.hpp"

namespace Rendering
{
	namespace Light
	{
		class BaseLight final
		{
		public:
			explicit BaseLight(Core::ObjectID id) : _objectID(id) {};

			void SetIntensity(float intensity);
			float GetIntensity() const;

			uint Get32BitMainColor() const;

			SET_ACCESSOR_DIRTY(Color,		const Color&,		_color);
			SET_ACCESSOR_DIRTY(Radius,		float,				_radius);
			SET_ACCESSOR_DIRTY(Lumen,		uint,				_lumen);
			SET_ACCESSOR_DIRTY(Flag,		uchar,				_flag);

			GET_CONST_ACCESSOR(Color,		const Color&,		_color);
			GET_CONST_ACCESSOR(Radius,		float,				_radius)
			GET_CONST_ACCESSOR(Lumen,		uint,				_lumen);
			GET_CONST_ACCESSOR(Flag,		uchar,				_flag);

			GET_CONST_ACCESSOR(ObjectID,	Core::ObjectID,		_objectID);

			SET_ACCESSOR(Dirty,			bool, _dirty);
			GET_CONST_ACCESSOR(Dirty,	bool, _dirty);

		private:
			Core::ObjectID			_objectID;

			Color					_color	= Color::White();
			float					_radius	= 10.0f;
			uint					_lumen	= 500; //intensity
			uchar					_flag	= 0;
			bool					_dirty	= true;
		};

		template<class LightType>
		using LightPool = Core::VectorHashMap<Core::ObjectID::LiteralType, LightType>;
	}
}
