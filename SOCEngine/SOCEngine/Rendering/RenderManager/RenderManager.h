#pragma once

#include <vector>
#include "Mesh.h"
#include "Utility.h"
#include <functional>

#include <sys/timeb.h>
#include <time.h>
#include "VectorHashMap.h"

#include "ShaderMacro.h"
#include <set>

#include "RenderTypes.h"

namespace Rendering
{
	namespace Manager
	{
		class RenderManager
		{
		public:
			struct MeshList
			{
				typedef address meshkey;

				//first key is vbkey.
				Structure::VectorHashMap<std::string, std::set<meshkey>> meshes;

				uint updateCounter;

				MeshList() : updateCounter(0) {}
				~MeshList(){}
			};
			enum class DefaultVertexInputTypeFlag : uint
			{
				UV0			= 1,
				UV1			= 2,
				NORMAL		= 4,
				TANGENT		= 8,
				COLOR		= 16,
				BONE		= 32, //BONE ID WITH WEIGHT -> float2(id, weight).
				USERS		= 8192
			};

		private:
			MeshList	_transparentMeshes;
			MeshList	_opaqueMeshes;
			MeshList	_alphaBlendMeshes;

			std::hash_map<uint, const Shader::ShaderGroup>	_renderShaders[(uint)RenderType::MAX_NUM];

		public:
			RenderManager();
			~RenderManager();

		public:
			void Initialize();
			Shader::ShaderGroup LoadDefaultSahder(RenderType renderType, uint defaultVertexInputTypeFlag,
				const std::string* customShaderFileName = nullptr, const std::vector<Rendering::Shader::ShaderMacro>* macros = nullptr);

			void UpdateRenderList(const Geometry::Mesh* mesh, bool prevShow);
			bool HasMeshInRenderList(const Geometry::Mesh* mesh, Geometry::MeshRenderer::Type type);

			bool FindShader(Shader::ShaderGroup& out, uint bufferFlag, RenderType renderType) const;
			bool HasShader(uint bufferFlag, RenderType renderType) const;

			void MakeDefaultSahderFileName(std::string& outFileName, RenderType renderType, uint bufferFlag) const;

			struct ShaderMainFuncName
			{
				Shader::ShaderForm::Type type;
				std::string name;
			};
			void MakeDefaultShaderMainFuncNames(std::vector<ShaderMainFuncName>& outMainFuncNames, RenderType renderType);

			void ClearMeshList(Geometry::MeshRenderer::Type type);
			void ClearAllMeshList();
			
			void DeleteDefaultShader(RenderType renderType);
			void DeleteAllDefaultShaders();
			
			void Destroy();

		public:
			GET_ACCESSOR(TransparentMeshes,	const MeshList&,	_transparentMeshes);
			GET_ACCESSOR(OpaqueMeshes,		const MeshList&,	_opaqueMeshes);
			GET_ACCESSOR(AlphaTestMeshes,	const MeshList&,	_alphaBlendMeshes);
		};
	}
}