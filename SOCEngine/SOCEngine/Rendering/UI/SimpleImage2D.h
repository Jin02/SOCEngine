#pragma once

#include "Mesh.h"
#include "UIObject.h"

namespace UI
{
	class SimpleImage2D : public UIObject
	{
	private:
		Rendering::Mesh::Mesh*		_mesh;
		Rendering::Material*		_material;
		bool						_isOtherMaterial;

	public:
		SimpleImage2D(const std::string& name, const Core::Object* parent = nullptr);
		virtual ~SimpleImage2D();

	public:
		void Create(Rendering::Material* material = nullptr);

	public:
		GET_ACCESSOR(Material, Rendering::Material*, _material);
	};
}
