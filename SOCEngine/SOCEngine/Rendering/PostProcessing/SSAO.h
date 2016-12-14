#pragma once

#include "FullScreen.h"
#include "GaussianBlur.h"
#include "MeshCamera.h"
#include "MathCommon.h"

namespace Rendering
{
	namespace PostProcessing
	{
		class SSAO
		{
		public:
			struct Param
			{
				Math::Vector2	stepUV;

			private:
				union
				{
					struct { ushort sampleCount, scale; };
					uint pacekdSampleCountWithScale;
				};
				union
				{
					struct { ushort skipDist, occluedRate; };
					uint packedSkipDistWithOccluedRate;
				};

			public:
				bool operator==(const Param& a) const;

				void SetScale(float s);
				void SetSkipDist(float d);
				void SetOccluedRate(float r);
				SET_ACCESSOR(SampleCount, ushort, sampleCount);
			};

		private:
			FullScreen*				_ssao;

			Param					_prevParam;
			Buffer::ConstBuffer*	_paramCB;

		public:
			SSAO();
			~SSAO();

		public:
			void Initialize();
			void UpdateParam(const Param& param);
			void Render(const Device::DirectX* dx, const Texture::RenderTexture* out, const Texture::RenderTexture* in, const Camera::MeshCamera* mainCam);
			void Destroy();

		public:
			GET_ACCESSOR(Param, const Param&, _prevParam);
		};
	}
}