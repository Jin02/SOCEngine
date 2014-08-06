#include "MeshRenderer.h"
#include "Director.h"

namespace Rendering
{
	using namespace Mesh;

	MeshRenderer::MeshRenderer() : _optionalVertexShaderConstBuffers(nullptr),
		_vertexShaderUsingTextures(nullptr), _pixelShaderUsingConstBuffer(nullptr)
	{
		_usingMatrix = UsingTransformMatrix::AllCalculate;
		_vertexShaderConstBufferUpdateType = VertexShaderConstBufferUpdateType::Add;
	}

	MeshRenderer::~MeshRenderer()
	{
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
		Buffer::ConstBufferManager* cbMgr = Device::Director::GetInstance()->GetCurrentScene()->GetConstBufferManager();

		unsigned int bufferSize = 0;
		const char* bufferType = "";
		const void* bufferData = nullptr;
		if(_usingMatrix == UsingTransformMatrix::OneByOne)
		{
			bufferSize = sizeof(Math::Matrix) * 3; //world,view,proj 행렬 3개
			bufferType = "Transform_OneByOne";
			bufferData = &transform; //wvp가 앞이라 걱정 ㄴㄴ
		}
		else if(_usingMatrix == UsingTransformMatrix::AllCalculate)
		{
			bufferSize = sizeof(Math::Matrix); //wvp 통합
			bufferType = "Transform_AllCalculate";
			bufferData = &(transform.worldViewProjMat);
		}

		Buffer::ConstBuffer* cb = cbMgr->Find(bufferType);
		if(cb == nullptr)
			cb = cbMgr->AddBuffer(bufferType, bufferSize);

		cb->UpdateSubresource(context, bufferData);
		vertexShaderConstBuffers.push_back(Shader::BaseShader::BufferType(0, cb));

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
		{
			vsConstBufferTarget = _optionalVertexShaderConstBuffers;
		}

		Sampler* sampler = Device::Director::GetInstance()->GetCurrentScene()->GetSampler();

		auto& v = _materials.GetVector();
		unsigned int index = 0;
		for(auto iter = v.begin(); iter != v.end(); ++iter, ++index)
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