#pragma once

#include "ComputeShader.h"
#include "ConstBuffer.h"
#include "EngineMath.h"
#include "RenderTexture.h"
#include <array>

#include "LightCulling_CSOutputBuffer.h"

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
				unsigned int 	maxLightNumInTile;	// 한 타일당 최대 빛 갯수. 
													// 이 값은 이 클래스 내부에서
													// 알아서 계산됨
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
			LightCulling_CSOutputBuffer*			_lightIndexBuffer;

			std::vector<GPGPU::DirectCompute::ComputeShader::InputBuffer>	_inputBuffers;

			//현재 하나만 사용. 추후 Blended용 버퍼 혹은 다양한 것들이 추가 예정
			std::vector<GPGPU::DirectCompute::ComputeShader::InputTexture>	_inputTextures;

			std::vector<GPGPU::DirectCompute::ComputeShader::OutputBuffer>	_outputBuffers;

		public:
			LightCulling();
			~LightCulling();

		private:
			void UpdateThreadGroup(GPGPU::DirectCompute::ComputeShader::ThreadGroup* outThreadGroup, bool updateComputeShader = true);

		public:
			void Init(const std::string& folderPath, const std::string& fileName, const Texture::RenderTexture* linearDepth);
			void UpdateInputBuffer(const Device::DirectX* dx, const CullingConstBuffer& cbData, const std::array<Math::Vector4, POINT_LIGHT_LIMIT_NUM>& pointLightCenterWithRadius, const std::array<Math::Vector4, SPOT_LIGHT_LIMIT_NUM>& spotLightCenterWithRadius);

			void Dispatch(const Device::DirectX* dx, const Texture::RenderTexture* linearDepth);

			const Math::Size<unsigned int> CalcThreadSize();
			unsigned int CalcMaxNumLightsInTile();

			void Destroy();
		};
	}
}