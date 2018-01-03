#include "DepthOfField.h"
#include "AutoBinder.hpp"

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
	std::vector<ShaderMacro> psMacro{ dx.GetMSAAShaderMacro() };

	FullScreen::InitParam param;
	{
		param.psMacros			= nullptr;
		param.psName			= "DoF_InFullScreen_PS";
		param.shaderFileName	= "DepthOfField";
		param.psMacros			= &psMacro;
	}

	_screen.Initialize(dx, param, shaderMgr);

	_paramCB.Initialize(dx);
	_paramCB.UpdateSubResource(dx, ParamCBData());

	_blur.Initialize(dx, shaderMgr);
	{
		_blurParamCBData.sigma				= 1.5f;
		_blurParamCBData.numPixelPerSide	= 4.0f;
		_blurParamCBData.blurSize			= 4.0f;
		_blurParamCBData.scale				= 1.0f;
	}
	_blur.UpdateParamCB(dx, _blurParamCBData);

	_blurredColorMap.Initialize(dx, renderSize, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN, 0, 1, 1);
}

void DepthOfField::UpdateParamCB(Device::DirectX& dx)
{
	if(_dirty == false)
		return;

	_paramCB.UpdateSubResource(dx, _paramData);
	_blur.UpdateParamCB(dx, _blurParamCBData);

	_dirty = false;
}

void DepthOfField::Render(DirectX& dx, RenderTexture& outRT, const RenderTexture& inColorMap, const MainRenderingSystemParam&& mains, const Copy& copy, TempTextures& tempTextures)
{
	// Blur color map
	{
		// down scale
		{
			AutoBinderSampler<PixelShader> sampler(dx, SamplerStateBindIndex(0), SamplerState::Linear);
			copy.Render(dx, tempTextures.downScaledTextures[0], *inColorMap.GetTexture2D());
		}

		for (uint i = 0; i < 2; ++i)
			_blur.Render(dx, tempTextures.downScaledTextures[0], tempTextures.downScaledTextures[0], tempTextures.halfSizeMap);

		// up
		{
			AutoBinderSampler<PixelShader> sampler(dx, SamplerStateBindIndex(0), SamplerState::Linear);
			copy.Render(dx, _blurredColorMap, *tempTextures.downScaledTextures[0].GetTexture2D());
		}
	}

	AutoBinderSRV<PixelShader> color(dx,		TextureBindIndex(0),						inColorMap.GetTexture2D()->GetShaderResourceView());
	AutoBinderSRV<PixelShader> blurred(dx,		TextureBindIndex(1),						_blurredColorMap.GetTexture2D()->GetShaderResourceView());
	AutoBinderSRV<PixelShader> depth(dx,		TextureBindIndex::GBuffer_Depth,			mains.renderer.GetGBuffers().opaqueDepthBuffer.GetTexture2D().GetShaderResourceView());

	AutoBinderCB<PixelShader> tbrParam(dx,		ConstBufferBindIndex::TBRParam,				mains.renderer.GetTBRParamCB());
	AutoBinderCB<PixelShader> dofParam(dx,		ConstBufferBindIndex(1),					_paramCB);
	AutoBinderCB<PixelShader> camParam(dx,		ConstBufferBindIndex::Camera,				mains.camera.GetCameraCB());

	AutoBinderSampler<PixelShader> pointSampler(dx,	SamplerStateBindIndex(0),				SamplerState::Point);
	AutoBinderSampler<PixelShader> linearSampler(dx,SamplerStateBindIndex(1),				SamplerState::Linear);

	_screen.Render(dx, outRT, true);
}