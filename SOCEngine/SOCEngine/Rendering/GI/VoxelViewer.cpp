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

Object* VoxelViewer::GenerateVoxelViewer(const Device::DirectX* dx, const VoxelMap* voxelMapAtlas, uint cascade, bool realloc, float voxelizeSize, Manager::MaterialManager* matMgr)
{
	float voxelSize = voxelizeSize / float(_dimension);

	if(realloc)
		DestroyAllVoxelMeshes();
	else
	{
		if(_voxelObjects.empty() == false)
			return _voxelsParent;
	}

	auto GetVoxelIdx = [&](uint x, uint y, uint z) -> uint
	{
		uint coff =  (_isAnisotropic ? 6 : 1);
		return (z * _dimension * _dimension * coff) + (y * _dimension * coff) + x;
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
	
			ID3D11UnorderedAccessView* uav = voxelMapAtlas->GetSourceMapUAV()->GetView();
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

	auto GetColor = [](uint voxelData) -> Color
	{
		Color color;
		color.r = float( voxelData & 0x000000FF)		/ 255.0f;
		color.g = float((voxelData & 0x0000FF00) >> 8)	/ 255.0f;
		color.b = float((voxelData & 0x00FF0000) >> 16)	/ 255.0f;
		color.a = float((voxelData & 0xFF000000) >> 24)	/ 255.0f;

		return color;
	};

	for(int z=0; z<int(_dimension); ++z)
	{
		for(int y=0; y<int(_dimension); ++y)
		{
			for(int x=0; x<int(_dimension); ++x)
			{
				Vector3 pos	= Vector3(float(x - int(_dimension / 2)), float(y - int(_dimension / 2)), float(z - int(_dimension / 2)));

				if(_isAnisotropic)
				{
					Color	leftColor(0.0f, 0.0f, 0.0f, 0.0f),	rightColor(0.0f, 0.0f, 0.0f, 0.0f),
							topColor(0.0f, 0.0f, 0.0f, 0.0f),	botColor(0.0f, 0.0f, 0.0f, 0.0f),
							frontColor(0.0f, 0.0f, 0.0f, 0.0f),	backColor(0.0f, 0.0f, 0.0f, 0.0f);

					rightColor	= GetColor(voxelMap[GetVoxelIdx(x + (_dimension * 0), y + (_dimension * cascade), z)]);
					leftColor	= GetColor(voxelMap[GetVoxelIdx(x + (_dimension * 1), y + (_dimension * cascade), z)]);
					botColor	= GetColor(voxelMap[GetVoxelIdx(x + (_dimension * 3), y + (_dimension * cascade), z)]);
					topColor	= GetColor(voxelMap[GetVoxelIdx(x + (_dimension * 2), y + (_dimension * cascade), z)]);
					backColor	= GetColor(voxelMap[GetVoxelIdx(x + (_dimension * 4), y + (_dimension * cascade), z)]);
					frontColor	= GetColor(voxelMap[GetVoxelIdx(x + (_dimension * 5), y + (_dimension * cascade), z)]);

					if((rightColor.Get32BitUintColor() 	|	leftColor.Get32BitUintColor()	| 
						botColor.Get32BitUintColor()	|	topColor.Get32BitUintColor()	| 
						backColor.Get32BitUintColor()	|	frontColor.Get32BitUintColor()	) == 0) //모든 칸에 값이 없으면 굳이 매쉬를 만들 필요가 없음
						continue;

					Object* meshObject = importer->Load("./Resources/Voxel/voxel.obj", false);
					{
						char arr[64];
						sprintf_s(arr, "X = %d, Y = %d, Z = %d", x, y, z);
						std::string name = arr;

						meshObject->SetName(name);
						meshObject->GetTransform()->UpdatePosition(pos);
						_voxelsParent->AddChild(meshObject);

						auto UpdateColor =[&](Object* meshObject, const Color& voxelColor)
						{
							auto LoadMaterial = [&](const Color& voxelColor) -> PhysicallyBasedMaterial*
							{
								sprintf_s(arr, "%x", voxelColor.Get32BitUintColor());
								std::string matKey = arr;

								PhysicallyBasedMaterial* material = dynamic_cast<PhysicallyBasedMaterial*>(matMgr->Find(matKey));
								if(material == nullptr)
								{
									material = new PhysicallyBasedMaterial(name);
									material->Initialize();
									material->UpdateMainColor(Color(voxelColor.r, voxelColor.g, voxelColor.b, 1.0f));

									matMgr->Add(matKey, material);
								}

								return material;
							};

							MeshRenderer* meshRenderer	= meshObject->GetComponent<Geometry::Mesh>()->GetMeshRenderer();
							PhysicallyBasedMaterial* material = LoadMaterial(voxelColor);
							meshRenderer->DeleteMaterial(0);
							meshRenderer->AddMaterial(material);
						};

						Object* container			= meshObject->GetChild(0);

						UpdateColor(container->GetChild(3), leftColor);
						UpdateColor(container->GetChild(0), rightColor);
						UpdateColor(container->GetChild(1), backColor);
						UpdateColor(container->GetChild(4), frontColor);
						UpdateColor(container->GetChild(5), topColor);
						UpdateColor(container->GetChild(2), botColor);
					}

					_voxelObjects.push_back(meshObject);
				}
				else
				{
					Color voxelColor = GetColor(voxelMap[GetVoxelIdx(x, y, z)]);

					if(voxelColor.Get32BitUintColor() == 0)
						continue;

					Object* meshObject = importer->Load("./Resources/Cube/Cube.obj", false);
					{
						char arr[64];
						sprintf_s(arr, "X = %d, Y = %d, Z = %d", x, y, z);
						std::string name = arr;

						meshObject->SetName(name);
						meshObject->GetTransform()->UpdatePosition( pos );
						_voxelsParent->AddChild(meshObject);

						sprintf_s(arr, "%x", voxelColor.Get32BitUintColor());
						std::string matKey = arr;

						PhysicallyBasedMaterial* material = dynamic_cast<PhysicallyBasedMaterial*>(matMgr->Find(matKey));
						if(material == nullptr)
						{
							material = new PhysicallyBasedMaterial(name);
							material->Initialize();
							material->UpdateMainColor(Color(voxelColor.r, voxelColor.g, voxelColor.b, 1.0f));

							matMgr->Add(matKey, material);
						}

						Object* container			= meshObject->GetChild(0);
						MeshRenderer* meshRenderer	= container->GetChild(0)->GetComponent<Geometry::Mesh>()->GetMeshRenderer();
						meshRenderer->DeleteMaterial(0);
						meshRenderer->AddMaterial(material);
					}

					_voxelObjects.push_back(meshObject);
				}
			}
		}
	}

	_voxelsParent->GetTransform()->UpdateScale(Vector3(voxelSize, voxelSize, voxelSize));
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