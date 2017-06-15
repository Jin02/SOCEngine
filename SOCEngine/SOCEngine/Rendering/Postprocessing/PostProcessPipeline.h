#pragma once

#include "GaussianBlur.h"
#include "DepthOfField.h"
#include "SSAO.h"
#include "Bloom.h"

namespace Rendering
{
	namespace PostProcessing
	{
		class PostProcessPipeline
		{
		private:
			TempTextures							_tempTextures;

			Texture::RenderTexture					_tempResultMap;
			Texture::RenderTexture					_bluredCurScene;

			DepthOfField							_depthOfField;
			Bloom									_bloom;
			SSAO									_ssao;
			Copy									_copy;

			bool									_useSSAO = false;
			bool									_useDoF = false;

		public:
			GET_SET_ACCESSOR(UseSSAO, bool, _useSSAO);
			GET_SET_ACCESSOR(UseDoF, bool, _useDoF);

		public:
			void Initialize(Device::DirectX& dx, Manager::ShaderManager& shaderMgr);
			void Render(Device::DirectX& dx, Renderer::MainRenderer& mainRenderer, Camera::MainCamera& mainMeshCamera);
		};
	}
}