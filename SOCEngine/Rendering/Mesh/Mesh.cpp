#include "Mesh.h"
#include "Director.h"

namespace Rendering
{
	namespace Mesh
	{
		Mesh::Mesh() : _filter(nullptr), _renderer(nullptr), _selectMaterialIndex(0), _indexCount(0)
		{
			_updateType = MaterialUpdateType::All;
		}

		Mesh::~Mesh()
		{
			Destroy();
		}

		bool Mesh::Create(const LPVoidType vertexBufferDatas, unsigned int vertexBufferDataCount, unsigned int vertexBufferSize,
				const ENGINE_INDEX_TYPE* indicesData, unsigned int indicesCount, Material::Material* material, bool isDynamic)
		{
			_filter = new MeshFilter;
			if(_filter->CreateBuffer(vertexBufferDatas.GetBuffer(), vertexBufferDataCount, vertexBufferSize,
				indicesData, indicesCount, isDynamic) == false)
			{
				SAFE_DELETE(_filter);
				return false;
			}
			_indexCount = indicesCount;

			_renderer = new MeshRenderer;
			if(_renderer->AddMaterial(material, false) == false)
				return false;

			if(_renderer->init() == false)
				return false;

			return true;
		}

		void Mesh::Initialize()
		{
		}

		void Mesh::Update(float deltaTime)
		{

		}

		void Mesh::Render(const Core::TransformPipelineParam& transform, const std::vector<Rendering::Light::LightForm*> *lights, const Math::Vector4& viewPos)
		{
			if(_renderer == nullptr || _filter == nullptr)
				return;

			ID3D11DeviceContext* context = Device::Director::GetInstance()->GetDirectX()->GetContext();
			_filter->UpdateBuffer(context);

			if(_updateType == MaterialUpdateType::All)
				_renderer->UpdateAllMaterial(context, transform);
			else if(_updateType == MaterialUpdateType::One)
			{
				if(_renderer->UpdateMaterial(context, _selectMaterialIndex, transform) == false)
				{
					//??
				}
			}

			context->DrawIndexed(_indexCount, 0, 0);
		}

		void Mesh::Destroy()
		{
			SAFE_DELETE(_filter);
			SAFE_DELETE(_renderer);
		}
	}
}