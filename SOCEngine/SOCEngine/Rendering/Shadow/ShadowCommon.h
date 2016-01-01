#pragma once

#include "Common.h"
#include "Color.h"
#include "Matrix.h"
#include <functional>

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
				float	bias;
				uint	index;

				CommonParam() : index(0), bias(0) {}
				~CommonParam() {}
			};

		private:
			std::function<void()>		_ownerAddUpdateCounter;

		private:
			Color						_color; //a is strength
			float						_bias;

		protected:
			const Light::LightForm*		_owner;

		public:
			ShadowCommon(const Light::LightForm* owner, const std::function<void()>& ownerUpdateCounter);
			~ShadowCommon();

		protected:
			void MakeParam(CommonParam& outParam) const;

		public:
			void Initialize();
			ushort FetchShadowCastingLightIndex() const;

			void SetBias(float bias);
			GET_ACCESSOR(Bias, float, _bias);

			void SetColor(const Color& color);
			GET_ACCESSOR(Color, const Color&, _color);
		};
	}
}