#pragma once

#include "ComputeShader.h"
#include "ConstBuffer.h"
#include "EngineMath.h"
#include "RenderTexture.h"
#include <array>

#include "Lightform.h"
#include "LightManager.h"

namespace Rendering
{
	namespace Light
	{
		class LightCulling
		{
		public:
			static const uint TileSize					= 16;
			static const uint LightMaxNumInTile			= 544;

			enum class InputBufferShaderIndex : unsigned int
			{
				PointLightRadiusWithCenter		= 0,
				SpotLightRadiusWithCenter		= 2,
				SpotLightParam					= 4
			};

			enum class InputTextureShaderIndex : unsigned int
			{
				InvetedOpaqueDepthBuffer		= 11,
				InvetedBlendedDepthBuffer		= 12
			};

			enum class CostBufferShaderIndex : unsigned int
			{
				GlobalData = 1
			};

		public:
			struct GlobalData
			{	
				Math::Matrix	viewMat;
				Math::Matrix 	invProjMat;
				Math::Vector2	screenSize;
				unsigned int 	lightNum;
				unsigned int 	maxNumOfperLightInTile;
			};

			static const int POINT_LIGHT_BUFFER_MAX_NUM			= 2048;
			static const int SPOT_LIGHT_BUFFER_MAX_NUM			= 2048;

		private:
			std::vector<GPGPU::DirectCompute::ComputeShader::InputBuffer>		_inputBuffers;
			std::vector<GPGPU::DirectCompute::ComputeShader::InputTexture>		_inputTextures;
			std::vector<GPGPU::DirectCompute::ComputeShader::Output>			_outputs;

			GPGPU::DirectCompute::ComputeShader*								_computeShader;

			Rendering::Buffer::ConstBuffer*										_globalDataBuffer;			
			bool																_useBlendedMeshCulling;

			GPGPU::DirectCompute::ComputeShader::InputBuffer*					_inputPointLightTransformBuffer;
			GPGPU::DirectCompute::ComputeShader::InputBuffer*					_inputSpotLightTransformBuffer;
			GPGPU::DirectCompute::ComputeShader::InputBuffer*					_inputSpotLightParamBuffer;

			uint _pointLightUpdateCounter;
			uint _spotLightUpdateCounter;

		public:
			LightCulling();
			virtual ~LightCulling();

		protected:
			void Initialize(const std::string& filePath, const std::string& mainFunc, bool useRenderBlendedMesh,
				const Texture::DepthBuffer* opaqueDepthBuffer, const Texture::DepthBuffer* blendedDepthBuffer);
			void UpdateThreadGroup(GPGPU::DirectCompute::ComputeShader::ThreadGroup* outThreadGroup, bool updateComputeShader = true);
			void Destroy();
			inline void SetOuputBuferToComputeShader(const std::vector<GPGPU::DirectCompute::ComputeShader::Output>&  outputs) { _computeShader->SetOutputs(outputs); }

		protected:
			void _Init_InputBuffer_And_Append_To_InputBufferList(GPGPU::DirectCompute::ComputeShader::InputBuffer*& outBuffer,	uint idx, uint bufferStride, uint bufferElementNum, DXGI_FORMAT format);
			void _Set_InputTexture_And_Append_To_InputTextureList(GPGPU::DirectCompute::ComputeShader::InputTexture** outTexture,	uint idx, const Texture::Texture2D* texture);

		public:	
			void UpdateInputBuffers(const Device::DirectX* dx, const GlobalData* globalData, const Rendering::Manager::LightManager* lightManager);
			void Dispatch(const Device::DirectX* dx);

		protected:
			GET_ACCESSOR(PointLightUpdateCounter, uint, _pointLightUpdateCounter);
			GET_ACCESSOR(SpotLightUpdateCounter, uint, _spotLightUpdateCounter);

		public:
			const Math::Size<unsigned int> CalcThreadGroupSize() const;
			unsigned int CalcMaxNumLightsInTile();
		};
	}
}