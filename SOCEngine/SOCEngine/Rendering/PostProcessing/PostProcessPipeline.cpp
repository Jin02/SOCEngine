#include "PostProcessPipeline.h"

using namespace Math;
using namespace Rendering;
using namespace Rendering::Buffer;
using namespace Rendering::Shader;
using namespace Rendering::PostProcessing;
using namespace Rendering::Texture;

PostProcessPipeline::PostProcessPipeline()
	: _iblPass(nullptr), _backBufferMaker(nullptr), _result(nullptr), _gaussianBlur(nullptr), _tempHalfMap(nullptr), _bluredCurScene(nullptr), _dof(nullptr),
	_currentAdaptedLuminanceIndx(0), _eyeAdaptation(nullptr), _bloomThreshold(nullptr), _hdrGlobalParamCB(nullptr), _bloomThresholdMap(nullptr),
	_tempDownSampledMinimumSizeMap(nullptr), _bloom(nullptr), _tempMap(nullptr), _ssao(nullptr)
{
	_adaptedLuminanceMaps.fill(nullptr);
}

PostProcessPipeline::~PostProcessPipeline()
{
	Destroy();

	SAFE_DELETE(_iblPass);
	SAFE_DELETE(_backBufferMaker);
	SAFE_DELETE(_result);
	SAFE_DELETE(_copy);
	SAFE_DELETE(_gaussianBlur);
	SAFE_DELETE(_tempHalfMap);
	SAFE_DELETE(_bluredCurScene);
	SAFE_DELETE(_dof);
	SAFE_DELETE(_bloomThreshold);
	SAFE_DELETE(_eyeAdaptation);
	SAFE_DELETE(_hdrGlobalParamCB);
	SAFE_DELETE(_bloomThresholdMap);
	SAFE_DELETE(_tempDownSampledMinimumSizeMap);
	SAFE_DELETE(_bloom);
	SAFE_DELETE(_tempMap);
	SAFE_DELETE(_ssao);

	for(uint i=0; i<_adaptedLuminanceMaps.size(); ++i)
		SAFE_DELETE(_adaptedLuminanceMaps[i]);
}

void PostProcessPipeline::Initialize(const Device::DirectX* dx, const Math::Size<uint>& resultTextureSize, uint downSampledTextureCount)
{
	_iblPass = new IBLPass;
	_iblPass->Initialize();

	_backBufferMaker = new BackBufferMaker;
	_backBufferMaker->Initialize(false);

	// Texture
	{
		auto CreateRT = [](const Size<uint>& size, DXGI_FORMAT format) -> RenderTexture*
		{
			RenderTexture* rt = new RenderTexture;
			rt->Initialize(size, format, format, DXGI_FORMAT_UNKNOWN, 0, 1);
	
			return rt;
		};
	
		_result = CreateRT(resultTextureSize, DXGI_FORMAT_R16G16B16A16_FLOAT);
		_tempMap = CreateRT(resultTextureSize, DXGI_FORMAT_R16G16B16A16_FLOAT);
		_bluredCurScene = CreateRT(resultTextureSize, DXGI_FORMAT_R16G16B16A16_FLOAT);
		_bloomThresholdMap = CreateRT(resultTextureSize, DXGI_FORMAT_R16G16B16A16_FLOAT);
	
		for(uint i=0; i<downSampledTextureCount; ++i)
		{
			Size<uint> size = Size<uint>(resultTextureSize.w / (1 << (i+1)), resultTextureSize.h / (1 << (i+1)));
			_downSampledTextures.push_back( CreateRT(size, DXGI_FORMAT_R16G16B16A16_FLOAT) );
		}
	
		Size<uint> size = Size<uint>(resultTextureSize.w / 2, resultTextureSize.h / 2);
		_tempHalfMap = CreateRT(size, DXGI_FORMAT_R16G16B16A16_FLOAT);
	
		for(uint i=0; i<_adaptedLuminanceMaps.size(); ++i)
			_adaptedLuminanceMaps[i] = CreateRT(Size<uint>(1, 1), DXGI_FORMAT_R32_FLOAT);
	
		_tempDownSampledMinimumSizeMap = CreateRT(_downSampledTextures.back()->GetSize(), DXGI_FORMAT_R16G16B16A16_FLOAT);
	}

	// shader
	{
		_gaussianBlur = new GaussianBlur;
		_gaussianBlur->Initialize(dx);

		_copy = new FullScreen;
		_copy->Initialize("Copy", "Copy_InFullScreen_PS", true, nullptr);

		_eyeAdaptation = new FullScreen;
		_eyeAdaptation->Initialize("EyeAdaptation", "EyeAdaptation_InFullScreen_PS", true, nullptr);

		_bloomThreshold = new FullScreen;
		_bloomThreshold->Initialize("Bloom", "Bloom_Threshold_InFullScreen_PS", true, nullptr);

		_bloom = new FullScreen;
		_bloom->Initialize("Bloom", "Bloom_InFullScreen_PS", true, nullptr);

		_dof = new DepthOfField;
		_dof->Initialize();

		_ssao = new SSAO;
		_ssao->Initialize();
	}

	// const buffer
	{
		_hdrGlobalParamCB = new ConstBuffer;
		_hdrGlobalParamCB->Initialize(sizeof(GlobalParam));
	}

	// Param
	{
		_globalParam.dt					= 0.0f;
		_globalParam.bloomThreshold		= 0.3f;
		_globalParam.exposureKey		= 0.1f;
		_globalParam.exposureSpeed		= 0.01f;

		{
			DepthOfField::ParamCB param;
			param.start = 0.0f;
			param.end = 0.0f;

			_dof->UpdateParam(param);
		}

		{
			GaussianBlur::ParamCB param;
			param.blurSize			= 2.5f;
			param.sigma				= 6.0f;
			param.numPixelPerSide	= 8.0f;
			param.scale				= 1.0f;

			_gaussianBlur->UpdateParam(dx, param);
		}
	}
}

