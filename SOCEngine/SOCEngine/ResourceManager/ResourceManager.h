#pragma once

#include "Singleton.h"

#include "CameraManager.h"
#include "ShaderManager.h"
#include "TextureManager.h"
#include "MaterialManager.h"
#include "BufferManager.h"
#include "UIManager.h"

namespace Resource
{
	class ResourceManager : public Singleton<ResourceManager>
	{
	private:
		friend class Singleton<ResourceManager>;

	private:
		Rendering::Manager::ShaderManager*		_shaderMgr;
		Rendering::Manager::TextureManager*		_textureMgr;
		Rendering::Manager::BufferManager*		_bufferManager;

	private:
		ResourceManager();
		~ResourceManager();

	public:
		void Initialize();
		void Destroy();

	public:
		GET_ACCESSOR(TextureManager, Rendering::Manager::TextureManager*, _textureMgr);
		GET_ACCESSOR(ShaderManager, Rendering::Manager::ShaderManager*, _shaderMgr);
		GET_ACCESSOR(BufferManager, Rendering::Manager::BufferManager*, _bufferManager);
	};
}