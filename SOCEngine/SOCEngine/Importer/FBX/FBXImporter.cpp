#include "FBXImporter.h"
#include "Utility.h"

using namespace Importer::FBX;
using namespace fbxsdk_2014_1;

FBXImporter::FBXImporter() : _importer(nullptr)
{
}

FBXImporter::~FBXImporter()
{
	Destroy();
}

void FBXImporter::Initialize()
{
	_importer = new TinyFBXImporter;
	_importer->Initialize();
}

void FBXImporter::Destroy()
{
	SAFE_DELETE(_importer);
}

void FBXImporter::LoadOnlyMesh(const std::string& filePath)
{
	const TinyFBXScene* scene = _importer->LoadScene(filePath);
	ASSERT_COND_MSG(scene, "Error, TinyFBXScene is Null");
}