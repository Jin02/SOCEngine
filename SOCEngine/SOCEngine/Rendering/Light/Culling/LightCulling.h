#pragma once

#include "ComputeShader.h"
#include "ConstBuffer.h"
#include "EngineMath.h"
#include "RenderTexture.h"
#include <array>

#include "Lightform.h"
#include "LightManager.h"

#include "TBRShaderIndexSlotInfo.h"

#define LIGHT_CULLING_TILE_RESOLUTION						16
#define LIGHT_CULLING_LIGHT_MAX_COUNT_IN_TILE 				256

namespace Rendering
{
	namespace Light
	{
		class LightCulling
		{
		public:
			struct TBRParam
			{	
				Math::Matrix		viewMat;
				Math::Matrix 		invProjMat;
				Math::Matrix 		invViewProjViewport;

				Math::Size<float>	viewportSize;
				unsigned int 		packedNumOfLights;
				unsigned int 		maxNumOfperLightInTile;

				Math::Vector4		camWorldPosition;

				TBRParam(){}
				~TBRParam(){}
			};

		private:
			std::vector<GPGPU::DirectCompute::ComputeShader::InputShaderResourceBuffer>		_inputBuffers;
			std::vector<GPGPU::DirectCompute::ComputeShader::InputTexture>					_inputTextures;
			std::vector<GPGPU::DirectCompute::ComputeShader::Output>						_outputs;

			GPGPU::DirectCompute::ComputeShader*											_computeShader;

			bool																			_useBlendedMeshCulling;

			GPGPU::DirectCompute::ComputeShader::InputShaderResourceBuffer*					_inputPointLightTransformBuffer;
			GPGPU::DirectCompute::ComputeShader::InputShaderResourceBuffer*					_inputSpotLightTransformBuffer;
			GPGPU::DirectCompute::ComputeShader::InputShaderResourceBuffer*					_inputSpotLightParamBuffer;

		public:
			LightCulling();
			virtual ~LightCulling();

		protected:
			void Initialize(const std::string& filePath, const std::string& mainFunc,
				const Texture::DepthBuffer* opaqueDepthBuffer, const Texture::DepthBuffer* blendedDepthBuffer,
				const std::vector<Shader::ShaderMacro>* opationalMacros = nullptr);

			void SetInputsToCS();

			void UpdateTBRCommonParam(const Device::DirectX* dx, const TBRParam* param);
			void UpdateThreadGroup(GPGPU::DirectCompute::ComputeShader::ThreadGroup* outThreadGroup, bool updateComputeShader = true);

			void Destroy();

			inline void SetOuputBuferToCS(const std::vector<GPGPU::DirectCompute::ComputeShader::Output>&  outputs) { _computeShader->SetOutputs(outputs); }

		protected:
			void AddInputBufferToList(GPGPU::DirectCompute::ComputeShader::InputShaderResourceBuffer*& outBuffer, uint idx, const Buffer::ShaderResourceBuffer*& buffer);
			void AddTextureToInputTextureList(uint idx, const Texture::Texture2D* texture);

		public:	
			void Dispatch(const Device::DirectX* dx,
				const Buffer::ConstBuffer* tbrConstBuffer);

		public:
			const Math::Size<unsigned int> CalcThreadGroupSize() const;
			static unsigned int CalcMaxNumLightsInTile();
		};
	}
}