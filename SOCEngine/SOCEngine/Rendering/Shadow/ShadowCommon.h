#pragma once

#include "Common.h"
#include "Color.h"
#include "Matrix.h"
#include <functional>

#include "GlobalDefine.h"

#include "BoundBox.h"

#define SET_SHADOW_ACCESSOR(name, type, variable, counter) inline void Set##name(type t)	{ variable = t; ++counter;}

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
			bool						_useVSM;

		protected:
			uint						_paramUpdateCounter;

			const Light::LightForm*		_owner;
			Math::Matrix				_invNearFarViewProjMat;
			Math::Matrix				_viewProjMat;
			Math::Matrix				_invViewProjViewportMat;

		public:
			ShadowCommon(const Light::LightForm* owner);
			~ShadowCommon();

		public:
			void Initialize();

		public:
			void MakeParam(CommonParam& outParam, uint lightIndex) const;

		public:
			GET_ACCESSOR(Bias,								float,						_bias);
			SET_SHADOW_ACCESSOR(Bias,						float,						_bias,			_paramUpdateCounter);

			GET_ACCESSOR(Color,								const Color&,				_color);
			SET_SHADOW_ACCESSOR(Color,						const Color&,				_color,			_paramUpdateCounter);

			GET_ACCESSOR(ProjectionNear,					float,						_projNear);
			void SetProjectionNear(float n);

			GET_ACCESSOR(UseVSM,							bool,						_useVSM);
			SET_SHADOW_ACCESSOR(UseVSM,						bool,						_useVSM,		_paramUpdateCounter);

			GET_ACCESSOR(InvNearFarViewProjectionMatrix,	const Math::Matrix&,		_invNearFarViewProjMat);
			GET_ACCESSOR(ViewProjectionMatrix,				const Math::Matrix&,		_viewProjMat);
			GET_ACCESSOR(Owner,								const Light::LightForm*,	_owner);

			GET_ACCESSOR(ParamUpdateCounter,				uint,						_paramUpdateCounter);
		};
	}
}