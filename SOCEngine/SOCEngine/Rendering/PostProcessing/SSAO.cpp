#include "SSAO.h"
#include "Director.h"

using namespace Rendering;
using namespace Rendering::Buffer;
using namespace Rendering::Shader;
using namespace Rendering::PostProcessing;

SSAO::SSAO()
	: _ssao(nullptr), _paramCB(nullptr)
{
}

SSAO::~SSAO()
{
	Destroy();

	SAFE_DELETE(_ssao);
	SAFE_DELETE(_paramCB);
}

bool SSAO::Param::operator==(const SSAO::Param& a) const
{
	return	(stepUV == a.stepUV) && 
		(pacekdSampleCountWithScale == a.pacekdSampleCountWithScale) &&
		(packedSkipDistWithOccluedRate == a.packedSkipDistWithOccluedRate);
}

void SSAO::Param::SetScale(float s)
{
	scale = Math::Common::FloatToHalf(s);
}

void SSAO::Param::SetSkipDist(float d)
{
	skipDist = Math::Common::FloatToHalf(d);
}

void SSAO::Param::SetOccluedRate(float r)
{
	occluedRate = Math::Common::FloatToHalf(r);
}


void SSAO::Initialize()
{
	_ssao = new FullScreen;
	_ssao->Initialize("SSAO", "SSAO_InFullScreen_PS", true, nullptr);

	_paramCB = new ConstBuffer;
	_paramCB->Initialize(sizeof(Param));
}

void SSAO::UpdateParam(const Param& param)
{
	if(param == _prevParam)
		return;

	const Device::DirectX* dx = Device::Director::SharedInstance()->GetDirectX();

	_prevParam = param;
	_paramCB->UpdateSubResource(dx->GetContext(), &param);
}

void SSAO::Render(const Device::DirectX* dx, const Texture::RenderTexture* out, const Texture::RenderTexture* in, const Camera::MeshCamera* mainCam)
{
	ID3D11DeviceContext* context = dx->GetContext();

	PixelShader::BindTexture(context,		TextureBindIndex(0),										in);
	PixelShader::BindTexture(context,		TextureBindIndex::GBuffer_Depth,							mainCam->GetOpaqueDepthBuffer());
	PixelShader::BindTexture(context,		TextureBindIndex::GBuffer_Normal_Roughness,					mainCam->GetGBufferNormalRoughness());

	PixelShader::BindConstBuffer(context,	ConstBufferBindIndex::TBRParam,								mainCam->GetTBRParamConstBuffer());
	PixelShader::BindConstBuffer(context,	ConstBufferBindIndex(1),									_paramCB);

	PixelShader::BindSamplerState(context,	SamplerStateBindIndex::DefaultSamplerState,					dx->GetSamplerStateLinear());

	_ssao->Render(dx, out, true);

	PixelShader::BindTexture(context,		TextureBindIndex(0),										nullptr);
	PixelShader::BindTexture(context,		TextureBindIndex::GBuffer_Depth,							nullptr);
	PixelShader::BindTexture(context,		TextureBindIndex::GBuffer_Normal_Roughness,					nullptr);

	PixelShader::BindConstBuffer(context,	ConstBufferBindIndex::TBRParam,								nullptr);
	PixelShader::BindConstBuffer(context,	ConstBufferBindIndex(1),									nullptr);

	PixelShader::BindSamplerState(context,	SamplerStateBindIndex::DefaultSamplerState,					nullptr);
}

void SSAO::Destroy()
{
	_ssao->Destroy();
	_paramCB->Destroy();
}