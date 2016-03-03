#pragma once

#include "IBLPass.h"
#include "BackBufferMaker.h"

namespace Rendering
{
	class PostProcessPipeline
	{
	private:
		Texture::RenderTexture*					_result;

	private:
		PostProcessing::IBLPass*				_iblPass;
		PostProcessing::BackBufferMaker*		_backBufferMaker;

	public:
		PostProcessPipeline();
		~PostProcessPipeline();

	public:
		void Initialize(const Math::Size<uint>& resultTextureSize);
		void Destroy();

	public:
		void Render(const Device::DirectX* dx, const Texture::RenderTexture* outRenderTarget,
					const Camera::MeshCamera* mainMeshCamera, const Sky::SkyForm* sky);
	};
}