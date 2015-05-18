import os

os.system("python ./Script/HLSLMetaDataGenerator.py -ProjectDir ./Template/Shader -IsShaderWithMetaData True")
os.system("python ./Script/ShaderFactoryCodeGenerator.py -OriginalShaderFactoryFile ./Template/ShaderFactoryTemplate.hpp -SaveShaderFactoryFile ./Rendering/Shader/EngineShaderFactory.hpp -ScriptRunStartDir ./Template/Shader -ClassName EngineFactory")