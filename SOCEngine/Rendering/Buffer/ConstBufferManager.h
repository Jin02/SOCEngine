#pragma once

#include "ConstBuffer.h"
#include "Structure.h"

namespace Rendering
{
	namespace Buffer
	{
		class ConstBufferManager
		{
		private:
			Structure::HashMap<ConstBuffer> _hash;

		public:
			ConstBufferManager();	
			~ConstBufferManager();

		public:
			ConstBuffer* AddBuffer(unsigned int bufferSize);
			ConstBuffer* AddBuffer(const std::string& key, unsigned int bufferSize);
	
			ConstBuffer* Find(unsigned int bufferSize);
			ConstBuffer* Find(const std::string& key);

			void Remove(unsigned int bufferSize);
			void Remove(const std::string& key);
			void RemoveAll();
		};
	}
}