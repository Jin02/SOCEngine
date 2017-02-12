#pragma once

#include "ComputeShader.h"
#include "ConstBuffer.h"
#include "VoxelMap.h"
#include "VXGICommon.h"
#include "DirectX.h"
#include "ShadowRenderer.h"
#include "Voxelization.h"

namespace Rendering
{
	namespace GI
	{
		class InjectRadiance
		{
		public:
			struct DispatchParam
			{
				struct Voxelization
				{
					const Buffer::RawBuffer*		AlbedoRawBuffer;
					const Buffer::RawBuffer*		NormalRawBuffer;
					const Buffer::RawBuffer*		EmissionRawBuffer;
					const Buffer::ConstBuffer*		InfoCB;
				};
				struct Global
				{
					const Buffer::ConstBuffer*		vxgiStaticInfo;
					const Buffer::ConstBuffer*		vxgiDynamicInfo;
				};

				Voxelization						voxelization;
				Global								global;

				const Buffer::ConstBuffer*			shadowGlobalInfo;
				const Buffer::ConstBuffer*			tbrParamCB;
				const View::UnorderedAccessView*	OutVoxelColorMap;

				uint								dimension;
			};

		protected:
			GPGPU::DirectCompute::ComputeShader*				_shader;

		protected:
			InjectRadiance();
			~InjectRadiance();

		protected:
			void Initialize(const std::string& fileName);
			void Dispath(const Device::DirectX* dx, const DispatchParam& param);

		public:
			void Destroy();
		};
	}
}