void PostProcessPipeline::UpdateGlobalParam(const Device::DirectX* dx)
{
	_hdrGlobalParamCB->UpdateSubResource(dx->GetContext(), &_globalParam);
}

void PostProcessPipeline::Render(const Device::DirectX* dx,
								 const Texture::RenderTexture* outBackBuffer,
								 const Camera::MeshCamera* mainMeshCamera,
								 const Sky::SkyForm* sky)
{
	if(mainMeshCamera == nullptr)
		return;

	RenderTexture* mainScene	= mainMeshCamera->GetRenderTarget();
	RenderTexture* back		= _result;
	RenderTexture* front	= mainScene;

	mainScene->GenerateMips(dx);

	dx->ClearDeviceContext();
	ID3D11DeviceContext* context = dx->GetContext();
	
	auto CopyMap = [&](const Texture::RenderTexture* out, const Texture::RenderTexture* in)
	{
		PixelShader::BindSamplerState(context, SamplerStateBindIndex::DefaultSamplerState, dx->GetSamplerStateLinear());
		PixelShader::BindTexture(context, TextureBindIndex(0), in);
		_copy->Render(dx, out, true);
		PixelShader::BindTexture(context, TextureBindIndex(0), nullptr);
		PixelShader::BindSamplerState(context, SamplerStateBindIndex::DefaultSamplerState, nullptr);
	};

	PixelShader::BindTexture(context,		TextureBindIndex(0),						mainScene);
	PixelShader::BindTexture(context,		TextureBindIndex(1),						_adaptedLuminanceMaps[!_currentAdaptedLuminanceIndx]);
	PixelShader::BindSamplerState(context,	SamplerStateBindIndex::DefaultSamplerState,	dx->GetSamplerStateLinear());
	PixelShader::BindConstBuffer(context,	ConstBufferBindIndex::HDRGlobalParamCB,		_hdrGlobalParamCB);

	// Eye Adaption
	_eyeAdaptation->Render(dx, _adaptedLuminanceMaps[_currentAdaptedLuminanceIndx], true);

	// Bloom Map
	{
		_bloomThreshold->Render(dx, _bloomThresholdMap, true);

		// Blur
		{
			// Down Scale
			{
				CopyMap(_downSampledTextures[0], _bloomThresholdMap);		// source	-> /2
				CopyMap(_downSampledTextures[1], _downSampledTextures[0]);	// /2		-> /4
				CopyMap(_downSampledTextures[2], _downSampledTextures[1]);	// /4		-> /8
				CopyMap(_downSampledTextures[3], _downSampledTextures[2]);	// /8		-> /16
			}

			for(uint i=0; i<2; ++i)
				_gaussianBlur->Render(dx, _downSampledTextures[3], _downSampledTextures[3], _tempDownSampledMinimumSizeMap);
				
			// Up Scale
			{
				CopyMap(_downSampledTextures[2],	_downSampledTextures[3]);	// /16		-> /8
				CopyMap(_downSampledTextures[1],	_downSampledTextures[2]);	// /8		-> /4
				CopyMap(_downSampledTextures[0],	_downSampledTextures[1]);	// /4		-> /2
				CopyMap(_bloomThresholdMap,			_downSampledTextures[0]);	// /2		-> source
			}
		}
	}

	PixelShader::BindTexture(context,		TextureBindIndex(0),						nullptr);
	PixelShader::BindTexture(context,		TextureBindIndex(1),						nullptr);	
	PixelShader::BindSamplerState(context,	SamplerStateBindIndex::DefaultSamplerState,	nullptr);
	PixelShader::BindConstBuffer(context,	ConstBufferBindIndex::HDRGlobalParamCB,		nullptr);

	// SSAO
	_ssao->Render(dx, back, front, mainMeshCamera);
	std::swap(front, back);

	// Depth Of Field
	{
		// Down Scale
		CopyMap(_downSampledTextures[0], front);

		for(uint i=0; i<2; ++i)
			_gaussianBlur->Render(dx, _downSampledTextures[0], _downSampledTextures[0], _tempHalfMap);

		// Up Scale
		CopyMap(_bluredCurScene, _downSampledTextures[0]);

		_dof->Render(dx, back, front, mainMeshCamera, _bluredCurScene);
	}

	std::swap(front, back);

	// Bloom Merger, Tone Mapping, Gamma Correction
	{
		PixelShader::BindTexture(context,		TextureBindIndex(0),							front);
		PixelShader::BindTexture(context,		TextureBindIndex(1),							_adaptedLuminanceMaps[!_currentAdaptedLuminanceIndx]);
		PixelShader::BindTexture(context,		TextureBindIndex(2),							_bloomThresholdMap);
		PixelShader::BindSamplerState(context,	SamplerStateBindIndex::DefaultSamplerState,		dx->GetSamplerStateLinear());
		PixelShader::BindConstBuffer(context,	ConstBufferBindIndex::HDRGlobalParamCB,			_hdrGlobalParamCB);
		PixelShader::BindConstBuffer(context,	ConstBufferBindIndex::TBRParam,					mainMeshCamera->GetTBRParamConstBuffer());

		_bloom->Render(dx, back, true);

		PixelShader::BindTexture(context,		TextureBindIndex(0),							nullptr);
		PixelShader::BindTexture(context,		TextureBindIndex(1),							nullptr);
		PixelShader::BindTexture(context,		TextureBindIndex(2),							nullptr);
		PixelShader::BindSamplerState(context,	SamplerStateBindIndex::DefaultSamplerState,		nullptr);
		PixelShader::BindConstBuffer(context,	ConstBufferBindIndex::HDRGlobalParamCB,			nullptr);
		PixelShader::BindConstBuffer(context,	ConstBufferBindIndex::TBRParam,					nullptr);
	}

	std::swap(front, back);

	// Restore Viewport
	{
		D3D11_VIEWPORT viewport;
		viewport.TopLeftX	= 0.0f;
		viewport.TopLeftY	= 0.0f;
		viewport.MinDepth	= 0.0f;
		viewport.MaxDepth	= 1.0f;
		viewport.Width		= outBackBuffer->GetSize().Cast<float>().w;
		viewport.Height		= outBackBuffer->GetSize().Cast<float>().h;
		
		context->RSSetViewports(1, &viewport);
	}

	_backBufferMaker->Render(outBackBuffer->GetRenderTargetView(),
							front, nullptr, mainMeshCamera->GetTBRParamConstBuffer());

	_currentAdaptedLuminanceIndx = !_currentAdaptedLuminanceIndx;
}

void PostProcessPipeline::Destroy()
{
	_dof->Destroy();
	_copy->Destroy();
	_iblPass->Destroy();
	_backBufferMaker->Destroy();
	_result->Destroy();
	_gaussianBlur->Destroy();
	_tempHalfMap->Destroy();
	_bluredCurScene->Destroy();
	_bloomThresholdMap->Destroy();
	_tempDownSampledMinimumSizeMap->Destroy();
	_bloom->Destroy();
	_tempMap->Destroy();
	_ssao->Destroy();

	uint size = _downSampledTextures.size();
	for(uint i=0; i<size; ++i)
	{
		_downSampledTextures[i]->Destroy();
		SAFE_DELETE(_downSampledTextures[i]);
	}

	_downSampledTextures.clear();

	for(auto& iter : _adaptedLuminanceMaps)
		iter->Destroy();

	_eyeAdaptation->Destroy();
	_bloomThreshold->Destroy();
	_hdrGlobalParamCB->Destroy();
}