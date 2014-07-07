#pragma once

#include "Application.h"
#include "Utility.h"
#include "Color.h"
#include "DeviceType.h"
//#include "VertexDeclaration.h"
#include "HashMap.h"
#include "Rect.h"
#include <string>

namespace Device
{

		class Graphics
		{
		public:
			enum ClearFlagType
			{
				FlagTarget	= 1,
				FlagZBuffer	= 2,
				FlagStencil	= 4
			};

			typedef SOC_dword ClearFlag;

		private:
			//SOCHashMap<const char *, LPDIRECT3DVERTEXDECLARATION9> declMap;

		private:
			//LPDIRECT3D9			d3dHandle;
			//LPDIRECT3DDEVICE9	device;

			//D3DPRESENT_PARAMETERS d3dpp;

		public:
			Graphics(PresentInterval interval, Application *app)
			{}
			~Graphics(void){}

		public:
			bool Initialize(){return false;}

		public:
		/*	bool Clear(unsigned int count, const Math::Rect<int> *rect, ClearFlag flags, SOC_dword color, float z, unsigned int stencil);
			bool Clear(unsigned int count, const Math::Rect<int> *rect, ClearFlag flags, Rendering::Color &color, float z, unsigned int stencil);
			bool Clear(ClearFlag flags, Rendering::Color &color);

			bool CreateVertexBuffer(int bufferLength, SOC_dword usage, SOC_POOL pool, DeviceVertexBuffer* outDeviceBuffer);
			bool CreateIndexBuffer(int bufferLength, SOC_POOL pool, DeviceIndexBuffer* outDeviceBuffer);
			VertexDeclaration CreateVertexDeclation( VertexDeclarationElements *ve );

			bool SetIndices( DeviceIndexBuffer indexBuffer );
			bool SetVertexStream(SOC_uint stream, DeviceVertexBuffer deviceVertexBuffer, SOC_uint stride);
			bool SetVertexStremFrequency(SOC_uint stream, SOC_uint frequency);
			bool SetVertexDeclaration( const char *description );
			bool SetVertexDeclaration( VertexDeclaration decl );

			bool DrawIndexedPrimitive(SOC_TRIANGLE type, SOC_int baseVertexIdx, SOC_uint minVertexIdx, SOC_uint numVertices, SOC_uint startIdx, SOC_uint primitiveCount);

			bool BeginScene();
			bool EndScene();
			void Present();

			bool GetRenderTarget(SOC_uint renderTargetIdx, DeviceSurface *renderTarget);
			bool SetRenderTarget(SOC_uint renderTargetIdx, DeviceSurface renderTarget);

			bool GetRenderTarget(SOC_uint renderTargetIdx, void* surface);
			bool SetRenderTarget(SOC_uint renderTargetIdx, void* surface);

		public:
			LPDIRECT3DDEVICE9 GetD3DDevice();*/
		};

	
}