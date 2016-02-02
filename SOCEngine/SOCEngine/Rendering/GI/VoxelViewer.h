#pragma once

#include "VoxelMap.h"
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
				Buffer::ConstBuffer*					_infoCB;
				GPGPU::DirectCompute::ComputeShader*	_shader;
				Core::Object*							_voxelsParent;
				bool									_isAnisotropic;

			public:
				VoxelViewer();
				~VoxelViewer();

			public:
				void Initialize(uint dimension, bool isAnisotropic);
				Core::Object* GenerateVoxelViewer(const Device::DirectX* dx, const VoxelMap* voxelMapAtlas, uint cascade, Type type, bool realloc, float voxelSize);
				void DestroyAllVoxelMeshes();

			public:
				GET_ACCESSOR(VoxelsParent, Core::Object*, _voxelsParent);
			};
		}
	}
}