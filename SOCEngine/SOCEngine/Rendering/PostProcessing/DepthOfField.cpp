#include "DepthOfField.h"
#include "Director.h"

using namespace Rendering;
using namespace Rendering::Buffer;
using namespace Rendering::Shader;
using namespace Rendering::PostProcessing;

DepthOfField::DepthOfField()
	: _dof(nullptr), _paramCB(nullptr)
{
}

DepthOfField::~DepthOfField()
{
	Destroy();
	SAFE_DELETE(_dof);
	SAFE_DELETE(_paramCB);
}

void DepthOfField::Initialize()
{
	_dof = new FullScreen;
	_dof->Initialize("DepthOfField", "DoF_InFullScreen_PS", nullptr);

	_paramCB = new ConstBuffer;
	_paramCB->Initialize(sizeof(ParamCB));
}

void DepthOfField::UpdateParam(const ParamCB& param)
{
	if(param == _prevParam)
		return;

	const Device::DirectX* dx = Device::Director::SharedInstance()->GetDirectX();

	_prevParam = param;
	_paramCB->UpdateSubResource(dx->GetContext(), &param);
}

void DepthOfField::Render(const Device::DirectX* dx, const Texture::RenderTexture* out,
						  const Camera::MeshCamera* mainCam, const Texture::Texture2D* bluredCurScene)
{
	ID3D11DeviceContext* context = dx->GetContext();

	PixelShader::BindTexture(context, TextureBindIndex(0),								mainCam->GetRenderTarget());
	PixelShader::BindTexture(context, TextureBindIndex(1),								bluredCurScene);
	PixelShader::BindTexture(context, TextureBindIndex::GBuffer_Depth,					mainCam->GetOpaqueDepthBuffer());

	PixelShader::BindConstBuffer(context, ConstBufferBindIndex::TBRParam,				mainCam->GetTBRParamConstBuffer());
	PixelShader::BindConstBuffer(context, ConstBufferBindIndex(1),						_paramCB);
	PixelShader::BindConstBuffer(context, ConstBufferBindIndex::Camera,					mainCam->GetCameraConstBuffer());

	PixelShader::BindSamplerState(context, SamplerStateBindIndex::DefaultSamplerState,	dx->GetSamplerStateLinear());

	_dof->Render(dx, out, true);

	PixelShader::BindTexture(context, TextureBindIndex(0),								nullptr);
	PixelShader::BindTexture(context, TextureBindIndex(1),								nullptr);
	PixelShader::BindTexture(context, TextureBindIndex::GBuffer_Depth,					nullptr);

	PixelShader::BindConstBuffer(context, ConstBufferBindIndex::TBRParam,				nullptr);
	PixelShader::BindConstBuffer(context, ConstBufferBindIndex(1),						nullptr);
	PixelShader::BindConstBuffer(context, ConstBufferBindIndex::Camera,					nullptr);

	PixelShader::BindSamplerState(context, SamplerStateBindIndex::DefaultSamplerState,	nullptr);
}

void DepthOfField::Destroy()
{
	if(_dof)_dof->Destroy();
	if(_paramCB) _paramCB->Destroy();
}