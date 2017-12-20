#pragma once

#include "MaterialManager.h"
#include "BufferManager.hpp"
#include "ShaderManager.h"
#include "Texture2DManager.h"
#include "PostProcessPipeline.h"
#include "MainRenderer.h"
#include "ShadowAtlasMapRenderer.h"
#include "SkyPreethamModelRenderer.h"

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
		DISALLOW_ASSIGN_COPY(RenderingSystem);

		void InitializeRenderer(Core::Engine& engine, const RenderSetting&& param);
		void Initialize(Core::Engine& engine);
		void Update(Core::Engine& engine, float dt);
		void Render(Core::Engine& engine, float dt);
		void Destroy(Core::Engine& engine);

		GET_ALL_ACCESSOR_REF(MaterialManager,			_materialManager);
		GET_ALL_ACCESSOR_REF(BufferManager,				_bufferManager);
		GET_ALL_ACCESSOR_REF(ShaderManager,				_shaderManager);
		GET_ALL_ACCESSOR_REF(Texture2DManager,			_tex2dManager);
		GET_ALL_ACCESSOR_REF(PostProcessPipeline,		_postProcessing);
		GET_ALL_ACCESSOR_REF(MainRenderer,				_mainRenderer);
		GET_ALL_ACCESSOR_REF(ShadowAtlasMapRenderer,	_shadowRenderer);

		Renderer::MeshRenderer::Param GetMeshRenderParam() const
		{
			return Renderer::MeshRenderer::Param(_bufferManager, _defaultShaders);
		}

		void ActivateSkyScattering(Core::Engine& engine, uint resolution);
		void DeactivateSkyScattering();

		GET_CONST_ACCESSOR(UseSkyScattering,	bool,	_useSkyScattering);

	private:
		Manager::MaterialManager			_materialManager;
		Manager::PostProcessPipeline		_postProcessing;
		Renderer::MainRenderer				_mainRenderer;
		Renderer::ShadowAtlasMapRenderer	_shadowRenderer;
		Renderer::SkyPreethamModelRenderer	_skyScatteringRenderer;

		Manager::BufferManager				_bufferManager;
		Manager::ShaderManager				_shaderManager;
		Manager::Texture2DManager			_tex2dManager;
		Manager::DefaultShaders				_defaultShaders;

		PostProcessing::Copy				_backBufferMaker;

	private:
		bool								_useSkyScattering = false;
	};
}