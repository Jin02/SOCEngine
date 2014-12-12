#include "MeshRenderer.h"
#include "Director.h"

namespace Rendering
{
	using namespace Mesh;
	using namespace Buffer;
	using namespace Shader;

	MeshRenderer::MeshRenderer() : _optionalVertexShaderConstBuffers(nullptr),
		_vertexShaderUsingTextures(nullptr), _pixelShaderUsingConstBuffer(nullptr),
		_transformBuffer(nullptr)
	{
		_vertexShaderConstBufferUpdateType = VertexShaderConstBufferUpdateType::Add;
	}

	MeshRenderer::~MeshRenderer()
	{
		SAFE_DELETE(_transformBuffer);
	}

	bool MeshRenderer::init()
	{
		_transformBuffer = new ConstBuffer;
		return _transformBuffer->Create(sizeof(Math::Matrix) * 3, 0, ConstBuffer::Usage::VertexShader);
	}

	bool MeshRenderer::AddMaterial(Material* material, bool copy)
	{
		if(_materials.Find(material->GetName()) )
			return false;

		_materials.Add(material->GetName(), material, copy);
		return true;
	}

	void MeshRenderer::UpdateVSBasicConstBufferData(ID3D11DeviceContext* context, std::vector<BaseShader::BufferType>& vertexShaderConstBuffers, const Core::TransformPipelineParam& transform)
	{	
		_transformBuffer->UpdateSubresource(context, &transform);
		vertexShaderConstBuffers.push_back(BaseShader::BufferType(Material::BasicConstBuffercSlot::Transform, _transformBuffer));

		if(_optionalVertexShaderConstBuffers)
			vertexShaderConstBuffers.insert(vertexShaderConstBuffers.end(), _optionalVertexShaderConstBuffers->begin(), _optionalVertexShaderConstBuffers->end());
	}

	void MeshRenderer::UpdateAllMaterial(ID3D11DeviceContext* context, const Core::TransformPipelineParam& transform)
	{
		std::vector<BaseShader::BufferType> vertexShaderConstBuffers;
		const std::vector<BaseShader::BufferType>* vsConstBufferTarget = &vertexShaderConstBuffers;
		if(_vertexShaderConstBufferUpdateType == VertexShaderConstBufferUpdateType::Add)
			UpdateVSBasicConstBufferData(context, vertexShaderConstBuffers, transform);
		else
			vsConstBufferTarget = _optionalVertexShaderConstBuffers;

		Sampler* sampler = Device::Director::GetInstance()->GetCurrentScene()->GetSampler();

		auto& materials = _materials.GetVector();
		for(auto iter = materials.begin(); iter != materials.end(); ++iter)
		{
			VertexShader* vs = GET_CONTENT_FROM_ITERATOR(iter)->GetVertexShader();
			vs->UpdateShader(context, vsConstBufferTarget, _vertexShaderUsingTextures);

			PixelShader* ps = GET_CONTENT_FROM_ITERATOR(iter)->GetPixelShader();
			const std::vector<BaseShader::TextureType>& textures = GET_CONTENT_FROM_ITERATOR(iter)->GetTextures();
			ps->UpdateShader(context,	_pixelShaderUsingConstBuffer, 
										&GET_CONTENT_FROM_ITERATOR(iter)->GetConstBuffers(),
										&textures, PixelShader::SamplerType(0, sampler));
		}
	}

	bool MeshRenderer::UpdateMaterial(ID3D11DeviceContext* context, unsigned int index, const Core::TransformPipelineParam& transform)
	{
		if( index >= _materials.GetSize() )
			return false;

		std::vector<BaseShader::BufferType> vertexShaderConstBuffers;
		const std::vector<BaseShader::BufferType>* vsConstBufferTarget = &vertexShaderConstBuffers;
		if(_vertexShaderConstBufferUpdateType == VertexShaderConstBufferUpdateType::Add)
			UpdateVSBasicConstBufferData(context, vertexShaderConstBuffers, transform);
		else
		{
			vsConstBufferTarget = _optionalVertexShaderConstBuffers;
		}

		Material* material = GET_CONTENT_FROM_ITERATOR((_materials.GetVector().begin()+index));

		VertexShader* vs = material->GetVertexShader();
		vs->UpdateShader(context, vsConstBufferTarget, _vertexShaderUsingTextures);


		PixelShader* ps = material->GetPixelShader();
		Sampler* sampler = Device::Director::GetInstance()->GetCurrentScene()->GetSampler();
		ps->UpdateShader(context, _pixelShaderUsingConstBuffer, &material->GetConstBuffers(), &material->GetTextures(), PixelShader::SamplerType(0, sampler));

		return true;
	}

	void MeshRenderer::SetOptionalVSConstBuffers(const std::vector<BaseShader::BufferType>* constBuffers, VertexShaderConstBufferUpdateType updateType)
	{
		_optionalVertexShaderConstBuffers = constBuffers;
		_vertexShaderConstBufferUpdateType = updateType;
	}
}