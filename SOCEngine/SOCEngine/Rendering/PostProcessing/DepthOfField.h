#pragma once

#include "FullScreen.h"
#include "GaussianBlur.h"
#include "MeshCamera.h"

namespace Rendering
{
	namespace PostProcessing
	{
		class DepthOfField
		{
		public:
			struct ParamCB
			{
				float fousuNear;
				float blurNear;
				float focusFar;
				float blurFar;
			};

		private:
			FullScreen*				_dof;

			ParamCB					_prevParam;
			Buffer::ConstBuffer*	_paramCB;

		public:
			DepthOfField();
			~DepthOfField();

		public:
			void Initialize();
			void UpdateParam(const ParamCB& param);
			void Render(const Device::DirectX* dx, const Texture::RenderTexture* out, const Texture::RenderTexture* in,
						const Camera::MeshCamera* mainCam, const Texture::Texture2D* bluredCurScene);
			void Destroy();

		public:
			GET_ACCESSOR(Param, const ParamCB&, _prevParam);
		};
	}
}