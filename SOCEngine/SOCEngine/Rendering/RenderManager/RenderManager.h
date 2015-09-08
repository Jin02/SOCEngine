#pragma once

#include <vector>
#include "Mesh.h"
#include "Utility.h"
#include <functional>

#include <sys/timeb.h>
#include <time.h>
#include "VectorMap.h"
#include <hash_map>

#include "ShaderMacro.h"

namespace Rendering
{
	namespace Manager
	{
		class RenderManager
		{
		public:
			enum class MeshType
			{
				Opaque,
				Transparent,
				AlphaTest
			};
			struct MeshList
			{
				uint updateCounter;

				//first value is just key.
				Structure::VectorMap<unsigned int, const Mesh::Mesh*> meshes;

				MeshList(){}
				~MeshList(){}
			};
			enum class DefaultVertexInputTypeFlag : uint
			{
				UV	= 1, //UV
				N	= 2, //Normal
				TB	= 4, //Tangent, Normal
			};

		private:
			MeshList	_transparentMeshes;
			MeshList	_opaqueMeshes;
			MeshList	_alphaTestMeshes;

			std::hash_map<uint, const Shader::ShaderGroup>	_gbufferShaders;
			std::hash_map<uint, const Shader::ShaderGroup>	_gbufferShaders_alphaTest;

			std::hash_map<uint, const Shader::ShaderGroup>	_transparentShaders;
			std::hash_map<uint, const Shader::ShaderGroup>	_transparent_depthOnly_Shaders;

		public:
			RenderManager();
			~RenderManager();

		public:
			bool TestInit();
			Shader::ShaderGroup LoadDefaultSahder(MeshType meshType, uint defaultVertexInputTypeFlag,
				const std::string* customShaderFileName = nullptr, const std::vector<Rendering::Shader::ShaderMacro>* macros = nullptr);

			void UpdateRenderList(const Mesh::Mesh* mesh, MeshType type);
			bool HasMeshInRenderList(const Mesh::Mesh* mesh, MeshType type);

			bool FindGBufferShader(Shader::ShaderGroup& out, uint bufferFlag, bool isAlphaTest) const;
			bool FindTransparencyShader(Shader::ShaderGroup& out, uint bufferFlag, bool isDepthOnly) const;

		public:
			GET_ACCESSOR(TransparentMeshes, const MeshList&, _transparentMeshes);
			GET_ACCESSOR(OpaqueMeshes, const MeshList&, _opaqueMeshes);
			GET_ACCESSOR(AlphaTestMeshes, const MeshList&, _alphaTestMeshes);
		};
	}
}