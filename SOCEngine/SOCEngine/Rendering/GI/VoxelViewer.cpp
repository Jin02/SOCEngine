#include "VoxelViewer.h"
#include "ResourceManager.h"
#include "PhysicallyBasedMaterial.h"
#include "ShaderManager.h"
#include "EngineShaderFactory.hpp"
#include "Scene.h"

using namespace GPGPU::DirectCompute;
using namespace Rendering;
using namespace Rendering::Manager;
using namespace Rendering::Shader;
using namespace Rendering::Buffer;
using namespace Rendering::Geometry;
using namespace Rendering::Factory;
using namespace Rendering::GI;
using namespace Rendering::GI::Debug;
using namespace Core;
using namespace Device;
using namespace Resource;
using namespace Importer;
using namespace Math;

VoxelViewer::VoxelViewer() //: _infoCB(nullptr), _shader(nullptr)
	: _readBuffer(nullptr), _dimension(0), _voxelsParent(nullptr)
{
}

VoxelViewer::~VoxelViewer()
{
//	SAFE_DELETE(_infoCB);
//	SAFE_DELETE(_shader);
	SAFE_DELETE(_readBuffer);

//	DestroyAllVoxelMeshes();
	SAFE_DELETE(_voxelsParent);
}

void VoxelViewer::Initialize(uint dimension)
{
	//_infoCB = new ConstBuffer;
	//_infoCB->Initialize(sizeof(InfoCB));

	//const ResourceManager* resourceMgr = ResourceManager::SharedInstance();
	//ShaderManager* shaderMgr = resourceMgr->GetShaderManager();

	//std::string filePath = "";
	//EngineFactory pathFinder(nullptr);
	//pathFinder.FetchShaderFullPath(filePath, "VoxelTextureToBuffer");

	//ASSERT_COND_MSG(filePath.empty() == false, "Error, File path is empty!");

	//ID3DBlob* blob = shaderMgr->CreateBlob(filePath, "cs", "CS", false, nullptr);
	//_shader = new ComputeShader(ComputeShader::ThreadGroup(0, 0, 0), blob);
	//ASSERT_COND_MSG(_shader->Initialize(), "can not create compute shader");

	//std::vector<ShaderForm::InputUnorderedAccessView> uavs;
	//uavs.push_back(ShaderForm::InputUnorderedAccessView(0, nullptr));
	//_shader->SetUAVs(uavs);

	_readBuffer = new CPUReadBuffer;
	_readBuffer->Initialize(4, (dimension * 6) * dimension * dimension);

	_dimension = dimension;
}

