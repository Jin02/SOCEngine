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

Texture2D* TextureManager::LoadTextureFromFile(const std::string& fileDir, bool hasAlpha, bool onlyUseSRV)
{
	std::string folderPath, name, extension;
	Utility::String::ParseDirectory(fileDir, &folderPath, &name, &extension);

	Texture::Texture2D* tex = Find(name);
	if(tex)
		return tex;

	ID3D11Device* device = Device::Director::GetInstance()->GetDirectX()->GetDevice();

	ID3D11Texture2D* texture2d = nullptr;
	ID3D11ShaderResourceView* srv = nullptr;

	if(onlyUseSRV == false)
	{
		ID3D11Resource* resource = nullptr;
		HRESULT hr = D3DX11CreateTextureFromFile(device, fileDir.c_str(), nullptr, nullptr, &resource, nullptr);
		ASSERT_COND_MSG(SUCCEEDED(hr), "Error, can't load texture");
		
		resource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&texture2d);

		D3D11_TEXTURE2D_DESC texture2dDesc;
		texture2d->GetDesc(&texture2dDesc);

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		{
			srvDesc.Format = texture2dDesc.Format;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MostDetailedMip = 0;
			srvDesc.Texture2D.MipLevels = texture2dDesc.MipLevels;
		}

		device->CreateShaderResourceView(texture2d, &srvDesc, &srv);
		SAFE_RELEASE(resource);
	}
	else
	{
		HRESULT hr = D3DX11CreateShaderResourceViewFromFile(device, fileDir.c_str(), nullptr, nullptr, &srv, nullptr);
		ASSERT_COND_MSG(SUCCEEDED(hr), "Error, can't load texture");
	}

	tex = new Texture::Texture2D(srv, texture2d, hasAlpha);
	_hash.insert(std::make_pair(name + extension, tex));

	return tex;
}

Texture2D* TextureManager::Find(const std::string& name)
{
	auto findIter = _hash.find(name);
	return findIter == _hash.end() ? nullptr : findIter->second;
}

void TextureManager::Remoave(const std::string& name)
{
	auto findIter = _hash.find(name);

	if(findIter != _hash.end())
	{
		SAFE_DELETE(findIter->second);
		_hash.erase(findIter);
	}
}

void TextureManager::RemoveAll()
{
	for(auto iter = _hash.begin(); iter != _hash.end(); ++iter)
		SAFE_DELETE(iter->second);

	_hash.clear();
}