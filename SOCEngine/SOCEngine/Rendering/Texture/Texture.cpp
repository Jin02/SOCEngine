#include "Texture.h"

using namespace Rendering::Texture;

Texture::Texture(ID3D11ShaderResourceView* srv, bool hasAlpha) :
	_srv(srv), _hasAlpha(hasAlpha)
{
}

Texture::~Texture()
{
	SAFE_RELEASE(_srv);
}