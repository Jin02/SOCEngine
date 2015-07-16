#pragma once

#include "ComputeShader.h"
#include "ConstBuffer.h"
#include "EngineMath.h"
#include "RenderTexture.h"
#include <array>

#include "LightCulling_CSOutputBuffer.h"
#include "Lightform.h"

namespace Rendering
{
	namespace Light
	{
		class LightCulling
		{
		public:
			static const uint TileSize					= 16;
			static const uint LightMaxNumInTile			= 544;

			enum class InputBuffer : unsigned int
			{
				PointLightRadiusWithCenter		= 0,
				SpotLightRadiusWithCenter		= 1
			};

			enum class InputTexture : unsigned int
			{
				InvetedOpaqueDepthBuffer		= 2,
				InvetedBlendedDepthBuffer		= 3
			};

			enum class OutputBuffer : unsigned int
			{
				LightIndexBuffer = 0
			};

		public:
			struct CullingConstBuffer
			{	
				Math::Matrix	worldViewMat;
				Math::Matrix 	invProjMat;
				Math::Vector2	screenSize;
				unsigned int 	lightNum;
				unsigned int 	lightStrideNumInLightIdxBuffer;
			};

			static const int POINT_LIGHT_LIMIT_NUM			= 2048;
			static const int SPOT_LIGHT_LIMIT_NUM			= 2048;
			static const int DIRECTIONAL_LIGHT_LIMIT_NUM	= 1024;

		protected:
			GPGPU::DirectCompute::ComputeShader*	_computeShader;
			Rendering::Buffer::ConstBuffer*			_globalDataBuffer;
			LightCulling_CSOutputBuffer*			_lightIndexBuffer;

			std::vector<GPGPU::DirectCompute::ComputeShader::InputBuffer>			_inputBuffers;
			std::vector<GPGPU::DirectCompute::ComputeShader::InputTexture>			_inputTextures;

			std::vector<GPGPU::DirectCompute::ComputeShader::OutputBuffer>			_outputBuffers;

			bool	_useBlendedMeshCulling;

		public:
			LightCulling();
			~LightCulling();

		protected:
			void Initialize(const std::string& filePath, const std::string& mainFunc, bool useRenderBlendedMesh);
			void UpdateThreadGroup(GPGPU::DirectCompute::ComputeShader::ThreadGroup* outThreadGroup, bool updateComputeShader = true);

		public:
			void InitializeOnlyLightCulling(const std::string& filePath, const std::string& mainFunc, bool useRenderBlendedMesh);
			void Destroy();

		public:
			void UpdateInputBuffer(const Device::DirectX* dx, const CullingConstBuffer& cbData, const Light::LightForm::LightTransformBuffer* pointLightTransformBuffer, const Light::LightForm::LightTransformBuffer* spotLightTransformBuffer);
			void Dispatch(const Device::DirectX* dx, const Texture::DepthBuffer* invertedDepthBuffer, const Texture::DepthBuffer* invertedBlendedDepthBuffer);

		public:
			const Math::Size<unsigned int> CalcThreadSize();
			unsigned int CalcMaxNumLightsInTile();
		};
	}
}