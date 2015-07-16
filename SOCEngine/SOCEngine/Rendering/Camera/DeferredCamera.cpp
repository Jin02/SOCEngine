#include "DeferredCamera.h"
#include "Director.h"
#include "EngineShaderFactory.hpp"
#include "ResourceManager.h"

using namespace Rendering::Camera;
using namespace Rendering::Texture;
using namespace Rendering::Shader;
using namespace Device;
using namespace Math;
using namespace Rendering::Factory;
using namespace Rendering::Light;
using namespace Rendering::Manager;

DeferredCamera::DeferredCamera() : CameraForm(),
	_transparentDepthBuffer(nullptr), _albedo_opacity(nullptr),
	_specular_fresnel0(nullptr), _normal_roughness(nullptr),
	_useTransparent(false), _opaqueDepthBuffer(nullptr)
{
	_renderType = RenderType::Deferred;
}

DeferredCamera::~DeferredCamera()
{
}

void DeferredCamera::OnInitialize()
{
	CameraForm::OnInitialize();

	Size<unsigned int> windowSize = Director::GetInstance()->GetWindowSize();

	_albedo_opacity = new Texture::RenderTexture;
	ASSERT_COND_MSG( _albedo_opacity->Initialize(windowSize, DXGI_FORMAT_R8G8B8A8_UNORM), "GBuffer Error : cant create albedo_opacity render texture" );

	_specular_fresnel0 = new Texture::RenderTexture;
	ASSERT_COND_MSG( _specular_fresnel0->Initialize(windowSize, DXGI_FORMAT_R8G8B8A8_UNORM), "GBuffer Error : cant create _specular_fresnel0 render texture" );

	_normal_roughness = new Texture::RenderTexture;
	ASSERT_COND_MSG( _normal_roughness->Initialize(windowSize, DXGI_FORMAT_R8G8B8A8_UNORM), "GBuffer Error : cant create _normal_roughness render texture" );

	EnableRenderBlendedMesh(true);

	//auto camMgr = Device::Director::GetInstance()->GetCurrentScene()->GetCameraManager();
	//const std::string key = _owner->GetName();
	//camMgr->Add(key, this);
}

void DeferredCamera::OnDestroy()
{
	SAFE_DELETE(_albedo_opacity);
	SAFE_DELETE(_specular_fresnel0);
	SAFE_DELETE(_normal_roughness);
	SAFE_DELETE(_transparentDepthBuffer);

	CameraForm::OnDestroy();
}

void DeferredCamera::Render(float dt, Device::DirectX* dx)
{
	ID3D11DeviceContext* context = dx->GetContext();

	//Clear
	{
		Color allZeroColor(0.f, 0.f, 0.f, 0.f);

		_albedo_opacity->Clear(context, allZeroColor);
		_specular_fresnel0->Clear(context, allZeroColor);
		_normal_roughness->Clear(context, allZeroColor);
	}

	_opaqueDepthBuffer->Clear(context, 0.0f, 0); //inverted depth

	if(_useTransparent)
	{
		_transparentDepthBuffer->Clear(context, 0.0f, 0);
		SortTransparentMeshRenderQueue();
	}

	RenderManager* renderMgr = Director::GetInstance()->GetCurrentScene()->GetRenderManager();

	// off alpha blending
	{
		float bf[4] = {0.f, 0.f, 0.f, 0.f};
		context->OMSetBlendState(dx->GetOpaqueBlendState(), bf, 0xffffffff);
	}

	struct MeshInForwardRendering
	{
		const Mesh::Mesh*	mesh;
		const Material*		material;
	};
	
	std::vector<MeshInForwardRendering> meshesInForwardRendering;
	auto AddMeshInForwardQueue = [&](const Material* material, const Mesh::Mesh* mesh)
	{
		if( material->GetCustomShader().IsAllEmpty() == false )
		{
			MeshInForwardRendering mfr;
			{
				mfr.material = material;
				mfr.mesh = mesh;
			}

			meshesInForwardRendering.push_back(mfr);
			return true;
		}

		return false;
	};

	//GBuffer
	{
		ID3D11RenderTargetView* renderTargetViews[] = {
			_albedo_opacity->GetRenderTargetView(),
			_normal_roughness->GetRenderTargetView(),
			_specular_fresnel0->GetRenderTargetView(),
		};

		context->OMSetRenderTargets(NumOfRenderTargets, renderTargetViews, _opaqueDepthBuffer->GetDepthStencilView());
		context->OMSetDepthStencilState(dx->GetDepthGreaterState(), 0);

		ID3D11SamplerState* samplerState = dx->GetAnisotropicSamplerState();
		context->PSSetSamplers(0, 1, &samplerState);

		const auto& renderMgr = Director::GetInstance()->GetCurrentScene()->GetRenderManager();

		//Opaque Mesh
		{
			//const std::vector<const Mesh::Mesh*>& meshes = renderMgr->GetOpaqueMeshes().meshes.GetVector();
			//for(auto meshIter = meshes.begin(); meshIter != meshes.end(); ++meshIter)
			//{
			//	const Mesh::Mesh* mesh = (*meshIter);
			//	Mesh::MeshFilter* filter = mesh->GetMeshFilter();
			//	filter->IASetBuffer(dx);

			//	const ShaderGroup* shaders = renderMgr->FindGBufferShader(filter->GetBufferElementFlag());
			//	ASSERT_COND_MSG(shaders->ableRender(), "Error, cant render mesh");

			//		Mesh::MeshRenderer* renderer = mesh->GetMeshRenderer();
			//		const auto& materials = renderer->GetMaterials();
			//		for(auto iter = materials.begin(); iter != materials.end(); ++iter)
			//		{
			//			Material* material = GET_CONTENT_FROM_ITERATOR(iter);	
			//			if(AddMeshInForwardQueue(material, mesh))
			//				continue;

			//			//shaders->vs->UpdateShader(context);
			//			//shaders->vs->UpdateInputLayout(context);
			//			//shaders->vs->UpdateResources(context, cb, nullptr);

			//			//shaders->ps->UpdateShader(context);
			//			//shaders->ps->UpdateResources(context, nullptr, tex);
			//		}
			//}
		}

		//Alpha Test Mesh
		{
			const std::vector<const Mesh::Mesh*>& opaqueMeshes = renderMgr->GetAlphaTestMeshes().meshes.GetVector();
			for(auto meshIter = opaqueMeshes.begin(); meshIter != opaqueMeshes.end(); ++meshIter)
			{

			}
		}

		//Transparent Mesh
		if(_useTransparent)
		{

		}
	}
}

void DeferredCamera::EnableRenderBlendedMesh(bool enable)
{
	const Size<unsigned int> windowSize = Director::GetInstance()->GetWindowSize();
	
	SAFE_DELETE(_transparentDepthBuffer);
	if(enable)
	{
		_transparentDepthBuffer =  new DepthBuffer;
		_transparentDepthBuffer->Initialize(windowSize);
	}

	//SAFE_DELETE(_lightCulling);
	//_lightCulling = new LightCulling;

	//EngineFactory shaderFactory(nullptr); //only use FetchShaderFullPath

	//std::string path = "";
	//shaderFactory.FetchShaderFullPath(path, "TileBasedDeferredShading");

	//ASSERT_COND_MSG(path.empty() == false, "Error, path is null");
	//_lightCulling->InitializeOnlyLightCulling(path, "CS", enable);
}

Core::Component* DeferredCamera::Clone() const
{
	return nullptr;
}