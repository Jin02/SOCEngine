#include "DirectX.h"
#include "Utility.h"

using namespace Device;

DirectX::DirectX(void) :
	_device(nullptr), _swapChain(nullptr), _immediateContext(nullptr),
	_renderTargetView(nullptr), _disableCulling(nullptr),
	_opaqueBlend(nullptr), _alphaToCoverageBlend(nullptr), _depthLessEqual(nullptr),
	_depthEqualAndDisableDepthWrite(nullptr), _depthBuffer(nullptr)
{

}

DirectX::~DirectX(void)
{
	SAFE_DELETE(_depthBuffer);
}

bool DirectX::CreateRenderTargetView()
{
	ID3D11Texture2D* backBuffer = nullptr;

	HRESULT hr = _swapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), (void**)&backBuffer );

	if( FAILED(hr) )
		return false;

	hr = _device->CreateRenderTargetView( backBuffer, nullptr, &_renderTargetView );
	backBuffer->Release();

	if( FAILED(hr) )
		return false;

	return true;
}

bool DirectX::CreateDeviceAndSwapChain(const Win32* win, const DXGI_SAMPLE_DESC* multiSampler)
{
	//swapChain setting
	DXGI_SWAP_CHAIN_DESC	sd;
	memset(&sd, 0, sizeof(DXGI_SWAP_CHAIN_DESC));

	const Math::Size<unsigned int>& winSize = win->GetSize();
	sd.BufferDesc.Width = winSize.w;
	sd.BufferDesc.Height = winSize.h;

	sd.BufferCount = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;

	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = win->GetHandle();
	sd.Windowed = win->GetIsWindowMode();
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	//msaa
	if(multiSampler == nullptr)
	{
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
	}
	else
	{
		HRESULT hr = _device->CheckMultisampleQualityLevels( sd.BufferDesc.Format, sd.SampleDesc.Count, &sd.SampleDesc.Quality);
		if( FAILED(hr) || sd.SampleDesc.Quality <= 0)
			ASSERT_MSG("Invalid multisampler count");
		sd.SampleDesc.Quality -= 1;
	}

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = ARRAYSIZE( driverTypes );

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};

	int numFeatureLevels = ARRAYSIZE( featureLevels );

	unsigned int createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	HRESULT hr = E_FAIL;
	for( UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++ )
    {
        _driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDeviceAndSwapChain( NULL, _driverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &sd, &_swapChain, &_device, &_featureLevel, &_immediateContext );

		if( SUCCEEDED( hr ) )
            break;
    }

    if( FAILED( hr ) )
        return false;

	return true;
}

bool DirectX::CreateViewport(const Math::Size<unsigned int>& winSize)
{
	D3D11_VIEWPORT vp;

	vp.Width = (FLOAT)winSize.w;
	vp.Height = (FLOAT)winSize.h;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;

	_immediateContext->RSSetViewports( 1, &vp );

	return true;
}

void DirectX::CheckAbleMultiSampler(std::vector<DXGI_SAMPLE_DESC>& outDescs, DXGI_FORMAT format)
{
	unsigned int ableMultiSample = D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT;
	for(ableMultiSample; ableMultiSample >= 0; ableMultiSample/=2)
	{
		DXGI_SAMPLE_DESC desc;
		HRESULT hr = _device->CheckMultisampleQualityLevels( format, ableMultiSample, &desc.Quality);

		if( FAILED(hr) )
			break;

		desc.Count = ableMultiSample;
		desc.Quality -= 1;

		outDescs.push_back(desc);
	}
}

