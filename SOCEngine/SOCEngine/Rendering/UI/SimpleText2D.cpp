#include "SimpleText2D.h"
#include "Director.h"
#include "EngineShaderFactory.hpp"
#include "FontLoader.h"

using namespace UI;
using namespace Rendering;
using namespace Device;

SimpleText2D::SimpleText2D(const std::string& name, const Core::Object* parent)
	: UIObject(parent), _meshFilter(nullptr), _material(nullptr), _isOtherMaterial(false)
{
	_name = name;
}

SimpleText2D::~SimpleText2D()
{
	if(_isOtherMaterial == false)
		SAFE_DELETE(_material);

	SAFE_DELETE(_meshFilter);
}

void SimpleText2D::Initialize(uint maxLength, Rendering::Material* material)
{
	const Device::Director* director	= Device::Director::GetInstance();
	const Device::DirectX* dx			= director->GetDirectX();

	_screenSize = director->GetWindowSize();
	_maxLength = maxLength;

	const FontLoader* fontLoader =
		FontLoader::GetInstance()->Initialize(	TEMP_FONT_DATA_PATH, 
												TEMP_FONT_TEXTURE_PATH);

	if(material == nullptr)
	{
		_material = new Material(_name, Material::Type::UI);
		_isOtherMaterial = true;

		auto shaderMgr = Director::GetInstance()->GetCurrentScene()->GetShaderManager();
		Factory::EngineFactory factory(shaderMgr);

		Shader::VertexShader*	vs = nullptr;
		Shader::PixelShader*	ps = nullptr;
		factory.LoadShader("SimpleUIImage2D", "VS", "PS", nullptr, &vs, &ps);

		_material->SetShaderTargets( Shader::RenderShaders(vs, ps) );

		const Texture::Texture* texture = fontLoader->GetTexture();
		_material->SetVariable<const Texture::Texture*>("font", texture);
	}

	std::vector<RectVertexInfo> emptyVertices(maxLength * 4);
	memset(emptyVertices.data(), 0, sizeof(RectVertexInfo) * (maxLength * 4));

	std::vector<uint> indices;
	{
		uint boxIndices[] =
		{
			0, 1, 2,
			1, 3, 2
		};

		for(uint i=0; i<maxLength; ++i)
		{
			for(uint j=0; j<6; ++j)
				indices.push_back(i * 4 + boxIndices[j]);
		}
//			std::copy(boxIndices.begin(), boxIndices.end(), indices.begin() + (i * boxIndices.size()));
	}

	Mesh::MeshFilter::CreateFuncArguments meshCreateArgs("UI", "Font");
	{
		meshCreateArgs.vertex.data		= emptyVertices.data();
		meshCreateArgs.vertex.count		= emptyVertices.size();
		meshCreateArgs.vertex.byteWidth	= sizeof(RectVertexInfo);

		meshCreateArgs.index.data		= indices.data();
		meshCreateArgs.index.count		= indices.size();
		meshCreateArgs.index.byteWidth	= 0; //not use

		meshCreateArgs.isDynamic		= true;
		meshCreateArgs.bufferFlag		= (uint)Mesh::MeshFilter::BufferElement::UV;
	}

	_meshFilter = new Mesh::MeshFilter;
	bool success = _meshFilter->CreateBuffer(meshCreateArgs);
	ASSERT_COND_MSG(success, "Error, cant create SimpleImage2D meshfilter");

	Manager::UIManager* uiMgr = Director::GetInstance()->GetCurrentScene()->GetUIManager();
	uiMgr->AddRenderQueue(_name, this);

	if(_root == this)
		uiMgr->AddUpdateQueue(this);

	UIObject::InitConstBuffer();
}

void SimpleText2D::UpdateText(const std::string& text)
{
	ASSERT_COND_MSG(text.length() < _maxLength, "Error, text's length is longer than maximum length");

	const FontLoader* fontLoader = FontLoader::GetInstance();
	const auto& fontTypes = fontLoader->GetFonts();
	const auto& fontTextureSize = fontLoader->GetFontTextureSize();
	float halfH = (float)fontTextureSize.h / 2.0f;

	std::vector<RectVertexInfo> vertices;
	float offsetX = 0.0f;
	for(uint i =0; i<text.length(); ++i)
	{
		int letter = ((int)text[i]) - 32;

		//If the letter is a space then just move over three pixels. 
		if(letter == 0) 
		{
			offsetX += 3.0f;
		}
		else
		{
			RectVertexInfo vertex;

			float left		= offsetX;
			float right		= offsetX + fontTypes[letter].size;
			float top		= halfH;
			float bottom	= -halfH;

			//Top Left
			vertex.position = Math::Vector3(left, top, 0.0f);
			vertex.uv		= Math::Vector2(fontTypes[letter].left, 0);
			vertices.push_back(vertex);

			//Top Right
			vertex.position = Math::Vector3(right, top, 0.0f);
			vertex.uv = Math::Vector2(fontTypes[letter].right, 0);
			vertices.push_back(vertex);

			//Bottom Left
			vertex.position = Math::Vector3(left, bottom, 0.0f);
			vertex.uv = Math::Vector2(fontTypes[letter].left, 1);
			vertices.push_back(vertex);

			//Bottom Right
			vertex.position = Math::Vector3(right, bottom, 0.0f);
			vertex.uv = Math::Vector2(fontTypes[letter].right, 1);
			vertices.push_back(vertex);

			offsetX += (fontTypes[letter].size + 1.0f);
		}
	}

	//중점으로 맞춤
	{
		for(auto iter : vertices)
			iter.position.x -= (offsetX / 2.0f);
	}

	const Device::Director* director	= Device::Director::GetInstance();
	const Device::DirectX* dx			= director->GetDirectX();
	_meshFilter->UpdateVertexBufferData(dx->GetContext(), vertices.data(), sizeof(RectVertexInfo) * vertices.size());
}

void SimpleText2D::Render(ID3D11DeviceContext* context, const Math::Matrix& viewProjMat)
{
	Shader::VertexShader* vs = _material->GetShaderTargets().vs;
	Shader::PixelShader* ps = _material->GetShaderTargets().ps;

	if(_material->GetShaderTargets().ableRender())
	{		
		_meshFilter->IASetBuffer(context);

		vs->UpdateShaderToContext(context);
		vs->UpdateInputLayoutToContext(context);

		ps->UpdateShader(context);

		std::vector<Shader::BaseShader::BufferType> constBuffers;
		{
			Shader::BaseShader::BufferType type;
			type.first = 0;
			type.second = dynamic_cast<Buffer::BaseBuffer*>(_transformCB);

			constBuffers.push_back(type);
		}
		std::vector<Shader::BaseShader::TextureType> textures;
		{
			Shader::BaseShader::TextureType type;
			type.first = 0;

			const Texture::Texture* tex = nullptr;
			_material->GetVariable<const Texture::Texture*>(tex, "font");

			type.second = tex;

			textures.push_back(type);
		}

		vs->UpdateResources(context, &constBuffers, nullptr);
		ps->UpdateResources(context, nullptr, &textures);
		context->DrawIndexed(_meshFilter->GetIndexCount(), 0, 0);
	}
}