#include "Mesh.h"
#include "Director.h"
//#include "TransformPipelineParam.h"

namespace Rendering
{
	using namespace Manager;
	namespace Mesh
	{
		Mesh::Mesh() : 
			_filter(nullptr), _renderer(nullptr), 
			_selectMaterialIndex(0), _indexCount(0), 
			_transformConstBuffer(nullptr)
		{
			_updateType = MaterialUpdateType::All;
		}

		Mesh::~Mesh()
		{
			Destroy();
		}

		bool Mesh::Create(const void* vertexBufferDatas, unsigned int vertexBufferDataCount, unsigned int vertexBufferSize,
				const ENGINE_INDEX_TYPE* indicesData, unsigned int indicesCount, Material* material, bool isDynamic, MeshFilter::BufferElementFlag flag)
		{
			_filter = new MeshFilter;
			if(_filter->CreateBuffer(vertexBufferDatas, vertexBufferDataCount, vertexBufferSize,
				indicesData, indicesCount, isDynamic, flag) == false)
			{
				ASSERT_MSG("Error, filter->cratebuffer");
				SAFE_DELETE(_filter);
				return false;
			}
			_indexCount = indicesCount;

			_renderer = new MeshRenderer;
			if(_renderer->AddMaterial(material, false) == false)
			{
				ASSERT_MSG("Error, renderer addmaterial");
				return false;
			}

			_transformConstBuffer = new Buffer::ConstBuffer;
			if(_transformConstBuffer->Create(sizeof(Core::TransformPipelineShaderInput)) == false)
			{
				ASSERT_MSG("Error, transformBuffer->Create");
				return false;
			}

			_renderer->ClassifyMaterialWithMesh(this);
			return true;
		}

		void Mesh::Initialize()
		{
		}

		void Mesh::Update(float deltaTime)
		{
		}

		void Mesh::UpdateTransformCB(const Core::TransformPipelineShaderInput& transpose_Transform)
		{
			ID3D11DeviceContext* context = Device::Director::GetInstance()->GetDirectX()->GetContext();
			_transformConstBuffer->Update(context, &transpose_Transform);

			//±ÍÂúÀ¸´Ï ¿©±â¼­ °â»ç°â»ç Ã¼Å© ÇÏÀÚ
			_renderer->ClassifyMaterialWithMesh(this);
		}

		void Mesh::Destroy()
		{
			SAFE_DELETE(_filter);
			SAFE_DELETE(_renderer);
			SAFE_DELETE(_transformConstBuffer);
		}
	}
}