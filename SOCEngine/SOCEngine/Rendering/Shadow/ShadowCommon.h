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
			struct Param
			{
				union
				{
					struct
					{
						ushort	projFar, projNear;			// x
						ushort	softness, underScanSize;	// y

						uchar	flag, bias;					// z
						ushort	lightIndex;					// z

						uint	shadowColor;				// a
					};

					uint packedParam[4];
				};
			};

		private:
			float						_projNear;
			float						_projFar;			//new
			float						_underScanSize;		//new
			float						_softness;			//new
			float						_bias;
			uchar						_flag;				//new

			Color						_color; //a is strength
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
			void MakeParam(Param& outParam, uint lightIndex) const;

		public:
			GET_ACCESSOR(UseVSM,							bool,						_useVSM);
			SET_SHADOW_ACCESSOR(UseVSM,						bool,						_useVSM,		_paramUpdateCounter);

			GET_ACCESSOR(InvNearFarViewProjectionMatrix,	const Math::Matrix&,		_invNearFarViewProjMat);
			GET_ACCESSOR(ViewProjectionMatrix,				const Math::Matrix&,		_viewProjMat);
			GET_ACCESSOR(Owner,								const Light::LightForm*,	_owner);

			GET_ACCESSOR(ParamUpdateCounter,				uint,						_paramUpdateCounter);



			GET_ACCESSOR(ProjectionNear,					float,						_projNear);
			void SetProjectionNear(float n);

			GET_ACCESSOR(ProjectionFar,						float,						_projFar);
			SET_SHADOW_ACCESSOR(ProjectionFar,				float,						_projFar,		_paramUpdateCounter);

			GET_ACCESSOR(UnderScanSize,						float,						_underScanSize);
			SET_SHADOW_ACCESSOR(UnderScanSize,				float,						_underScanSize,	_paramUpdateCounter);

			GET_ACCESSOR(Softness,							float,						_softness);
			SET_SHADOW_ACCESSOR(Softness,					float,						_softness,		_paramUpdateCounter);

			GET_ACCESSOR(Bias,								float,						_bias);
			SET_SHADOW_ACCESSOR(Bias,						float,						_bias,			_paramUpdateCounter);

			GET_ACCESSOR(Flag,								uchar,						_flag);

			GET_ACCESSOR(Color,								const Color&,				_color);
			SET_SHADOW_ACCESSOR(Color,						const Color&,				_color,			_paramUpdateCounter);
		};
	}
}