#pragma once

#include "SkyForm.h"
#include "SkyBoxMaterial.h"
#include "SkyBoxCubeMaterial.h"

namespace Rendering
{
	namespace Sky
	{
		class SkyBox : public SkyForm
		{
		public:
			struct InitParam
			{
				std::string materialName;
				std::string cubeMapFilePath;

				SkyBoxMaterial* material;
				InitParam() : material(nullptr) {}
				~InitParam() {}
			};

		private:
			Math::Matrix			_prevWorldViewProjMat;

			SkyBoxMaterial*			_skyBoxMaterial;
			SkyBoxCubeMaterial*		_skyBoxMaterialForReflectionProbe;

			Texture::Texture2D*		_cubeMap;

		public:
			SkyBox();
			virtual ~SkyBox();

		public:
			void Initialize(const std::string& materialName, const std::string& cubeMapFilePath);
			virtual void Destroy();

		public:
			virtual void Render(const Device::DirectX* dx, const Camera::CameraForm* camera, const Texture::RenderTexture* renderTarget, const Texture::DepthBuffer* opaqueDepthBuffer);
			virtual void Render(const Device::DirectX* dx, const Camera::ReflectionProbe* probe, const Texture::TextureCube* renderTarget, const Texture::DepthBufferCube* opaqueDepthBuffer);

		public:
			GET_ACCESSOR(SkyCubeMap, const Texture::Texture2D*, _cubeMap);
		};
	}
}