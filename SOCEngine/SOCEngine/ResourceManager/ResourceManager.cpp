#include "ResourceManager.h"
#include "Director.h"

using namespace Core;
using namespace std;
using namespace Structure;
using namespace Rendering;
using namespace Resource;
using namespace Importer;
using namespace Rendering::Manager;
using namespace Rendering::Offline;
using namespace UI;

ResourceManager::ResourceManager()
	:	_meshImporter(nullptr), _shaderMgr(nullptr), _fontLoader(nullptr),
		_textureMgr(nullptr), _bufferManager(nullptr),
		_preIntergrateEnvBRDF(nullptr)
{
}

ResourceManager::~ResourceManager()
{
}

void ResourceManager::Initialize()
{
	_shaderMgr		= new ShaderManager;
	_textureMgr		= new TextureManager;
	_bufferManager	= new BufferManager;

	_meshImporter	= new MeshImporter;
	_meshImporter->Initialize();

	_fontLoader		= new SimpleFontLoader;
	//_fontLoader->Initialize(TEMP_FONT_DATA_PATH, TEMP_FONT_TEXTURE_PATH);

	_preIntergrateEnvBRDF = new PreIntegrateEnvBRDF;
	_preIntergrateEnvBRDF->FetchPreBRDFMap();
}

void ResourceManager::DestroyManagers()
{
	_meshImporter->Destroy();
	_shaderMgr->Destroy();
	_textureMgr->DeleteAll();
	_bufferManager->Destroy();
	_fontLoader->Destroy();

	SAFE_DELETE(_shaderMgr);
	SAFE_DELETE(_textureMgr);
	SAFE_DELETE(_bufferManager);
	SAFE_DELETE(_meshImporter);
	SAFE_DELETE(_fontLoader);
	SAFE_DELETE(_preIntergrateEnvBRDF);
}