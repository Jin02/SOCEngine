#include "SimpleText2D.h"
#include "Director.h"
#include "EngineShaderFactory.hpp"
#include "SimpleFontLoader.h"

#include "ResourceManager.h"

using namespace UI;
using namespace Rendering;
using namespace Device;
using namespace Resource;

SimpleText2D::SimpleText2D(const std::string& name, Core::Object* parent)
	: UIObject(name, parent), _meshFilter(nullptr), _material(nullptr), _isOtherMaterial(false)
{
}

SimpleText2D::~SimpleText2D()
{
	Destroy();
}

void SimpleText2D::Destroy()
{
	UIObject::Destroy();

	if(_isOtherMaterial == false)
		SAFE_DELETE(_material);

	SAFE_DELETE(_meshFilter);
}

void SimpleText2D::Initialize(uint maxLength, const std::string& sharedVerticesKey, Rendering::Material* material)
{
	const Device::Director* director	= Device::Director::GetInstance();
	const Device::DirectX* dx			= director->GetDirectX();

	_screenSize = director->GetBackBufferSize();
	_maxLength = maxLength;

	SimpleFontLoader* fontLoader = ResourceManager::GetInstance()->GetSimpleFontLoader();
	fontLoader->Initialize(TEMP_FONT_DATA_PATH, TEMP_FONT_TEXTURE_PATH);

	if(material == nullptr)
	{
		_material = new Material(_name, Material::Type::UI);
		_isOtherMaterial = true;

		const ResourceManager* resourceMgr = ResourceManager::GetInstance();

		auto shaderMgr = ResourceManager::GetInstance()->GetShaderManager();
		Factory::EngineFactory factory(shaderMgr);

		Shader::VertexShader*	vs = nullptr;
		Shader::PixelShader*	ps = nullptr;
		factory.LoadShader("SimpleUIImage2D", "VS", "PS", "", nullptr, &vs, &ps, nullptr);

		Material::CustomShader customShader;
		{
			customShader.isDeferred = false;
			customShader.shaderGroup = Shader::ShaderGroup(vs, ps, nullptr, nullptr);
		}
		_material->SetCustomShader(customShader);

		const Texture::Texture2D* texture = fontLoader->GetTexture();
		_material->SetVariable<const Texture::Texture2D*>("font", texture);
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

	Geometry::MeshFilter::CreateFuncArguments meshCreateArgs("UI:Font", sharedVerticesKey);
	{
		meshCreateArgs.vertices.data		= emptyVertices.data();
		meshCreateArgs.vertices.count		= emptyVertices.size();
		meshCreateArgs.vertices.byteWidth	= sizeof(RectVertexInfo);

		meshCreateArgs.indices				= &indices;
		meshCreateArgs.useDynamicVB			= true;
	}

	_meshFilter = new Geometry::MeshFilter;
	bool success = _meshFilter->Initialize(meshCreateArgs);
	ASSERT_COND_MSG(success, "Error, cant create SimpleImage2D meshfilter");

	Manager::UIManager* uiMgr = Director::GetInstance()->GetCurrentScene()->GetUIManager();
	uiMgr->AddRenderQueue(_name, this);

	if(_root == this)
		uiMgr->AddUpdateQueue(this);

	UIObject::Initialize();
}

void SimpleText2D::UpdateText(const std::string& text)
{
	ASSERT_COND_MSG(text.length() < _maxLength, "Error, text's length is longer than maximum length");
	if(text.empty())
		return;

	const SimpleFontLoader* fontLoader = ResourceManager::GetInstance()->GetSimpleFontLoader();
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

			offsetX += (fontTypes[letter].size + 1.5f);
		}
	}

	//�������� ����
	{
		for(auto& iter : vertices)
			iter.position.x -= (offsetX / 2.0f);
	}

	const Device::Director* director	= Device::Director::GetInstance();
	const Device::DirectX* dx			= director->GetDirectX();
	_meshFilter->GetVertexBuffer()->UpdateVertexData(dx->GetContext(), vertices.data(), sizeof(RectVertexInfo) * vertices.size());
}

void SimpleText2D::Render(const Device::DirectX* dx, const Math::Matrix& viewProjMat)
{
	ID3D11DeviceContext* context = dx->GetContext();

	Shader::VertexShader* vs = _material->GetCustomShader().shaderGroup.vs;
	Shader::PixelShader* ps = _material->GetCustomShader().shaderGroup.ps;

	if(_material->GetCustomShader().shaderGroup.ableRender())
	{		
		_meshFilter->GetVertexBuffer()->IASetBuffer(context);
		_meshFilter->GetIndexBuffer()->IASetBuffer(context);

		vs->BindShaderToContext(context);
		vs->BindInputLayoutToContext(context);

		ps->BindShaderToContext(context);

		std::vector<Shader::ShaderForm::InputConstBuffer> constBuffers;
		{
			auto buffer = _transformCB;
			constBuffers.push_back(Shader::ShaderForm::InputConstBuffer(0, buffer, true, false, false, false));
		}
		std::vector<Shader::ShaderForm::InputTexture> textures;
		{
			const Texture::Texture2D* tex = nullptr;
			_material->GetVariable<const Texture::Texture2D*>(tex, "font");
			textures.push_back(Shader::ShaderForm::InputTexture(0, tex, false, false, false, true));
		}

		vs->BindResourcesToContext(context, &constBuffers, nullptr, nullptr);
		ps->BindResourcesToContext(context, nullptr, &textures, nullptr);
		context->DrawIndexed(_meshFilter->GetIndexCount(), 0, 0);
	}
}