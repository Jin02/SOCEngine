#include "ReflectionProbe.h"
#include "MathCommon.h"
#include "Transform.h"
#include "CameraForm.h"
#include "Object.h"
#include "BindIndexInfo.h"
#include "Scene.h"

using namespace Core;
using namespace Math;
using namespace Intersection;
using namespace Rendering;
using namespace Rendering::Camera;
using namespace Rendering::Texture;
using namespace Rendering::Buffer;
using namespace Rendering::Manager;
using namespace Rendering::Shadow;

#define REFLECTION_PROBE_SIZE 512.0f

ReflectionProbe::ReflectionProbe()
	: Component(), _cubeMap(nullptr), _rpInfoCB(nullptr), _opaqueDepthBuffer(nullptr),
	_useTransparent(true), _projNear(0.01f), _range(10.0f), _type(Type::RealTime)
{
}

ReflectionProbe::~ReflectionProbe()
{
	SAFE_DELETE(_cubeMap);
	SAFE_DELETE(_rpInfoCB);
	SAFE_DELETE(_opaqueDepthBuffer);
}

void ReflectionProbe::OnInitialize()
{
	const Size<uint> size = Size<uint>((uint)REFLECTION_PROBE_SIZE, (uint)REFLECTION_PROBE_SIZE);

	_cubeMap = new TextureCube;
	_cubeMap->Initialize(size, DXGI_FORMAT_R8G8B8A8_UNORM, true, true);

	_rpInfoCB = new ConstBuffer;
	_rpInfoCB->Initialize(sizeof(RPInfo));

	_opaqueDepthBuffer = new DepthBufferCube;
	_opaqueDepthBuffer->Initialize(size, false);
}

void ReflectionProbe::OnDestroy()
{
	_cubeMap->Destroy();
	_rpInfoCB->Destory();
	_opaqueDepthBuffer->Destroy();
}

void ReflectionProbe::UpdateReflectionProbeCB(const Device::DirectX*& dx, uint packedNumOfLights)
{
	Vector3 forwards[6] = 
	{
		Vector3( 0.0f,  0.0f,  1.0f),
		Vector3( 0.0f,  0.0f, -1.0f),
		Vector3( 1.0f,  0.0f,  0.0f),
		Vector3(-1.0f,  0.0f,  0.0f),
		Vector3( 0.0f,  1.0f,  0.0f),
		Vector3( 0.0f, -1.0f,  0.0f)
	};
	Vector3 ups[6] = 
	{
		Vector3( 0.0f,  1.0f,  0.0f),
		Vector3( 0.0f,  1.0f,  0.0f),
		Vector3( 0.0f,  1.0f,  0.0f),
		Vector3( 0.0f,  1.0f,  0.0f),
		Vector3( 0.0f,  0.0f, -1.0f),
		Vector3( 0.0f,  0.0f,  1.0f),
	};

	auto ComputeViewProj = [](Matrix& outViewProj,
		const Vector3& eyePos, const Vector3& forward, const Vector3& up, const Matrix& projMat)
	{
		Matrix view;
		{
			Transform tf0(nullptr);
			tf0.UpdatePosition(eyePos);
			tf0.LookAtWorld(eyePos + forward, &up);

			tf0.FetchWorldMatrix(view);
			CameraForm::GetViewMatrix(view, view);
		}

		outViewProj				= view * projMat;
		Matrix::Transpose(outViewProj, outViewProj);
	};

	float projNear	= _range;
	float projFar	= _projNear;

	Matrix proj;
	Matrix::PerspectiveFovLH(proj, 1.0f, Common::Deg2Rad(90.0f), projNear, projFar);

	_owner->GetTransform()->FetchWorldPosition(_worldPos);

	RPInfo info;
	info.camWorldPos		= _worldPos;
	info.packedNumOfLights	= packedNumOfLights;

	Matrix& frontZViewProj = info.viewProjs[0];
	ComputeViewProj(frontZViewProj, _worldPos, forwards[0], ups[0], proj);

	bool isSameMat = memcmp(&_prevFrontZViewProjMat, &frontZViewProj, sizeof(Matrix)) == 0;
	if( isSameMat && (_prevPackedNumOfLights == packedNumOfLights) )
		return;

	for(uint i=1; i<6; ++i)
		ComputeViewProj(info.viewProjs[i], _worldPos, forwards[i], ups[i], proj);

	_rpInfoCB->UpdateSubResource(dx->GetContext(), &info);

	_prevFrontZViewProjMat = frontZViewProj;
	_prevPackedNumOfLights = packedNumOfLights;
}

