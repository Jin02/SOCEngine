#pragma once

#include "ComputeShader.h"
#include "ConstBuffer.h"
#include "EngineMath.h"
#include "RenderTexture.h"
#include <array>

namespace Rendering
{
	namespace Light
	{
		class LightCulling
		{
		public:
			struct CullingConstBuffer
			{	
				Math::Matrix	worldViewMat;
				Math::Matrix 	invProjMat;
				Math::Vector2	screenSize;
				float 			clippingFar;
				unsigned int 	maxLightNumInTile; //ÇÑ Å¸ÀÏ´ç ÃÖ´ë ºû °¹¼ö.
				unsigned int 	lightNum;

				unsigned int 	dummy1;
				unsigned int 	dummy2;
				unsigned int 	dummy3;
			};

			static const int POINT_LIGHT_LIMIT_NUM			= 2048;
			static const int SPOT_LIGHT_LIMIT_NUM			= 2048;
			static const int DIRECTIONAL_LIGHT_LIMIT_NUM	= 1024;

		private:
			GPGPU::DirectCompute::ComputeShader*	_computeShader;
			Rendering::Buffer::ConstBuffer*			_globalDataBuffer;

			std::vector<GPGPU::DirectCompute::ComputeShader::InputBuffer>	_inputBuffers;
			std::vector<GPGPU::DirectCompute::ComputeShader::OutputBuffer>	_outputBuffers;

		public:
			LightCulling();
			~LightCulling();

		public:
			void Init(const std::string& folderPath, const std::string& fileName);
			void UpdateBuffer(const CullingConstBuffer& cbData, const std::array<Math::Vector4, POINT_LIGHT_LIMIT_NUM>& pointLightCenterWithRadius, const std::array<Math::Vector4, SPOT_LIGHT_LIMIT_NUM>& spotLightCenterWithRadius, const Texture::RenderTexture* linearDepth);
			void Dispatch(ID3D11DeviceContext* context);

		private:
			void UpdateThreadGroup(GPGPU::DirectCompute::ComputeShader::ThreadGroup* outThreadGroup, bool updateComputeShader = true);
		};
	}
}