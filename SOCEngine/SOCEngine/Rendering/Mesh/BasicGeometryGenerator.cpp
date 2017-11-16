#include "BasicGeometryGenerator.h"
#include "Engine.h"

using namespace Rendering::Geometry;
using namespace Rendering::Manager;
using namespace Rendering::Shader;
using namespace Math;
using namespace Device;
using namespace Core;
using namespace Rendering;

bool BasicGeometryGenerator::HasFlag(uint vtxInputTypeFlag, DefaultVertexInputTypeFlag flag)
{
	return (static_cast<uint>(flag) & vtxInputTypeFlag) != 0;
}

std::string BasicGeometryGenerator::UintToStr(uint data)
{
	char str[16] = {0, };
	sprintf_s(str, "%08x", data);

	return str;
}

void BasicGeometryGenerator::AppendVertexData(	std::vector<float>& inoutVertexDatas,
											  const Math::Vector3& pos, const Math::Vector3& normal,
											  const Math::Vector3& tangent, const Math::Vector2& uv,
											  uint flag)
{
	inoutVertexDatas.push_back(pos.x);
	inoutVertexDatas.push_back(pos.y);	
	inoutVertexDatas.push_back(pos.z);

	if(HasFlag(flag, DefaultVertexInputTypeFlag::NORMAL))
	{
		inoutVertexDatas.push_back(normal.x);
		inoutVertexDatas.push_back(normal.y);
		inoutVertexDatas.push_back(normal.z);
	}

	if(HasFlag(flag, DefaultVertexInputTypeFlag::TANGENT))
	{
		inoutVertexDatas.push_back(tangent.x);
		inoutVertexDatas.push_back(tangent.y);
		inoutVertexDatas.push_back(tangent.z);
	}

	if(	HasFlag(flag, DefaultVertexInputTypeFlag::UV0) || 
		HasFlag(flag, DefaultVertexInputTypeFlag::UV1) )
	{
		inoutVertexDatas.push_back(uv.x);
		inoutVertexDatas.push_back(uv.y);
	}
}

void BasicGeometryGenerator::MakeMeshInfo(MeshInfo& outInfo, uint vtxInputTypeFlag)
{
	std::vector<VertexShader::SemanticInfo> semantics; // attributes
	uint stride = 12; //position
	{
		auto AppendSemanticInfo = [&](const std::string& name, uint semanticIndex, uint size)
		{
			VertexShader::SemanticInfo info;
			{
				info.name = name;
				info.size = size;
				info.semanticIndex = semanticIndex;
			}
			semantics.push_back(info);
		};
		AppendSemanticInfo("POSITION", 0, 12);

		if(HasFlag(vtxInputTypeFlag, DefaultVertexInputTypeFlag::NORMAL))
		{
			stride += 12;
			AppendSemanticInfo("NORMAL", 0, 12);
		}
		if(HasFlag(vtxInputTypeFlag, DefaultVertexInputTypeFlag::TANGENT))
		{
			stride += 12;
			AppendSemanticInfo("TANGENT", 0, 12);
		}
		if(	HasFlag(vtxInputTypeFlag, DefaultVertexInputTypeFlag::UV0) ||
			HasFlag(vtxInputTypeFlag, DefaultVertexInputTypeFlag::UV1) )
		{
			stride += 8;
			AppendSemanticInfo("TEXCOORD", 0, 8);
		}
	}

	outInfo.stride		= stride;
	outInfo.semantics	= semantics;
}

