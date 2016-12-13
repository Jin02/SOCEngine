#pragma once

#include "IBLPass.h"
#include "BackBufferMaker.h"
#include "GaussianBlur.h"
#include "DepthOfField.h"

namespace Rendering
{
	class PostProcessPipeline
	{
	public:
		struct GlobalParam
		{
			float dt;
			float exposureKey;
			float exposureSpeed;
			float bloomThreshold;
		};
	private:
		GlobalParam								_globalParam;
		Texture::RenderTexture*					_result;

	private:
		PostProcessing::FullScreen*				_copy;
		PostProcessing::FullScreen*				_eyeAdaptation;
		PostProcessing::FullScreen*				_bloomThreshold;
		PostProcessing::FullScreen*				_bloom;

		PostProcessing::GaussianBlur*			_gaussianBlur;
		PostProcessing::DepthOfField*			_dof;

		PostProcessing::IBLPass*				_iblPass;


		PostProcessing::BackBufferMaker*		_backBufferMaker;

		std::vector<Texture::RenderTexture*>	_downSampledTextures;

		Texture::RenderTexture*					_tempMap;
		Texture::RenderTexture*					_tempHalfMap;
		Texture::RenderTexture*					_tempDownSampledMinimumSizeMap;

		Texture::RenderTexture*					_bluredCurScene;
		Texture::RenderTexture*					_bloomThresholdMap;

		Buffer::ConstBuffer*					_hdrGlobalParamCB;

		std::array<Texture::RenderTexture*, 2>	_adaptedLuminanceMaps; //prev, curr
		bool									_currentAdaptedLuminanceIndx;

	public:
		PostProcessPipeline();
		~PostProcessPipeline();

	public:
		void Initialize(const Device::DirectX* dx, const Math::Size<uint>& resultTextureSize, uint downSampledTextureCount);
		void Destroy();

	public:
		void UpdateGlobalParam(const Device::DirectX* dx);
		void Render(const Device::DirectX* dx, const Texture::RenderTexture* outRenderTarget,
					const Camera::MeshCamera* mainMeshCamera, const Sky::SkyForm* sky);

	public:
		GET_ACCESSOR(DepthOfField,		PostProcessing::DepthOfField*,		_dof);
		GET_SET_ACCESSOR(GlobalParam,	const GlobalParam&,					_globalParam);
		SET_ACCESSOR(DeltaTime,			float,								_globalParam.dt);
	};
}