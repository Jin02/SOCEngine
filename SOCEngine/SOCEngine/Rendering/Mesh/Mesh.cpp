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
				const ENGINE_INDEX_TYPE* indicesData, unsigned int indicesCount, Material* material, bool isDynamic)
		{
			_filter = new MeshFilter;
			if(_filter->CreateBuffer(vertexBufferDatas, vertexBufferDataCount, vertexBufferSize,
				indicesData, indicesCount, isDynamic) == false)
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

			//귀찮으니 여기서 겸사겸사 체크 하자
			_renderer->ClassifyMaterialWithMesh(this);
		}

		void Mesh::Render(Material* custom, const Buffer::ConstBuffer* cameraConstBuffer)
		{
			if(_renderer == nullptr || _filter == nullptr)
				return;

			ID3D11DeviceContext* context = Device::Director::GetInstance()->GetDirectX()->GetContext();
			_filter->IASetBuffer(context);

			if(custom == nullptr)
			{
				//if(_updateType == MaterialUpdateType::All)
				//	_renderer->UpdateAllMaterial(context, _transformConstBuffer, cameraConstBuffer);
				//else if(_updateType == MaterialUpdateType::One)
				//	_renderer->UpdateMaterial(context, _selectMaterialIndex, _transformConstBuffer, cameraConstBuffer);
			}
			else
			{
				//직접 입력하는 Material의 경우,
				//Renderer에서 처리하지 않고 그냥 여기서 바로 처리
				//custom->UpdateConstBuffer(context, _transformConstBuffer, cameraConstBuffer);
				//custom->UpdateResources(context);
			}

			context->DrawIndexed(_indexCount, 0, 0);
//			_renderer->ClearResource(context);
		}

		void Mesh::Destroy()
		{
			SAFE_DELETE(_filter);
			SAFE_DELETE(_renderer);
			SAFE_DELETE(_transformConstBuffer);
		}
	}
}