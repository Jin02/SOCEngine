#include "DepthOfField.h"

using namespace Device;
using namespace Rendering;
using namespace Rendering::Buffer;
using namespace Rendering::Shader;
using namespace Rendering::PostProcessing;
using namespace Rendering::Texture;
using namespace Rendering::Renderer;
using namespace Rendering::RenderState;
using namespace Rendering::Camera;

void DepthOfField::Initialize(Device::DirectX& dx, Manager::ShaderManager& shaderMgr, const Size<uint>& renderSize)
{
	FullScreen::InitParam param;
	{
		param.psMacros			= nullptr;
		param.psName			= "DoF_InFullScreen_PS";
		param.shaderFileName	= "DepthOfField";
	}

	_screen.Initialize(dx, param, shaderMgr);

	_paramCB.Initialize(dx);
	_paramCB.UpdateSubResource(dx, ParamCBData());

	_blur.Initialize(dx, shaderMgr);
	_blurredColorMap.Initialize(dx, renderSize, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN, 0, 1, 1);
}

void DepthOfField::UpdateParamCB(Device::DirectX& dx)
{
	if(_dirty == false)
		return;

	_paramCB.UpdateSubResource(dx, _paramData);
	_dirty = false;
}

void DepthOfField::Render(DirectX& dx, RenderTexture& outRT, const RenderTexture& inColorMap, const MainRenderingSystemParam&& mains, const Copy& copy, TempTextures& tempTextures)
{
	// Blur color map
	{
		// down scale
		copy.Render(dx, tempTextures.downScaledTextures[0], inColorMap);

		for (uint i = 0; i < 2; ++i)
			_blur.Render(dx, tempTextures.downScaledTextures[0], tempTextures.downScaledTextures[0], tempTextures.halfSizeMap);

		// up
		copy.Render(dx, _blurredColorMap, tempTextures.downScaledTextures[0]);
	}

	PixelShader::BindShaderResourceView(dx, TextureBindIndex(0),				inColorMap.GetTexture2D().GetShaderResourceView());
	PixelShader::BindShaderResourceView(dx, TextureBindIndex(1),				_blurredColorMap.GetTexture2D().GetShaderResourceView());
	PixelShader::BindShaderResourceView(dx, TextureBindIndex::GBuffer_Depth,	mains.renderer.GetGBuffers().opaqueDepthBuffer.GetTexture2D().GetShaderResourceView());

	PixelShader::BindConstBuffer(dx, ConstBufferBindIndex::TBRParam,			mains.renderer.GetTBRParamCB());
	PixelShader::BindConstBuffer(dx, ConstBufferBindIndex(1),					_paramCB);
	PixelShader::BindConstBuffer(dx, ConstBufferBindIndex::Camera,				mains.camera.GetCameraCB());

	PixelShader::BindSamplerState(dx, SamplerStateBindIndex::DefaultSamplerState, SamplerState::Linear);

	_screen.Render(dx, outRT, true);

	PixelShader::UnBindShaderResourceView(dx, TextureBindIndex(0));
	PixelShader::UnBindShaderResourceView(dx, TextureBindIndex(1));
	PixelShader::UnBindShaderResourceView(dx, TextureBindIndex::GBuffer_Depth);

	PixelShader::UnBindConstBuffer(dx, ConstBufferBindIndex::TBRParam);
	PixelShader::UnBindConstBuffer(dx, ConstBufferBindIndex(1));
	PixelShader::UnBindConstBuffer(dx, ConstBufferBindIndex::Camera);

	PixelShader::UnBindSamplerState(dx, SamplerStateBindIndex::DefaultSamplerState);
}