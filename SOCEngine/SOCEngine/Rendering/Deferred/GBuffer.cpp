#include "GBuffer.h"
#include "Director.h"
#include <array>

using namespace Rendering;
using namespace Rendering::Manager;
using namespace Rendering::Texture;
using namespace Rendering::Deferred;
using namespace Device;
using namespace Math;

GBuffer::GBuffer()
	: _albedo_opacity(nullptr), _specular_fresnel0(nullptr),
	_normal_roughness(nullptr), _linearDepth(nullptr)
{
}

GBuffer::~GBuffer()
{
	Destroy();
}

void GBuffer::Init()
{
	Size<unsigned int> windowSize = Director::GetInstance()->GetWindowSize();

	_albedo_opacity = new Texture::RenderTexture;
	ASSERT_COND_MSG( _albedo_opacity->Create(windowSize), "GBuffer Error : cant create albedo_opacity render texture" );

	_specular_fresnel0 = new Texture::RenderTexture;
	ASSERT_COND_MSG( _specular_fresnel0->Create(windowSize), "GBuffer Error : cant create _specular_fresnel0 render texture" );

	_normal_roughness = new Texture::RenderTexture;
	ASSERT_COND_MSG( _normal_roughness->Create(windowSize), "GBuffer Error : cant create _normal_roughness render texture" );

	_linearDepth = new Texture::RenderTexture;
	ASSERT_COND_MSG( _linearDepth->Create(windowSize), "GBuffer Error : cant create _linearDepth render texture" );
}

void GBuffer::Destroy()
{
	SAFE_DELETE(_albedo_opacity);
	SAFE_DELETE(_specular_fresnel0);
	SAFE_DELETE(_normal_roughness);
	SAFE_DELETE(_linearDepth);
}

void GBuffer::Clear(const Device::DirectX* dx)
{
	// clear device depth

}

void GBuffer::Render(const Device::DirectX* dx, const Buffer::ConstBuffer* _cameraConstBuffer, const Rendering::Manager::RenderManager* renderMgr)
{
	auto MeshRender = [&](ID3D11DeviceContext* context, RenderManager::MeshType type)
	{
		//if(type == RenderManager::MeshType::nonAlpha)
		//	context->RSSetState(dx->GetDefaultCullingRasterizerState());
		//else if(type == RenderManager::MeshType::hasAlpha)
		//	context->RSSetState(dx->GetDisableCullingRasterizerState());
		//else
		//	ASSERT_MSG("GBuffer Error : cant support mesh type");

		//Material* currentUseMaterial = nullptr;
		//auto RenderIter = [&](Material* material, Mesh::Mesh* mesh)
		//{
		//	if(currentUseMaterial != material)
		//	{
		//		currentUseMaterial = material;	
		//		currentUseMaterial->UpdateShader(context);
		//	}
		//	
		//	mesh->Render(currentUseMaterial, _cameraConstBuffer);
		//};

		//renderMgr->Iterate(RenderIter, type);
	};


	// clear device depth
	DepthBuffer* deviceDepthBuffer = dx->GetDepthBuffer();
	deviceDepthBuffer->Clear(0.0f, 0);

	{

	}

	ID3D11DeviceContext* context = dx->GetContext();
	context->OMSetDepthStencilState(dx->GetDepthLessEqualState(), 0);

	const uint rtCount = 4; //albedo, specular, normal, depth
	std::array<ID3D11RenderTargetView*, rtCount> renderTargets;
	{
		renderTargets[0] = _albedo_opacity->GetRenderTargetView();
		renderTargets[1] = _specular_fresnel0->GetRenderTargetView();
		renderTargets[2] = _normal_roughness->GetRenderTargetView();
		renderTargets[3] = _linearDepth->GetRenderTargetView();
	}

	context->OMSetRenderTargets(4, &renderTargets.front(), deviceDepthBuffer->GetDepthStencilView());
	context->OMSetBlendState(dx->GetOpaqueBlendState(), 0, 0xFFFFFFFF);

	MeshRender(context, Manager::RenderManager::MeshType::nonAlpha);
	MeshRender(context, Manager::RenderManager::MeshType::hasAlpha);

	context->OMSetRenderTargets(0, nullptr, nullptr);
}