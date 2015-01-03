#include "BasicMaterial.h"
#include "Utility.h"
#include "Director.h"

using namespace Rendering;
using namespace Rendering::Buffer;

BasicMaterial::Color::Color() 
	:main(1.0f, 1.0f, 1.0f, 1.0f), specular(1.0f, 1.0f, 1.0f, 1.0f)
{
}

BasicMaterial::Color::~Color()
{
}


BasicMaterial::BasicMaterial(const std::string& name) : Material(name)
{
}

BasicMaterial::BasicMaterial(const std::string& name, const Color& color) :Material(name),
	_color(color)
{
	_hasAlpha = _color.main.a < 1.0f;
}

BasicMaterial::~BasicMaterial(void)
{
	SAFE_DELETE(_colorBuffer);
}

void BasicMaterial::InitColorBuffer(ID3D11DeviceContext* context)
{	
	auto& psConstBuffers = _constbuffers.usagePS;
	if( psConstBuffers.size() != 0 )
		ASSERT("Error!, ps constbuffer already exists");

	_colorBuffer = new Buffer::ConstBuffer;	
	_colorBuffer->Create(sizeof(BasicMaterial::Color));

	psConstBuffers.push_back(std::make_pair(BasicConstBuffercSlot::MaterialColor, _colorBuffer));	
	UpdateColorBuffer(context);
}

void BasicMaterial::UpdateColorBuffer(ID3D11DeviceContext* context)
{
	if(_colorBuffer)
		_colorBuffer->Update(context, &_color);
}

void BasicMaterial::UpdateColor(const Color& color)
{
	_color = color;
	_hasAlpha = (_color.main.a < 1.0f);
	_changedAlpha = true;
}