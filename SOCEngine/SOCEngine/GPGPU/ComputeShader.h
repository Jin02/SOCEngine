#pragma once

#include "BaseShader.h"
#include "CSInputBuffer.h"
#include "Texture2D.h"
#include "CSRWBuffer.h"

namespace GPGPU
{
	namespace DirectCompute
	{
		class ComputeShader : public Rendering::Shader::BaseShader
		{
		public:
			struct InputBuffer
			{
				unsigned int idx;
				CSInputBuffer* buffer;
				InputBuffer() : idx(0), buffer(nullptr){}
				~InputBuffer(){}
			};
			struct InputTexture
			{
				unsigned int idx;
				const Rendering::Texture::Texture2D* texture;
				InputTexture() : idx(0), texture(nullptr){}
				~InputTexture(){}
			};
			struct Output
			{
				unsigned int idx;
				CSOutput* output;
				Output() : idx(0), output(nullptr){}
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
			std::vector<InputBuffer>	_inputBuffers;
			std::vector<InputTexture>	_inputTextures;

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
			inline void ClearOutputSlot()		{ _outputs.clear();			}
			inline void ClearInputBufferSlot()	{ _inputBuffers.clear();	}
			inline void ClearInputTextureSlot()	{ _inputTextures.clear();	}

		public:
			GET_SET_ACCESSOR(ThreadGroupInfo, const ThreadGroup&, _threadGroup);
			GET_SET_ACCESSOR(InputBuffers, const std::vector<InputBuffer>&, _inputBuffers);
			GET_SET_ACCESSOR(InputTextures, const std::vector<InputTexture>&, _inputTextures);

			GET_SET_ACCESSOR(Outputs, const std::vector<Output>&, _outputs);
		};
	}
}