void ReflectionProbe::Render(const Device::DirectX*& dx, const Core::Scene* scene)
{
	const RenderManager* renderManager	= scene->GetRenderManager();
	ID3D11DeviceContext* context = dx->GetContext();

	if(_useTransparent)
		CameraForm::SortTransparentMeshRenderQueue(_transparentMeshQueue, _owner->GetTransform(), renderManager);

	// Clear
	{
		_cubeMap->Clear(dx);
		_opaqueDepthBuffer->Clear(context, 0.0f, 0);
	}

	ID3D11Buffer* buffer = _rpInfoCB->GetBuffer();
	context->GSSetConstantBuffers(uint(ConstBufferBindIndex::ReflectionProbe_Info), 1, &buffer);
	context->PSSetConstantBuffers(uint(ConstBufferBindIndex::ReflectionProbe_Info), 1, &buffer);

	BoundBox boundBox(_worldPos, Vector3(_range, _range, _range));
	auto Intersect = [&](const Intersection::Sphere& sphere) -> bool
	{
		float squaredDistance = 0.0f;
		{
			auto Check = [](float p, float bMin, float bMax) -> float
			{
				float out	= 0.0f;
				float v		= p;

				if(v < bMin)
				{
					float val = bMin - v;
					out += val * val;
				}
				if(v > bMax)
				{
					float val = v - bMax;
					out += val * val;
				}

				return out;
			};

			const Vector3& bMin = boundBox.GetMin();
			const Vector3& bMax = boundBox.GetMax();

			squaredDistance += Check(sphere.center.x, bMin.x, bMax.x);
			squaredDistance += Check(sphere.center.y, bMin.y, bMax.y);
			squaredDistance += Check(sphere.center.z, bMin.z, bMax.z);
		}

		return squaredDistance <= (sphere.radius * sphere.radius);
	};
	std::function<bool(const Intersection::Sphere&)> intersectFunc = Intersect;

	D3D11_VIEWPORT viewport;
	{
		viewport.TopLeftX	= 0.0f;
		viewport.TopLeftY	= 0.0f;
		viewport.MinDepth	= 0.0f;
		viewport.MaxDepth	= 1.0f;
		viewport.Width		= REFLECTION_PROBE_SIZE;
		viewport.Height		= REFLECTION_PROBE_SIZE;
	}
	context->RSSetViewports(1, &viewport);

	// early z culling
	{
		ID3D11RenderTargetView* rtv = nullptr;
		context->OMSetRenderTargets(1, &rtv, _opaqueDepthBuffer->GetDepthStencilView());

		// Opaque
		{
			const auto& meshes = renderManager->GetOpaqueMeshes();
			uint count = meshes.meshes.GetVector().size();

			context->RSSetState( nullptr );

			if(count > 0)
				MeshCamera::RenderMeshesUsingSortedMeshVectorByVB(dx, renderManager, meshes, RenderType::ReflectionProbe_OnlyDepth, nullptr, &intersectFunc);
		}

		rtv = _cubeMap->GetRenderTargetView();
		context->OMSetRenderTargets(1, &rtv, _opaqueDepthBuffer->GetDepthStencilView());

		// AlphaTest
		{
			const auto& meshes = renderManager->GetAlphaTestMeshes();
			uint count = meshes.meshes.GetVector().size();

			if(count > 0)
			{
				bool useMSAA = dx->GetMSAADesc().Count > 1;

				float blendFactor[4] = {0.0f, 0.0f, 0.0f, 0.0f};

				if(useMSAA) //on alpha blending
					context->OMSetBlendState(dx->GetBlendStateAlphaToCoverage(), blendFactor, 0xffffffff);

				context->RSSetState( dx->GetRasterizerStateCWDisableCulling() );
		
				MeshCamera::RenderMeshesUsingSortedMeshVectorByVB(dx, renderManager, meshes, RenderType::ReflectionProbe_AlphaTestWithDiffuse, nullptr, &intersectFunc);

				context->RSSetState( nullptr );

				if(useMSAA) //off alpha blending
					context->OMSetBlendState(dx->GetBlendStateOpaque(), blendFactor, 0xffffffff);
			}
		}
	}

	// Render Scene
	{
		const LightManager* lightManager	= scene->GetLightManager();
		const ShadowRenderer* shadowManager	= scene->GetShadowManager();

		lightManager->BindResources(dx, false, false, true);
		shadowManager->BindResources(dx, false, false, true);

		ID3D11RenderTargetView* rtv = _cubeMap->GetRenderTargetView();
		context->OMSetRenderTargets(1, &rtv, _opaqueDepthBuffer->GetDepthStencilView());
	
		// off alpha blending
		float blendFactor[4] = {0.0f, 0.0f, 0.0f, 0.0f};

		// Opaque
		{
			context->OMSetBlendState(dx->GetBlendStateOpaque(), blendFactor, 0xffffffff);

			ID3D11SamplerState* samplerState = dx->GetSamplerStateAnisotropic();
			context->PSSetSamplers((uint)SamplerStateBindIndex::DefaultSamplerState, 1, &samplerState);
			context->OMSetDepthStencilState(dx->GetDepthStateEqualAndDisableDepthWrite(), 0);
			context->RSSetState(dx->GetRasterizerStateCWDefaultState());

			const auto& opaqueMeshes = renderManager->GetOpaqueMeshes();
			if(opaqueMeshes.meshes.GetSize() > 0)
				MeshCamera::RenderMeshesUsingSortedMeshVectorByVB(dx, renderManager, opaqueMeshes, RenderType::ReflectionProbe_OnlyFrontFace, nullptr, &intersectFunc);
		}

		// AlphaTest
		{
			const auto& alphaTestMeshes = renderManager->GetAlphaTestMeshes();
			if(alphaTestMeshes.meshes.GetSize() > 0)
			{
				bool useMSAA = dx->GetMSAADesc().Count > 1;

				if(useMSAA) //on alpha blending
					context->OMSetBlendState(dx->GetBlendStateAlphaToCoverage(), blendFactor, 0xffffffff);

				context->RSSetState( dx->GetRasterizerStateCWDisableCulling() );
				MeshCamera::RenderMeshesUsingSortedMeshVectorByVB(dx, renderManager, alphaTestMeshes, RenderType::ReflectionProbe_AlphaTestWithDiffuse, nullptr, &intersectFunc);

				context->RSSetState( nullptr );

				if(useMSAA) //off alpha blending
					context->OMSetBlendState(dx->GetBlendStateOpaque(), blendFactor, 0xffffffff);
			}
		}

		// Transparency
		{
			context->RSSetState(dx->GetRasterizerStateCWDisableCulling());
			context->OMSetBlendState(dx->GetBlendStateAlpha(), blendFactor, 0xffffffff);
			context->OMSetDepthStencilState(dx->GetDepthStateGreaterAndDisableDepthWrite(), 0);

			const std::vector<const Geometry::Mesh*>& meshes = _transparentMeshQueue.meshes;
			MeshCamera::RenderMeshesUsingMeshVector(dx, renderManager, meshes, RenderType::ReflectionProbe_Transparency, nullptr, &intersectFunc);

			context->RSSetState(nullptr);
			context->OMSetBlendState(dx->GetBlendStateOpaque(), blendFactor, 0xffffffff);
		}

		lightManager->UnbindResources(dx, false, false, true);
		shadowManager->UnbindResources(dx, false, false, true);
	}

	buffer = nullptr;
	context->GSSetConstantBuffers(uint(ConstBufferBindIndex::ReflectionProbe_Info), 1, &buffer);
	context->PSSetConstantBuffers(uint(ConstBufferBindIndex::ReflectionProbe_Info), 1, &buffer);
}