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
			_selectMaterialIndex(0), _transformConstBuffer(nullptr)
		{
			_updateType = MaterialUpdateType::All;
		}

		Mesh::~Mesh()
		{
			OnDestroy();
		}

		bool Mesh::Initialize(const CreateFuncArguments& args)
		{
			_filter = new MeshFilter;
			if(_filter->CreateBuffer(args) == false)
			{
				ASSERT_MSG("Error, filter->cratebuffer");
				SAFE_DELETE(_filter);
				return false;
			}

			_renderer = new MeshRenderer;
			if(_renderer->AddMaterial(args.material, false) == false)
			{
				ASSERT_MSG("Error, renderer addmaterial");
				return false;
			}

			_transformConstBuffer = new Buffer::ConstBuffer;
			if(_transformConstBuffer->Initialize(sizeof(Core::TransformPipelineShaderInput)) == false)
			{
				ASSERT_MSG("Error, transformBuffer->Initialize");
				return false;
			}

			_renderer->ClassifyMaterialWithMesh(this);
			return true;
		}

		void Mesh::OnInitialize()
		{
		}

		void Mesh::OnUpdate(float deltaTime)
		{
		}

		void Mesh::OnUpdateTransformCB(const Core::TransformPipelineShaderInput& transpose_Transform)
		{
			ID3D11DeviceContext* context = Device::Director::GetInstance()->GetDirectX()->GetContext();
			_transformConstBuffer->Update(context, &transpose_Transform);

			//±ÍÂúÀ¸´Ï ¿©±â¼­ °â»ç°â»ç Ã¼Å© ÇÏÀÚ
			_renderer->ClassifyMaterialWithMesh(this);
		}

		void Mesh::OnDestroy()
		{
			SAFE_DELETE(_filter);
			SAFE_DELETE(_renderer);
			SAFE_DELETE(_transformConstBuffer);
		}
	}
}