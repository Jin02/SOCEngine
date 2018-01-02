#include "Bloom.h"
#include "AutoBinder.hpp"

using namespace Device;
using namespace Rendering;
using namespace Rendering::Buffer;
using namespace Rendering::Shader;
using namespace Rendering::PostProcessing;
using namespace Rendering::Texture;
using namespace Rendering::Renderer;
using namespace Rendering::Camera;
using namespace Rendering::RenderState;

void Bloom::Initialize(Device::DirectX& dx, Manager::ShaderManager& shaderMgr, const Size<uint>& renderSize, bool use)
{
	using Param = FullScreen::InitParam;
	
	if (_use = use)
	{
		std::vector<ShaderMacro> macro{ShaderMacro("USE_BLOOM")};
		_bloom.Initialize(dx, Param("Bloom", "Bloom_InFullScreen_PS", &macro), shaderMgr);
		_bloomThreshold.Initialize(dx, Param("Bloom", "Bloom_Threshold_InFullScreen_PS", nullptr), shaderMgr);
		_eyeAdaptation.Initialize(dx, Param("EyeAdaptation", "EyeAdaptation_InFullScreen_PS", nullptr), shaderMgr);

		_blur.Initialize(dx, shaderMgr);

		GaussianBlur::ParamCBData param;
		{
			param.sigma				= 1.2f;
			param.numPixelPerSide	= 2.0f;
			param.blurSize			= 2.0f;
			param.scale				= 1.0f;
		};
		_blur.UpdateParamCB(dx, param);

		_adaptedLuminanceMaps[0].Initialize(dx, Size<uint>(1, 1), DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_UNKNOWN, 0, 1, 1);
		_adaptedLuminanceMaps[1].Initialize(dx, Size<uint>(1, 1), DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_UNKNOWN, 0, 1, 1);

		_bloomThresholdMap.Initialize(dx, renderSize, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN, 0, 1, 1);

		_paramCB.Initialize(dx);
		_paramCB.UpdateSubResource(dx, ParamCBData());
	}
	else
	{
		_bloom.Initialize(dx, Param("Bloom", "Bloom_InFullScreen_PS", nullptr), shaderMgr);
	}
}

void Bloom::Destroy()
{
	_bloom.Destroy();
	_bloomThreshold.Destroy();
	_eyeAdaptation.Destroy();

	_blur.Destroy();
	_paramCB.Destroy();

	_adaptedLuminanceMaps[0].Destroy();
	_adaptedLuminanceMaps[1].Destroy();
	_bloomThresholdMap.Destroy();

	_currentAdaptedLuminanceIndx	= false;
	_use							= false;
	_paramData						= ParamCBData();
}

void Bloom::UpdateParamCB(Device::DirectX& dx)
{
	_paramCB.UpdateSubResource(dx, _paramData);
}

void Bloom::RenderThresholdMap(DirectX& dx, const RenderTexture& inColorMap, const Copy& copy, TempTextures& temp, const TBRParamCB& tbrParamCB)
{
	// Render EyeAdaptation / BloomThreshold Map
	{
		AutoBinderSRV<PixelShader> inColor(dx,		TextureBindIndex(0),						inColorMap.GetTexture2D()->GetShaderResourceView());
		AutoBinderSRV<PixelShader> lumiance(dx,		TextureBindIndex(1),						_adaptedLuminanceMaps[!_currentAdaptedLuminanceIndx].GetTexture2D()->GetShaderResourceView());
	
		AutoBinderCB<PixelShader> hdrParam(dx,		ConstBufferBindIndex::HDRGlobalParamCB,		_paramCB);
		AutoBinderCB<PixelShader> tbrParam(dx,		ConstBufferBindIndex::TBRParam,				tbrParamCB);
	
		AutoBinderSampler<PixelShader> sampler(dx,	SamplerStateBindIndex(0),					SamplerState::Point);
	
		_eyeAdaptation.Render(dx, _adaptedLuminanceMaps[_currentAdaptedLuminanceIndx], true);
		_bloomThreshold.Render(dx, _bloomThresholdMap, true);
	}

	// Scalling / Blurring
	{
		// Down Scale
		{
			AutoBinderSampler<PixelShader> sampler(dx, SamplerStateBindIndex(0), SamplerState::Linear);

			copy.Render(dx, temp.downScaledTextures[0], *_bloomThresholdMap.GetTexture2D());			// source	-> /2
			copy.Render(dx, temp.downScaledTextures[1], *temp.downScaledTextures[0].GetTexture2D());	// /2		-> /4
			copy.Render(dx, temp.downScaledTextures[2], *temp.downScaledTextures[1].GetTexture2D());	// /4		-> /8
			copy.Render(dx, temp.downScaledTextures[3], *temp.downScaledTextures[2].GetTexture2D());	// /8		-> /16
		}

		for (uint i = 0; i<2; ++i)
			_blur.Render(dx, temp.downScaledTextures[3], temp.downScaledTextures[3], temp.minSizeMap);

		// Up Scale
		{
			AutoBinderSampler<PixelShader> sampler(dx, SamplerStateBindIndex(0), SamplerState::Linear);

			copy.Render(dx, temp.downScaledTextures[2], *temp.downScaledTextures[3].GetTexture2D());	// /16		-> /8
			copy.Render(dx, temp.downScaledTextures[1], *temp.downScaledTextures[2].GetTexture2D());	// /8		-> /4
			copy.Render(dx, temp.downScaledTextures[0], *temp.downScaledTextures[1].GetTexture2D());	// /4		-> /2
			copy.Render(dx, _bloomThresholdMap, *temp.downScaledTextures[0].GetTexture2D());			// /2		-> source
		}
	}
}

void Bloom::RenderBloom(DirectX& dx, RenderTexture& outRT, const RenderTexture& inputColorMap, const TBRParamCB& tbrParamCB)
{	
	AutoBinderSRV<PixelShader> color(dx,		TextureBindIndex(0),	inputColorMap.GetTexture2D()->GetShaderResourceView());
	AutoBinderSRV<PixelShader> luminance(dx,	TextureBindIndex(1),	_adaptedLuminanceMaps[!_currentAdaptedLuminanceIndx].GetTexture2D()->GetShaderResourceView());
	AutoBinderSRV<PixelShader> threshold(dx,	TextureBindIndex(2),	_bloomThresholdMap.GetTexture2D()->GetShaderResourceView());

	AutoBinderCB<PixelShader> hdrParam(dx,		ConstBufferBindIndex::HDRGlobalParamCB,			_paramCB);
	AutoBinderCB<PixelShader> tbrParam(dx,		ConstBufferBindIndex::TBRParam,					tbrParamCB);

	AutoBinderSampler<PixelShader> sampler(dx,	SamplerStateBindIndex::DefaultSamplerState,		SamplerState::Point);

	_bloom.Render(dx, outRT, true);

	_currentAdaptedLuminanceIndx = !_currentAdaptedLuminanceIndx;
}