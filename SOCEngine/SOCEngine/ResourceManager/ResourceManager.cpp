#include "ResourceManager.h"
#include "Director.h"

using namespace Core;
using namespace std;
using namespace Structure;
using namespace Rendering;
using namespace Resource;
using namespace Importer;
using namespace Rendering::Manager;

ResourceManager::ResourceManager()
	:	_meshImporter(nullptr), _shaderMgr(nullptr),
		_textureMgr(nullptr), _bufferManager(nullptr)
{
}

ResourceManager::~ResourceManager()
{
	Destroy();

	SAFE_DELETE(_shaderMgr);
	SAFE_DELETE(_textureMgr);
	SAFE_DELETE(_bufferManager);
	SAFE_DELETE(_meshImporter);
}

void ResourceManager::Initialize()
{
	_shaderMgr		= new ShaderManager;
	_textureMgr		= new TextureManager;
	_bufferManager	= new BufferManager;

	_meshImporter	= new MeshImporter;
	_meshImporter->Initialize();
}

void ResourceManager::Destroy()
{
	_meshImporter->Destroy();
	_shaderMgr->Destroy();
	_textureMgr->DeleteAll();
	_bufferManager->Destroy();
}