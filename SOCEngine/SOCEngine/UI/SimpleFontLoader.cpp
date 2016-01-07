#include "SimpleFontLoader.h"
#include "Director.h"
#include "EngineShaderFactory.hpp"
#include "ResourceManager.h"

using namespace UI;
using namespace Rendering;
using namespace Device;
using namespace Resource;

SimpleFontLoader::SimpleFontLoader()
	: _texture(nullptr)
{
}

SimpleFontLoader::~SimpleFontLoader()
{
}

void SimpleFontLoader::Destroy()
{
	SAFE_DELETE(_texture);
}

const SimpleFontLoader* SimpleFontLoader::Initialize(const std::string& fontDataFilePath, const std::string& fontTexturePath)
{
	if(_texture == nullptr)
	{
		ASSERT_COND_MSG(LoadFontData(fontDataFilePath), "Error, can not load font datas");
		ASSERT_COND_MSG(LoadTexture(fontTexturePath), "Error, can not load font texture");
	}

	return this;
}

bool SimpleFontLoader::LoadFontData(const std::string& filePath)
{
	std::ifstream fileStream;

	fileStream.open(filePath.c_str());
	if(fileStream.fail())
		return false;

	for(int i=0; i<95; i++)
	{
		char temp = 0;
		fileStream.get(temp);
		while(temp != ' ')
			fileStream.get(temp);
		fileStream.get(temp);
		while(temp != ' ')
			fileStream.get(temp);

		fileStream >> _font[i].left;
		fileStream >> _font[i].right;
		fileStream >> _font[i].size;
	}

	fileStream.close();
	return true;
}

bool SimpleFontLoader::LoadTexture(const std::string& texturePath)
{
	const ResourceManager* resourceManager = ResourceManager::GetInstance();

	auto textureMgr = resourceManager->GetTextureManager();
	_texture = textureMgr->LoadTextureFromFile(texturePath, false);

	ASSERT_COND_MSG(_texture, "Error! can not create texture");

	_fontTextureSize = _texture->FetchSize();

	return true;
}