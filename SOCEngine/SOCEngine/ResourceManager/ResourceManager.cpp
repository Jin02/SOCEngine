#include "ResourceManager.h"
#include "Director.h"

using namespace Core;
using namespace std;
using namespace Structure;
using namespace Rendering;
using namespace Resource;

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
	Destroy();
}

void ResourceManager::Initialize()
{
	_shaderMgr		= new Manager::ShaderManager;
	_textureMgr		= new Manager::TextureManager;
	_materialMgr	= new Manager::MaterialManager;
	_bufferManager	= new Manager::BufferManager;
	_originObjMgr	= new OriginObjectManager;
}

void ResourceManager::Destroy()
{
	SAFE_DELETE(_shaderMgr);
	SAFE_DELETE(_textureMgr);
	SAFE_DELETE(_materialMgr);
	SAFE_DELETE(_bufferManager);
	SAFE_DELETE(_originObjMgr);
}