Object* VoxelViewer::GenerateVoxelViewer(const Device::DirectX* dx, const AnisotropicVoxelMapAtlas* voxelMapAtlas, uint cascade, Type type, bool realloc)
{
	if(realloc)
		DestroyAllVoxelMeshes();
	else
	{
		if(_voxelObjects.empty() == false)
			return _voxelsParent;
	}

	auto GetVoxelIdx = [&](uint x, uint y, uint z) -> uint
	{
		return (z * _dimension * _dimension) + (y * _dimension) + x;
	};

	std::vector<uint> voxelMap;
	{
		ID3D11DeviceContext* context = dx->GetContext();

		// Texture3D to Buffer
		{
			//InfoCB info;
			//info.curCascade	= cascade;
			//info.dimension	= dimension;
	
			//uint threadCount = (dimension + 8 - 1) / 8;
			//_shader->SetThreadGroupInfo(ComputeShader::ThreadGroup(threadCount, threadCount, threadCount));
	
			//_infoCB->UpdateSubResource(context, &info);
	
			//ID3D11UnorderedAccessView* uav = voxelMapAtlas->GetUnorderedAccessView()->GetView();
			//context->CSSetUnorderedAccessViews(1, 1, &uav, nullptr);
	
			//_shader->Dispatch(context);
	
			//uav = nullptr;
			//context->CSSetUnorderedAccessViews(1, 1, &uav, nullptr);
		}

		// Buffer to vector
		{
			auto Recive = [&](const void* data) -> void
			{
				uint maxCount = GetVoxelIdx(_dimension * 6, _dimension, _dimension);
				voxelMap.reserve( maxCount );

				uint* start	= (uint*)data + GetVoxelIdx(0, _dimension * cascade, 0);
				uint* end	= start + GetVoxelIdx(_dimension * 6, _dimension, _dimension);

				std::copy(start, end, voxelMap.begin());
			};
			_readBuffer->Read(context, voxelMapAtlas->GetTexture(), Recive);
		}
	}

	if( voxelMap.empty() ) return nullptr;

	_voxelsParent = new Object("VoxelViwer");

	const ResourceManager* resourceMgr = ResourceManager::SharedInstance();
	MeshImporter* importer = resourceMgr->GetMeshImporter();

	const float scale = 1.0f;

	auto GetColor = [](uint voxelData) -> Color
	{
		Color color;
		color.r = ((voxelData >> 24) & 0xff) * 255.0f;
		color.g = ((voxelData >> 16) & 0xff) * 255.0f;
		color.b = ((voxelData >> 8 ) & 0xff) * 255.0f;
		color.a = ((voxelData >> 0 ) & 0xff) * 255.0f;

		return color;
	};

	for(uint z=0; z<_dimension; ++z)
	{
		for(uint y=0; y<_dimension; ++y)
		{
			for(uint x=0; x<_dimension; ++x)
			{
				Color	leftColor(0.0f, 0.0f, 0.0f, 0.0f),	rightColor(0.0f, 0.0f, 0.0f, 0.0f),
						topColor(0.0f, 0.0f, 0.0f, 0.0f),	botColor(0.0f, 0.0f, 0.0f, 0.0f),
						frontColor(0.0f, 0.0f, 0.0f, 0.0f),	backColor(0.0f, 0.0f, 0.0f, 0.0f);

				if(type == Type::Color)
				{
					rightColor	= GetColor(GetVoxelIdx(x + (_dimension * 0), y + (_dimension * cascade), z));
					leftColor	= GetColor(GetVoxelIdx(x + (_dimension * 1), y + (_dimension * cascade), z));
					botColor	= GetColor(GetVoxelIdx(x + (_dimension * 2), y + (_dimension * cascade), z));
					topColor	= GetColor(GetVoxelIdx(x + (_dimension * 3), y + (_dimension * cascade), z));
					backColor	= GetColor(GetVoxelIdx(x + (_dimension * 4), y + (_dimension * cascade), z));
					frontColor	= GetColor(GetVoxelIdx(x + (_dimension * 5), y + (_dimension * cascade), z));
				}

				if((rightColor.Get32BitUintColor()	|	leftColor.Get32BitUintColor()	| 
					botColor.Get32BitUintColor()	|	topColor.Get32BitUintColor()	| 
					backColor.Get32BitUintColor()	|	frontColor.Get32BitUintColor()	) == 0) //��� ĭ�� ���� ������ ���� �Ž��� ���� �ʿ䰡 ����
					continue;

				Object* meshObject = importer->Load("./Resources/Voxel/voxel.obj", false);
				{
					meshObject->GetTransform()->UpdatePosition( Vector3(float(x), float(y), float(z)) * scale );
					meshObject->GetTransform()->UpdateScale(Vector3(scale, scale, scale));
					_voxelsParent->AddChild(meshObject);

					Object* container = meshObject->GetChild(0);

					PhysicallyBasedMaterial* left	= static_cast<PhysicallyBasedMaterial*>(container->GetChild(3)->GetComponent<Geometry::Mesh>()->GetMeshRenderer()->GetMaterials().front());
					left->UpdateMainColor(leftColor);

					PhysicallyBasedMaterial* right	= static_cast<PhysicallyBasedMaterial*>(container->GetChild(0)->GetComponent<Geometry::Mesh>()->GetMeshRenderer()->GetMaterials().front());
					right->UpdateMainColor(rightColor);

					PhysicallyBasedMaterial* back	= static_cast<PhysicallyBasedMaterial*>(container->GetChild(1)->GetComponent<Geometry::Mesh>()->GetMeshRenderer()->GetMaterials().front());
					back->UpdateMainColor(backColor);

					PhysicallyBasedMaterial* front	= static_cast<PhysicallyBasedMaterial*>(container->GetChild(4)->GetComponent<Geometry::Mesh>()->GetMeshRenderer()->GetMaterials().front());
					front->UpdateMainColor(frontColor);

					PhysicallyBasedMaterial* top	= static_cast<PhysicallyBasedMaterial*>(container->GetChild(5)->GetComponent<Geometry::Mesh>()->GetMeshRenderer()->GetMaterials().front());
					top->UpdateMainColor(topColor);

					PhysicallyBasedMaterial* bottom	= static_cast<PhysicallyBasedMaterial*>(container->GetChild(2)->GetComponent<Geometry::Mesh>()->GetMeshRenderer()->GetMaterials().front());
					bottom->UpdateMainColor(botColor);
				}

				_voxelObjects.push_back(meshObject);
			}
		}
	}

	return _voxelsParent;
}

void VoxelViewer::DestroyAllVoxelMeshes()
{
	for(auto iter = _voxelObjects.begin(); iter !=  _voxelObjects.end(); ++iter)
	{
		(*iter)->DeleteAllChild();
		SAFE_DELETE(*iter);
	}

	_voxelObjects.clear();
}