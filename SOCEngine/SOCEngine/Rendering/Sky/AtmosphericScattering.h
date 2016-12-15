#pragma once

#include "GaussianBlur.h"
#include "MeshCamera.h"
#include "LightManager.h"
#include "MeshFilter.h"
#include "Matrix.h"
#include "SkyForm.h"

namespace Rendering
{
	namespace Sky
	{
		class AtmosphericScattering : public SkyForm
		{
		public:
			class Material : public Rendering::Material
			{
			public:
			struct Param
			{
				union
				{
					struct{ ushort rayleigh, turbidity; };
					uint rayleighWithTurbidity;
				};
				union
				{
					struct{ ushort dlIndex, luminance; };
					uint dlIndexWithLuminance;
				};

				float mieCoefficient;
				float mieDirectionalG;

				bool operator==(const Param& a) const
				{
					return	(mieCoefficient == a.mieCoefficient) &&
							(mieDirectionalG == a.mieDirectionalG) &&
							(rayleighWithTurbidity == a.rayleighWithTurbidity) &&
							(dlIndexWithLuminance == a.dlIndexWithLuminance);
				}
			};
			struct SSTransform
			{
				Math::Matrix worldMat;
				Math::Matrix worldViewProjMat;

				bool operator==(const SSTransform& a) const { return (worldMat == a.worldMat) && (worldViewProjMat == a.worldViewProjMat); }
			};

			private:
				SSTransform						_prevSSTransform;
				Param							_prevParam;

			private:
				Buffer::ConstBuffer*			_paramCB;
				Buffer::ConstBuffer*			_ssTransformCB;

			public:
				Material(const std::string& name);
				virtual ~Material();
	
			private:
				virtual void Initialize(){}

			public:
				void Initialize(const Device::DirectX* dx, bool useGS);
				virtual void Destroy();

				void UpdateParam(const Param& param);
				void UpdateTransform(const Camera::CameraForm* camera);

				GET_ACCESSOR(Param, const Param&,	_prevParam);
				GET_ACCESSOR(UseGS, bool,			_customShaders.shaderGroup.gs != nullptr);
			};

		private:
			Material*						_material;

		public:
			AtmosphericScattering();
			~AtmosphericScattering();

		private:
			void BindParamToShader(ID3D11DeviceContext* context, const Camera::MeshCamera* mainCam, const Manager::LightManager* lightMgr);
			void UnBindParamToShader(ID3D11DeviceContext* context);

			void _Render(const Device::DirectX* dx, const Texture::RenderTexture* out, const Camera::MeshCamera* mainCam, const Manager::LightManager* lightMgr);

		public:
			virtual void Render(const Device::DirectX* dx, const Camera::CameraForm* camera,
								const Texture::RenderTexture* out, const Texture::DepthBuffer* depthBuffer, const Manager::LightManager* lightMgr);
			virtual void Render(const Device::DirectX* dx, const Camera::ReflectionProbe* probe,
								const Texture::TextureCube* out, const Texture::DepthBufferCube* depthBuffer, const Manager::LightManager* lightMgr);

		public:
			void Initialize(const Device::DirectX* dx);
			void Destroy();

		public:
			inline void UpdateTransform(const Camera::CameraForm* camera)
			{ if(camera && _material) _material->UpdateTransform(camera); }
			inline void UpdateParam(const Material::Param& param)
			{ _material->UpdateParam(param); }

			GET_ACCESSOR(Material, const Material*, _material);
		};
	}
}