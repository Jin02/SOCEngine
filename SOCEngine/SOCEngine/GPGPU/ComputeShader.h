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
			struct Output
			{
				unsigned int bindIndex;
				const Rendering::View::UnorderedAccessView* output;
				Output() : bindIndex(-1), output(nullptr){}
				~Output(){}
			};
			struct ThreadGroup
			{
				unsigned int x, y, z;
				ThreadGroup() : x(0), y(0), z(0){}
				ThreadGroup(unsigned int _x, unsigned int _y, unsigned _z) : x(_x), y(_y), z(_z){}
				~ThreadGroup(){}
			};

		private:
			std::vector<InputShaderResourceBuffer>	_inputSRBuffers;
			std::vector<InputTexture>				_inputTextures;
			std::vector<InputConstBuffer>			_inputConstBuffers;

			std::vector<Output>			_outputs;

			ID3D11ComputeShader*		_shader;
			ThreadGroup					_threadGroup;

		public:
			ComputeShader(const ThreadGroup& threadGroup, ID3DBlob* blob);
			~ComputeShader(void);

		public:
			bool Initialize();
			void Dispatch(ID3D11DeviceContext* context);

		public:
			inline void ClearOutputSlot()						{ _outputs.clear();				}
			inline void ClearInputShaderResourceBufferSlot()	{ _inputSRBuffers.clear();		}
			inline void ClearInputTextureSlot()					{ _inputTextures.clear();		}
			inline void ClearInputConstBuffer()					{ _inputConstBuffers.clear();	}

		public:
			GET_SET_ACCESSOR(ThreadGroupInfo, const ThreadGroup&, _threadGroup);
			GET_SET_ACCESSOR(InputConstBuffers, const std::vector<InputConstBuffer>&, _inputConstBuffers); 
			GET_SET_ACCESSOR(InputSRBuffers, const std::vector<InputShaderResourceBuffer>&, _inputSRBuffers);
			GET_SET_ACCESSOR(InputTextures, const std::vector<InputTexture>&, _inputTextures);

			GET_SET_ACCESSOR(Outputs, const std::vector<Output>&, _outputs);
		};
	}
}
