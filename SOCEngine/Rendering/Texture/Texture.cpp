#include "Texture.h"

using namespace Rendering::Texture;

Texture::Texture(ID3D11ShaderResourceView* srv) : _srv(srv)
{
}

Texture::~Texture()
{
	SAFE_RELEASE(_srv);
}