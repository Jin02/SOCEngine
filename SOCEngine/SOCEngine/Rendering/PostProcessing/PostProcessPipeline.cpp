#include "PostProcessPipeline.h"

using namespace Math;
using namespace Rendering;
using namespace Rendering::Shader;
using namespace Rendering::PostProcessing;
using namespace Rendering::Texture;

PostProcessPipeline::PostProcessPipeline()
	: _iblPass(nullptr), _backBufferMaker(nullptr), _result(nullptr), _gaussianBlur(nullptr), _tempHalfMap(nullptr), _bluredCurScene(nullptr), _dof(nullptr)
{
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
}

void PostProcessPipeline::Initialize(const Device::DirectX* dx, const Math::Size<uint>& resultTextureSize, uint downSampledTextureCount)
{
	_iblPass = new IBLPass;
	_iblPass->Initialize();

	_backBufferMaker = new BackBufferMaker;
	_backBufferMaker->Initialize(false);

	_result = new RenderTexture;
	_result->Initialize(resultTextureSize, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 1);

	auto CreateRT = [](const Size<uint>& size) -> RenderTexture*
	{
		RenderTexture* rt = new RenderTexture;
		rt->Initialize(size, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN, 0, 1);

		return rt;
	};

	for(uint i=0; i<downSampledTextureCount; ++i)
	{
		Size<uint> size = Size<uint>(resultTextureSize.w / (1 << (i+1)), resultTextureSize.h / (1 << (i+1)));
		_downSampledTextures.push_back( CreateRT(size) );
	}

	Size<uint> size = Size<uint>(resultTextureSize.w / 2, resultTextureSize.h / 2);
	_tempHalfMap = CreateRT(size);

	_copy = new FullScreen;
	_copy->Initialize("Copy", "Copy_InFullScreen_PS", nullptr);

	_gaussianBlur = new GaussianBlur;
	_gaussianBlur->Initialize(dx);
	{
		GaussianBlur::ParamCB param;
		param.blurSize			= 5.0f;
		param.sigma				= 12.0f;
		param.numPixelPerSide	= 16.0f;
		param.scale				= 1.0f;

		_gaussianBlur->UpdateParam(dx, param);
	}

	_bluredCurScene = CreateRT(resultTextureSize);

	_dof = new DepthOfField;
	_dof->Initialize();

	DepthOfField::ParamCB param;
	{
		param.start = 0.0f;
		param.end = 0.0f;

		_dof->UpdateParam(param);
	}
}

void PostProcessPipeline::Render(const Device::DirectX* dx,
								 const Texture::RenderTexture* outBackBuffer,
								 const Camera::MeshCamera* mainMeshCamera,
								 const Sky::SkyForm* sky)
{
	if(mainMeshCamera == nullptr)
		return;

	const RenderTexture* mainScene = mainMeshCamera->GetRenderTarget();

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

	// Depth Of Field
	{
		// Down Scale
		CopyMap(_downSampledTextures[0], mainScene);

		for(uint i=0; i<2; ++i)
			_gaussianBlur->Render(dx, _downSampledTextures[0], _downSampledTextures[0], _tempHalfMap);

		// Up Scale
		CopyMap(_bluredCurScene, _downSampledTextures[0]);

		_dof->Render(dx, _result, mainMeshCamera, _bluredCurScene);
	}


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
							_result, nullptr, mainMeshCamera->GetTBRParamConstBuffer());
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

	uint size = _downSampledTextures.size();
	for(uint i=0; i<size; ++i)
	{
		_downSampledTextures[i]->Destroy();
		SAFE_DELETE(_downSampledTextures[i]);
	}

	_downSampledTextures.clear();
}