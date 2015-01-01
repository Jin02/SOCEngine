#pragma once

#include "ComputeShader.h"

namespace Rendering
{
	namespace Light
	{
		class LightCulling
		{
		private:
			GPGPU::DirectCompute::ComputeShader* _computeShader;
			GPGPU::DirectCompute::ComputeShader::ThreadGroup _threadGroupInfo;

		public:
			LightCulling(const GPGPU::DirectCompute::ComputeShader::ThreadGroup& threadGroupInfo);
			~LightCulling();

		public:
			void Init(const std::string& folderPath, const std::string& fileName);
			void Run();
		};
	}
}