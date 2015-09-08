#include "TextureManager.h"
#include "Director.h"
#include "Utility.h"

#include "DirectXTK/WICTextureLoader.h"

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
	std::string folderPath, name, extension;
	Utility::String::ParseDirectory(fileDir, &folderPath, &name, &extension);

	Texture::Texture2D* tex = Find(name);
	if(tex)
		return tex;

	auto dx = Device::Director::GetInstance()->GetDirectX();
	ID3D11Device* device = dx->GetDevice();
	ID3D11DeviceContext* context = dx->GetContext();

	ID3D11Texture2D* texture2d = nullptr;
	ID3D11ShaderResourceView* srv = nullptr;

	ID3D11Resource* resource = nullptr;

	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

	std::wstring wFilePath = converter.from_bytes(fileDir);
	HRESULT hr = CreateWICTextureFromFile(device, context, wFilePath.c_str(), &resource, &srv); 

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