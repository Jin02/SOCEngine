#include "BaseTexture.h"

using namespace Rendering::Texture;
using namespace Rendering::View;

Rendering::Texture::BaseTexture::BaseTexture()
	: _srv(), _uav()
{
}

Rendering::Texture::BaseTexture::BaseTexture(const View::ShaderResourceView & srv)
	: _srv(srv), _uav()
{
}

Rendering::Texture::BaseTexture::BaseTexture(const View::ShaderResourceView & srv, const View::UnorderedAccessView & uav)
	: _srv(srv), _uav(uav)
{
}

void BaseTexture::Destroy()
{
	_srv.Destroy();
	_uav.Destroy();
}