#include "TransparencyRenderer.h"
#include "TBRShaderIndexSlotInfo.h"
#include "Director.h"

using namespace Rendering::Renderer;
using namespace Rendering::Shader;
using namespace Rendering::Buffer;
using namespace Rendering::TBDR;
using namespace Rendering;

TransparencyRenderer::TransparencyRenderer()
{
}

TransparencyRenderer::~TransparencyRenderer()
{
}

void TransparencyRenderer::Initialize()
{
	// Setting LightBuffer
	{
		auto AddLightSRBufferToList = [&](uint semanticIndex, const ShaderResourceBuffer* srb)
		{
			ShaderForm::InputShaderResourceBuffer isrb(semanticIndex, srb, false, false, false, true);
			_inputSRBuffers.push_back(isrb);
		};

		const auto& lightMgr = Device::Director::GetInstance()->GetCurrentScene()->GetLightManager();

		// pl transform
		AddLightSRBufferToList(
			(uint)InputBufferShaderIndex::PointLightRadiusWithCenter, 
			lightMgr->GetPointLightTransformBufferSR());

		// pl color
		AddLightSRBufferToList(
			(uint)InputBufferShaderIndex::PointLightColor, 
			lightMgr->GetPointLightColorBufferSR());

		// sl transform
		AddLightSRBufferToList((uint)InputBufferShaderIndex::SpotLightRadiusWithCenter,
			lightMgr->GetSpotLightTransformBufferSR());

		// sl color
		AddLightSRBufferToList((uint)InputBufferShaderIndex::SpotLightColor,
			lightMgr->GetSpotLightColorBufferSR());

		// sl param
		AddLightSRBufferToList((uint)InputBufferShaderIndex::SpotLightParam,
			lightMgr->GetSpotLightParamBufferSR());

		// dl transfrom
		AddLightSRBufferToList((uint)InputBufferShaderIndex::DirectionalLightCenterWithDirZ,
			lightMgr->GetDirectionalLightTransformBufferSR());

		// dl color
		AddLightSRBufferToList((uint)InputBufferShaderIndex::DirectionalLightColor,
			lightMgr->GetDirectionalLightColorBufferSR());

		// dl param
		AddLightSRBufferToList((uint)InputBufferShaderIndex::DirectionalLightParam,
			lightMgr->GetDirectionalLightParamBufferSR());
	}
}

void TransparencyRenderer::Destroy()
{
	_inputConstBuffers.clear();
	_inputTextures.clear();
	_inputSRBuffers.clear();
}

void TransparencyRenderer::Render(ID3D11RenderTargetView* outResultRT,
								  const std::function<void(const std::vector<const Mesh::Mesh*>& meshes, RenderType renderType)>& renderMesh)
{

}