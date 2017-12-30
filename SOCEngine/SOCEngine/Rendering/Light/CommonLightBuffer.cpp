#include "CommonLightBuffer.h"

using namespace Core;
using namespace Rendering;
using namespace Rendering::Light;
using namespace Rendering::Light::Buffer;

void CommonLightBuffer::Initialize(Device::DirectX& dx, uint count, const void* dummy)
{
	_colorBuffer.Initialize(dx, count, DXGI_FORMAT_R8G8B8A8_UNORM, dummy);
	_optionalParamIndexBuffer.Initialize(dx, count, DXGI_FORMAT_R32_UINT, dummy);	
}

void CommonLightBuffer::SetData(uint index, const Light::BaseLight& light, ushort shadowIndex, uint shaftIndex)
{	
	_colorBuffer[index]					= light.Get32BitMainColor();
	_optionalParamIndexBuffer[index]	= ComputeOptionalParamIndex(light, shadowIndex, shaftIndex);
}

void CommonLightBuffer::PushData(const Light::BaseLight & light, ushort shadowIndex, uint lightShaftIndex)
{
	_colorBuffer.PushData(light.Get32BitMainColor());
	_optionalParamIndexBuffer.PushData(ComputeOptionalParamIndex(light, shadowIndex, lightShaftIndex));
}

void CommonLightBuffer::UpdateSRBuffer(Device::DirectX& dx)
{
	_colorBuffer.UpdateSRBuffer(dx);
	_optionalParamIndexBuffer.UpdateSRBuffer(dx);
}

void CommonLightBuffer::Delete(uint index)
{
	_colorBuffer.Delete(index);
	_optionalParamIndexBuffer.Delete(index);
}

void CommonLightBuffer::DeleteAll()
{
	_colorBuffer.DeleteAll();
	_optionalParamIndexBuffer.DeleteAll();
}