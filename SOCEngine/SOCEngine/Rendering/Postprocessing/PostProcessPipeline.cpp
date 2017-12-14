#include "PostProcessPipeline.h"

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

void PostProcessPipeline::Initialize(DirectX& dx, ShaderManager& shaderMgr, const MainCamera& mainCamera)
{
	const auto& renderSize = mainCamera.GetRenderRect().size;
	// Texture
	{
		_bluredCurScene.Initialize(dx, renderSize, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN, 0, 1, 1);
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
	_copy.Initialize(dx, shaderMgr);
}

void PostProcessPipeline::Render(	DirectX& dx,
									MainRenderer& mainRenderer,
									const MainCamera& mainMeshCamera	)
{
	MainRenderingSystemParam mains{mainRenderer, mainMeshCamera};

	RenderTexture& mainScene = mainRenderer.GetResultMap();
	mainScene.GetTexture2D().GenerateMips(dx);

	RenderTexture& output		= _tempResultMap;
	RenderTexture& input		= mainScene;

	dx.ClearDeviceContext();

	GetPostproessing<Bloom>().RenderThresholdMap(dx, mainScene, _copy, _tempTextures, mainRenderer);

	if(_useSSAO)
	{
		GetPostproessing<SSAO>().Render(dx, output, input, mainRenderer);
		std::swap(input, output);
	}

	if(_useDoF)
	{
		GetPostproessing<DepthOfField>().Render(dx, output, input, std::move(mains), _copy, _tempTextures);
		std::swap(input, output);
	}

	GetPostproessing<Bloom>().RenderBloom(dx, output, input, mainRenderer);
	std::swap(input, output);
}

void PostProcessPipeline::UpdateCB(DirectX & dx)
{
	GetPostproessing<DepthOfField>().UpdateParamCB(dx);
	GetPostproessing<Bloom>().UpdateParamCB(dx);
	GetPostproessing<SSAO>().UpdateParamCB(dx);
}
