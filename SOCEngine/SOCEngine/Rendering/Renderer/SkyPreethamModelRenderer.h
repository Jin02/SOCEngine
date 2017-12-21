#pragma once

#include "SkyGeometry.h"
#include "SkyBoxMaterial.h"
#include "ShaderManager.h"

#include "TileBasedShadingHeader.h"
#include "Transform.h"

#include "LightManager.h"
#include "MainCamera.h"

#include "RenderTextureCube.h"
#include "DepthMapCube.h"

#include "MaterialManager.h"

namespace Rendering
{
	namespace Renderer
	{
		class SkyPreethamModelRenderer final
		{
		public:
			SkyPreethamModelRenderer();

			struct LightProbeParam	// TODO : 나중에 LightProbe 관련 기능 구현할 때 옮겨야 한다.
			{
				std::array<Math::Matrix, 6>	viewProjMats;
				Math::Matrix				worldMat;
			};
			struct SkyScatteringParam
			{
				Half rayleigh					= Half(2.0f);	// x
				Half turbidity					= Half(10.0f);	// x

				ushort directionalLightIndex	= -1;			// y
				Half luminance					= Half(1.0f);	// y

				float mieCoefficient			= 0.005f;		// z
				float mieDirectionalG			= 0.8f;			// w
			};

		public:
			void Initialize(Device::DirectX& dx, Manager::BufferManager& bufferMgr, Manager::ShaderManager& shaderMgr, Manager::MaterialManager& materialMgr, uint resolution);
			void Destroy();

			void CheckRenderAbleWithUpdateCB(Device::DirectX& dx, const Core::TransformPool& tfPool, const Manager::LightManager& lightMgr);
			void Render(Device::DirectX& dx, const Camera::MainCamera& mainCam, const Manager::LightManager& lightMgr);

			GET_CONST_ACCESSOR_REF(ResultMaterial,		_resultMaterial);
			GET_CONST_ACCESSOR_REF(SkyScatteringParam,	_ssParamData);

			SET_ACCESSOR_DIRTY(Rayleigh,		Half, _ssParamData.rayleigh);
			SET_ACCESSOR_DIRTY(Turbidity,		Half, _ssParamData.turbidity);
			SET_ACCESSOR_DIRTY(Luminance,		Half, _ssParamData.luminance);
			SET_ACCESSOR_DIRTY(MieCoefficient,	Half, _ssParamData.mieCoefficient);
			SET_ACCESSOR_DIRTY(MieDirectionalG,	Half, _ssParamData.mieDirectionalG);

			GET_CONST_ACCESSOR(DirectionalLightID, Core::ObjectID, _directionalLightID);
			SET_ACCESSOR_DIRTY(DirectionalLightID, Core::ObjectID, _directionalLightID);

			GET_CONST_ACCESSOR(RenderAble, bool,		_renderAble);
			GET_CONST_ACCESSOR(MaterialID, MaterialID,	_materialID);

		private:
			Material::SkyBoxMaterial		_resultMaterial;
			MaterialID						_materialID;

		private:
			SkyScatteringParam				_ssParamData;

			Sky::SkyGeometry				_geometry;
			Texture::RenderTextureCube		_renderTarget;			

			Shader::VertexShader			_vertexShader;
			Shader::GeometryShader			_geometryShader;
			Shader::PixelShader				_pixelShader;

			Buffer::ExplicitConstBuffer<LightProbeParam>	_lpParamCB;
			Buffer::ExplicitConstBuffer<SkyScatteringParam>	_ssParamCB;

			Core::ObjectID									_directionalLightID;
			bool											_dirty		= true; // Param
			bool											_renderAble	= false;
		};
	}
}