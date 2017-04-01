#pragma once

#include "Sphere.h"
#include "Color.h"
#include "Common.h"
#include "BoundBox.h"
#include "ObjectId.hpp"
#include <functional>

#define SET_LIGHT_ACCESSOR(name, type, variable) inline void Set##name(type t)	{ variable = t; _dirty = true; }

namespace Rendering
{
	namespace Light
	{
		class BaseLight final
		{
		public:
			BaseLight(Core::ObjectId);

			void SetIntensity(float intensity);
			float GetIntensity() const;

			uint Get32BitMainColor() const;

			SET_LIGHT_ACCESSOR(Color, const Color&, _color);
			SET_LIGHT_ACCESSOR(Radius, float, _radius);
			SET_LIGHT_ACCESSOR(uint, uint, _lumen);
			SET_LIGHT_ACCESSOR(Flag, uchar, _flag);

			GET_CONST_ACCESSOR(Color, const Color&, _color);
			GET_CONST_ACCESSOR(Radius, float, _radius);
			GET_CONST_ACCESSOR(uint, uint, _lumen);
			GET_CONST_ACCESSOR(Flag, uchar, _flag);

			GET_CONST_ACCESSOR(ObjectId, const Core::ObjectId&, _id);

			GET_SET_ACCESSOR(Dirty, bool, _dirty);

		private:
			Core::ObjectId			_id;
			Color					_color = Color::White();
			float					_radius = 10.0f;
			uint					_lumen = 500; //intensity]]
			uchar					_flag = 0;
			bool					_dirty = true;
		};
	}
}