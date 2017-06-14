#include "MainRenderer.h"
#include "LightCullingUtility.h"

using namespace Rendering;
using namespace Rendering::Camera;
using namespace Rendering::Renderer;
using namespace Math;

void MainRenderer::Initialize(Device::DirectX& dx, Manager::ShaderManager& shaderMgr, const MainCamera& mainCamera)
{
	_blendedDepthLC.Initialize(dx, shaderMgr, mainCamera.GetRenderRect().size);

	// Light Buffer
	{
		auto size = dx.GetBackBufferSize().Cast<uint>();
		{
			if (dx.GetMSAADesc().Count > 1)
			{
				size.w *= 2;
				size.h *= 2;
			}
		}

		_diffuseLightBuffer.Initialize(dx, size, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, D3D11_BIND_UNORDERED_ACCESS, 1);
		_specularLightBuffer.Initialize(dx, size, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, D3D11_BIND_UNORDERED_ACCESS, 1);
	}

	// setting gbuffer, render target
	{
		const auto& size = mainCamera.GetRenderRect().size;

		_gbuffer.albedo_occlusion.Initialize(dx, size, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN, 0);
		_gbuffer.normal_roughness.Initialize(dx, size, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN, 0);
		_gbuffer.velocity_metallic_specularity.Initialize(dx, size, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN, 0);

		_gbuffer.emission_materialFlag.Initialize(dx, size, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN, 0);

		_gbuffer.opaqueDepthBuffer.Initialize(dx, size, true);
		_gbuffer.blendedDepthBuffer.Initialize(dx, size, true);
	}

	_tbrCB.Initialize(dx);
}

void MainRenderer::UpdateCB(Device::DirectX & dx, const MainCamera& mainCamera, Manager::LightManager& lightMgr)
{
	const auto& renderRect = mainCamera.GetRenderRect();

	_tbrCBData.invProjMat = Matrix::Inverse(mainCamera.GetProjMatrix());
	_tbrCBData.invViewProjMat = Matrix::Inverse(mainCamera.GetViewProjMatrix());

	Matrix invViewportMat = Matrix::ComputeInvViewportMatrix(mainCamera.GetRenderRect());
	_tbrCBData.invViewProjViewport = invViewportMat * _tbrCBData.invViewProjMat;

	_tbrCBData.invProjMat = Matrix::Transpose(_tbrCBData.invProjMat);
	_tbrCBData.invViewProjMat = Matrix::Transpose(_tbrCBData.invViewProjMat);
	_tbrCBData.invViewProjViewport = Matrix::Transpose(_tbrCBData.invViewProjViewport);

	// packed tbr cb data
	{
		auto& packed = _tbrCBData.packedParam;
		packed.maxNumOfperLightInTile = Light::CullingUtility::CalcMaxNumLightsInTile(mainCamera.GetRenderRect().size);
		packed.packedNumOfLights = lightMgr.GetPackedLightCount();

		packed.packedViewportSize = (renderRect.x << 16) | renderRect.y;
	}

	_tbrCB.UpdateSubResource(dx, _tbrCBData);
	_dirty = false;
}