void BasicGeometryGenerator::CreateBox(std::function<void(const Mesh::CreateFuncArguments&)> createMeshCallback, const Math::Vector3& size, uint defautVertexInputTypeFlag)
{
	const uint vtxCount = 24;

	std::vector<float> vertexDatas;

	float halfWidth		= 0.5f * size.x;
	float halfHeight	= 0.5f * size.y;
	float halfDepth		= 0.5f * size.z;

	// Fill in the front face vertex data.
	AppendVertexData(vertexDatas, Vector3(-halfWidth, -halfHeight, -halfDepth), Vector3(0.0f, 0.0f, -1.0f), Vector3(1.0f, 0.0f, 0.0f), Vector2(0.0f, 1.0f), defautVertexInputTypeFlag);
	AppendVertexData(vertexDatas, Vector3(-halfWidth, +halfHeight, -halfDepth), Vector3(0.0f, 0.0f, -1.0f), Vector3(1.0f, 0.0f, 0.0f), Vector2(0.0f, 0.0f), defautVertexInputTypeFlag);
	AppendVertexData(vertexDatas, Vector3(+halfWidth, +halfHeight, -halfDepth), Vector3(0.0f, 0.0f, -1.0f), Vector3(1.0f, 0.0f, 0.0f), Vector2(1.0f, 0.0f), defautVertexInputTypeFlag);
	AppendVertexData(vertexDatas, Vector3(+halfWidth, -halfHeight, -halfDepth), Vector3(0.0f, 0.0f, -1.0f), Vector3(1.0f, 0.0f, 0.0f), Vector2(1.0f, 1.0f), defautVertexInputTypeFlag);

	// Fill in the back face vertex data.
	AppendVertexData(vertexDatas, Vector3(-halfWidth, -halfHeight, +halfDepth), Vector3(0.0f, 0.0f, 1.0f), Vector3(-1.0f, 0.0f, 0.0f), Vector2(1.0f, 1.0f), defautVertexInputTypeFlag);
	AppendVertexData(vertexDatas, Vector3(+halfWidth, -halfHeight, +halfDepth), Vector3(0.0f, 0.0f, 1.0f), Vector3(-1.0f, 0.0f, 0.0f), Vector2(0.0f, 1.0f), defautVertexInputTypeFlag);
	AppendVertexData(vertexDatas, Vector3(+halfWidth, +halfHeight, +halfDepth), Vector3(0.0f, 0.0f, 1.0f), Vector3(-1.0f, 0.0f, 0.0f), Vector2(0.0f, 0.0f), defautVertexInputTypeFlag);
	AppendVertexData(vertexDatas, Vector3(-halfWidth, +halfHeight, +halfDepth), Vector3(0.0f, 0.0f, 1.0f), Vector3(-1.0f, 0.0f, 0.0f), Vector2(1.0f, 0.0f), defautVertexInputTypeFlag);

	// Fill in the top face vertex data.
	AppendVertexData(vertexDatas, Vector3(-halfWidth, +halfHeight, -halfDepth), Vector3(0.0f, 1.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), Vector2(0.0f, 1.0f), defautVertexInputTypeFlag);
	AppendVertexData(vertexDatas, Vector3(-halfWidth, +halfHeight, +halfDepth), Vector3(0.0f, 1.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), Vector2(0.0f, 0.0f), defautVertexInputTypeFlag);
	AppendVertexData(vertexDatas, Vector3(+halfWidth, +halfHeight, +halfDepth), Vector3(0.0f, 1.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), Vector2(1.0f, 0.0f), defautVertexInputTypeFlag);
	AppendVertexData(vertexDatas, Vector3(+halfWidth, +halfHeight, -halfDepth), Vector3(0.0f, 1.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), Vector2(1.0f, 1.0f), defautVertexInputTypeFlag);

	// Fill in the bottom face vertex data.
	AppendVertexData(vertexDatas, Vector3(-halfWidth, -halfHeight, -halfDepth), Vector3(0.0f, -1.0f, 0.0f), Vector3(-1.0f, 0.0f, 0.0f), Vector2(1.0f, 1.0f), defautVertexInputTypeFlag);
	AppendVertexData(vertexDatas, Vector3(+halfWidth, -halfHeight, -halfDepth), Vector3(0.0f, -1.0f, 0.0f), Vector3(-1.0f, 0.0f, 0.0f), Vector2(0.0f, 1.0f), defautVertexInputTypeFlag);
	AppendVertexData(vertexDatas, Vector3(+halfWidth, -halfHeight, +halfDepth), Vector3(0.0f, -1.0f, 0.0f), Vector3(-1.0f, 0.0f, 0.0f), Vector2(0.0f, 0.0f), defautVertexInputTypeFlag);
	AppendVertexData(vertexDatas, Vector3(-halfWidth, -halfHeight, +halfDepth), Vector3(0.0f, -1.0f, 0.0f), Vector3(-1.0f, 0.0f, 0.0f), Vector2(1.0f, 0.0f), defautVertexInputTypeFlag);

	// Fill in the left face vertex data.
	AppendVertexData(vertexDatas, Vector3(-halfWidth, -halfHeight, +halfDepth), Vector3(-1.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, -1.0f), Vector2(0.0f, 1.0f), defautVertexInputTypeFlag);
	AppendVertexData(vertexDatas, Vector3(-halfWidth, +halfHeight, +halfDepth), Vector3(-1.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, -1.0f), Vector2(0.0f, 0.0f), defautVertexInputTypeFlag);
	AppendVertexData(vertexDatas, Vector3(-halfWidth, +halfHeight, -halfDepth), Vector3(-1.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, -1.0f), Vector2(1.0f, 0.0f), defautVertexInputTypeFlag);
	AppendVertexData(vertexDatas, Vector3(-halfWidth, -halfHeight, -halfDepth), Vector3(-1.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, -1.0f), Vector2(1.0f, 1.0f), defautVertexInputTypeFlag);

	// Fill in the right face vertex data.
	AppendVertexData(vertexDatas, Vector3(+halfWidth, -halfHeight, -halfDepth), Vector3(1.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f), Vector2(0.0f, 1.0f), defautVertexInputTypeFlag);
	AppendVertexData(vertexDatas, Vector3(+halfWidth, +halfHeight, -halfDepth), Vector3(1.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f), Vector2(0.0f, 0.0f), defautVertexInputTypeFlag);
	AppendVertexData(vertexDatas, Vector3(+halfWidth, +halfHeight, +halfDepth), Vector3(1.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f), Vector2(1.0f, 0.0f), defautVertexInputTypeFlag);
	AppendVertexData(vertexDatas, Vector3(+halfWidth, -halfHeight, +halfDepth), Vector3(1.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f), Vector2(1.0f, 1.0f), defautVertexInputTypeFlag);

	//
	// Create the indices.
	//

	std::vector<uint> indices;
	indices.resize(36);

	// Fill in the front face index data
	indices[0] = 0; indices[1] = 1; indices[2] = 2;
	indices[3] = 0; indices[4] = 2; indices[5] = 3;

	// Fill in the back face index data
	indices[6] = 4; indices[7] = 5; indices[8] = 6;
	indices[9] = 4; indices[10] = 6; indices[11] = 7;

	// Fill in the top face index data
	indices[12] = 8; indices[13] = 9; indices[14] = 10;
	indices[15] = 8; indices[16] = 10; indices[17] = 11;

	// Fill in the bottom face index data
	indices[18] = 12; indices[19] = 13; indices[20] = 14;
	indices[21] = 12; indices[22] = 14; indices[23] = 15;

	// Fill in the left face index data
	indices[24] = 16; indices[25] = 17; indices[26] = 18;
	indices[27] = 16; indices[28] = 18; indices[29] = 19;

	// Fill in the right face index data
	indices[30] = 20; indices[31] = 21; indices[32] = 22;
	indices[33] = 20; indices[34] = 22; indices[35] = 23;


	std::string key = "";
	{
		Math::Vector3 _size = size;

		key += UintToStr(*reinterpret_cast<uint*>(&_size.x));
		key += UintToStr(*reinterpret_cast<uint*>(&_size.y));
		key += UintToStr(*reinterpret_cast<uint*>(&_size.z));
		key += UintToStr(defautVertexInputTypeFlag);
	}

	MeshInfo info;
	MakeMeshInfo(info, defautVertexInputTypeFlag);
	Mesh::CreateFuncArguments args("@DefaultBox", std::hash<std::string>()(key), "[Box]", indices, info.semantics);
	{
		args.vertices.byteWidth = info.stride;
		args.vertices.count = static_cast<uint>(vertexDatas.size() / (info.stride / 4));
		args.vertices.data = vertexDatas.data();
	}

	createMeshCallback(args);
}

