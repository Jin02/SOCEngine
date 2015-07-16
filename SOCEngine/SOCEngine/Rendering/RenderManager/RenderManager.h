#pragma once

#include <vector>
#include "Mesh.h"
#include "Utility.h"
#include <functional>

#include "DeferredCamera.h"
#include "BackBufferMaker.h"

#include "GlobalSetting.h"

#include <sys/timeb.h>
#include <time.h>
#include "VectorMap.h"
#include <hash_map>

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

		private:
			MeshList	_transparentMeshes;
			MeshList	_opaqueMeshes;
			MeshList	_alphaTestMeshes;

			std::hash_map<Mesh::MeshFilter::BufferElementFlag, const Shader::ShaderGroup>	_gbufferShaders;
			std::hash_map<Mesh::MeshFilter::BufferElementFlag, const Shader::ShaderGroup>	_gbufferShaders_alphaTest;

		public:
			RenderManager();
			~RenderManager();

		public:
			bool Init();

			void UpdateRenderList(const Mesh::Mesh* mesh, MeshType type);
			const Mesh::Mesh* FindMeshFromRenderList(const Mesh::Mesh* mesh, MeshType type);
			bool FindGBufferShader(Shader::ShaderGroup& out, Mesh::MeshFilter::BufferElementFlag bufferFlag, bool isAlphaTest);

		public:
			GET_ACCESSOR(TransparentMeshes, const MeshList&, _transparentMeshes);
			GET_ACCESSOR(OpaqueMeshes, const MeshList&, _opaqueMeshes);
			GET_ACCESSOR(AlphaTestMeshes, const MeshList&, _alphaTestMeshes);
		};
	}
}