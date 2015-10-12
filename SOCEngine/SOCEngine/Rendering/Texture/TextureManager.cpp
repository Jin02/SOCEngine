#include "TextureManager.h"
#include "Director.h"
#include "Utility.h"

#include "DirectXTK/WICTextureLoader.h"
#include "DirectXTex/DirectXTex.h"

#include <locale>
#include <codecvt>

using namespace Rendering::Manager;
using namespace Rendering::Texture;

using namespace DirectX;

TextureManager::TextureManager()
{
}

TextureManager::~TextureManager()
{
	RemoveAll();
}

Texture2D* TextureManager::LoadTextureFromFile(const std::string& fileDir, bool hasAlpha)
{
	std::string folderPath, name, format;
	Utility::String::ParseDirectory(fileDir, &folderPath, &name, &format);

	// Find texture
	{
		Texture::Texture2D* tex = Find(name + format);
		if(tex)
			return tex;
	}

	const Device::DirectX*	dx		= Device::Director::GetInstance()->GetDirectX();
	ID3D11Device*			device	= dx->GetDevice();
	ID3D11DeviceContext*	context	= dx->GetContext();

	ID3D11ShaderResourceView*	srv			= nullptr;
	ID3D11Resource*				resource	= nullptr;

	DirectX::TexMetadata	metaData;
	ScratchImage			image;

	bool notCreatedSRV	= false;
	HRESULT hr			= -1;

	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::wstring wFilePath = converter.from_bytes(fileDir);

	if(format == "tga")	
	{
		hr = LoadFromTGAFile(wFilePath.c_str(), &metaData, image);
		notCreatedSRV = true;
	}
	else if(format == "dds")
	{
		hr = LoadFromDDSFile(wFilePath.c_str(), 0u,  &metaData, image);
		notCreatedSRV = true;
	}
	else
		hr = CreateWICTextureFromFile(device, context, wFilePath.c_str(), &resource, &srv);

	if(notCreatedSRV && SUCCEEDED(hr))
	{
		hr = CreateShaderResourceView(device, image.GetImages(), image.GetImageCount(), metaData, &srv);
		srv->GetResource(&resource);
	}
	else if(notCreatedSRV)
		ASSERT_MSG("Error, Can't load this texture");

	if(SUCCEEDED(hr))
	{
		ID3D11Texture2D* texture2d = nullptr;
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

		Texture::Texture2D* tex = new Texture::Texture2D(srv, texture2d, hasAlpha);
		_hash.insert(std::make_pair(name + format, tex));

		return tex;
	}
	else
	{
		DEBUG_LOG("Error, can't load texture");
	}

	return nullptr;
}

Texture2D* TextureManager::Find(const std::string& key)
{
	auto findIter = _hash.find(key);
	return findIter == _hash.end() ? nullptr : findIter->second;
}

void TextureManager::Remoave(const std::string& key)
{
	auto findIter = _hash.find(key);

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