#pragma once

#include "MaterialManager.h"
#include "BufferManager.hpp"
#include "ShaderManager.h"
#include "Texture2DManager.h"
#include "PostProcessPipeline.h"
#include "MainRenderer.h"
#include "ShadowAtlasMapRenderer.h"

#include "LightManager.h"
#include "ShadowManager.h"

#include "RenderSetting.h"

namespace Core
{
	class Engine;
}

namespace Rendering
{
	class RenderingSystem final
	{
	public:
		RenderingSystem() = default;

		void InitializeRenderer(Core::Engine& engine, const RenderSetting&& param);
		void Initialize(Core::Engine& engine);
		void Update(Core::Engine& engine, float dt);
		void Render(Core::Engine& engine);
		void Destroy(Core::Engine& engine);

		GET_ACCESSOR_REF(MaterialManager,			_materialManager);
		GET_ACCESSOR_REF(BufferManager,				_bufferManager);
		GET_ACCESSOR_REF(ShaderManager,				_shaderManager);
		GET_ACCESSOR_REF(Texture2DManager,			_tex2dManager);
		GET_ACCESSOR_REF(PostProcessPipeline,		_postProcessing);
		GET_ACCESSOR_REF(MainRenderer,				_mainRenderer);
		GET_ACCESSOR_REF(ShadowAtlasMapRenderer,	_shadowRenderer);

		Renderer::MeshRenderer::Param GetMeshRenderParam() const
		{
			return Renderer::MeshRenderer::Param(_bufferManager, _defaultShaders);
		}

	private:
		Manager::MaterialManager			_materialManager;
		Manager::PostProcessPipeline		_postProcessing;
		Renderer::MainRenderer				_mainRenderer;
		Renderer::ShadowAtlasMapRenderer	_shadowRenderer;

		Manager::BufferManager				_bufferManager;
		Manager::ShaderManager				_shaderManager;
		Manager::Texture2DManager			_tex2dManager;
		Manager::DefaultShaders				_defaultShaders;
	};
}