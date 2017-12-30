#pragma once

#include "Common.h"
#include "Color.h"
#include "ObjectID.hpp"
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
				struct
				{
					float	projNear		= 0.1f;			// x
	
					Half	softness		= Half(1.0f);	// y
					Half	underScanSize	= Half(4.25f);	// y
	
					uchar	flag			= 0;			// z
					uchar	bias			= 0;			// z
					ushort	lightIndex		= -1;			// z
	
					uint	shadowColor		= Color::Get32BitUintColor(0.0f, 0.0f, 0.0f, 0.8f);	// a
				};
				uint packedParam[4];

				Param() = default;
			};

		public:
			BaseShadow(Core::ObjectID id) : _objectID(id) { }

			GET_CONST_ACCESSOR(Dirty,			bool,			_dirty);
			SET_ACCESSOR(Dirty,					bool,			_dirty);

			GET_CONST_ACCESSOR(Param,			const Param&,	_param);
			GET_CONST_ACCESSOR(ParamCBData,		Param,			_param);

			SET_ACCESSOR_DIRTY(ProjNear,		float,			_param.projNear);
			GET_CONST_ACCESSOR(ProjNear,		float,			_param.projNear);

			SET_ACCESSOR_DIRTY(Softness,		Half,			_param.softness);
			GET_CONST_ACCESSOR(Softness,		Half,			_param.softness);

			SET_ACCESSOR_DIRTY(UnderScanSize,	Half,			_param.underScanSize);
			GET_CONST_ACCESSOR(UnderScanSize,	Half,			_param.underScanSize);

			SET_ACCESSOR_DIRTY(Flag,			uchar,			_param.flag);
			GET_CONST_ACCESSOR(Flag,			uchar,			_param.flag);

			GET_CONST_ACCESSOR(ObjectID,		Core::ObjectID,	_objectID);

			inline float GetBias() const { return static_cast<float>(_param.bias) / 1020.0f; }
			inline void SetBias(float f) { _param.bias = static_cast<uchar>(f * 1020.0f); _dirty = true; }

			inline Color GetColor() const { return Color(_param.shadowColor); }
			inline void SetColor(const Color& c) { _param.shadowColor = c.Get32BitUintColor(); _dirty = true; }

		private:
			Core::ObjectID		_objectID;

			Param				_param;
			bool				_dirty = true;
		};

		template<class ShadowClass>
		class ShadowPool final : public Core::VectorHashMap<Core::ObjectID::LiteralType, typename ShadowClass>
		{
		public:
			using LightType		= typename ShadowClass::LightType;
			using ShadowType	= ShadowClass;
		};
	}
}