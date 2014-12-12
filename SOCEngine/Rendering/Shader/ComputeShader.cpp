#include "ComputeShader.h"
#include "Director.h"

using namespace Device;
using namespace Rendering;
using namespace Rendering::Shader;

ComputeShader::ComputeShader(ID3DBlob* blob) : BaseShader(blob),
	_shader(nullptr)
{

}

ComputeShader::~ComputeShader(void)
{
	SAFE_RELEASE(_shader);
}

bool ComputeShader::CreateShader()
{
	if(_blob == nullptr)
		return false;

	const DirectX* dx = Director::GetInstance()->GetDirectX();
	ID3D11Device* device = dx->GetDevice();

	HRESULT hr = device->CreateComputeShader(_blob->GetBufferPointer(), _blob->GetBufferSize(), 0, &_shader);
	if( FAILED(hr) )
		return false;

	return true;
}