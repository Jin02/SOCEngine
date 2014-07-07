//#include "Graphics.h"
//#include "Surface.h"
//
//namespace Device
//{
//
//	Graphics::Graphics(PresentInterval interval, Application *app) 
//	{
//		ZeroMemory(&d3dpp, sizeof(d3dpp));
//
//		Application *win = app;
//
//		d3dpp.Windowed = win->IsWindowMode();
//
//		if(interval == PresentInterval::Default)
//			d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
//		else if( interval == PresentInterval::Immediate )
//			d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
//		else d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
//
//		d3dpp.BackBufferWidth				= win->GetSize().w;
//		d3dpp.BackBufferHeight				= win->GetSize().h;
//		d3dpp.BackBufferFormat				= D3DFMT_X8R8G8B8;
//		d3dpp.BackBufferCount				= 1;
//		d3dpp.MultiSampleType				= D3DMULTISAMPLE_NONE;
//		d3dpp.MultiSampleQuality			= 0;
//		d3dpp.SwapEffect					= D3DSWAPEFFECT_DISCARD;
//		d3dpp.hDeviceWindow					= win->GetHandle();
//		d3dpp.EnableAutoDepthStencil		= TRUE;
//		d3dpp.AutoDepthStencilFormat		= D3DFMT_D24X8;
//		d3dpp.Flags							= D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
//		d3dpp.FullScreen_RefreshRateInHz	= 0;
//	}
//
//	Graphics::~Graphics(void)
//	{
//		if(d3dHandle)
//		{
//			d3dHandle->Release();
//			d3dHandle = nullptr;
//		}
//
//		if(device)
//		{
//			device->Release();
//			device = nullptr;
//		}
//	}
//
//	bool Graphics::Initialize()
//	{
//		d3dHandle = Direct3DCreate9( D3D_SDK_VERSION );
//
//		if(FAILED(d3dHandle->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3dpp.hDeviceWindow, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &device)))
//			return false;
//
//		//device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
//		//device->SetRenderState( D3DRS_ZENABLE,  true );
//
//		return true;
//	}
//
//	bool Graphics::Clear(unsigned int count, const Common::Rect<int> *rect, ClearFlag flags, SOC_dword color, float z, unsigned int stencil)
//	{
//		bool success = false;
//
//		if(rect == nullptr)	
//			success = SUCCEEDED( device->Clear(0, NULL, flags, color, z, stencil) );
//		else
//		{
//			D3DRECT d3dRect;
//			d3dRect.x1 = rect->x;
//			d3dRect.y1 = rect->y;
//			d3dRect.x2 = rect->x + rect->size.w;
//			d3dRect.y2 = rect->y + rect->size.h;
//
//			success = SUCCEEDED( device->Clear(count, &d3dRect, flags, color, z, stencil) );
//		}
//
//		return success;
//	}
//
//	bool Graphics::Clear(unsigned int count, const Common::Rect<int> *rect, ClearFlag flags, Rendering::Color &color, float z, unsigned int stencil)
//	{				
//		D3DCOLOR d3dColor = D3DCOLOR_COLORVALUE(color.r, color.g, color.b, color.a);
//
//		//SOC_dword flag;
//		//if(flags & FlagTarget)
//		//	flag |= D3DCLEAR_TARGET;
//		//if(flags & FlagStencil)
//		//	flag |= D3DCLEAR_STENCIL
//
//		return Clear(count, rect, flags, d3dColor, z, stencil);
//	}
//
//	bool Graphics::Clear(ClearFlag flags, Rendering::Color &color)
//	{
//		return Clear(0, NULL, flags, color, 1.0f, 0L);
//	}
//
//	bool Graphics::CreateVertexBuffer(int bufferLength, SOC_dword usage, SOC_POOL pool, DeviceVertexBuffer* outDeviceBuffer)
//	{
//		LPDIRECT3DVERTEXBUFFER9 *buffer = (LPDIRECT3DVERTEXBUFFER9*)outDeviceBuffer;
//		return SUCCEEDED( device->CreateVertexBuffer(bufferLength, usage, 0, pool == SOC_POOL_DEFAULT ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED, buffer, NULL) );
//	}
//
//	bool Graphics::CreateIndexBuffer(int bufferLength, SOC_POOL pool, DeviceIndexBuffer* outDeviceBuffer)
//	{
//		D3DPOOL d3dPool = pool == SOC_POOL_DEFAULT ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED;				
//		return SUCCEEDED( device->CreateIndexBuffer( bufferLength, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, d3dPool, (LPDIRECT3DINDEXBUFFER9*)outDeviceBuffer, NULL) );;
//	}
//
//	bool Graphics::SetIndices( DeviceIndexBuffer indexBuffer )
//	{
//		LPDIRECT3DINDEXBUFFER9 idxBuffer = (LPDIRECT3DINDEXBUFFER9)indexBuffer;
//		return SUCCEEDED( device->SetIndices(idxBuffer) );
//	}
//
//	bool Graphics::DrawIndexedPrimitive(SOC_TRIANGLE type, SOC_int baseVertexIdx, SOC_uint minVertexIdx, SOC_uint numVertices, SOC_uint startIdx, SOC_uint primitiveCount)
//	{
//		D3DPRIMITIVETYPE d3dPrimitiType = (D3DPRIMITIVETYPE)type;
//		return SUCCEEDED( device->DrawIndexedPrimitive(d3dPrimitiType, baseVertexIdx, minVertexIdx, numVertices, startIdx, primitiveCount) );
//		//				return SUCCEEDED( device->DrawPrimitive(d3dPrimitiType, 0, primitiveCount) );
//	}
//
//	bool Graphics::SetVertexStream(SOC_uint stream, DeviceVertexBuffer deviceVertexBuffer, SOC_uint stride)
//	{
//		LPDIRECT3DVERTEXBUFFER9 vb = (LPDIRECT3DVERTEXBUFFER9)deviceVertexBuffer;
//		return SUCCEEDED(device->SetStreamSource( stream, vb, 0, stride));
//	}
//
//	bool Graphics::SetVertexStremFrequency(SOC_uint stream, SOC_uint frequency)
//	{
//		if(frequency == 0)
//			frequency = 1;
//
//		frequency = stream == 0 ? D3DSTREAMSOURCE_INDEXEDDATA | frequency : D3DSTREAMSOURCE_INSTANCEDATA | frequency;
//		return SUCCEEDED( device->SetStreamSourceFreq(stream, frequency) );
//	}
//
//	VertexDeclaration Graphics::CreateVertexDeclation( VertexDeclarationElements *ve )
//	{
//		LPDIRECT3DVERTEXDECLARATION9 decl = declMap[ve->description.c_str()];
//
//		if(decl)
//			return decl;
//
//		int size = ve->vertexElement.size();
//		D3DVERTEXELEMENT9 *elements = new D3DVERTEXELEMENT9[size + 1];
//		//			d3ddeclusage_norm
//
//		for(int i=0; i<size; ++i)
//		{
//			VertexDeclarationElement *e = &ve->vertexElement[i];
//			e->GetD3DElemnts( &(elements[i]) );
//		}
//
//		//decl end
//		//D3DDECL_END()
//		elements[size].Stream = 0xFF;
//		elements[size].Offset = elements[size].Usage = 
//			elements[size].UsageIndex = elements[size].Method = 0;
//		elements[size].Type = D3DDECLTYPE_UNUSED;
//		//DONE
//
//		if( SUCCEEDED(device->CreateVertexDeclaration(elements, &decl) ) )
//		{
//			SOCHashMap<const char *, LPDIRECT3DVERTEXDECLARATION9>::value_type value(ve->description.c_str(), decl);
//			declMap.insert(value);
//		}
//
//		return decl;
//	}
//
//	bool Graphics::SetVertexDeclaration( const char *description )
//	{
//		LPDIRECT3DVERTEXDECLARATION9 decl = declMap[description];
//
//		if(decl == nullptr)
//			return false;
//
//		return SUCCEEDED( device->SetVertexDeclaration( decl ) );
//	}
//
//	bool Graphics::SetVertexDeclaration( VertexDeclaration decl )
//	{				
//		if(decl == nullptr)
//			return false;
//
//		return SUCCEEDED( device->SetVertexDeclaration( decl ) );
//	}
//
//
//	bool Graphics::BeginScene()
//	{
//		return SUCCEEDED(	device->BeginScene() );
//	}
//
//	bool Graphics::EndScene()
//	{
//		return SUCCEEDED( device->EndScene() );
//	}
//
//	void Graphics::Present()
//	{
//		device->Present(NULL, NULL, NULL, NULL);
//	}
//
//	LPDIRECT3DDEVICE9 Graphics::GetD3DDevice()
//	{
//		return device;
//	}
//
//	bool Graphics::GetRenderTarget(SOC_uint renderTargetIdx, DeviceSurface *renderTarget)
//	{
//		return SUCCEEDED( device->GetRenderTarget(renderTargetIdx, renderTarget) );
//	}
//
//	bool Graphics::SetRenderTarget(SOC_uint renderTargetIdx, DeviceSurface renderTarget)
//	{
//		return SUCCEEDED( device->SetRenderTarget(renderTargetIdx, renderTarget) );
//	}
//
//	bool Graphics::GetRenderTarget(SOC_uint renderTargetIdx, void* surface)
//	{
//		DeviceSurface ds = nullptr;
//		Rendering::Buffer::Surface *s = static_cast<Rendering::Buffer::Surface*>(surface);
//
//		bool res = SUCCEEDED( device->GetRenderTarget(renderTargetIdx, &ds) );
//		s->surface = ds;
//
//		return res;
//	}
//
//	bool Graphics::SetRenderTarget(SOC_uint renderTargetIdx, void* surface)
//	{
//		Rendering::Buffer::Surface *s = static_cast<Rendering::Buffer::Surface*>(surface);
//		return SUCCEEDED( device->SetRenderTarget(renderTargetIdx, s->surface) );
//	}
//
//}