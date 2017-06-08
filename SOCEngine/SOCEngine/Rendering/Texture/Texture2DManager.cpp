#include "Texture2DManager.h"

#include <WICTextureLoader.h>
#include <DirectXTex.h>
#include <locale>
#include <codecvt>

#include "Utility.hpp"

using namespace Rendering::Manager;
using namespace Rendering::Texture;
using namespace Rendering::View;
using namespace DirectX;

Texture2DManager::Texture2DPtr Texture2DManager::LoadTextureFromFile(Device::DirectX& dx, const std::string& fileDir, bool hasAlpha)
{
	std::string folderPath, name, format;
	Utility::String::ParseDirectory(fileDir, &folderPath, &name, &format);

	// Find texture
	{
		Texture2DPtr tex = Find(name + format);
		if(tex)
			return tex;
	}

	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::wstring wFilePath = converter.from_bytes(fileDir);

	ID3D11ShaderResourceView*	srv			= nullptr;
	ID3D11Resource*				resource	= nullptr;
	Texture2DPtr				tex			= nullptr;
	TexMetadata					metaData;
	ScratchImage				image;

	if(format == "tga")	
		ASSERT_SUCCEEDED(LoadFromTGAFile(wFilePath.c_str(), &metaData, image));
	else if(format == "dds")
		ASSERT_SUCCEEDED(LoadFromDDSFile(wFilePath.c_str(), 0u,  &metaData, image));
	else
		ASSERT_SUCCEEDED(CreateWICTextureFromFile(dx.GetDevice(), dx.GetContext(), wFilePath.c_str(), &resource, &srv));

	bool notCreatedSRV = (format == "tga") || (format == "dds");
	if (notCreatedSRV)
	{
		ASSERT_SUCCEEDED(CreateShaderResourceView(dx.GetDevice(), image.GetImages(), image.GetImageCount(), metaData, &srv));
		srv->GetResource(&resource);
	}

	ID3D11Texture2D* tex2d = nullptr;
	resource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&tex2d);

	uint texW = image.GetImages()[0].width;
	uint texH = image.GetImages()[0].height;
	auto dxResource = DXSharedResource<ID3D11ShaderResourceView>(srv);
	tex = std::make_shared<Texture2D>(ShaderResourceView(dxResource), DXSharedResource<ID3D11Texture2D>(tex2d), hasAlpha, Size<uint>(texW, texH));
	_hash.insert(std::make_pair(name + format, tex));
	
	return tex;
}

Texture2DManager::Texture2DPtr Texture2DManager::Find(const std::string& key)
{
	auto findIter = _hash.find(key);
	return findIter == _hash.end() ? nullptr : findIter->second;
}

void Texture2DManager::Delete(const std::string& key)
{
	auto findIter = _hash.find(key);

	if(findIter != _hash.end())
	{
		findIter->second.reset();
		_hash.erase(findIter);
	}
}

void Texture2DManager::DeleteAll()
{
	_hash.clear();
}