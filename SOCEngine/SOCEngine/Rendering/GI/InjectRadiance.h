#pragma once

#include "ComputeShader.h"
#include "ConstBuffer.h"

namespace Rendering
{
	namespace GI
	{
		class InjectRadiance
		{
		protected:
			GPGPU::DirectCompute::ComputeShader* _shader;

		public:
			InjectRadiance();
			~InjectRadiance();

		protected:
			void Initialize(const std::string& fileName,
							const GPGPU::DirectCompute::ComputeShader::ThreadGroup& threadGroup,
							const Buffer::ConstBuffer*& giInfoConstBuffer);
			void Destroy();
		};
	}
}