void BasicGeometryGenerator::CreateSphere(std::function<void(const Mesh::CreateFuncArguments&)> createMeshCallback, float radius, uint sliceCount, uint stackCount, uint defautVertexInputTypeFlag)
{
	std::vector<float> vertexDatas;
	AppendVertexData(vertexDatas, Vector3(0.0f, radius, 0.0f), Vector3(0.0f, 1.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), Vector2(0.0f, 0.0f), defautVertexInputTypeFlag);

	float phiStep = MATH_PI / float(stackCount);
	float thetaStep = 2.0f * MATH_PI / float(sliceCount);

	for (uint i = 1; i <= stackCount - 1; ++i)
	{
		float phi = float(i) * phiStep;

		for (uint j = 0; j <= sliceCount; ++j)
		{
			float theta = j*thetaStep;

			Vector3 position(	radius * sinf(phi) * cosf(theta),
								radius * cosf(phi),
								radius * sinf(phi) * sinf(theta)	);
			Vector2 uv(theta / MATH_PI * 2.0f, phi / MATH_PI);

			Vector3 tangent = Vector3(	-radius*sinf(phi)*sinf(theta),
										0.0f,
										radius*sinf(phi)*cosf(theta)	).Normalized();

			Vector3 normal = position.Normalized();

			AppendVertexData(vertexDatas, position, normal, tangent, uv, defautVertexInputTypeFlag);
		}
	}

	AppendVertexData(vertexDatas, Vector3(0.0f, -radius, 0.0f), Vector3(0.0f, -1.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), Vector2(0.0f, 1.0f), defautVertexInputTypeFlag);

	std::vector<uint> indices;
	for (uint i = 1; i <= sliceCount; ++i)
	{
		indices.push_back(0);
		indices.push_back(i + 1);
		indices.push_back(i);
	}

	uint baseIndex = 1;
	uint ringVertexCount = sliceCount + 1;
	for (uint i = 0; i < stackCount - 2; ++i)
	{
		for (uint j = 0; j < sliceCount; ++j)
		{
			indices.push_back(baseIndex + i * ringVertexCount + j);
			indices.push_back(baseIndex + i * ringVertexCount + j + 1);
			indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);

			indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);
			indices.push_back(baseIndex + i * ringVertexCount + j + 1);
			indices.push_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
		}
	}

	MeshInfo info;
	MakeMeshInfo(info, defautVertexInputTypeFlag);

	uint southPoleIndex = static_cast<uint>(vertexDatas.size() / (info.stride / 4)) - 1;
	baseIndex = southPoleIndex - ringVertexCount;

	for (uint i = 0; i < sliceCount; ++i)
	{
		indices.push_back(southPoleIndex);
		indices.push_back(baseIndex + i);
		indices.push_back(baseIndex + i + 1);
	}

	std::string key = "";
	{
		key += UintToStr(*reinterpret_cast<uint*>(&radius));
		key += UintToStr(sliceCount);
		key += UintToStr(stackCount);
		key += UintToStr(defautVertexInputTypeFlag);
	}

	Mesh::CreateFuncArguments args("@DefaultSphere", std::hash<std::string>()(key), "[Sphere]", indices, info.semantics);
	{
		args.vertices.byteWidth	= info.stride;
		args.vertices.count		= static_cast<uint>(vertexDatas.size() / (info.stride / 4));
		args.vertices.data		= vertexDatas.data();
	}

	createMeshCallback(args);
}

