#pragma once

#include "Vector3.h"
#include "Mesh.h"
#include "RenderManager.h"

namespace Rendering
{
	namespace Geometry
	{
		class BasicGeometryGenerator
		{
		public:
			BasicGeometryGenerator(){}
			~BasicGeometryGenerator(){}

		private:
			struct MeshInfo
			{
				uint											stride;
				std::vector<Shader::VertexShader::SemanticInfo>	semantics;
			};
			std::string UintToStr(uint data);
			void AppendVertexData(	std::vector<float>& inoutVertexDatas,
									const Math::Vector3& pos, const Math::Vector3& normal,
									const Math::Vector3& tangent, const Math::Vector2& uv, uint flag);
			bool HasFlag(uint vtxInputTypeFlag, Manager::RenderManager::DefaultVertexInputTypeFlag flag);
			void MakeMeshInfo(MeshInfo& outInfo, uint vtxInputTypeFlag);

		public:
			void CreateBox(std::function<void(const Mesh::CreateFuncArguments&)> createMeshCallback,		const Math::Vector3& size, uint defautVertexInputTypeFlag);
			void CreateSphere(std::function<void(const Mesh::CreateFuncArguments&)> createMeshCallback,		float radius, uint sliceCount, uint stackCount, uint defautVertexInputTypeFlag);
			void CreateCylinder(std::function<void(const Mesh::CreateFuncArguments&)> createMeshCallback,	float botRadius, float topRadius, float height, uint sliceCount, uint stackCount, uint defautVertexInputTypeFlag);
			void CreatePlane(std::function<void(const Mesh::CreateFuncArguments&)> createMeshCallback,		float width, float height, uint widthVertexCount, uint heightVertexCount, uint defautVertexInputTypeFlag);

			Core::Object* CreateBox(const Math::Vector3& size, uint defautVertexInputTypeFlag);
			Core::Object* CreateSphere(float radius, uint sliceCount, uint stackCount, uint defautVertexInputTypeFlag);
			Core::Object* CreateCylinder(float botRadius, float topRadius, float height, uint sliceCount, uint stackCount, uint defautVertexInputTypeFlag);
			Core::Object* CreatePlane(float width, float height, uint widthVertexCount, uint heightVertexCount, uint defautVertexInputTypeFlag);
		};
	}
}