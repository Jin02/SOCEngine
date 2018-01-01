#include "SunShaft.h"
#include "BindIndexInfo.h"
#include "AutoBinder.hpp"
#include "LightManager.h"

using namespace Rendering::PostProcessing;
using namespace Rendering::Texture;
using namespace Rendering::Shader;
using namespace Rendering::Buffer;
using namespace Rendering::Manager;
using namespace Rendering::RenderState;
using namespace Rendering::Light;
using namespace Rendering::Camera;
using namespace Rendering::Renderer;
using namespace Device;
using namespace Core;
using namespace Math;

void SunShaft::Initialize(DirectX& dx, ShaderManager& shaderMgr, const MainCamera& mainCam)
{
	std::vector<ShaderMacro> macros{dx.GetMSAAShaderMacro()};
	FullScreen::InitParam param;
	{
		param.psMacros			= &macros;
		param.psName			= "OcclusionMapPS";
		param.shaderFileName	= "SunShaft";
	}

	_occlusionMapScreen.Initialize(dx, param, shaderMgr);

	param.psMacros	= nullptr;
	param.psName	= "SunShaftPS";
	_shaftScreen.Initialize(dx, param, shaderMgr);

	_paramCB.Initialize(dx);

	auto renderRect = mainCam.GetRenderRect().size;

	_paramCBData.bufferSizeW = renderRect.w;
	_paramCBData.bufferSizeH = renderRect.h;

	_occlusionMap.Initialize(dx, renderRect, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN, 0, 1, 1);
}

void SunShaft::UpdateParamCB(DirectX& dx, const ObjectManager& objMgr, const LightManager& lightMgr, const TransformPool& tfPool, const MainCamera& mainCam)
{
	auto findTF	= tfPool.Find(_dlObjID.Literal());			assert(findTF);
	auto findDL	= lightMgr.Find<DirectionalLight>(_dlObjID);assert(findDL);

	_dirty |= (findTF->GetDirty() | findDL->GetBase()->GetDirty());

	//tf worldPos를 쓰면 안되.
	Vector3 forward = findTF->GetWorldForward();
	Vector3 worldPos = -forward * 1000.0f;

	Vector3 clipSpacePos	= Vector3::TransformCoord(worldPos, mainCam.GetViewProjMatrix());
	Vector2 uv(	clipSpacePos.x * 0.5f + 0.5f,
				clipSpacePos.y * -0.5f + 0.5f );

	_renderAble =	((clipSpacePos.z + 0.0001f) > 0.0f) & (
					((uv.x + _paramCBData.size) > 0.0f) & ((uv.x - _paramCBData.size) < 1.0f) &
					((uv.y + _paramCBData.size) > 0.0f) & ((uv.y - _paramCBData.size) < 1.0f) );
	_renderAble &=	objMgr.Find(_dlObjID)->GetUse();
	if( (_renderAble == false) | (_dirty == false) )
		return;
	
	_paramCBData.sunUV_X = Half(uv.x);
	_paramCBData.sunUV_Y = Half(uv.y);

	auto size = mainCam.GetRenderRect().size.Cast<float>();
	_paramCBData.aspect = Half(size.w / size.h);

	if (_changedDL)
	{
		uint dlIndex = lightMgr.GetIndexer<DirectionalLight>().Find(_dlObjID.Literal());
		_paramCBData.dlIndex = dlIndex;

		_changedDL = false;
	}

	_paramCB.UpdateSubResource(dx, _paramCBData);
	_dirty = false;
}

void SunShaft::Render(DirectX& dx, RenderTexture& resultMap, const Texture2D& inputColorMap,
	const MainRenderer& renderer, const MainCamera& mainCamera, const LightManager& lightMgr)
{
	_occlusionMap.Clear(dx, Color::Clear());

	AutoBinderSampler<PixelShader> samplerState(dx,	SamplerStateBindIndex(0),	SamplerState::Point);
	AutoBinderCB<PixelShader> paramCB(dx,			ConstBufferBindIndex(0),	_paramCB);
	{
		AutoBinderCB<PixelShader> cameraCB(dx,	ConstBufferBindIndex::Camera,	mainCamera.GetCameraCB());
	
		const auto& dlBuffers = lightMgr.GetBuffer<DirectionalLight>();
		AutoBinderSRV<PixelShader> dirCB(dx,			TextureBindIndex::DirectionalLightDirXY,				dlBuffers.GetTransformSRBuffer().GetShaderResourceView());
		AutoBinderSRV<PixelShader> colorCB(dx,			TextureBindIndex::DirectionalLightColor,				dlBuffers.GetColorSRBuffer().GetShaderResourceView());
		AutoBinderSRV<PixelShader> optionalParamCB(dx,	TextureBindIndex::DirectionalLightOptionalParamIndex,	dlBuffers.GetOptionalParamIndexSRBuffer().GetShaderResourceView());
		AutoBinderSRV<PixelShader> depthMap(dx,			TextureBindIndex::GBuffer_Depth,						renderer.GetGBuffers().opaqueDepthBuffer.GetTexture2D().GetShaderResourceView());

		_occlusionMapScreen.Render(dx, _occlusionMap, true);
	}

	AutoBinderSRV<PixelShader> occlusionMap(dx, TextureBindIndex(27), _occlusionMap.GetTexture2D()->GetShaderResourceView());
	AutoBinderSRV<PixelShader> inputColorMapSRV(dx, TextureBindIndex(28), inputColorMap.GetShaderResourceView());

	_shaftScreen.Render(dx, resultMap, true);
}

void SunShaft::Destroy()
{
	_occlusionMap.Destroy();
	_paramCB.Destroy();

	_paramCBData	= ParamCBData();
	_dlObjID		= ObjectID::Undefined();
	_renderAble		= false;
	_dirty			= true;
	_changedDL		= true;
}