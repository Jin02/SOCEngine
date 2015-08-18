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
	_blendedDepthBuffer(nullptr), _albedo_metallic(nullptr),
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

	_albedo_metallic = new Texture::RenderTexture;
	ASSERT_COND_MSG( _albedo_metallic->Initialize(windowSize, DXGI_FORMAT_R8G8B8A8_UNORM), "GBuffer Error : cant create albedo_opacity render texture" );

	_specular_fresnel0 = new Texture::RenderTexture;
	ASSERT_COND_MSG( _specular_fresnel0->Initialize(windowSize, DXGI_FORMAT_R8G8B8A8_UNORM), "GBuffer Error : cant create _specular_fresnel0 render texture" );

	_normal_roughness = new Texture::RenderTexture;
	ASSERT_COND_MSG( _normal_roughness->Initialize(windowSize, DXGI_FORMAT_R8G8B8A8_UNORM), "GBuffer Error : cant create _normal_roughness render texture" );

	_opaqueDepthBuffer = new Texture::DepthBuffer;
	_opaqueDepthBuffer->Initialize(windowSize);

	_deferredShadingWithLightCulling = new DeferredShadingWithLightCulling;
	_deferredShadingWithLightCulling->Initialize(_opaqueDepthBuffer, _albedo_metallic, _specular_fresnel0, _normal_roughness, windowSize);

	EnableRenderTransparentMesh(true);

	auto camMgr = Device::Director::GetInstance()->GetCurrentScene()->GetCameraManager();
	CameraForm* thisCam = this;
	camMgr->Add(_owner->GetName(), thisCam);
}

void DeferredCamera::OnDestroy()
{
	SAFE_DELETE(_albedo_metallic);
	SAFE_DELETE(_specular_fresnel0);
	SAFE_DELETE(_normal_roughness);
	SAFE_DELETE(_blendedDepthBuffer);

	CameraForm::OnDestroy();
}

void DeferredCamera::Render(float dt, Device::DirectX* dx)
{
	ID3D11DeviceContext* context = dx->GetContext();

	//Clear
	{
		Color allZeroColor(0.f, 0.f, 0.f, 0.f);

		_albedo_metallic->Clear(context, allZeroColor);
		_specular_fresnel0->Clear(context, allZeroColor);
		_normal_roughness->Clear(context, allZeroColor);
	}

	_opaqueDepthBuffer->Clear(context, 0.0f, 0); //inverted depth

	if(_useTransparent)
	{
		_blendedDepthBuffer->Clear(context, 0.0f, 0);
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
	
	// TBFR = Tile Based Forward Rendering.
	// Used to render transparent meshes
	std::vector<MeshInForwardRendering> tbfrQueue;
	auto AddMeshToTBFRQueue = [&](const Material* material, const Mesh::Mesh* mesh)
	{
		if( material->GetCustomShader().IsAllEmpty() == false )
		{
			MeshInForwardRendering mfr;
			{
				mfr.material = material;
				mfr.mesh = mesh;
			}

			tbfrQueue.push_back(mfr);
			return true;
		}

		return false;
	};

	//GBuffer
	{
		ID3D11RenderTargetView* renderTargetViews[] = {
			_albedo_metallic->GetRenderTargetView(),
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

void DeferredCamera::EnableRenderTransparentMesh(bool enable)
{	
	if(enable)
	{
		const Size<unsigned int> windowSize = Director::GetInstance()->GetWindowSize();

		ASSERT_COND_MSG(_blendedDepthBuffer, "Error, Already allocated depth");
		{
			_blendedDepthBuffer =  new DepthBuffer;
			_blendedDepthBuffer->Initialize(windowSize);
		}


		ASSERT_COND_MSG(_blendedMeshLightCulling, "Error, Already allocated depth");
		{
			_blendedMeshLightCulling = new OnlyLightCulling;
			{
				EngineFactory shaderFactory(nullptr); //only use FetchShaderFullPath

				std::string path = "";
				shaderFactory.FetchShaderFullPath(path, "TileBasedDeferredShading");

				ASSERT_COND_MSG(path.empty() == false, "Error, path is null");
				_blendedMeshLightCulling->Initialize(path, "CS", true, _opaqueDepthBuffer, _blendedDepthBuffer);
			}
		}
	}
	else // enable == false
	{
		SAFE_DELETE(_blendedDepthBuffer);
		SAFE_DELETE(_blendedMeshLightCulling);
	}
}

Core::Component* DeferredCamera::Clone() const
{
	return nullptr;
}