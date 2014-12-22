#include "Mesh.h"
#include "Director.h"

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
			SAFE_DELETE(_transformConstBuffer);
		}

		bool Mesh::Create(const void* vertexBufferDatas, unsigned int vertexBufferDataCount, unsigned int vertexBufferSize,
				const ENGINE_INDEX_TYPE* indicesData, unsigned int indicesCount, Material* material, bool isDynamic)
		{
			_filter = new MeshFilter;
			if(_filter->CreateBuffer(vertexBufferDatas, vertexBufferDataCount, vertexBufferSize,
				indicesData, indicesCount, isDynamic) == false)
			{
				ASSERT("Error, filter->cratebuffer");
				SAFE_DELETE(_filter);
				return false;
			}
			_indexCount = indicesCount;

			_renderer = new MeshRenderer;
			if(_renderer->AddMaterial(material, false) == false)
			{
				ASSERT("Error, renderer addmaterial");
				return false;
			}

			_transformConstBuffer = new Buffer::ConstBuffer;
			if(_transformConstBuffer->Create(sizeof(Math::Matrix) * 3) == false)
			{
				ASSERT("Error, transformBuffer->Create");
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

		void Mesh::UpdateConstBuffer(const Core::TransformPipelineParam& transpose_Transform)
		{
			ID3D11DeviceContext* context = Device::Director::GetInstance()->GetDirectX()->GetContext();
			_transformConstBuffer->Update(context, &transpose_Transform);

			//귀찮으니 여기서 겸사겸사 체크 하자
			_renderer->ClassifyMaterialWithMesh(this);
		}

		void Mesh::Render(Material* custom)
		{
			if(_renderer == nullptr || _filter == nullptr)
				return;

			ID3D11DeviceContext* context = Device::Director::GetInstance()->GetDirectX()->GetContext();
			_filter->IASetBuffer(context);

			if(custom == nullptr)
			{
				if(_updateType == MaterialUpdateType::All)
					_renderer->UpdateAllMaterial(context, _transformConstBuffer);
				else if(_updateType == MaterialUpdateType::One)
					_renderer->UpdateMaterial(context, _selectMaterialIndex, _transformConstBuffer);
			}
			else
			{
				//직접 입력하는 Material의 경우,
				//Renderer에서 처리하지 않고 그냥 여기서 바로 처리
				custom->UpdateResources(context);
				custom->UpdateTransformBuffer(context, _transformConstBuffer);
			}

			context->DrawIndexed(_indexCount, 0, 0);
			_renderer->ClearResource(context);
		}

		void Mesh::Destroy()
		{
			SAFE_DELETE(_filter);
			SAFE_DELETE(_renderer);
		}
	}
}