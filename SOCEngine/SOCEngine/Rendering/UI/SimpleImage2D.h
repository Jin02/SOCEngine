#pragma once

#include "Mesh.h"
#include "UIObject.h"

namespace UI
{
	class SimpleImage2D : public UIObject
	{
	public:
		struct RectVertexInfo
		{
			Math::Vector4	position;
			Math::Vector2	uv;
		};

	private:
		Rendering::Mesh::MeshFilter*		_meshFilter;
		Rendering::Material*				_material;
		
		bool								_isOtherMaterial;
		Math::Size<uint>					_size;
		bool								_changeSize;

	public:
		SimpleImage2D(const std::string& name, const Core::Object* parent = nullptr);
		virtual ~SimpleImage2D();

	public:
		void Create(const Math::Size<uint>& size, Rendering::Material* material = nullptr);
		void UpdateMainImage(Rendering::Texture::Texture* tex);

	public:
		virtual void Render(const Math::Matrix& viewProjMat);

	public:
		GET_ACCESSOR(Material, Rendering::Material*, _material);
		GET_ACCESSOR(Size, const Math::Size<uint>&, _size);
		void SetSize(const Math::Size<uint>& size);

	};
}
