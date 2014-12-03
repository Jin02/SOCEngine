#include "DirectX.h"
#include "Utility.h"

using namespace Device;

DirectX::DirectX(void) :
	_device(nullptr), _swapChain(nullptr), _immediateContext(nullptr),
	_depthStencil(nullptr), _depthStencilView(nullptr),
	_renderTargetView(nullptr), _disableCulling(nullptr), _depthStencilSRV(nullptr)
{

}

DirectX::~DirectX(void)
{
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

bool DirectX::CreateDepthStencilRenderTarget(const Math::Size<int>& winSize, IDXGISwapChain* swapChain)
{
	D3D11_TEXTURE2D_DESC descDepth;
	memset(&descDepth, 0, sizeof(D3D11_TEXTURE2D_DESC));
	descDepth.Width					= winSize.w;
	descDepth.Height				= winSize.h;
	descDepth.MipLevels				= 1;
	descDepth.ArraySize				= 1;
	descDepth.Format				= DXGI_FORMAT_R32_TYPELESS;
	descDepth.Usage					= D3D11_USAGE_DEFAULT;
	descDepth.BindFlags				= D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	descDepth.CPUAccessFlags		= 0;
	descDepth.MiscFlags				= 0;

	//multisampler
	{
		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		swapChain->GetDesc(&swapChainDesc);
		descDepth.SampleDesc.Count		= swapChainDesc.SampleDesc.Count;
		descDepth.SampleDesc.Quality	= swapChainDesc.SampleDesc.Quality;
	}

	HRESULT hr = _device->CreateTexture2D( &descDepth, nullptr, &_depthStencil );
	if( FAILED( hr ) )
	{
		ASSERT("not create depthStencil Texture");
		return false;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	memset(&descDSV, 0, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;

	descDSV.ViewDimension = descDepth.SampleDesc.Count > 1 ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;

	hr = _device->CreateDepthStencilView( _depthStencil, &descDSV, &_depthStencilView );
	if( FAILED(hr) )
	{
		ASSERT("Not Create Depth Stencil View");
		return false;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srdesc;
	srdesc.Format = DXGI_FORMAT_R32_FLOAT;
	srdesc.ViewDimension = descDepth.SampleDesc.Count > 1 ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
	srdesc.Texture2D.MostDetailedMip = 0;
	srdesc.Texture2D.MipLevels = 1;

	hr = _device->CreateShaderResourceView(_depthStencil, &srdesc, &_depthStencilSRV);
	if( FAILED(hr) )
	{
		ASSERT("Error");
		return false;
	}

	return true;
}

bool DirectX::CreateDeviceAndSwapChain(const Win32* win, const DXGI_SAMPLE_DESC* multiSampler)
{
	//swapChain setting
	DXGI_SWAP_CHAIN_DESC	sd;
	memset(&sd, 0, sizeof(DXGI_SWAP_CHAIN_DESC));

	const Math::Size<int>& winSize = win->GetSize();
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
			ASSERT("Invalid multisampler count");
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

bool DirectX::CreateViewport(const Math::Size<int>& winSize)
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

	if( CreateDepthStencilRenderTarget(win->GetSize(), _swapChain) == false)
		return false;

	_immediateContext->OMSetRenderTargets(1, &_renderTargetView, _depthStencilView);
	_immediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	CreateViewport(win->GetSize());

	//Create disableCulling rasterizer State
	{
		D3D11_RASTERIZER_DESC rsState;
		rsState.FillMode				= D3D11_FILL_SOLID;
		rsState.CullMode				= D3D11_CULL_NONE;		//ÄÃ¸µ ²û
		rsState.FrontCounterClockwise	= true;
		rsState.DepthBias				= 0;
		rsState.DepthBiasClamp			= 0.0f;
		rsState.SlopeScaledDepthBias	= 0.0f;
		rsState.DepthClipEnable			= true;
		rsState.ScissorEnable			= false;
		rsState.MultisampleEnable		= false;
		rsState.AntialiasedLineEnable	= false;

		if( FAILED(_device->CreateRasterizerState(&rsState, &_disableCulling)) )
			return false;
	}

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