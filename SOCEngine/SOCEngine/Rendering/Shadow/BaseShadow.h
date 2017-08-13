#pragma once

#include "Common.h"
#include "Color.h"
#include "ObjectId.hpp"
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
			BaseShadow(Core::ObjectId id) : _objectId(id) { }

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

			GET_CONST_ACCESSOR(ObjectId, Core::ObjectId, _objectId);

			GET_CONST_ACCESSOR(LightIndex, ushort, _param.lightIndex);

			inline float GetBias() const { return static_cast<float>(_param.bias) * 1020.0f; }
			inline void SetBias(float f) { _param.bias = static_cast<uchar>(f / 1020.0f); _dirty = true; }

			inline Color GetColor() const { return Color(_param.shadowColor); }
			inline void SetColor(const Color& c) { _param.shadowColor = c.Get32BitUintColor(); _dirty = true; }

		private:
			Core::ObjectId		_objectId;

			Param				_param;
			bool				_dirty = true;
		};

		template<class ShadowClass>
		class ShadowPool final : public Core::VectorHashMap<Core::ObjectId::LiteralType, typename ShadowClass>
		{
		public:
			using LightType		= typename ShadowClass::LightType;
			using ShadowType	= ShadowClass;
		};
	}
}