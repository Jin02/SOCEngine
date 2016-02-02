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

VoxelViewer::VoxelViewer() :
	_infoCB(nullptr), _shader(nullptr), _readBuffer(nullptr), _dimension(0), _voxelsParent(nullptr)
{
}

VoxelViewer::~VoxelViewer()
{
	SAFE_DELETE(_infoCB);
	SAFE_DELETE(_shader);
	SAFE_DELETE(_readBuffer);

	DestroyAllVoxelMeshes();
	SAFE_DELETE(_voxelsParent);
}

void VoxelViewer::Initialize(uint dimension, bool isAnisotropic)
{
	_infoCB = new ConstBuffer;
	_infoCB->Initialize(sizeof(InfoCB));

	const ResourceManager* resourceMgr = ResourceManager::SharedInstance();
	ShaderManager* shaderMgr = resourceMgr->GetShaderManager();

	std::string filePath = "";
	EngineFactory pathFinder(nullptr);
	pathFinder.FetchShaderFullPath(filePath, "VoxelTextureToBuffer");

	ASSERT_COND_MSG(filePath.empty() == false, "Error, File path is empty!");

	std::vector<ShaderMacro> macros;
	{
		if(isAnisotropic)
			macros.push_back(ShaderMacro("USE_FACE_INDEX", ""));
	}

	ID3DBlob* blob = shaderMgr->CreateBlob(filePath, "cs", "CS", false, &macros);
	_shader = new ComputeShader(ComputeShader::ThreadGroup(0, 0, 0), blob);
	ASSERT_COND_MSG(_shader->Initialize(), "can not create compute shader");

	_readBuffer = new CPUReadBuffer;
	_readBuffer->Initialize(4, dimension * dimension * dimension * (isAnisotropic ? 6 : 1), DXGI_FORMAT_R32_UINT);

	std::vector<ShaderForm::InputUnorderedAccessView> uavs;
	uavs.push_back( ShaderForm::InputUnorderedAccessView(1, _readBuffer->GetUAV()) );
	_shader->SetUAVs(uavs);

	std::vector<ShaderForm::InputConstBuffer> cbs;
	cbs.push_back(ShaderForm::InputConstBuffer(0, _infoCB));
	_shader->SetInputConstBuffers(cbs);

	_dimension		= dimension;
	_isAnisotropic	= isAnisotropic;
}

