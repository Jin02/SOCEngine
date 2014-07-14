#include "Material.h"

namespace Rendering
{
	namespace Material
	{

		Material::Material(const char *name) : useShader(nullptr)
		{
			if(name != nullptr)
				this->name = name;			
		}

		Material::~Material(void)
		{
			DeleteAllShader();
		}

		void Material::Begin()
		{
			useShader->Begin();
		}

		void Material::BeginPass(int pass)
		{
			useShader->BeginPass(pass);
		}

		void Material::End()
		{
			useShader->End();
		}

		void Material::EndPass()
		{
			useShader->EndPass();
		}

		bool Material::SelectUseShader(SOC_uint idx)
		{
			if(idx > shaders.size())
				return false;

			useShader = shaders[idx];
			return true;
		}

		bool Material::SelectUseShader( std::string name )
		{
			std::vector< Shader::Shader* >::iterator iter;
			for(iter = shaders.begin(); iter != shaders.end(); ++iter)
			{
				if((*iter)->GetName() == name)
				{
					useShader = (*iter);
					return true;
				}
			}

			return false;
		}

		bool Material::SelectUseShader(Shader::Shader *shader)
		{
			if(shader == nullptr)
				return false;

			if(HasShader(shader) == false)
				AddShader(shader);

			useShader = shader;
			return true;
		}

		void Material::AddShader( Shader::Shader *shader )
		{
			if(useShader == nullptr)
				useShader = shader;

			shaders.push_back( shader );
		}

		void Material::DeleteShader( Shader::Shader *shader )
		{
			std::vector<Shader::Shader *>::iterator iter;
			for(iter = shaders.begin(); iter != shaders.end(); ++iter)
			{
				if( (*iter) == shader )
				{
					shaders.erase(iter);
					return;
				}
			}
		}

		Shader::Shader* Material::FindShader( const char *name )
		{
			std::vector<Shader::Shader *>::iterator iter;
			for(iter = shaders.begin(); iter != shaders.end(); ++iter)
			{
				if( strcmp(name, (*iter)->GetName()) == 0 )
					return *iter;
			}

			return nullptr;
		}

		bool Material::HasShader(Shader::Shader *shader)
		{
			std::vector<Shader::Shader *>::iterator iter;
			for(iter = shaders.begin(); iter != shaders.end(); ++iter)
			{
				if( (*iter) == shader )
					return true;
			}

			return false;
		}

		void Material::DeleteAllShader( )
		{
			shaders.clear();
		}

		int Material::GetShaderCount()
		{
			return shaders.size();
		}

		SOC_uint Material::GetUseShaderPass()
		{
			return useShader->GetNumPasses();
		}

		const char* Material::GetName()
		{
			return name.c_str();
		}

		bool Material::GetShaderRequiredParameters(unsigned int index, SOC_byte *outMatrixParameters, SOC_byte *outLightParameters)
		{
			if(index > shaders.size())
				return false;

			Shader::Shader *shader = (*(shaders.begin() + index));
			shader->GetRequiredParameters(outMatrixParameters, outLightParameters);

			return true;
		}

		bool Material::GetUseShaderRequiredParameters(SOC_byte *outMatrixParameters, SOC_byte *outLightParameters)
		{
			useShader->GetRequiredParameters(outMatrixParameters, outLightParameters);
			return true;
		}

