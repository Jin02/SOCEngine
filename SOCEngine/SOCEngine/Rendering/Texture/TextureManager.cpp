#include "TextureManager.h"
#include "Director.h"
#include "Utility.h"

using namespace Rendering::Manager;
using namespace Rendering::Texture;

TextureManager::TextureManager()
{
}

TextureManager::~TextureManager()
{
	RemoveAll();
}

bool TextureManager::LoadTextureFromFile(ID3D11ShaderResourceView** outShaderResourceView, const std::string& fileDir)
{
	ID3D11Device* device = Device::Director::GetInstance()->GetDirectX()->GetDevice();
	HRESULT hr = D3DX11CreateShaderResourceViewFromFile(device, fileDir.c_str(), nullptr, nullptr, outShaderResourceView, nullptr);
	if( FAILED(hr) )
		return false;

	return true;
}


Texture* TextureManager::LoadTextureFromFile(const std::string& fileDir, bool hasAlpha)
{
	std::string folderPath, name, extension;
	Utility::String::ParseDirectory(fileDir, &folderPath, &name, &extension);

	Texture::Texture* tex = _hash.Find(name);
	if(tex)
		return tex;

	ID3D11ShaderResourceView* srv = nullptr;

	ASSERT_COND_MSG(LoadTextureFromFile(&srv, fileDir), "Fail, Not Load Texture!");
	tex = new Texture::Texture(srv, hasAlpha);

	return _hash.Add(name+extension, tex);
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