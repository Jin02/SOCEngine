#pragma once

#include "Sphere.h"
#include "Color.h"
#include "Common.h"
#include "BoundBox.h"
#include "ObjectId.hpp"
#include "LightId.hpp"
#include <functional>
#include "VectorIndexer.hpp"

namespace Rendering
{
	namespace Light
	{
		class BaseLight final
		{
		public:
			explicit BaseLight(Core::ObjectId id, LightId lightId)
				: _objectId(id), _lightId(lightId) {};

			void SetIntensity(float intensity);
			float GetIntensity() const;

			uint Get32BitMainColor() const;

			SET_ACCESSOR_DIRTY(Color, const Color&, _color);
			SET_ACCESSOR_DIRTY(Radius, float, _radius);
			SET_ACCESSOR_DIRTY(uint, uint, _lumen);
			SET_ACCESSOR_DIRTY(Flag, uchar, _flag);

			GET_CONST_ACCESSOR(Color, const Color&, _color);
			GET_CONST_ACCESSOR(Radius, float, _radius);
			GET_CONST_ACCESSOR(uint, uint, _lumen);
			GET_CONST_ACCESSOR(Flag, uchar, _flag);

			GET_CONST_ACCESSOR(ObjectId, Core::ObjectId, _objectId);
			GET_CONST_ACCESSOR(LightId, LightId, _lightId);

			GET_SET_ACCESSOR(Dirty, bool, _dirty);

		private:
			Core::ObjectId			_objectId;
			LightId					_lightId;

			Color					_color = Color::White();
			float					_radius = 10.0f;
			uint					_lumen = 500; //intensity]]
			uchar					_flag = 0;
			bool					_dirty = true;
		};

		template<class LightType>
		using LightPool = Core::VectorHashMap<LightId::LiteralType, LightType>;
	}
}
