#include "MeshRenderer.h"
#include "Director.h"

namespace Rendering
{
	using namespace Mesh;
	using namespace Buffer;
	using namespace Shader;

	MeshRenderer::MeshRenderer()
	{
	}

	MeshRenderer::~MeshRenderer()
	{
	}

	bool MeshRenderer::AddMaterial(Material* material, bool copy)
	{
		if(_materials.Find(material->GetName()) )
			return false;

		_materials.Add(material->GetName(), material, copy);
		return true;
	}

	void MeshRenderer::UpdateAllMaterial(ID3D11DeviceContext* context, Buffer::ConstBuffer* transformBuffer)
	{
		Sampler* sampler = Device::Director::GetInstance()->GetCurrentScene()->GetSampler();
		std::vector<Shader::PixelShader::SamplerType> samplers;
		
		//임시로, 0번에 linear만 넣음.
		samplers.push_back(std::make_pair(0, sampler));

		auto& materials = _materials.GetVector();
		for(auto iter = materials.begin(); iter != materials.end(); ++iter)
		{
			Material* material = GET_CONTENT_FROM_ITERATOR(iter);
			material->UpdateTransformBuffer(context, transformBuffer);
			material->UpdateShader(context, samplers);
		}
	}

	bool MeshRenderer::UpdateMaterial(ID3D11DeviceContext* context, unsigned int index, Buffer::ConstBuffer* transformBuffer)
	{
		if( index >= _materials.GetSize() )
			return false;

		Material* material = GET_CONTENT_FROM_ITERATOR( (_materials.GetVector().begin() + index) );
		if(material == nullptr)
			return false;

		Sampler* sampler = Device::Director::GetInstance()->GetCurrentScene()->GetSampler();
		std::vector<Shader::PixelShader::SamplerType> samplers;
		
		//임시로, 0번에 linear만 넣음.
		samplers.push_back(std::make_pair(0, sampler));
		material->UpdateTransformBuffer(context, transformBuffer);
		material->UpdateShader(context, samplers);
		return true;
	}

	void MeshRenderer::ClearResource(ID3D11DeviceContext* context)
	{
		auto& materials = _materials.GetVector();
		for(auto iter = materials.begin(); iter != materials.end(); ++iter)
			GET_CONTENT_FROM_ITERATOR(iter)->ClearResource(context);
	}

	bool MeshRenderer::CheckAlphaMaterial()
	{
		auto& materials = _materials.GetVector();
		for(auto iter = materials.begin(); iter != materials.end(); ++iter)
		{
			Material* material = GET_CONTENT_FROM_ITERATOR(iter);
			bool hasAlpha = material->GetHasAlpha();
			if(hasAlpha)
				return true;
		}

		return false;
	}
}