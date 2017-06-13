#pragma once

#include "Common.h"
#include "Color.h"
#include "ObjectId.hpp"
#include "LightId.hpp"
#include "ShadowId.hpp"
#include "VectorIndexer.hpp"

namespace Rendering
{
	namespace Shadow
	{
		class BaseShadow final
		{
		public:
			struct Param
			{
				float	projNear		= 0.1f;

				Half	softness		= Half(1.0f);
				Half	underScanSize	= Half(4.25f);

				uchar	flag			= 0;
				uchar	bias			= 1;
				ushort	lightIndex		= -1;

				uint	shadowColor		= Color::Get32BitUintColor(0.0f, 0.0f, 0.0f, 0.8f);

				Param() = default;
			};

		public:
			BaseShadow(Light::LightId lightId, Shadow::ShadowId shadowId)
				: _lightId(lightId), _shadowId(shadowId)
			{

			}

			GET_SET_ACCESSOR(Dirty, bool, _dirty);
			GET_CONST_ACCESSOR(Param, Param, _param);

		public:
			SET_ACCESSOR_DIRTY(ProjNear, float, _param.projNear);
			GET_ACCESSOR(ProjNear, float, _param.projNear);

			SET_ACCESSOR_DIRTY(Softness, Half, _param.softness);
			GET_ACCESSOR(Softness, Half, _param.softness);

			SET_ACCESSOR_DIRTY(UnderScanSize, Half, _param.underScanSize);
			GET_ACCESSOR(UnderScanSize, Half, _param.underScanSize);

			SET_ACCESSOR_DIRTY(Flag, uchar, _param.flag);
			GET_ACCESSOR(Flag, uchar, _param.flag);

			GET_CONST_ACCESSOR(LightId, Light::LightId, _lightId);
			GET_CONST_ACCESSOR(ShadowId, Shadow::ShadowId, _shadowId);

			GET_CONST_ACCESSOR(LightIndex, ushort, _param.lightIndex);

			inline float GetBias() const { return static_cast<float>(_param.bias) * 1020.0f; }
			inline void SetBias(float f) { _param.bias = static_cast<uchar>(f / 1020.0f); _dirty = true; }

			inline Color GetColor() const { return Color(_param.shadowColor); }
			inline void SetColor(const Color& c) { _param.shadowColor = c.Get32BitUintColor(); _dirty = true; }

		private:
			Light::LightId		_lightId;
			Shadow::ShadowId	_shadowId;

			Param				_param;
			bool				_dirty = true;
		};

		template<class LightObjType>
		class ShadowPool final : public Core::VectorHashMap<ShadowId::LiteralType, typename LightObjType::ShadowType>
		{
		public:
			using LightType = LightObjType;
			using ShadowType = typename LightObjType::ShadowType;
		};
	}
}