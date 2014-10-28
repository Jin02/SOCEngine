#include "TextureManager.h"
#include "Director.h"

using namespace Rendering::Texture;

TextureManager::TextureManager()
{
}

TextureManager::~TextureManager()
{
	RemoveAll();
}

bool TextureManager::LoadTextureFromFile(ID3D11ShaderResourceView** outShaderResourceView, const std::string& path, const std::string& nameWithExtension)
{
	ID3D11Device* device = Device::Director::GetInstance()->GetDirectX()->GetDevice();
	HRESULT hr = D3DX11CreateShaderResourceViewFromFile(device, (path+nameWithExtension).c_str(), nullptr, nullptr, outShaderResourceView, nullptr);
	if( FAILED(hr) )
		return false;

	return true;
}


Texture* TextureManager::LoadTextureFromFile(const std::string& path, const std::string& nameWithExtension, const std::string& key)
{
	Texture* tex = _hash.Find(key);
	if(tex)
		return tex;

	ID3D11ShaderResourceView* srv = nullptr;
	if( LoadTextureFromFile(&srv, path, nameWithExtension) == false )
		return nullptr;

	return _hash.Add(key, new Texture(srv));
}

Texture* TextureManager::Find(const std::string& name)
{
	return _hash.Find(name);
}

void TextureManager::Remoave(const std::string& name)
{
	_hash.Delete(name, true);
}

void TextureManager::RemoveAll()
{
	_hash.DeleteAll(true);
}