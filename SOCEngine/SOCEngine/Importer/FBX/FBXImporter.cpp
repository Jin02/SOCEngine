#include "FBXImporter.h"
#include "Utility.h"

using namespace Importer;
using namespace Importer::FBX;
using namespace fbxsdk_2014_1;

TinyFBXImporter::TinyFBXImporter()
	: _importer(nullptr), _sdkManager(nullptr)
{
}

TinyFBXImporter::~TinyFBXImporter()
{
	Destroy();
}

void TinyFBXImporter::Initialize()
{
	FBX_SAFE_DELETE(_sdkManager);
	_sdkManager = FbxManager::Create();

	FbxIOSettings* ioSetting = FbxIOSettings::Create(_sdkManager, IOSROOT);
	_sdkManager->SetIOSettings(ioSetting);

	FbxString path = FbxGetApplicationDirectory();
#if defined (FBXSDK_ENV_WIN)
	FbxString IExtension = "dll";
#elif defined (FBXSDK_ENV_MAC)
	FbxString IExtension = "dylib";				
#elif defined (FBXSDK_ENV_LINUX)
	FbxString IExtension = "so";
#endif

	_sdkManager->LoadPluginsDirectory(path.Buffer(), IExtension.Buffer());
	_importer = FbxImporter::Create(_sdkManager, "");
}

void TinyFBXImporter::Destroy()
{
	auto vector = _tinyScenes.GetVector();
	for(auto iter=vector.begin(); iter != vector.end(); ++iter)
		(*iter)->Cleanup();

	_sdkManager->Destroy();
	_importer->Destroy(true);
}

const TinyFBXScene* TinyFBXImporter::LoadScene(const std::string& filePath)
{
	std::string fileName;
	Utility::String::ParseDirectory(filePath, nullptr, &fileName, nullptr);

	// Check duplicated scene
	{
		TinyFBXScene** found = _tinyScenes.Find(fileName);
		if(found)
			return (*found);
	}

	TinyFBXScene* tinyScene = new TinyFBXScene(_importer, _sdkManager);
	tinyScene->LoadScene(filePath);

	_tinyScenes.Add(fileName, tinyScene);
	return tinyScene;
}