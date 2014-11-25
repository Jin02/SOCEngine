#include "MeshRenderer.h"
#include "Director.h"

namespace Rendering
{
	using namespace Mesh;

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
		_transformBuffer = new Buffer::ConstBuffer;
		return _transformBuffer->Create(sizeof(Math::Matrix) * 3);
	}

	bool MeshRenderer::AddMaterial(Material::Material* material, bool copy)
	{
		if(_materials.Find(material->GetName()) )
			return false;

		_materials.Add(material->GetName(), material, copy);
		return true;
	}

	void MeshRenderer::UpdateVSBasicConstBufferData(ID3D11DeviceContext* context, std::vector<Shader::BaseShader::BufferType>& vertexShaderConstBuffers, const Core::TransformPipelineParam& transform)
	{	
		_transformBuffer->UpdateSubresource(context, &transform);
		vertexShaderConstBuffers.push_back(Shader::BaseShader::BufferType(0, _transformBuffer));

		if(_optionalVertexShaderConstBuffers)
			vertexShaderConstBuffers.insert(vertexShaderConstBuffers.end(), _optionalVertexShaderConstBuffers->begin(), _optionalVertexShaderConstBuffers->end());
	}

	void MeshRenderer::UpdateAllMaterial(ID3D11DeviceContext* context, const Core::TransformPipelineParam& transform)
	{
		std::vector<Shader::BaseShader::BufferType> vertexShaderConstBuffers;
		const std::vector<Shader::BaseShader::BufferType>* vsConstBufferTarget = &vertexShaderConstBuffers;
		if(_vertexShaderConstBufferUpdateType == VertexShaderConstBufferUpdateType::Add)
			UpdateVSBasicConstBufferData(context, vertexShaderConstBuffers, transform);
		else
			vsConstBufferTarget = _optionalVertexShaderConstBuffers;

		Sampler* sampler = Device::Director::GetInstance()->GetCurrentScene()->GetSampler();

		auto& materials = _materials.GetVector();
		unsigned int index = 0;
		for(auto iter = materials.begin(); iter != materials.end(); ++iter, ++index)
		{
			Shader::VertexShader* vs = GET_CONTENT_FROM_ITERATOR(iter)->GetVertexShader();
			vs->UpdateShader(context, vsConstBufferTarget, _vertexShaderUsingTextures);

			Shader::PixelShader* ps = GET_CONTENT_FROM_ITERATOR(iter)->GetPixelShader();
			const std::vector<const Texture::Texture*>& textures = GET_CONTENT_FROM_ITERATOR(iter)->GetTextures();
			ps->UpdateShader(context, _pixelShaderUsingConstBuffer, textures, Shader::PixelShader::SamplerType(0, sampler));
		}
	}

	bool MeshRenderer::UpdateMaterial(ID3D11DeviceContext* context, unsigned int index, const Core::TransformPipelineParam& transform)
	{
		if( index >= _materials.GetSize() )
			return false;

		std::vector<Shader::BaseShader::BufferType> vertexShaderConstBuffers;
		const std::vector<Shader::BaseShader::BufferType>* vsConstBufferTarget = &vertexShaderConstBuffers;
		if(_vertexShaderConstBufferUpdateType == VertexShaderConstBufferUpdateType::Add)
			UpdateVSBasicConstBufferData(context, vertexShaderConstBuffers, transform);
		else
		{
			vsConstBufferTarget = _optionalVertexShaderConstBuffers;
		}

		Material::Material* material = GET_CONTENT_FROM_ITERATOR((_materials.GetVector().begin()+index));

		Shader::VertexShader* vs = material->GetVertexShader();
		vs->UpdateShader(context, vsConstBufferTarget, _vertexShaderUsingTextures);

		Shader::PixelShader* ps = material->GetPixelShader();
		Sampler* sampler = Device::Director::GetInstance()->GetCurrentScene()->GetSampler();
		ps->UpdateShader(context, _pixelShaderUsingConstBuffer, material->GetTextures(), Shader::PixelShader::SamplerType(0, sampler));

		return true;
	}

	void MeshRenderer::SetOptionalVSConstBuffers(const std::vector<Shader::BaseShader::BufferType>* constBuffers, VertexShaderConstBufferUpdateType updateType)
	{
		_optionalVertexShaderConstBuffers = constBuffers;
		_vertexShaderConstBufferUpdateType = updateType;
	}
}