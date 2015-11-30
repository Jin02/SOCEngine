#include "TextureForm.h"

using namespace Rendering::Texture;

TextureForm::TextureForm(Type type)
	: _type(type), _srv(nullptr), _uav(nullptr)
{

}

TextureForm::~TextureForm()
{
	Destroy();

	SAFE_DELETE(_srv);
	SAFE_DELETE(_uav);
}

void TextureForm::Destroy()
{
	_srv->Destory();
	_uav->Destroy();
}