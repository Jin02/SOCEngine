#pragma once

#include "AnisotropicVoxelMapAtlas.h"
#include "Object.h"
#include "ConstBuffer.h"
#include "ComputeShader.h"
#include "CPUReadBuffer.h"

namespace Core
{
	class Scene;
}

namespace Rendering
{
	namespace GI
	{
		namespace Debug
		{
			class VoxelViewer
			{
			public:
				enum class Type { Color, Normal };
				struct InfoCB
				{
					uint curCascade;
					uint dimension;

					uint dummy1;
					uint dummy2;
				};

			private:
				std::vector<Core::Object*>				_voxelObjects;
				Buffer::CPUReadBuffer*					_readBuffer;
				uint									_dimension;
				//Buffer::ConstBuffer*					_infoCB;
				//GPGPU::DirectCompute::ComputeShader*	_shader;
				Core::Object*							_voxelsParent;

			public:
				VoxelViewer();
				~VoxelViewer();

			public:
				void Initialize(uint dimension);
				Core::Object* GenerateVoxelViewer(const Device::DirectX* dx, const AnisotropicVoxelMapAtlas* voxelMapAtlas, uint cascade, Type type, bool realloc = false);
				void DestroyAllVoxelMeshes();

			public:
				GET_ACCESSOR(VoxelsParent, Core::Object*, _voxelsParent);
			};
		}
	}
}