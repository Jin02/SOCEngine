#pragma once

#include "ShaderForm.h"
#include "ShaderResourceBuffer.h"
#include "Texture2D.h"

namespace GPGPU
{
	namespace DirectCompute
	{
		class ComputeShader : public Rendering::Shader::ShaderForm
		{
		public:
			struct ThreadGroup
			{
				unsigned int x, y, z;
				ThreadGroup() : x(0), y(0), z(0){}
				ThreadGroup(unsigned int _x, unsigned int _y, unsigned _z) : x(_x), y(_y), z(_z){}
				~ThreadGroup(){}
				bool IsValid() const { return x && y && z; }
			};

		private:
			std::vector<InputShaderResourceBuffer>	_inputSRBuffers;
			std::vector<InputTexture>				_inputTextures;
			std::vector<InputConstBuffer>			_inputConstBuffers;

			std::vector<InputUnorderedAccessView>	_uavs;

			ID3D11ComputeShader*					_shader;
			ThreadGroup								_threadGroup;

		public:
			ComputeShader(const ThreadGroup& threadGroup, ID3DBlob* blob);
			virtual ~ComputeShader(void);

		public:
			bool Initialize();
			void Dispatch(ID3D11DeviceContext* context);
			
			static void BindTexture(ID3D11DeviceContext* context, TextureBindIndex bind, const Texture::Texture2D* tex);
			static void BindSamplerState(ID3D11DeviceContext* context, SamplerStateBindIndex bind, ID3D11SamplerState* samplerState);
			static void BindConstBuffer(ID3D11DeviceContext* context, ConstBufferBindIndex bind, const Buffer::ConstBuffer* cb);
			static void BindShaderResourceBuffer(ID3D11DeviceContext* context, TextureBindIndex bind, const Buffer::ShaderResourceBuffer* srBuffer);
			static void BindUnorderedAccessView(ID3D11DeviceContext* context, UAVBindIndex bind, const View::UnorderedAccessView* uav, const uint* initialCounts = nullptr);
		public:
			inline void ClearUAVSlot()						{ _uavs.clear();		}
			inline void ClearInputShaderResourceBufferSlot()			{ _inputSRBuffers.clear();	}
			inline void ClearInputTextureSlot()					{ _inputTextures.clear();	}
			inline void ClearInputConstBuffer()					{ _inputConstBuffers.clear();	}

		public:
			GET_SET_ACCESSOR(ThreadGroupInfo,	const ThreadGroup&,								_threadGroup);
			GET_SET_ACCESSOR(InputConstBuffers,	const std::vector<InputConstBuffer>&,			_inputConstBuffers); 
			GET_SET_ACCESSOR(InputSRBuffers,	const std::vector<InputShaderResourceBuffer>&,	_inputSRBuffers);
			GET_SET_ACCESSOR(InputTextures,		const std::vector<InputTexture>&,				_inputTextures);
			GET_SET_ACCESSOR(UAVs,				const std::vector<InputUnorderedAccessView>&,	_uavs);
		};
	}
}
