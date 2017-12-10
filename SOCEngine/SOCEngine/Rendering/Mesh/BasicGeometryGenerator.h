#pragma once

#include "Vector2.h"
#include "Vector3.h"

#include "Mesh.h"
#include "Object.h"
#include "DefaultRenderTypes.h"

namespace Core
{
	class Engine;
}

namespace Rendering
{
	namespace Geometry
	{
		class BasicGeometryGenerator
		{
		public:
			static void CreateBox(std::function<void(const Mesh::CreateFuncArguments&)> createMeshCallback,		const Math::Vector3& size, uint defautVertexInputTypeFlag);
			static void CreateSphere(std::function<void(const Mesh::CreateFuncArguments&)> createMeshCallback,		float radius, uint sliceCount, uint stackCount, uint defautVertexInputTypeFlag);
			static void CreateCylinder(std::function<void(const Mesh::CreateFuncArguments&)> createMeshCallback,	float botRadius, float topRadius, float height, uint sliceCount, uint stackCount, uint defautVertexInputTypeFlag);
			static void CreatePlane(std::function<void(const Mesh::CreateFuncArguments&)> createMeshCallback,		float width, float height, uint widthVertexCount, uint heightVertexCount, uint defautVertexInputTypeFlag);
		
			static void CreateBox(Core::Object& targetObj, Core::Engine& engine, const Math::Vector3& size, uint defautVertexInputTypeFlag);
			static void CreateSphere(Core::Object& targetObj, Core::Engine& engine, float radius, uint sliceCount, uint stackCount, uint defautVertexInputTypeFlag);
			static void CreateCylinder(Core::Object& targetObj, Core::Engine& engine, float botRadius, float topRadius, float height, uint sliceCount, uint stackCount, uint defautVertexInputTypeFlag);
			static void CreatePlane(Core::Object& targetObj, Core::Engine& engine, float width, float height, uint widthVertexCount, uint heightVertexCount, uint defautVertexInputTypeFlag);

		private:
			struct MeshInfo
			{
				uint											stride;
				std::vector<Shader::VertexShader::SemanticInfo>	semantics;
			};
			static std::string UintToStr(uint data);
			static void AppendVertexData(std::vector<float>& inoutVertexDatas,
				const Math::Vector3& pos, const Math::Vector3& normal,
				const Math::Vector3& tangent, const Math::Vector2& uv, uint flag);
			static bool HasFlag(uint vtxInputTypeFlag, DefaultVertexInputTypeFlag flag);
			static void MakeMeshInfo(MeshInfo& outInfo, uint vtxInputTypeFlag);
		};
	}
}