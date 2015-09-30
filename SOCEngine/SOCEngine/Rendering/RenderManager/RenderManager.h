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

namespace Rendering
{
	namespace Manager
	{
		class RenderManager
		{
		public:
			struct MeshList
			{
				typedef unsigned __int64 meshkey; //is address

				//first key is vbkey.
				Structure::VectorHashMap<std::string, std::set<meshkey>> meshes;

				uint updateCounter;

				MeshList(){}
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

			std::hash_map<uint, const Shader::ShaderGroup>	_gbufferShaders;
			std::hash_map<uint, const Shader::ShaderGroup>	_gbufferShaders_alphaTest;

			std::hash_map<uint, const Shader::ShaderGroup>	_transparentShaders;
			std::hash_map<uint, const Shader::ShaderGroup>	_transparent_depthOnly_Shaders;

		public:
			RenderManager();
			~RenderManager();

		private:
			bool FindShaderFromHashMap(Shader::ShaderGroup& outObject, const std::hash_map<uint, const Shader::ShaderGroup>& hashMap, uint key) const;

		public:
			bool TestInit();
			Shader::ShaderGroup LoadDefaultSahder(Mesh::MeshRenderer::Type meshType, uint defaultVertexInputTypeFlag,
				const std::string* customShaderFileName = nullptr, const std::vector<Rendering::Shader::ShaderMacro>* macros = nullptr);

			void UpdateRenderList(const Mesh::Mesh* mesh);
			bool HasMeshInRenderList(const Mesh::Mesh* mesh, Mesh::MeshRenderer::Type type);

			bool FindGBufferShader(Shader::ShaderGroup& out, uint bufferFlag, bool isAlphaTest, bool useAutoLoad = true) const;
			bool FindTransparencyShader(Shader::ShaderGroup& out, uint bufferFlag, bool isDepthOnly, bool useAutoLoad = true) const;

			bool HasGBufferShader(uint bufferFlag, bool isAlphaTest) const;
			bool HasTransparencyShader(uint bufferFlag, bool isDepthOnly) const;

			void MakeDefaultSahderFileName(std::string& outFileName, Mesh::MeshRenderer::Type meshType, uint bufferFlag) const;
			bool Fuck(Mesh::MeshRenderer::Type meshType, uint defaultShaderBufferFlag,
				const std::vector<Shader::VertexShader::SemanticInfo>& semanticInfos);
		
		public:
			GET_ACCESSOR(TransparentMeshes,	const MeshList&,	_transparentMeshes);
			GET_ACCESSOR(OpaqueMeshes,		const MeshList&,	_opaqueMeshes);
			GET_ACCESSOR(AlphaTestMeshes,	const MeshList&,	_alphaBlendMeshes);
		};
	}
}