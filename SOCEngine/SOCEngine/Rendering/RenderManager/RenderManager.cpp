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

	if(renderType == RenderType::Deferred)
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
		const std::string keys[] = {"N_UV", "UV", "TBN_UV"};

		const std::string frontFileName = "PhysicallyBased_ForwardPlus_";
		const std::string includeFileName = "PhysicallyBased_ForwardPlus_Common";

		std::vector<std::string> macros;
		{
			bool enableMSAA = Device::Director::GetInstance()->GetDirectX()->GetUseMSAA();
			if(enableMSAA)
			{
				macros.push_back("#define MSAA_ENABLE");
			}
		}

		auto LoadShader = [&](const std::string& fileName, const std::string& vsFuncName, const std::string& psFuncName)
		{
			ShaderGroup shaders;
			shaderLoader.LoadShader(fileName, vsFuncName, psFuncName, &includeFileName, &macros, &shaders.vs, &shaders.ps);

			ASSERT_COND_MSG(shaders.vs, "RenderManager Error : can not load physically based material shader");
			return shaders;
		};

		Shaders shaders;
		for(int i = 0; i < ARRAYSIZE(keys); ++i)
		{
			std::string fileName = frontFileName + keys[i];
			shaders.renderScene = LoadShader(fileName, "VS", "PS");
			shaders.depthWrite = LoadShader(fileName, "PositionOnlyVS", "");
			shaders.alphaTestWithDiffuse = LoadShader(fileName, "AlphaTestWithDiffuseVS", "AlphaTestWithDiffusePS");

			_physicallyBasedShaders.insert(std::make_pair(fileName, shaders));
		}

		shaderMgr->RemoveAllShaderCode();
	}

	return true;
}

void RenderManager::UpdateRenderQueue(const Mesh::Mesh* mesh, MeshType type)
{
	unsigned int meshAddress = reinterpret_cast<unsigned int>(mesh);

	if( type == MeshType::Transparent )
	{
		_opaqueMeshes.Delete(meshAddress, false);

		if(_transparentMeshes.Find(meshAddress) == nullptr)
			_transparentMeshes.Add(meshAddress, mesh);
	}
	else if( type == MeshType::Opaque )
	{
		_transparentMeshes.Delete(meshAddress, false);

		if(_opaqueMeshes.Find(meshAddress) == nullptr)
			_opaqueMeshes.Add(meshAddress, mesh);
	}
}

const Mesh::Mesh* RenderManager::FindMeshFromRenderQueue(const Mesh::Mesh* mesh, MeshType type)
{
	unsigned int meshAddress = reinterpret_cast<unsigned int>(mesh);

	if(type == MeshType::Transparent)
		return _transparentMeshes.Find(meshAddress);
	else if(type == MeshType::Opaque)
		return _opaqueMeshes.Find(meshAddress);
	else
		ASSERT_MSG("Error!, undefined MeshType");

	return nullptr;
}

//void RenderManager::Iterate(const std::function<void(const Material* material, const Mesh::Mesh* mesh)>& recvFunc, MeshType type) const
//{
//	auto MapInMapIter = [&](Structure::Map<unsigned int, std::pair<const Material*, const Mesh::Mesh*>>* content)
//	{
//		auto MapIter = [&](std::pair<const Material*, const Mesh::Mesh*>* content)
//		{
//			recvFunc(content->first, content->second);
//		};
//		content->IterateContent(MapIter);
//	};
//
//	if(type == MeshType::Transparent)
//		_transparentMeshes.GetMapInMap().IterateContent(MapInMapIter);
//	else if(type == MeshType::Opaque)
//		_opaqueMeshes.GetMapInMap().IterateContent(MapInMapIter);
//	else
//	{
//		ASSERT_MSG("Error!, undefined MeshType");
//	}
//}

void RenderManager::Iterate(const std::function<void(const Mesh::Mesh* mesh)>& recvFunc, Camera::Camera* criterionCamera, MeshType type)
{

}