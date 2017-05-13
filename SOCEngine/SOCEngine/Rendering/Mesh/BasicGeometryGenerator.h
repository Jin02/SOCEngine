#pragma once

#include "Vector2.h"
#include "Vector3.h"

#include "Mesh.h"
#include "Object.hpp"
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
			BasicGeometryGenerator() = default;

			void CreateBox(std::function<void(const Mesh::CreateFuncArguments&)> createMeshCallback,		const Math::Vector3& size, uint defautVertexInputTypeFlag);
			void CreateSphere(std::function<void(const Mesh::CreateFuncArguments&)> createMeshCallback,		float radius, uint sliceCount, uint stackCount, uint defautVertexInputTypeFlag);
			void CreateCylinder(std::function<void(const Mesh::CreateFuncArguments&)> createMeshCallback,	float botRadius, float topRadius, float height, uint sliceCount, uint stackCount, uint defautVertexInputTypeFlag);
			void CreatePlane(std::function<void(const Mesh::CreateFuncArguments&)> createMeshCallback,		float width, float height, uint widthVertexCount, uint heightVertexCount, uint defautVertexInputTypeFlag);

			void CreateBox(Core::Object& targetObj, Core::Engine& engine, const Math::Vector3& size, uint defautVertexInputTypeFlag);
			void CreateSphere(Core::Object& targetObj, Core::Engine& engine, float radius, uint sliceCount, uint stackCount, uint defautVertexInputTypeFlag);
			void CreateCylinder(Core::Object& targetObj, Core::Engine& engine, float botRadius, float topRadius, float height, uint sliceCount, uint stackCount, uint defautVertexInputTypeFlag);
			void CreatePlane(Core::Object& targetObj, Core::Engine& engine, float width, float height, uint widthVertexCount, uint heightVertexCount, uint defautVertexInputTypeFlag);

		private:
			struct MeshInfo
			{
				uint											stride;
				std::vector<Shader::VertexShader::SemanticInfo>	semantics;
			};
			std::string UintToStr(uint data);
			void AppendVertexData(std::vector<float>& inoutVertexDatas,
				const Math::Vector3& pos, const Math::Vector3& normal,
				const Math::Vector3& tangent, const Math::Vector2& uv, uint flag);
			bool HasFlag(uint vtxInputTypeFlag, DefaultVertexInputTypeFlag flag);
			void MakeMeshInfo(MeshInfo& outInfo, uint vtxInputTypeFlag);
		};
	}
}