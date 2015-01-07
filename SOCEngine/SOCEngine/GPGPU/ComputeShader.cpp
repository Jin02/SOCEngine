#include "ComputeShader.h"
#include "Director.h"

using namespace Device;
using namespace GPGPU::DirectCompute;
using namespace Rendering::Shader;

ComputeShader::ComputeShader(const ThreadGroup& threadGroup, ID3DBlob* blob) : BaseShader(blob),
	_shader(nullptr), _threadGroup(threadGroup)
{

}

ComputeShader::~ComputeShader(void)
{
	SAFE_RELEASE(_shader);
}

bool ComputeShader::Create()
{
	if(_blob == nullptr)
		return false;

	const DirectX* dx = Director::GetInstance()->GetDirectX();
	ID3D11Device* device = dx->GetDevice();

	HRESULT hr = device->CreateComputeShader(_blob->GetBufferPointer(), _blob->GetBufferSize(), 0, &_shader);

	if( FAILED(hr) )
		ASSERT("Error!, system does not create compute shaer component");

	return true;
}

void ComputeShader::Dispatch(ID3D11DeviceContext* context)
{
	context->CSSetShader(_shader, nullptr, 0);
	for(auto iter = _inputBuffers.begin(); iter != _inputBuffers.end(); ++iter)
	{
		auto buffer = iter->buffer;
		if(buffer)
		{
			ID3D11ShaderResourceView* srv = buffer->GetShaderResourceView();
			context->CSSetShaderResources(iter->idx, 1, &srv);
		}
	}
	for(auto iter = _inputTextures.begin(); iter != _inputTextures.end(); ++iter)
	{
		auto texture = iter->texture;
		if(texture)
		{
			ID3D11ShaderResourceView* srv = texture->GetShaderResourceView();
			context->CSSetShaderResources(iter->idx, 1, &srv);
		}
	}

	for(auto iter = _outputBuffers.begin(); iter != _outputBuffers.end(); ++iter)
	{
		auto buffer = iter->buffer;
		if(buffer)
		{
			ID3D11UnorderedAccessView* uav = buffer->GetUnorderedAccessView();
			context->CSSetUnorderedAccessViews(iter->idx, 1, &uav, nullptr);
		}
	}

	context->Dispatch(_threadGroup.x, _threadGroup.y, _threadGroup.z);

	ID3D11ShaderResourceView* nullSRV = nullptr;
	{
		for(auto iter = _inputBuffers.begin(); iter != _inputBuffers.end(); ++iter)
			context->CSSetShaderResources(iter->idx, 1, &nullSRV);

		for(auto iter = _inputTextures.begin(); iter != _inputTextures.end(); ++iter)
			context->CSSetShaderResources(iter->idx, 1, &nullSRV);
	}

	ID3D11UnorderedAccessView* nullUAV = nullptr;
	{
		for(auto iter = _outputBuffers.begin(); iter != _outputBuffers.end(); ++iter)
			context->CSSetUnorderedAccessViews(iter->idx, 0, &nullUAV, nullptr);
	}
	context->CSSetShader(nullptr, nullptr, 0);
}