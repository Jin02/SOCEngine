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
				unsigned int 	maxLightNumInTile;	// �� Ÿ�ϴ� �ִ� �� ����. 
													// �� ���� �� Ŭ���� ���ο���
													// �˾Ƽ� ����
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

			//���� �ϳ��� ���. ���� Blended�� ���� Ȥ�� �پ��� �͵��� �߰� ����
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