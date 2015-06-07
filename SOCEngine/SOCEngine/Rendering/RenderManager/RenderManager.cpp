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

void RenderManager::Iterate(const std::function<void(const Mesh::Mesh* mesh)>& recvFunc, MeshType type, Camera::Camera* camera)
{

}