void BasicGeometryGenerator::CreateCylinder(std::function<void(const Mesh::CreateFuncArguments&)> createMeshCallback, float botRadius, float topRadius, float height, uint sliceCount, uint stackCount, uint defautVertexInputTypeFlag)
{
	float stackHeight	= height / stackCount;
	float radiusStep	= (topRadius - botRadius) / stackCount;

	uint ringCount		= stackCount + 1;

	std::vector<float> vertices;
	// Compute vertices for each stack ring starting at the bottom and moving up.
	for (uint i = 0; i < ringCount; ++i)
	{
		float y = -0.5f * height + i * stackHeight;
		float r = botRadius + i * radiusStep;

		// vertices of ring
		float dTheta = 2.0f * MATH_PI / sliceCount;
		for (uint j = 0; j <= sliceCount; ++j)
		{
			float c = cosf(j * dTheta);
			float s = sinf(j * dTheta);

			Vector3 position = Vector3(r * c, y, r * s);

			Vector2 uv((float)j / sliceCount, 1.0f - (float)i / stackCount);
			Vector3 tangent = Vector3(-s, 0.0f, c);

			float dr = botRadius - topRadius;
			Vector3 bitangent(dr * c, -height, dr * s);

			Vector3 normal = Vector3::Cross(tangent, bitangent).Normalized();

			AppendVertexData(vertices, position, normal, tangent, uv, defautVertexInputTypeFlag);
		}
	}

	std::vector<uint> indices;
	uint ringVertexCount = sliceCount + 1;

	// Compute indices for each stack.
	for (uint i = 0; i < stackCount; ++i)
	{
		for (uint j = 0; j < sliceCount; ++j)
		{
			indices.push_back(i * ringVertexCount + j);
			indices.push_back((i + 1) * ringVertexCount + j);
			indices.push_back((i + 1) * ringVertexCount + j + 1);

			indices.push_back(i * ringVertexCount + j);
			indices.push_back((i + 1) * ringVertexCount + j + 1);
			indices.push_back(i * ringVertexCount + j + 1);
		}
	}

	MeshInfo info;
	MakeMeshInfo(info, defautVertexInputTypeFlag);

	// TopCap
	{
		uint baseIndex = static_cast<uint>(vertices.size() / (info.stride / 4));

		float y = 0.5f * height;
		float dTheta = 2.0f * MATH_PI / float(sliceCount);

		// Duplicate cap ring vertices because the texture coordinates and normals differ.
		for (uint i = 0; i <= sliceCount; ++i)
		{
			float x = topRadius * cosf(i * dTheta);
			float z = topRadius * sinf(i * dTheta);

			// Scale down by the height to try and make top cap texture coord area
			// proportional to base.
			float u = float(x) / height + 0.5f;
			float v = float(z) / height + 0.5f;

			AppendVertexData(vertices, Vector3(x, y, z), Vector3(0.0f, 1.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), Vector2(u, v), defautVertexInputTypeFlag);
		}

		// Cap center vertex.
		AppendVertexData(vertices, Vector3(0.0f, y, 0.0f), Vector3(0.0f, 1.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), Vector2(0.5f, 0.5f), defautVertexInputTypeFlag);

		// Index of center vertex.
		uint centerIndex = static_cast<uint>(vertices.size() / (info.stride / 4)) - 1;

		for (uint i = 0; i < sliceCount; ++i)
		{
			indices.push_back(centerIndex);
			indices.push_back(baseIndex + i + 1);
			indices.push_back(baseIndex + i);
		}
	}

	// BotCap
	{
		uint baseIndex = static_cast<uint>(vertices.size() / (info.stride / 4));
		float y = -0.5f * height;
		float dTheta = 2.0f * MATH_PI / float(sliceCount);

		for (uint i = 0; i <= sliceCount; ++i)
		{
			float x = botRadius * cosf(i * dTheta);
			float z = botRadius * sinf(i * dTheta);

			float u = x / height + 0.5f;
			float v = z / height + 0.5f;

			AppendVertexData(vertices, Vector3(x, y, z), Vector3(0.0f, -1.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), Vector2(u, v), defautVertexInputTypeFlag);
		}

		// Cap center vertex.
		AppendVertexData(vertices, Vector3(0.0f, y, 0.0f), Vector3(0.0f, -1.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), Vector2(0.5f, 0.5f), defautVertexInputTypeFlag);

		// Cache the index of center vertex.
		uint centerIndex = static_cast<uint>(vertices.size() / (info.stride / 4)) - 1;

		for (uint i = 0; i < sliceCount; ++i)
		{
			indices.push_back(centerIndex);
			indices.push_back(baseIndex + i);
			indices.push_back(baseIndex + i + 1);
		}
	}

	std::string key = "";
	{
		key += UintToStr(*reinterpret_cast<uint*>(&botRadius));
		key += UintToStr(*reinterpret_cast<uint*>(&topRadius));

		key += UintToStr(*reinterpret_cast<uint*>(&height));
		key += UintToStr(sliceCount);
		key += UintToStr(stackCount);
		key += UintToStr(defautVertexInputTypeFlag);
	}

	Mesh::CreateFuncArguments args("@DefaultCylinder", std::hash<std::string>()(key), "[Cylinder]", indices, info.semantics);
	{
		args.vertices.byteWidth = info.stride;
		args.vertices.count = static_cast<uint>(vertices.size() / (info.stride / 4));
		args.vertices.data = vertices.data();
	}

	createMeshCallback(args);
}

