#include "PostProcessPipeline.h"
#include "AutoBinder.hpp"

using namespace Math;
using namespace Rendering;
using namespace Rendering::Buffer;
using namespace Rendering::Shader;
using namespace Rendering::PostProcessing;
using namespace Rendering::Texture;
using namespace Rendering::Camera;
using namespace Rendering::Renderer;
using namespace Rendering::Manager;
using namespace Rendering::RenderState;
using namespace Device;
using namespace Core;

void PostProcessPipeline::Initialize(DirectX& dx, ShaderManager& shaderMgr, const MainCamera& mainCamera)
{
	const auto& renderSize = mainCamera.GetRenderRect().size;
	// Texture
	{
		_tempResultMap.Initialize(dx, renderSize, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN, 0, 1, 1);

		for (uint i = 0; i < _tempTextures.downScaledTextures.size(); ++i)
		{
			auto size = Size<uint>(renderSize.w / (1 << (i + 1)), renderSize.h / (1 << (i + 1)));
			_tempTextures.downScaledTextures[i].Initialize(dx, size, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN, 0, 1, 1);
		}

		_tempTextures.originSizeMap.Initialize(dx, renderSize, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN, 0, 1, 1);
		_tempTextures.halfSizeMap.Initialize(dx, Size<uint>(renderSize.w / 2, renderSize.h / 2), DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN, 0, 1, 1);
		_tempTextures.minSizeMap.Initialize(dx, _tempTextures.downScaledTextures.back().GetSize(), DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN, 0, 1, 1);
	}

	GetPostproessing<DepthOfField>().Initialize(dx, shaderMgr, renderSize);
	GetPostproessing<Bloom>().Initialize(dx, shaderMgr, renderSize);
	GetPostproessing<SSAO>().Initialize(dx, shaderMgr);
	GetPostproessing<SunShaft>().Initialize(dx, shaderMgr, mainCamera);
	_copy.Initialize(dx, shaderMgr);
}

void PostProcessPipeline::Render(DirectX& dx, MainRenderer& mainRenderer, const MainCamera& mainMeshCamera, const LightManager& lightMgr)
{
	MainRenderingSystemParam mains{mainRenderer, mainMeshCamera};

	RenderTexture* mainScene = mainRenderer.GetResultMap();
	mainScene->GetTexture2D()->GenerateMips(dx);

	RenderTexture* output		= &_tempTextures.originSizeMap;
	RenderTexture* input		= &_tempResultMap;

	dx.ClearDeviceContext();

	GetPostproessing<Bloom>().RenderThresholdMap(dx, *mainScene, _copy, _tempTextures, mainRenderer.GetTBRParamCB());

	if(_useSSAO)
	{
		GetPostproessing<SSAO>().Render(dx, *output, *mainScene, mainRenderer);
		std::swap(input, output);
	}

	if(_useDoF)
	{
		GetPostproessing<DepthOfField>().Render(dx, *output, _useSSAO ? *input : *mainScene, std::move(mains), _copy, _tempTextures);
		std::swap(input, output);
	}

	auto& sunShaft = GetPostproessing<SunShaft>();
	bool runSunShaft = _useSunShaft & sunShaft.GetRenderAble();
	if (runSunShaft)
	{
		GetPostproessing<SunShaft>().Render(dx, *output, *input->GetTexture2D(), mainRenderer, mainMeshCamera, lightMgr);
		std::swap(input, output);
	}

	bool allOff = !_useSSAO & !_useDoF & !runSunShaft;
	if (allOff)
	{
		AutoBinderSampler<PixelShader> sampler(dx, SamplerStateBindIndex(0), SamplerState::Point);
		_copy.Render(dx, *input, *mainScene->GetTexture2D());
	}

	GetPostproessing<Bloom>().RenderBloom(dx, *mainScene, *input, mainRenderer.GetTBRParamCB());
}

void PostProcessPipeline::UpdateCB(DirectX& dx, const ObjectManager& objMgr,
	const LightManager& lightMgr, const TransformPool& tfPool, const MainCamera& mainCam)
{
	GetPostproessing<Bloom>().UpdateParamCB(dx);
	if(_useDoF)			GetPostproessing<DepthOfField>().UpdateParamCB(dx);
	if(_useSSAO)		GetPostproessing<SSAO>().UpdateParamCB(dx);
	if(_useSunShaft)	GetPostproessing<SunShaft>().UpdateParamCB(dx, objMgr, lightMgr, tfPool, mainCam);
}
