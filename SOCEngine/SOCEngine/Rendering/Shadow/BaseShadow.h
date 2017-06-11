#pragma once

#include "Common.h"
#include "Color.h"
#include "ObjectId.hpp"
#include <functional>

namespace Rendering
{
	namespace Shadow
	{
		class BaseShadow final
		{
		public:
			struct Param
			{
				Half	projFar			= Half(0.1f);
				Half	projNear		= Half(32.0f);

				Half	softness		= Half(1.0f);
				Half	underScanSize	= Half(4.25f);

				uchar	flag			= 0;
				uchar	bias			= 1;
				ushort	lightIndex		= 0;

				uint	shadowColor		= Color::Get32BitUintColor(0.0f, 0.0f, 0.0f, 0.8f);

				Param() = default;
				explicit Param(ushort _lightIndex)
					: lightIndex(_lightIndex)
				{
				}
			};

		private:
			LightId				_lightId;
			Param				_param;
			bool				_dirty = true;

		public:
			BaseShadow(LightId id, ushort lightIndex)
				: _lightId(id), _param(lightIndex)
			{
			}

			GET_SET_ACCESSOR(Dirty, bool, _dirty);
			GET_CONST_ACCESSOR(Param, const Param&, _param);

		public:
			SET_ACCESSOR_DIRTY(ProjNear, Half, _param.projNear);
			GET_ACCESSOR(ProjNear, Half, _param.projNear);

			SET_ACCESSOR_DIRTY(ProjFar, Half, _param.projFar);
			GET_ACCESSOR(ProjFar, Half, _param.projFar);

			SET_ACCESSOR_DIRTY(Softness, Half, _param.softness);
			GET_ACCESSOR(Softness, Half, _param.softness);

			SET_ACCESSOR_DIRTY(UnderScanSize, Half, _param.underScanSize);
			GET_ACCESSOR(UnderScanSize, Half, _param.underScanSize);

			SET_ACCESSOR_DIRTY(Flag, uchar, _param.flag);
			GET_ACCESSOR(Flag, uchar, _param.flag);

			GET_CONST_ACCESSOR(LightId, LightId, _lightId);
			GET_CONST_ACCESSOR(LightIndex, ushort, _param.lightIndex);

			inline float GetBias() const { return static_cast<float>(_param.bias) * 1020.0f; }
			inline void SetBias(float f) { _param.bias = static_cast<uchar>(f / 1020.0f); _dirty = true; }

			inline Color GetColor() const { return Color(_param.shadowColor); }
			inline void SetColor(const Color& c) { _param.shadowColor = c.Get32BitUintColor(); _dirty = true; }
		};
	}
}