void BasicGeometryGenerator::CreatePlane(std::function<void(const Mesh::CreateFuncArguments&)> createMeshCallback, float width, float height, uint widthVertexCount, uint heightVertexCount, uint defautVertexInputTypeFlag)
{
	uint vertexCount = widthVertexCount * heightVertexCount;
	uint faceCount = (widthVertexCount - 1) * (heightVertexCount - 1) * 2;

	//
	// Create the vertices.
	//

	float halfWidth		= 0.5f * width;
	float halfHeight	= 0.5f * height;

	float dx = width / float(widthVertexCount - 1);
	float dz = height / float(heightVertexCount - 1);

	float du = 1.0f / float(widthVertexCount - 1);
	float dv = 1.0f / float(heightVertexCount - 1);

	std::vector<float> vertices;
	for (uint i = 0; i < heightVertexCount; ++i)
	{
		float z = halfHeight - float(i) * dz;
		for (uint j = 0; j < widthVertexCount; ++j)
		{
			float x = -halfWidth + float(j) * dx;
			AppendVertexData(vertices, Vector3(x, 0.0f, z), Vector3(0.0f, 1.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), Vector2(j * du, i * dv), defautVertexInputTypeFlag);
		}
	}

	//
	// Create the indices.
	//

	std::vector<uint> indices;
	indices.resize(faceCount * 3); // 3 indices per face

	// Iterate over each quad and compute indices.
	uint k = 0;
	for (uint i = 0; i < heightVertexCount - 1; ++i)
	{
		for (uint j = 0; j < widthVertexCount - 1; ++j)
		{
			indices[k + 0] = i * widthVertexCount + j;
			indices[k + 1] = i * widthVertexCount + j + 1;
			indices[k + 2] = (i + 1) * widthVertexCount + j;

			indices[k + 3] = (i + 1) * widthVertexCount + j;
			indices[k + 4] = i * widthVertexCount + j + 1;
			indices[k + 5] = (i + 1) * widthVertexCount + j + 1;

			k += 6; // next quad
		}
	}

	std::string key = "";
	{
		key += UintToStr(*reinterpret_cast<uint*>(&width));
		key += UintToStr(*reinterpret_cast<uint*>(&height));

		key += UintToStr(widthVertexCount);
		key += UintToStr(heightVertexCount);
		key += UintToStr(defautVertexInputTypeFlag);
	}

	MeshInfo info;
	MakeMeshInfo(info, defautVertexInputTypeFlag);

	Mesh::CreateFuncArguments args("@DefaultCylinder", std::hash<std::string>()(key), "[Plane]", indices, info.semantics);
	{
		args.vertices.byteWidth = info.stride;
		args.vertices.count = static_cast<uint>(vertices.size() / (info.stride / 4));
		args.vertices.data = vertices.data();
	}

	createMeshCallback(args);
}

