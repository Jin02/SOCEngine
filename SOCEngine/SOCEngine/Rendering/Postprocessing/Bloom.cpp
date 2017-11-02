#include "Bloom.h"

using namespace Device;
using namespace Rendering;
using namespace Rendering::Buffer;
using namespace Rendering::Shader;
using namespace Rendering::PostProcessing;
using namespace Rendering::Texture;
using namespace Rendering::Renderer;
using namespace Rendering::Camera;

void Bloom::Initialize(Device::DirectX& dx, Manager::ShaderManager& shaderMgr)
{
	using Param = FullScreen::InitParam;

	_bloom.Initialize(dx, Param("Bloom", "Bloom_InFullScreen_PS", nullptr), shaderMgr);
	_bloomThreshold.Initialize(dx, Param("Bloom", "Bloom_Threshold_InFullScreen_PS", nullptr), shaderMgr);
	_eyeAdaptation.Initialize(dx, Param("EyeAdaptation", "EyeAdaptation_InFullScreen_PS", nullptr), shaderMgr);

	_blur.Initialize(dx, shaderMgr);

	_paramCB.Initialize(dx);
	_paramCB.UpdateSubResource(dx, ParamCBData());

	_adaptedLuminanceMaps[0].Initialize(dx, Size<uint>(1, 1), DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_UNKNOWN, 0, 1, 1);
	_adaptedLuminanceMaps[1].Initialize(dx, Size<uint>(1, 1), DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_UNKNOWN, 0, 1, 1);

	_bloomThresholdMap.Initialize(dx, dx.GetBackBufferSize().Cast<uint>(), DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN, 0, 1, 1);
}

void Bloom::UpdateParamCB(Device::DirectX& dx)
{
	// dt때문에 계속 업데이트를 할 수 밖에 없다.
	_paramCB.UpdateSubResource(dx, _paramData);
}

void Bloom::RenderThresholdMap(	DirectX& dx, 
								RenderTexture& inColorMap, Copy& copy,
								TempTextures& temp, MainRenderer& renderer)
{
	PixelShader::BindShaderResourceView(dx, TextureBindIndex(0), inColorMap.GetTexture2D().GetShaderResourceView());
	PixelShader::BindShaderResourceView(dx, TextureBindIndex(1), _adaptedLuminanceMaps[!_currentAdaptedLuminanceIndx].GetTexture2D().GetShaderResourceView());

	PixelShader::BindConstBuffer(dx,	ConstBufferBindIndex::HDRGlobalParamCB,		_paramCB);
	PixelShader::BindConstBuffer(dx,	ConstBufferBindIndex::TBRParam,				renderer.GetTBRParamCB());

	PixelShader::BindSamplerState(dx,	SamplerStateBindIndex::DefaultSamplerState,	dx.GetSamplerStateLinear());

	_eyeAdaptation.Render(dx, _adaptedLuminanceMaps[_currentAdaptedLuminanceIndx], true);

	// Bloom Map
	{
		_bloomThreshold.Render(dx, _bloomThresholdMap, true);

		// Blur
		{
			// Down Scale
			{
				copy.Render(dx, temp.downScaledTextures[0], _bloomThresholdMap);			// source	-> /2
				copy.Render(dx, temp.downScaledTextures[1], temp.downScaledTextures[0]);	// /2		-> /4
				copy.Render(dx, temp.downScaledTextures[2], temp.downScaledTextures[1]);	// /4		-> /8
				copy.Render(dx, temp.downScaledTextures[3], temp.downScaledTextures[2]);	// /8		-> /16
			}

			for (uint i = 0; i<2; ++i)
				_blur.Render(dx, temp.downScaledTextures[3], temp.downScaledTextures[3], temp.minSizeMap);

			// Up Scale
			{
				copy.Render(dx, temp.downScaledTextures[2], temp.downScaledTextures[3]);	// /16		-> /8
				copy.Render(dx, temp.downScaledTextures[1], temp.downScaledTextures[2]);	// /8		-> /4
				copy.Render(dx, temp.downScaledTextures[0], temp.downScaledTextures[1]);	// /4		-> /2
				copy.Render(dx, _bloomThresholdMap, temp.downScaledTextures[0]);			// /2		-> source
			}
		}
	}

	PixelShader::UnBindShaderResourceView(dx, TextureBindIndex(0));
	PixelShader::UnBindShaderResourceView(dx, TextureBindIndex(1));

	PixelShader::UnBindConstBuffer(dx, ConstBufferBindIndex::HDRGlobalParamCB);
	PixelShader::UnBindConstBuffer(dx, ConstBufferBindIndex::TBRParam);

	PixelShader::UnBindSamplerState(dx, SamplerStateBindIndex::DefaultSamplerState);
}

void Bloom::RenderBloom(Device::DirectX & dx, Texture::RenderTexture & outRT, Texture::RenderTexture & inputColorMap, Renderer::MainRenderer & mainRenderer)
{
	PixelShader::BindShaderResourceView(dx, TextureBindIndex(0), inputColorMap.GetTexture2D().GetShaderResourceView());
	PixelShader::BindShaderResourceView(dx, TextureBindIndex(1), _adaptedLuminanceMaps[!_currentAdaptedLuminanceIndx].GetTexture2D().GetShaderResourceView());
	PixelShader::BindShaderResourceView(dx, TextureBindIndex(2), _bloomThresholdMap.GetTexture2D().GetShaderResourceView());

	PixelShader::BindSamplerState(dx, SamplerStateBindIndex::DefaultSamplerState, dx.GetSamplerStateLinear());

	PixelShader::BindConstBuffer(dx, ConstBufferBindIndex::HDRGlobalParamCB, _paramCB);
	PixelShader::BindConstBuffer(dx, ConstBufferBindIndex::TBRParam, mainRenderer.GetTBRParamCB());

	_bloom.Render(dx, outRT, true);

	PixelShader::UnBindShaderResourceView(dx, TextureBindIndex(0));
	PixelShader::UnBindShaderResourceView(dx, TextureBindIndex(1));
	PixelShader::UnBindShaderResourceView(dx, TextureBindIndex(2));

	PixelShader::UnBindSamplerState(dx, SamplerStateBindIndex::DefaultSamplerState);

	PixelShader::UnBindConstBuffer(dx, ConstBufferBindIndex::HDRGlobalParamCB);
	PixelShader::UnBindConstBuffer(dx, ConstBufferBindIndex::TBRParam);

	_currentAdaptedLuminanceIndx = !_currentAdaptedLuminanceIndx;
}