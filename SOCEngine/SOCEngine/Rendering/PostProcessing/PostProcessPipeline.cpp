#include "PostProcessPipeline.h"

using namespace Rendering;
using namespace Rendering::PostProcessing;
using namespace Rendering::Texture;

PostProcessPipeline::PostProcessPipeline()
	: _iblPass(nullptr), _backBufferMaker(nullptr), _result(nullptr)
{
}

PostProcessPipeline::~PostProcessPipeline()
{
	Destroy();

	SAFE_DELETE(_iblPass);
	SAFE_DELETE(_backBufferMaker);
	SAFE_DELETE(_result);
}

void PostProcessPipeline::Initialize(const Math::Size<uint>& resultTextureSize)
{
	_iblPass = new IBLPass;
	_iblPass->Initialize();

	_backBufferMaker = new BackBufferMaker;
	_backBufferMaker->Initialize(false);

	_result = new RenderTexture;
	_result->Initialize(resultTextureSize, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 1);
}

void PostProcessPipeline::Render(const Device::DirectX* dx,
								 const Texture::RenderTexture* backBuffer,
								 const Camera::MeshCamera* mainMeshCamera,
								 const Sky::SkyForm* sky)
{
	if(mainMeshCamera == nullptr)
		return;

	dx->ClearDeviceContext();

	_iblPass->Render(dx, _result, mainMeshCamera, sky);

	const RenderTexture* resultRT = _result;
	_backBufferMaker->Render(backBuffer->GetRenderTargetView(),
							resultRT, nullptr, mainMeshCamera->GetTBRParamConstBuffer());
}

void PostProcessPipeline::Destroy()
{
	_iblPass->Destroy();
	_backBufferMaker->Destroy();
	_result->Destroy();
}