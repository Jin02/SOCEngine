#include "LightShaftManager.h"
#include "Object.h"

using namespace Math;
using namespace Rendering;
using namespace Core;
using namespace Rendering::Manager;
using namespace Rendering::Buffer;
using namespace Rendering::Light;

LightShaftManager::LightShaftManager()
	: _lightPosInNDCSRBuffer(nullptr), _lightSizeSRBuffer(nullptr)
{
}

LightShaftManager::~LightShaftManager()
{
	SAFE_DELETE(_lightPosInNDCSRBuffer);
	SAFE_DELETE(_lightSizeSRBuffer);
}

void LightShaftManager::Initialize()
{
	Destroy();

	if(_lightPosInNDCSRBuffer == nullptr)
		_lightPosInNDCSRBuffer = new ShaderResourceBuffer;

	_lightPosInNDCSRBuffer->Initialize(
		sizeof(std::pair<ushort, ushort>), LIGHT_SHAFT_MAXIMUM, DXGI_FORMAT_R16G16_FLOAT,
		nullptr, false, 0, D3D11_USAGE_DEFAULT);

	if(_lightSizeSRBuffer == nullptr)
		_lightSizeSRBuffer = new ShaderResourceBuffer;
	_lightSizeSRBuffer->Initialize(sizeof(ushort), LIGHT_SHAFT_MAXIMUM, DXGI_FORMAT_R16_FLOAT, nullptr, false, 0, D3D11_USAGE_DEFAULT);
}

void LightShaftManager::Destroy()
{
	if(_lightPosInNDCSRBuffer)
		_lightPosInNDCSRBuffer->Destroy();

	_lightPosInNDCBuffer.clear();
	_lights.DeleteAll();

	if(_lightSizeSRBuffer)
		_lightSizeSRBuffer->Destroy();
}

void LightShaftManager::Add(const Light::LightForm*& light)
{
	address key = reinterpret_cast<address>(light);
	ASSERT_MSG_IF(Has(light), "Error, Duplicated light.");

	_lightPosInNDCBuffer.push_back(std::make_pair(0, 0));
	_lightSizeBuffer.push_back(0);
	_lights.Add(key, LightPack(-1, _lightPosInNDCBuffer.size() - 1, light));
}

bool LightShaftManager::Has(const Light::LightForm*& light) const
{
	return _lights.Has(reinterpret_cast<address>(light));
}

void LightShaftManager::Delete(const Light::LightForm*& light)
{
	address key = reinterpret_cast<address>(light);
	ASSERT_MSG_IF(Has(light) == false, "Error, Do not have this light");

	uint index = _lights.Find(key)->index;
	_lightPosInNDCBuffer.erase(_lightPosInNDCBuffer.begin() + index);
	_lightSizeBuffer.erase(_lightSizeBuffer.begin() + index);

	_lights.Delete(key);
}

void LightShaftManager::UpdateSRBuffer(const Device::DirectX* dx, const Math::Matrix& viewProjMat)
{
	bool isUpdated	= false;
	uint size		= _lights.GetSize();
	for(uint i=0; i<size; ++i)
	{
		uint& prevCounter	= _lights.Get(i).prevUpdateCounter;

		const LightForm* light	= _lights.Get(i).light;
		uint curCounter			= light->GetOwner()->GetTransform()->GetUpdateCounter();

		if(prevCounter == curCounter) continue;
		prevCounter = curCounter;
		isUpdated = true;

		Transform* tf = light->GetOwner()->GetTransform();
		const Matrix& worldMat = tf->GetWorldMatrix();
		
		Matrix wvpMat		= worldMat * viewProjMat;
		
		_lightPosInNDCBuffer[i].first	= Math::Common::FloatToHalf(wvpMat._41);
		_lightPosInNDCBuffer[i].second	= Math::Common::FloatToHalf(wvpMat._42);
		_lightSizeBuffer[i]				= (light->GetType() == LightForm::LightType::Directional) ? 
											Math::Common::FloatToHalf(light->GetRadius()) : 0;
	}

	if(isUpdated == false)
		return;

	_lightPosInNDCSRBuffer->UpdateSubResource(dx->GetContext(), _lightPosInNDCBuffer.data());
	_lightSizeSRBuffer->UpdateSubResource(dx->GetContext(), _lightSizeBuffer.data());
}

uint LightShaftManager::GetLightShaftIndex(const Light::LightForm*& light) const
{
	address key				= reinterpret_cast<address>(light);
	const LightPack* found	= _lights.Find(key);

	return found ? found->index : -1;
}