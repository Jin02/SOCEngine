#pragma once

#include "Common.h"
#include "Color.h"
#include "Matrix.h"

namespace Rendering
{
	namespace Light
	{
		class LightForm;
	}

	namespace Shadow
	{
		class ShadowCommon
		{
		public:
			struct CommonParam
			{
				ushort	bias;
				ushort	index;
			};

		private:
			Color						_color; //a is strength
			float						_bias;

		protected:
			const Light::LightForm*		_owner;

		public:
			ShadowCommon(const Light::LightForm* owner);
			~ShadowCommon();

		protected:
			void MakeParam(CommonParam& outParam) const;

		public:
			ushort FetchShadowCastingLightIndex() const;
			GET_SET_ACCESSOR(Bias, float, _bias);
			GET_SET_ACCESSOR(Color, const Color&, _color);
		};
	}
}