bool DirectX::InitDevice(const Win32* win)
{
	if( CreateDeviceAndSwapChain(win) == false )
		return false;

	if( CreateRenderTargetView() == false )
		return false;

	_immediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	CreateViewport(win->GetSize());

	//Create disableCulling rasterizer State
	{
		D3D11_RASTERIZER_DESC desc;
		desc.FillMode				= D3D11_FILL_SOLID;
		desc.CullMode				= D3D11_CULL_NONE;		//ÄÃ¸µ ²û
		desc.FrontCounterClockwise	= true;
		desc.DepthBias				= 0;
		desc.DepthBiasClamp			= 0.0f;
		desc.SlopeScaledDepthBias	= 0.0f;
		desc.DepthClipEnable			= true;
		desc.ScissorEnable			= false;
		desc.MultisampleEnable		= false;
		desc.AntialiasedLineEnable	= false;

		if( FAILED(_device->CreateRasterizerState(&desc, &_disableCulling)) )
			ASSERT_MSG("Error!, device does not create rasterizer state");
	}
	
	//Create Blend State
	{
		D3D11_BLEND_DESC desc;
		desc.AlphaToCoverageEnable					= false;
		desc.IndependentBlendEnable					= false;
		desc.RenderTarget[0].BlendEnable			= false;
		desc.RenderTarget[0].BlendOp				= D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlend				= D3D11_BLEND_ONE; 
		desc.RenderTarget[0].DestBlend				= D3D11_BLEND_ZERO; 
		desc.RenderTarget[0].BlendOpAlpha			= D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha			= D3D11_BLEND_ONE; 
		desc.RenderTarget[0].DestBlendAlpha			= D3D11_BLEND_ZERO; 
		desc.RenderTarget[0].RenderTargetWriteMask	= D3D11_COLOR_WRITE_ENABLE_ALL;
		if( FAILED(_device->CreateBlendState(&desc, &_opaqueBlend)) )
			ASSERT_MSG("Error!, device does not create opaque blend state");
		desc.AlphaToCoverageEnable = true;
		if( FAILED(_device->CreateBlendState(&desc, &_alphaToCoverageBlend)) )
			ASSERT_MSG("Error!, device does not create alphaToCoverage blend state");
	}

	//Create Depth State
	{
		D3D11_DEPTH_STENCIL_DESC desc;
		desc.DepthEnable		= true; 
		desc.DepthWriteMask		= D3D11_DEPTH_WRITE_MASK_ALL; 
		desc.DepthFunc			= D3D11_COMPARISON_LESS_EQUAL;
		desc.StencilEnable		= false; 
		desc.StencilReadMask	= D3D11_DEFAULT_STENCIL_READ_MASK; 
		desc.StencilWriteMask	= D3D11_DEFAULT_STENCIL_WRITE_MASK; 
		if( FAILED(_device->CreateDepthStencilState( &desc, &_depthLessEqual)) )
			ASSERT_MSG("Error!, device does not create lessEqual dpeth state");
		desc.DepthWriteMask		= D3D11_DEPTH_WRITE_MASK_ZERO; 
		desc.DepthFunc			= D3D11_COMPARISON_EQUAL; 
		if( FAILED(_device->CreateDepthStencilState( &desc, &_depthEqualAndDisableDepthWrite)) )
			ASSERT_MSG("Error!, device does not create depth state equal and disable writing");
	}

	_depthBuffer = new Rendering::Texture::DepthBuffer(this);
	_depthBuffer->Create(win->GetSize());

	return true;
}

unsigned int DirectX::CalcFormatSize(DXGI_FORMAT format) const
{
	if(		DXGI_FORMAT_R32G32B32A32_TYPELESS	<= format && format <= DXGI_FORMAT_R32G32B32A32_SINT)
		return 16;
	else if(DXGI_FORMAT_R32G32B32_TYPELESS		<= format && format <= DXGI_FORMAT_R32G32B32_SINT)
		return 12;
	else if(DXGI_FORMAT_R16G16B16A16_TYPELESS	<= format && format <= DXGI_FORMAT_R16G16B16A16_SINT)
		return 8;
	else if(DXGI_FORMAT_R32G32_TYPELESS			<= format && format <= DXGI_FORMAT_R32G32_SINT)
		return 8;
	else if(DXGI_FORMAT_R8G8B8A8_TYPELESS		<= format && format <= DXGI_FORMAT_R8G8B8A8_SINT)
		return 4;
	else if(DXGI_FORMAT_R16G16_TYPELESS			<= format && format <= DXGI_FORMAT_R16G16_SINT)
		return 4;
	else if(DXGI_FORMAT_R32_TYPELESS			<= format && format <= DXGI_FORMAT_R32_SINT)
		return 4;
	else if(DXGI_FORMAT_R8G8_TYPELESS			<= format && format <= DXGI_FORMAT_R8G8_SINT)
		return 4;
	else if(DXGI_FORMAT_R16_TYPELESS			<= format && format <= DXGI_FORMAT_R16_SINT)
		return 2;
	else if(DXGI_FORMAT_R8_TYPELESS				<= format && format <= DXGI_FORMAT_R8_SINT)
		return 1;

	DEBUG_LOG("Not Support Format");
	return 0;
}