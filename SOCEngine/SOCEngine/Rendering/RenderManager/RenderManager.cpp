#include "RenderManager.h"
#include "EngineShaderFactory.hpp"
#include "Director.h"

using namespace Rendering;
using namespace Rendering::Manager;
using namespace Rendering::Shader;

RenderManager::RenderManager()
{
}

RenderManager::~RenderManager()
{
	_transparentMeshes.DeleteAll(true);
	_opaqueMeshes.DeleteAll(true);
}

void RenderManager::FindShader(const Shader::VertexShader** outVertexShader, const Shader::PixelShader** outPixelShader, Rendering::Mesh::MeshFilter::BufferElementFlag bufferFlag, Rendering::Material::Type materialType, RenderType renderType, const std::string& frontShaderTypeName)
{
	std::string materialFileName = "";

	if(frontShaderTypeName.empty() == false)
		materialFileName = frontShaderTypeName + "_";
	else
	{
		ASSERT_COND_MSG(materialType == Material::Type::PhysicallyBasedModel, "RenderManager Error : currently, can not support this material type")
		{
			materialFileName = "PhysicallyBased_";
		}
	}

	if(renderType == RenderType::TileBasedDeferred)
		materialFileName += "GBuffer_";
	else if(renderType == RenderType::ForwardPlus)
		materialFileName += "ForwardPlus_";
	else
		ASSERT_MSG("Error, can not support this render type");

	if(bufferFlag & (uint)Mesh::MeshFilter::BufferElement::Normal)
	{
		bool hasBinormalFlag	= (bufferFlag & (uint)Mesh::MeshFilter::BufferElement::Binormal)	!= 0;
		bool hasTangentFlag		= (bufferFlag & (uint)Mesh::MeshFilter::BufferElement::Tangent)		!= 0;

		materialFileName += (hasBinormalFlag && hasTangentFlag) ? "_TBN" : "_N";
	}

	if(bufferFlag & (uint)Mesh::MeshFilter::BufferElement::UV)
		materialFileName += "_UV";


	const Core::Scene* scene = Device::Director::GetInstance()->GetCurrentScene();
	ShaderManager* shaderMgr = scene->GetShaderManager();

	if(outVertexShader)
		(*outVertexShader) = shaderMgr->FindVertexShader(materialFileName, BASIC_VS_MAIN_FUNC_NAME);

	if(outPixelShader)
		(*outPixelShader) = shaderMgr->FindPixelShader(materialFileName, BASIC_PS_MAIN_FUNC_NAME);
}

bool RenderManager::Init()
{
	const Core::Scene*		currentScene	= Device::Director::GetInstance()->GetCurrentScene();
	Manager::ShaderManager* shaderMgr		= currentScene->GetShaderManager();

	Factory::EngineFactory shaderLoader(shaderMgr);

	//Physically Based Shading
	{
		const std::string keys[] = {"N", "N_T0", "T0", "TBN_T0"};

		const std::string frontFileName = "PhysicallyBased_GBuffer_";
		const std::string includeFileName = "PhysicallyBased_GBuffer_Common";

		for(int i = 0; i < ARRAYSIZE(keys); ++i)
		{
			std::string fileName = frontFileName + keys[i];

			RenderShaders shaders;
			bool loadSuccess = shaderLoader.LoadShader(fileName, "VS", "PS", &includeFileName, &shaders.vs, &shaders.ps);

			ASSERT_COND_MSG(loadSuccess, "RenderManager Error : can not load physically based geometry buffer shader");
			{
				shaderMgr->RemoveShaderCode(fileName + ":vs:" + BASIC_VS_MAIN_FUNC_NAME);
				shaderMgr->RemoveShaderCode(fileName + ":vs:" + BASIC_PS_MAIN_FUNC_NAME);
			}
		}
	}

	return true;
}

bool RenderManager::Add(const Material* material, const Mesh::Mesh* mesh, MeshType type)
{
	unsigned int materialAddress = reinterpret_cast<unsigned int>(material);
	unsigned int meshAddress = reinterpret_cast<unsigned int>(mesh);

	std::pair<const Material*, const Mesh::Mesh*>* pair = Find(material, mesh, type);
	if(pair == nullptr)
		pair = new std::pair<const Material*, const Mesh::Mesh*>(material, mesh);

	if(type == MeshType::transparent)
		_transparentMeshes.Add(materialAddress, meshAddress, pair);
	else if(type == MeshType::opaque)
		_opaqueMeshes.Add(materialAddress, meshAddress, pair);
	else
	{
		DEBUG_LOG("undeclartion MeshType");
		return false;
	}

	return true;
}

void RenderManager::Change(const Material* material, const Mesh::Mesh* mesh, MeshType type)
{
	unsigned int materialAddress = reinterpret_cast<unsigned int>(material);
	unsigned int meshAddress = reinterpret_cast<unsigned int>(mesh);

	if( type == MeshType::transparent )
	{
		auto pair = _opaqueMeshes.Find(materialAddress, meshAddress);
		_opaqueMeshes.Delete(materialAddress, meshAddress, false);

		_transparentMeshes.Add(materialAddress, meshAddress, pair);
	}
	else if( type == MeshType::opaque )
	{
		auto pair = _transparentMeshes.Find(materialAddress, meshAddress);
		_transparentMeshes.Delete(materialAddress, meshAddress, false);

		_opaqueMeshes.Add(materialAddress, meshAddress, pair);
	}
}

std::pair<const Material*, const Mesh::Mesh*>* RenderManager::Find(const Material* material, const Mesh::Mesh* mesh, MeshType type)
{
	unsigned int materialAddress = reinterpret_cast<unsigned int>(material);
	unsigned int meshAddress = reinterpret_cast<unsigned int>(mesh);

	if(type == MeshType::transparent)
		return _transparentMeshes.Find(materialAddress, meshAddress);
	else if(type == MeshType::opaque)
		return _opaqueMeshes.Find(materialAddress, meshAddress);
	else
		ASSERT_MSG("Error!, undefined MeshType");

	return nullptr;
}

void RenderManager::Iterate(const std::function<void(const Material* material, const Mesh::Mesh* mesh)>& recvFunc, MeshType type) const
{
	auto MapInMapIter = [&](Structure::Map<unsigned int, std::pair<const Material*, const Mesh::Mesh*>>* content)
	{
		auto MapIter = [&](std::pair<const Material*, const Mesh::Mesh*>* content)
		{
			recvFunc(content->first, content->second);
		};
		content->IterateContent(MapIter);
	};

	if(type == MeshType::transparent)
		_transparentMeshes.GetMapInMap().IterateContent(MapInMapIter);
	else if(type == MeshType::opaque)
		_opaqueMeshes.GetMapInMap().IterateContent(MapInMapIter);
	else
	{
		ASSERT_MSG("Error!, undefined MeshType");
	}
}

void RenderManager::ForwardPlusRender(ID3D11DeviceContext* context, const Camera::ForwardPlusCamera* camera)
{
}

void RenderManager::Render(const Camera::Camera* camera)
{
	ID3D11DeviceContext* context = Device::Director::GetInstance()->GetDirectX()->GetContext();

	if(camera->GetRenderType() == RenderType::ForwardPlus)
	{
		

	}

	//Iterate(NonAlphaMeshRender, MeshType::opaque);
}