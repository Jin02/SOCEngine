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
				unsigned int 	maxLightNumInTile; //한 타일당 최대 빛 갯수.
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

			//현재 하나만 사용. 추후 Blended용 버퍼 혹은 다양한 것들이 추가 예정
			std::vector<GPGPU::DirectCompute::ComputeShader::InputTexture>	_inputTextures;

			std::vector<GPGPU::DirectCompute::ComputeShader::OutputBuffer>	_outputBuffers;

		public:
			LightCulling();
			~LightCulling();

		public:
			void Init(const std::string& folderPath, const std::string& fileName, const Texture::RenderTexture* linearDepth);
			void UpdateInputBuffer(const CullingConstBuffer& cbData, const std::array<Math::Vector4, POINT_LIGHT_LIMIT_NUM>& pointLightCenterWithRadius, const std::array<Math::Vector4, SPOT_LIGHT_LIMIT_NUM>& spotLightCenterWithRadius);
			void Dispatch(ID3D11DeviceContext* context);

		private:
			void UpdateThreadGroup(GPGPU::DirectCompute::ComputeShader::ThreadGroup* outThreadGroup, bool updateComputeShader = true);
		};
	}
}