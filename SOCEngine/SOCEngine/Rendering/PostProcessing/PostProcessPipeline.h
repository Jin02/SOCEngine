#pragma once

#include "IBLPass.h"
#include "BackBufferMaker.h"
#include "GaussianBlur.h"
#include "DepthOfField.h"

namespace Rendering
{
	class PostProcessPipeline
	{
	private:
		Texture::RenderTexture*					_result;

	private:
		PostProcessing::FullScreen*				_copy;
		PostProcessing::GaussianBlur*			_gaussianBlur;
		PostProcessing::DepthOfField*			_dof;

		PostProcessing::IBLPass*				_iblPass;


		PostProcessing::BackBufferMaker*		_backBufferMaker;

		std::vector<Texture::RenderTexture*>	_downSampledTextures;
		Texture::RenderTexture*					_tempHalfMap;
		Texture::RenderTexture*					_bluredCurScene;

	public:
		PostProcessPipeline();
		~PostProcessPipeline();

	public:
		void Initialize(const Device::DirectX* dx, const Math::Size<uint>& resultTextureSize, uint downSampledTextureCount);
		void Destroy();

	public:
		void Render(const Device::DirectX* dx, const Texture::RenderTexture* outRenderTarget,
					const Camera::MeshCamera* mainMeshCamera, const Sky::SkyForm* sky);

	public:
		GET_ACCESSOR(DepthOfField, PostProcessing::DepthOfField*, _dof);
	};
}