#include "TextureGPUView.h"

using namespace Rendering::Texture;
using namespace Rendering::View;

Rendering::Texture::TextureGPUView::TextureGPUView()
	: _srv(), _uav()
{
}

Rendering::Texture::TextureGPUView::TextureGPUView(const View::ShaderResourceView & srv)
	: _srv(srv), _uav()
{
}

Rendering::Texture::TextureGPUView::TextureGPUView(const View::ShaderResourceView & srv, const View::UnorderedAccessView & uav)
	: _srv(srv), _uav(uav)
{
}

void TextureGPUView::Destroy()
{
	_srv.Destroy();
	_uav.Destroy();
}