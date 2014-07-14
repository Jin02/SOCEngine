#pragma once

#include "Shader.h"
#include "MaterialElements.h"
#include <vector>

namespace Rendering
{
	namespace Material
	{
		class Material
		{
		private:
			std::string name;
			std::vector<Shader::Shader*> shaders;
			Shader::Shader* useShader;

			Material::MaterialElements elements;

		public:
			Material(const char *name = nullptr);
			~Material(void);

		public:
			void Begin();
			void BeginPass(int pass = 0);
			void End();
			void EndPass();

		public:
			bool SelectUseShader(SOC_uint idx);
			bool SelectUseShader( std::string name );
			bool SelectUseShader(Shader::Shader *shader);

			void AddShader( Shader::Shader *shader );
			void DeleteShader( Shader::Shader *shader );
			void DeleteAllShader();

			Shader::Shader* FindShader( const char *name );
			bool HasShader(Shader::Shader *shader);

		public:
			template<typename Type>
			void SetVariable(int idx, char *parameter, Type value)
			{
				(*(shaders.begin() + idx))->SetVariable(parameter, value);
			}
			template<typename Type>
			void SetVariable(int idx, char *parameter, Type value, SOC_uint count)
			{
				(*(shaders.begin() + idx))->SetVariable(parameter, value, count);
			}
			template<typename Type>
			void SetVariable(char *parameter, Type value)
			{
				useShader->SetVariable(parameter, value);
			}
			template<typename Type>
			void SetVariable(char *parameter, Type value, SOC_uint count)
			{
				useShader->SetVariable(parameter, value, count);
			}

		public:
			int GetShaderCount();
			SOC_uint GetUseShaderPass();
			const char* GetName();

			bool GetShaderRequiredParameters(unsigned int index, SOC_byte *outMatrixParameters, SOC_byte *outLightParameters);
			bool GetUseShaderRequiredParameters(SOC_byte *outMatrixParameters, SOC_byte *outLightParameters);

			void SetUseShaderRequiredParameters(TransformParameters *transform, std::vector<Light::LightParameters> *lights, SOC_Vector4 &viewPos);

			virtual void ConnectParamater(){}

			void SetElements(MaterialElements &element);
		};

	}
}