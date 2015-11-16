#include "Voxelization.h"
#include "Object.h"

using namespace Core;
using namespace Math;
using namespace Rendering::Buffer;
using namespace Rendering::GI;
using namespace Rendering::Texture;
using namespace Rendering::Camera;

Voxelization::Voxelization()
	: _viewProjAxisesConstBuffer(nullptr), _infoConstBuffer(nullptr),
	_numOfCascades(0), _changedInitVoxelizationInfo(false)
{
}

Voxelization::~Voxelization()
{
	Destroy();

	for(auto& iter : _voxelColorMaps)	SAFE_DELETE(iter);
	_voxelColorMaps.clear();

	for(auto& iter : _voxelNormalMaps)	SAFE_DELETE(iter);
	_voxelNormalMaps.clear();

	SAFE_DELETE(_infoConstBuffer);
	SAFE_DELETE(_viewProjAxisesConstBuffer);
}

void Voxelization::Initialize(uint cascades, float minWorldSize, uint dimension)
{
	ASSERT_COND_MSG(cascades != 0, "Error, voxelization cascade num is zero.");
	_numOfCascades = cascades;

	Size<uint> mathSize = Size<uint>(dimension, dimension);

	for(uint i=0; i<cascades; ++i)
	{
		VoxelMap* voxelColorMap = new VoxelMap;
		voxelColorMap->Initialize(mathSize, dimension, DXGI_FORMAT_R8G8B8A8_UNORM);

		VoxelMap* voxelNormalMap = new VoxelMap;
		voxelNormalMap->Initialize(mathSize, dimension, DXGI_FORMAT_R8G8B8A8_UNORM);

		_voxelColorMaps.push_back(voxelColorMap);
		_voxelNormalMaps.push_back(voxelNormalMap);
	}

	_infoConstBuffer = new ConstBuffer;
	_infoConstBuffer->Initialize(sizeof(Info));

	_viewProjAxisesConstBuffer = new ConstBuffer;
	_viewProjAxisesConstBuffer->Initialize(sizeof(ViewProjAxisesCBData));

	Info info;
	{
		info.dimension = dimension;
		info.voxelizeSize = minWorldSize;
	}
	UpdateInitVoxelizationInfo(info);
}

void Voxelization::Destroy()
{
	for(auto& iter : _voxelColorMaps)	iter->Destroy();
	for(auto& iter : _voxelNormalMaps)	iter->Destroy();

	_infoConstBuffer->Destory();
	_viewProjAxisesConstBuffer->Destory();
}

void Voxelization::Clear(Device::DirectX* dx)
{
}

void Voxelization::Voxelize(Device::DirectX* dx, const MeshCamera* camera)
{
	Math::Matrix camWorldMat;
	{
		const Transform* camTf = camera->GetOwner()->GetTransform();
		camTf->FetchWorldMatrix(camWorldMat);
	}

	//Check Duplicated Work.
	{
		Matrix viewMat;
		CameraForm::GetViewMatrix(viewMat, camWorldMat);
		
		bool isDifferentViewMat = memcmp(&_prevViewMat, &viewMat, sizeof(Matrix)) != 0;
		if((isDifferentViewMat || _changedInitVoxelizationInfo) == false)
			return;

		_prevViewMat = viewMat;
		_changedInitVoxelizationInfo = false;
	}

	Vector3 camWorldPos(camWorldMat._41, camWorldMat._42, camWorldMat._43);

	float cameraNear = camera->GetNear();
	float cameraFar  = camera->GetFar();
	ID3D11DeviceContext* context = dx->GetContext();

	for(uint currentCascade=0; currentCascade<_numOfCascades; ++currentCascade)
	{
		// Compute & Update Const Buffers
		{
			InfoCBData currentVoxelizeInfo;
			{
				currentVoxelizeInfo.currentCascade	= currentCascade;
				currentVoxelizeInfo.dimension		= _initVoxelizationInfo.dimension;

				uint uscale = currentCascade + 1;
				float scale = (float)(uscale * uscale);
				currentVoxelizeInfo.voxelizeSize	= _initVoxelizationInfo.voxelizeSize * scale;
				currentVoxelizeInfo.voxelSize		= currentVoxelizeInfo.voxelizeSize / (float)currentVoxelizeInfo.dimension;
			}
			_infoConstBuffer->UpdateSubResource(context, &currentVoxelizeInfo);

			float worldSize = currentVoxelizeInfo.voxelizeSize;

			Matrix orthoProjMat;
			Matrix::OrthoLH(orthoProjMat, worldSize, worldSize, cameraNear, cameraFar);

			float halfWorldSize = worldSize / 2.0f;
			float cascadeScale = (float)currentCascade + 1.0f;

			float offset = (worldSize / (float)(currentCascade + 1)) / 2.0f;
			Vector3 bbMin = (camWorldPos - Vector3(offset, offset, offset)) * Vector3(cascadeScale, cascadeScale, cascadeScale);
			Vector3 bbMax = bbMin + Vector3(worldSize, worldSize, worldSize);
			Vector3 bbMid = (bbMin + bbMax) / 2.0f;

			auto LookAtView = [](
				Matrix& outViewMat,
				const Vector3& worldPos, const Vector3& targetPos, const Vector3& up
				)
			{
				Transform tf(nullptr);
				tf.UpdatePosition(worldPos);
				tf.LookAtWorld(targetPos, &up);

				Matrix worldMat;
				tf.FetchWorldMatrix(worldMat);

				CameraForm::GetViewMatrix(outViewMat, worldMat);
			};

			Matrix viewAxisX, viewAxisY, viewAxisZ;
			LookAtView(viewAxisZ, Vector3(bbMid.x, bbMid.y, bbMin.z), Vector3(bbMid.x, bbMid.y, bbMax.z), Vector3(0.0f, 1.0f, 0.0f)); //z
			LookAtView(viewAxisX, Vector3(bbMin.x, bbMid.y, bbMid.z), Vector3(bbMax.x, bbMid.y, bbMid.z), Vector3(0.0f, 1.0f, 0.0f)); //x
			LookAtView(viewAxisY, Vector3(bbMid.x, bbMin.y, bbMid.z), Vector3(bbMid.x, bbMax.y, bbMid.z), Vector3(0.0f, 0.0f,-1.0f)); //y

			ViewProjAxisesCBData viewProjAxises;
			viewProjAxises.viewProjX = viewAxisX * orthoProjMat;
			viewProjAxises.viewProjY = viewAxisY * orthoProjMat;
			viewProjAxises.viewProjZ = viewAxisZ * orthoProjMat;

			_viewProjAxisesConstBuffer->UpdateSubResource(context, &viewProjAxises);
		}

		// Render Voxel
		{
		}
	}
}

void Voxelization::UpdateInitVoxelizationInfo(const Info& info)
{
	InfoCBData& initInfo = _initVoxelizationInfo;

	initInfo.currentCascade	= 0;
	initInfo.voxelizeSize	= info.voxelizeSize;
	initInfo.dimension		= info.dimension;
	initInfo.voxelSize		= info.voxelizeSize / (float)info.dimension;

	_changedInitVoxelizationInfo = true;
}