Object* VoxelViewer::GenerateVoxelViewer(const Device::DirectX* dx, const VoxelMap* voxelMapAtlas, uint cascade, Type type, bool realloc)
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
			InfoCB info;
			info.curCascade	= cascade;
			info.dimension	= _dimension;
	
			uint threadCount = (_dimension + 8 - 1) / 8;
			_shader->SetThreadGroupInfo(ComputeShader::ThreadGroup(threadCount, threadCount, threadCount));
	
			_infoCB->UpdateSubResource(context, &info);
	
			ID3D11UnorderedAccessView* uav = voxelMapAtlas->GetUnorderedAccessView()->GetView();
			context->CSSetUnorderedAccessViews(0, 1, &uav, nullptr);
	
			_shader->Dispatch(context);
	
			uav = nullptr;
			context->CSSetUnorderedAccessViews(0, 1, &uav, nullptr);
		}

		// Buffer to vector
		{
			auto Recive = [&](const void* data) -> void
			{
				uint coff =  (_isAnisotropic ? 6 : 1);
				uint maxCount = GetVoxelIdx(_dimension * coff - 1, _dimension - 1, _dimension - 1) + 1;
				voxelMap.resize( maxCount );

				uint* start	= (uint*)data + GetVoxelIdx(0, _dimension * cascade, 0);
				uint* end	= start + maxCount;

				std::copy(start, end, voxelMap.begin());
			};
			_readBuffer->Read(context, Recive);
		}
	}

	_voxelsParent = new Object("VoxelViwer");

	const ResourceManager* resourceMgr = ResourceManager::SharedInstance();
	MeshImporter* importer = resourceMgr->GetMeshImporter();

	const float scale = 1.0f;

	auto GetColor = [](uint voxelData) -> Color
	{
		Color color;
		color.r = float( voxelData & 0x000000FF)		/ 255.0f;
		color.g = float((voxelData & 0x0000FF00) >> 8)	/ 255.0f;
		color.b = float((voxelData & 0x00FF0000) >> 16)	/ 255.0f;
		color.a = float((voxelData & 0xFF000000) >> 24)	/ 255.0f;

		return color;
	};

	for(uint z=0; z<_dimension; ++z)
	{
		for(uint y=0; y<_dimension; ++y)
		{
			for(uint x=0; x<_dimension; ++x)
			{
				if(_isAnisotropic)
				{
					Color	leftColor(0.0f, 0.0f, 0.0f, 0.0f),	rightColor(0.0f, 0.0f, 0.0f, 0.0f),
						topColor(0.0f, 0.0f, 0.0f, 0.0f),	botColor(0.0f, 0.0f, 0.0f, 0.0f),
						frontColor(0.0f, 0.0f, 0.0f, 0.0f),	backColor(0.0f, 0.0f, 0.0f, 0.0f);

					if(type == Type::Color)
					{
						rightColor	= GetColor(voxelMap[GetVoxelIdx(x + (_dimension * 0), y + (_dimension * cascade), z)]);
						leftColor	= GetColor(voxelMap[GetVoxelIdx(x + (_dimension * 1), y + (_dimension * cascade), z)]);
						botColor	= GetColor(voxelMap[GetVoxelIdx(x + (_dimension * 2), y + (_dimension * cascade), z)]);
						topColor	= GetColor(voxelMap[GetVoxelIdx(x + (_dimension * 3), y + (_dimension * cascade), z)]);
						backColor	= GetColor(voxelMap[GetVoxelIdx(x + (_dimension * 4), y + (_dimension * cascade), z)]);
						frontColor	= GetColor(voxelMap[GetVoxelIdx(x + (_dimension * 5), y + (_dimension * cascade), z)]);
					}
					else if(type == Type::Normal)
					{
						auto asfloat = [&](uint idx) -> float
						{
							uint uintValue = voxelMap[idx];
							return *reinterpret_cast<float*>(&uintValue);
						};
						float px =  asfloat(GetVoxelIdx(x + (_dimension * 0), y + (_dimension * cascade), z));
						float nx = -asfloat(GetVoxelIdx(x + (_dimension * 1), y + (_dimension * cascade), z));
						float py =  asfloat(GetVoxelIdx(x + (_dimension * 2), y + (_dimension * cascade), z));
						float ny = -asfloat(GetVoxelIdx(x + (_dimension * 3), y + (_dimension * cascade), z));
						float pz =  asfloat(GetVoxelIdx(x + (_dimension * 4), y + (_dimension * cascade), z));
						float nz = -asfloat(GetVoxelIdx(x + (_dimension * 5), y + (_dimension * cascade), z));

						Vector3 normal(px + nx, py + ny, pz + nz);
						if(normal.x != 0.0f || normal.y != 0.0f || normal.z != 0.0f)
						{
							Vector3 resColorVec = (normal.Normalized() * 0.5f + Vector3(1.0f, 1.0f, 1.0f));
							Color resColor = Color(resColorVec.x, resColorVec.y, resColorVec.z, 1.0f);

							rightColor	= resColor;
							leftColor	= resColor;
							botColor	= resColor;
							topColor	= resColor;
							backColor	= resColor;
							frontColor	= resColor;
						}
					}

					if((rightColor.Get32BitUintColor()	|	leftColor.Get32BitUintColor()	| 
						botColor.Get32BitUintColor()	|	topColor.Get32BitUintColor()	| 
						backColor.Get32BitUintColor()	|	frontColor.Get32BitUintColor()	) == 0) //모든 칸에 값이 없으면 굳이 매쉬를 만들 필요가 없음
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
				else
				{
					Color voxelColor = GetColor(voxelMap[GetVoxelIdx(x, y, z)]);

					if((voxelColor.Get32BitUintColor() & 0x00ffffff) == 0)
						continue;

					Object* meshObject = importer->Load("./Resources/Cube/Cube.obj", false);
					{
						char arr[64];
						sprintf_s(arr, "X = %d, Y = %d, Z = %d", x, y, z);
						std::string name = arr;

						meshObject->SetName(name);
						meshObject->GetTransform()->UpdatePosition( Vector3(float(x), float(y), float(z)) * scale );
						meshObject->GetTransform()->UpdateScale(Vector3(scale, scale, scale));
						_voxelsParent->AddChild(meshObject);

						Object* container = meshObject->GetChild(0);

						PhysicallyBasedMaterial* material	= static_cast<PhysicallyBasedMaterial*>(container->GetChild(0)->GetComponent<Geometry::Mesh>()->GetMeshRenderer()->GetMaterials().front());
						material->UpdateMainColor(Color(voxelColor.r, voxelColor.g, voxelColor.b, 1.0f));
					}

					_voxelObjects.push_back(meshObject);
				}
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