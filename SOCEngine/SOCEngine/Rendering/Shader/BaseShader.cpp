#include "BaseShader.h"

using namespace Rendering;
using namespace Rendering::Shader;
using namespace Rendering::Texture;
using namespace Rendering::Buffer;
using namespace Rendering::View;

BaseShader::BaseShader(const DXResource<ID3DBlob>& blob, const std::string& key)
	: _blob(blob), _key(key)
{
}