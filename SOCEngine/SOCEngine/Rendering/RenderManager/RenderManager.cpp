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
	_alphaMeshes.DeleteAll(true);
	_nonAlphaMeshes.DeleteAll(true);
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

	if(type == MeshType::hasAlpha)
		_alphaMeshes.Add(materialAddress, meshAddress, pair);
	else if(type == MeshType::nonAlpha)
		_nonAlphaMeshes.Add(materialAddress, meshAddress, pair);
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

	if( type == MeshType::hasAlpha )
	{
		auto pair = _nonAlphaMeshes.Find(materialAddress, meshAddress);
		_nonAlphaMeshes.Delete(materialAddress, meshAddress, false);

		_alphaMeshes.Add(materialAddress, meshAddress, pair);
	}
	else if( type == MeshType::nonAlpha )
	{
		auto pair = _alphaMeshes.Find(materialAddress, meshAddress);
		_alphaMeshes.Delete(materialAddress, meshAddress, false);

		_nonAlphaMeshes.Add(materialAddress, meshAddress, pair);
	}
}

std::pair<const Material*, const Mesh::Mesh*>* RenderManager::Find(const Material* material, const Mesh::Mesh* mesh, MeshType type)
{
	unsigned int materialAddress = reinterpret_cast<unsigned int>(material);
	unsigned int meshAddress = reinterpret_cast<unsigned int>(mesh);

	if(type == MeshType::hasAlpha)
		return _alphaMeshes.Find(materialAddress, meshAddress);
	else if(type == MeshType::nonAlpha)
		return _nonAlphaMeshes.Find(materialAddress, meshAddress);
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

	if(type == MeshType::hasAlpha)
		_alphaMeshes.GetMapInMap().IterateContent(MapInMapIter);
	else if(type == MeshType::nonAlpha)
		_nonAlphaMeshes.GetMapInMap().IterateContent(MapInMapIter);
	else
	{
		ASSERT_MSG("Error!, undefined MeshType");
	}
}

void RenderManager::ForwardPlusRender(ID3D11DeviceContext* context, const Camera::ForwardPlusCamera* camera)
{
	const Device::DirectX* dx = Device::Director::GetInstance()->GetDirectX();

	const Material* prevMtl = nullptr;
	auto NonAlphaMeshRender = [&](const Material* material, const Mesh::Mesh* mesh)
	{
		const RenderShaders& shaders = material->GetShaderTargets();

		if(shaders.ableRender() == false)
			return;

		if(material != prevMtl)
		{
			VertexShader* vs = shaders.vs;
			vs->UpdateShader(context);

			PixelShader* ps = shaders.ps;
			ps->UpdateShader(context);

			GeometryShader* gs = shaders.gs;
			ASSERT_COND_MSG(gs, "cant support this version");

			HullShader* hs = shaders.hs;
			ASSERT_COND_MSG(hs, "cant support this version");

			prevMtl = material;
		}		

		Mesh::MeshFilter* filter = mesh->GetMeshFilter();
		filter->IASetBuffer(context);

		context->DrawIndexed(filter->GetIndexCount(), 0, 0);
	};
	auto AlphaMeshRender = [&](const Material* material, const Mesh::Mesh* mesh)
	{
	};
}

void RenderManager::Render(const Camera::Camera* camera)
{
	ID3D11DeviceContext* context = Device::Director::GetInstance()->GetDirectX()->GetContext();

	if(camera->GetRenderType() == RenderType::ForwardPlus)
	{
		

	}

	//Iterate(NonAlphaMeshRender, MeshType::nonAlpha);
}