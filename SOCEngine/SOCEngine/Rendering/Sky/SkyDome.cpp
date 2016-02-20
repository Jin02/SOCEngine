#include "SkyDome.h"
#include "ResourceManager.h"

using namespace Rendering::Sky;
using namespace Rendering::Geometry;
using namespace Rendering::Manager;
using namespace Rendering::Buffer;
using namespace Resource;

SkyDome::SkyDome()
	: _filter(nullptr), _renderer(nullptr)
{
}

SkyDome::~SkyDome()
{
}

void SkyDome::Initialize(const std::string& domeTexturePath)
{
	const ResourceManager* resMgr	= ResourceManager::SharedInstance();
	const BufferManager* bufferMgr	= resMgr->GetBufferManager();

	//VertexBuffer* vb = nullptr;
	//bufferMgr->Find(&vb, "file", "key");

	//IndexBuffer* ib = nullptr;
	//bufferMgr->Find(&ib, "file", "key");
}