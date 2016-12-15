#pragma once

#include "GaussianBlur.h"
#include "MeshCamera.h"
#include "LightManager.h"
#include "MeshFilter.h"
#include "Matrix.h"

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
			struct SSTransform
			{
				Math::Matrix worldMat;
				Math::Matrix worldViewProjMat;
			};

		private:
			Math::Matrix					_prevWorldMat;
			Param							_prevParam;

		private:
			Shader::VertexShader*			_vs;
			Shader::PixelShader*			_ps;

			Geometry::MeshFilter*			_meshFilter;

			Buffer::ConstBuffer*			_paramCB;
			Buffer::ConstBuffer*			_worldMatCB;

		public:
			SkyScattering();
			~SkyScattering();

		public:
			void Initialize();
			void UpdateParam(const Param& param);
			void UpdateWorldMat(const Camera::CameraForm* mainCam);
			void Render(const Device::DirectX* dx, const Texture::RenderTexture* out, const Camera::MeshCamera* mainCam, const Manager::LightManager* lightMgr);
			void Destroy();

		public:
			GET_ACCESSOR(Param, const Param&, _prevParam);
		};
	}
}