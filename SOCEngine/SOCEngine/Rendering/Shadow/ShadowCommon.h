#pragma once

#include "Common.h"
#include "Color.h"
#include "Matrix.h"
#include <functional>

#include "GlobalDefine.h"

#include "BoundBox.h"

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
				uint lightIndexWithBiasWithFlag;
				uint color;
			};

		private:
			Color						_color; //a is strength
			float						_bias;
			float						_projNear;
			uint						_paramUpdateCounter;
			bool						_useVSM;

		protected:
			const Light::LightForm*		_owner;
			Math::Matrix				_invNearFarViewProjMat;
			Math::Matrix				_viewProjMat;
			Math::Matrix				_invViewProjViewportMat;

			uint						_transformUpdateCounter;

		public:
			ShadowCommon(const Light::LightForm* owner);
			~ShadowCommon();

		public:
			void Initialize();

		public:
			void MakeParam(CommonParam& outParam, uint lightIndex) const;

		public:
			void SetBias(float bias);
			void SetProjNear(float n);
			void SetColor(const Color& color);
			void SetUseVSM(bool b);

			GET_ACCESSOR(Bias,								float,						_bias);
			GET_ACCESSOR(Color,								const Color&,				_color);
			GET_ACCESSOR(ProjectionNear,					float,						_projNear);
			GET_ACCESSOR(UseVSM,							bool,						_useVSM);

			GET_ACCESSOR(InvNearFarViewProjectionMatrix,	const Math::Matrix&,		_invNearFarViewProjMat);
			GET_ACCESSOR(ViewProjectionMatrix,				const Math::Matrix&,		_viewProjMat);
			GET_ACCESSOR(Owner,								const Light::LightForm*,	_owner);

			GET_ACCESSOR(ParamUpdateCounter,				uint,						_paramUpdateCounter);
			GET_ACCESSOR(TransformUpdateCounter,			uint,						_transformUpdateCounter);
		};
	}
}