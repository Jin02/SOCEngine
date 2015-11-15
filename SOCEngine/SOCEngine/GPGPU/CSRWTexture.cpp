#include "CSRWTexture.h"
#include "Director.h"

using namespace GPGPU::DirectCompute;
using namespace Rendering::Texture;


CSRWTexture::CSRWTexture() : UnorderedAccessView(), _renderTexture(nullptr)
{
}

CSRWTexture::~CSRWTexture()
{
	Destroy();
}

void CSRWTexture::Initialize(const Math::Size<uint>& size, DXGI_FORMAT format, uint optionalBindFlags)
{
	uint bindFlag = D3D11_BIND_UNORDERED_ACCESS | optionalBindFlags;

	_renderTexture = new RenderTexture;
	_renderTexture->Initialize(size, format, bindFlag, 1);

	// create uav
	UnorderedAccessView::Initialize(format, size.w * size.h, _renderTexture->GetTexture(), UnorderedAccessView::Type::Texture);
}

void CSRWTexture::Destroy()
{
	SAFE_DELETE(_renderTexture);
	UnorderedAccessView::Destroy();
}