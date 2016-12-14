#pragma once

#include "FullScreen.h"
#include "GaussianBlur.h"
#include "MeshCamera.h"
#include "LightManager.h"

namespace Rendering
{
	namespace Sky
	{
		class SkyScattering
		{
		public:
			struct Param
			{
				union
				{
					struct{ ushort rayleigh, turbidity; };
					uint rayleighWithTurbidity;
				};
				union
				{
					struct{ ushort dlIndex, luminance; };
					uint dlIndexWithLuminance;
				};

				float mieCoefficient;
				float mieDirectionalG;

				bool operator==(const Param& a) const
				{
					return	(mieCoefficient == a.mieCoefficient) &&
							(mieDirectionalG == a.mieDirectionalG) &&
							(rayleighWithTurbidity == a.rayleighWithTurbidity) &&
							(dlIndexWithLuminance == a.dlIndexWithLuminance);
				}
			};

		private:
			Shader::VertexShader*			_vs;
			Shader::PixelShader*			_ps;

			Param							_prevParam;
			Buffer::ConstBuffer*			_paramCB;

		public:
			SkyScattering();
			~SkyScattering();

		public:
			void Initialize();
			void UpdateParam(const Param& param);
			void Render(const Device::DirectX* dx, const Texture::RenderTexture* out, const Camera::MeshCamera* mainCam, const Manager::LightManager* lightMgr);
			void Destroy();

		public:
			GET_ACCESSOR(Param, const Param&, _prevParam);
		};
	}
}