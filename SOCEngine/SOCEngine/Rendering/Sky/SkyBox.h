#pragma once

#include "SkyForm.h"
#include "SkyBoxMaterial.h"

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
			SkyBoxMaterial*		_material;
			Math::Matrix		_prevWorldViewProjMat;

		public:
			SkyBox();
			virtual ~SkyBox();

		public:
			void Initialize(const std::string& materialName, const std::string& cubeMapFilePath);
			void Initialize(SkyBoxMaterial* material);

			virtual void Destroy();

		public:
			virtual void Render(const Device::DirectX* dx, const Camera::CameraForm* camera, const Texture::RenderTexture*& renderTarget, const Texture::DepthBuffer*& opaqueDepthBuffer);

		public:
			GET_SET_ACCESSOR(Material, SkyBoxMaterial*, _material);
		};
	}
}