		void Material::SetUseShaderRequiredParameters(TransformParameters *transform, std::vector<Light::LightParameters> *lights, SOC_Vector4 &viewPos)
		{
			using namespace Shader;

			SOC_byte tp = useShader->GetRequiredMatrixParameters();
			SOC_byte lp = useShader->GetRequiredLightParameters();

			if ( REQUIRED_TRANSFORM::WORLD & tp )
				useShader->SetVariable(BasicParameterNames::GetWorldMatrix(), transform->worldMatrix);
			if ( REQUIRED_TRANSFORM::VIEW & tp )
				useShader->SetVariable(BasicParameterNames::GetViewMatrix(), transform->viewMatrix);
			if ( REQUIRED_TRANSFORM::PROJECTION & tp )
				useShader->SetVariable(BasicParameterNames::GetProjMatrix(), transform->projMatrix);
			if ( REQUIRED_TRANSFORM::VIEW_PROJECTION & tp )
				useShader->SetVariable(BasicParameterNames::GetViewProjMatrix(), transform->viewProjMatrix);
			if ( REQUIRED_TRANSFORM::WORLD_VIEW_PROJECTION & tp )
				useShader->SetVariable(BasicParameterNames::GetWorldViewProjMatrix(), transform->worldViewProjMatrix);

			if ( REQUIRED_TRANSFORM::WORLD_VIEW_INV_TRANS & tp )
				useShader->SetVariable(BasicParameterNames::GetWorldViewInvTnsMatrix(), transform->worldViewInvTns);

			bool test = false;
			if ( REQUIRED_LIGHTING::MATERIAL & lp )
			{
				SOC_Vector3 &v = elements.ambient.GetVector3();
				std::string &str = BasicParameterNames::GetMaterialElement(BasicParameterNames::GetAmbient());
				test = useShader->SetVariable(str.c_str(), &v);

				v = elements.diffuse.GetVector3();
				str = BasicParameterNames::GetMaterialElement(BasicParameterNames::GetDiffuse());
				test = useShader->SetVariable(str.c_str(), &v);

				v = elements.specular.GetVector3();
				str = BasicParameterNames::GetMaterialElement(BasicParameterNames::GetSpecular());
				test = useShader->SetVariable(str.c_str(), &v);

				v = elements.emissive.GetVector3();
				str = BasicParameterNames::GetMaterialElement(BasicParameterNames::GetMaterialEmissive());
				test = useShader->SetVariable(str.c_str(), &v);

				str = BasicParameterNames::GetMaterialElement(BasicParameterNames::GetMaterialTransparent());
				test = useShader->SetVariable(str.c_str(), elements.transparentFactor);

				if(REQUIRED_LIGHTING::MATERIAL_SHININESS & lp)
				{
					str = BasicParameterNames::GetMaterialElement(BasicParameterNames::GetMaterialShininess());
					test = useShader->SetVariable(str.c_str(), elements.shininess);
				}
			}

			if ( REQUIRED_LIGHTING::LIGHT & lp )
			{
				const char *structVariable = BasicParameterNames::GetLight();

				int i = 0;
				for(std::vector<Light::LightParameters>::iterator iter = lights->begin(); iter != lights->end(); ++iter, ++i)
				{
					test = useShader->SetStructArrayVariable(structVariable, i, BasicParameterNames::GetAmbient(), &iter->ambient, sizeof(iter->ambient));
					test = useShader->SetStructArrayVariable(structVariable, i, BasicParameterNames::GetDiffuse(), &iter->diffuse, sizeof(iter->diffuse));
					test = useShader->SetStructArrayVariable(structVariable, i, BasicParameterNames::GetSpecular(), &iter->specular, sizeof(iter->specular));

					test = useShader->SetStructArrayVariable(structVariable, i, BasicParameterNames::GetLightType(), &iter->type, sizeof(iter->type));
					test = useShader->SetStructArrayVariable(structVariable, i, BasicParameterNames::GetLightDir(), &iter->lightDir, sizeof(iter->lightDir));
					test = useShader->SetStructArrayVariable(structVariable, i, BasicParameterNames::GetLightPos(), &iter->lightPos, sizeof(iter->lightPos));					

					if(REQUIRED_LIGHTING::LIGHT_RANGE & lp)
					{
						test = useShader->SetStructArrayVariable(structVariable, i, BasicParameterNames::GetLightRange(), &iter->range, sizeof(iter->range));

						if(REQUIRED_LIGHTING::LIGHT_SPOTANGLE & lp)
							test = useShader->SetStructArrayVariable(structVariable, i, BasicParameterNames::GetLightSpotAngle(), &iter->spotAngle, sizeof(iter->spotAngle));
					}
				}

				test = useShader->SetVariable(BasicParameterNames::GetLightNum(), i);
				test = useShader->SetVariable(BasicParameterNames::GetViewPos(), &viewPos);
			}
		}

		void Material::SetElements(MaterialElements &element)
		{
			this->elements = element;
		}
	}

}