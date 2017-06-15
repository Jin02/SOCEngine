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
using namespace Device;

void PostProcessPipeline::Initialize(DirectX& dx, ShaderManager& shaderMgr)
{
	// Texture
	{
		auto backBufferSize = dx.GetBackBufferSize().Cast<uint>();

		_bluredCurScene.Initialize(dx, backBufferSize, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN, 0, 1, 1);
		_tempResultMap.Initialize(dx, backBufferSize, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN, 0, 1, 1);

		for (uint i = 0; i < _tempTextures.downScaledTextures.size(); ++i)
		{
			auto size = Size<uint>(backBufferSize.w / (1 << (i + 1)), backBufferSize.h / (1 << (i + 1)));
			_tempTextures.downScaledTextures[i].Initialize(dx, size, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN, 0, 1, 1);
		}

		_tempTextures.originSizeMap.Initialize(dx, backBufferSize, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN, 0, 1, 1);
		_tempTextures.halfSizeMap.Initialize(dx, Size<uint>(backBufferSize.w / 2, backBufferSize.h / 2), DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN, 0, 1, 1);
		_tempTextures.minSizeMap.Initialize(dx, _tempTextures.downScaledTextures.back().GetSize(), DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN, 0, 1, 1);
	}

	_depthOfField.Initialize(dx, shaderMgr);
	_bloom.Initialize(dx, shaderMgr);
	_ssao.Initialize(dx, shaderMgr);
	_copy.Initialize(dx, shaderMgr);
}

void PostProcessPipeline::Render(	DirectX& dx,
									MainRenderer& mainRenderer,
									MainCamera& mainMeshCamera	)
{
	MainRenderingSystemParam mains(mainRenderer, mainMeshCamera);

	RenderTexture& mainScene	= mainRenderer.GetResultMap();
	mainScene.GetTexture2D().GenerateMips(dx);

	RenderTexture& back			= _tempResultMap;
	RenderTexture& front		= mainScene;

	dx.ClearDeviceContext();

	_bloom.RenderThresholdMap(dx, mainScene, _copy, _tempTextures, mainRenderer);

	if(_useSSAO)
	{
		_ssao.Render(dx, back, front, mainRenderer);
		std::swap(front, back);
	}

	if(_useDoF)
	{
		_depthOfField.Render(dx, back, front, mains, _copy, _tempTextures);
		std::swap(front, back);
	}

	_bloom.RenderBloom(dx, back, front, mainRenderer);
	std::swap(front, back);
}