void BasicGeometryGenerator::CreateBox(Object& targetObj, Core::Engine& engine, const Vector3& size, uint defautVertexInputTypeFlag)
{
	auto CreateObject = [&targetObj, &engine](const Mesh::CreateFuncArguments& args)
	{
		targetObj.AddComponent<Mesh>().Initialize(engine.GetDirectX(), engine.GetRenderingSystem().GetBufferManager(), args);
	};

	CreateBox(CreateObject, size, defautVertexInputTypeFlag);
}

void BasicGeometryGenerator::CreateSphere(Object& targetObj, Core::Engine& engine, float radius, uint sliceCount, uint stackCount, uint defautVertexInputTypeFlag)
{
	auto CreateObject = [&targetObj, &engine](const Mesh::CreateFuncArguments& args)
	{
		targetObj.AddComponent<Mesh>().Initialize(engine.GetDirectX(), engine.GetRenderingSystem().GetBufferManager(), args);
	};

	CreateSphere(CreateObject, radius, sliceCount, stackCount, defautVertexInputTypeFlag);
}

void BasicGeometryGenerator::CreateCylinder(Object& targetObj, Core::Engine& engine, float botRadius, float topRadius, float height, uint sliceCount, uint stackCount, uint defautVertexInputTypeFlag)
{
	auto CreateObject = [&targetObj, &engine](const Mesh::CreateFuncArguments& args)
	{
		targetObj.AddComponent<Mesh>().Initialize(engine.GetDirectX(), engine.GetRenderingSystem().GetBufferManager(), args);
	};

	CreateCylinder(CreateObject, botRadius, topRadius, height, sliceCount, stackCount, defautVertexInputTypeFlag);
}

void BasicGeometryGenerator::CreatePlane(Object& targetObj, Core::Engine& engine, float width, float height, uint widthVertexCount, uint heightVertexCount, uint defautVertexInputTypeFlag)
{
	auto CreateObject = [&targetObj, &engine](const Mesh::CreateFuncArguments& args)
	{
		targetObj.AddComponent<Mesh>().Initialize(engine.GetDirectX(), engine.GetRenderingSystem().GetBufferManager(), args);
	};

	CreatePlane(CreateObject, width, height, widthVertexCount, heightVertexCount, defautVertexInputTypeFlag);
}