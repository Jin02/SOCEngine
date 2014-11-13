#include "DirectX.h"
#include "Utility.h"

using namespace Device;

DirectX::DirectX(void) :
	_device(nullptr), _swapChain(nullptr), _immediateContext(nullptr),
	_depthStencil(nullptr), _depthStencilView(nullptr),
	_renderTargetView(nullptr), _rasterizerState(nullptr),
	_multiSample(1)
{

}

DirectX::~DirectX(void)
{
}

bool DirectX::CreateRasterize()
{
	//D3D11_RASTERIZER_DESC rsState;
	//rsState.FillMode = D3D11_FILL_SOLID;
	//rsState.CullMode = D3D11_CULL_BACK;
	//rsState.FrontCounterClockwise = true;
	//rsState.DepthBias = 0;
	//rsState.DepthBiasClamp = 0;
	//rsState.SlopeScaledDepthBias = 0;
	//rsState.DepthClipEnable = false;
	//rsState.ScissorEnable = false;
	//rsState.MultisampleEnable = _multiSample > 1;
	//rsState.AntialiasedLineEnable = false;

	//if( FAILED(_device->CreateRasterizerState(&rsState, &_rasterizerState)) )
	//	return false;

	//_immediateContext->RSSetState( _rasterizerState );
	return true;
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

bool DirectX::CreateDefaultDepthStencilView(const Math::Size<int>& winSize)
{
	D3D11_TEXTURE2D_DESC descDepth;
	memset(&descDepth, 0, sizeof(D3D11_TEXTURE2D_DESC));
	descDepth.Width = winSize.w;
	descDepth.Height = winSize.h;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;

	HRESULT hr = _device->CreateTexture2D( &descDepth, nullptr, &_depthStencil );
	if( FAILED( hr ) )
		return false;

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	memset(&descDSV, 0, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

	switch( descDepth.Format )
	{
	case DXGI_FORMAT_R8_TYPELESS:
		descDSV.Format = DXGI_FORMAT_R8_UNORM;
		break;
	case DXGI_FORMAT_R16_TYPELESS:
		descDSV.Format = DXGI_FORMAT_D16_UNORM;
		break;
	case DXGI_FORMAT_R32_TYPELESS:
		descDSV.Format = DXGI_FORMAT_D32_FLOAT;
		break;
	case DXGI_FORMAT_R24G8_TYPELESS:
		descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		break;
	default:
		descDSV.Format = descDepth.Format;
	}

	descDSV.ViewDimension = _multiSample > 1 ? 
D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;

	hr = _device->CreateDepthStencilView( _depthStencil, &descDSV, &_depthStencilView );
	if( FAILED(hr) )
		return false;

	//D3D11_DEPTH_STENCIL_DESC dsState;
	//memset(&dsState, 0, sizeof(D3D11_DEPTH_STENCIL_DESC));
	//dsState.DepthEnable = true;
	//dsState.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	//dsState.DepthFunc = D3D11_COMPARISON_LESS;
	//dsState.StencilEnable = false;

	//ID3D11DepthStencilState* depthStencilState = nullptr;
	//if( FAILED(_device->CreateDepthStencilState( &dsState, &depthStencilState)) )
	//	return false;

	//_immediateContext->OMSetDepthStencilState(depthStencilState, 0);

	return true;
}

bool DirectX::CreateDeviceAndSwapChain(const Win32* win)
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
	//sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	//sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	//msaa
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;


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

void DirectX::CalcMaxMultiSampler(unsigned int& outAbleMultiSample, unsigned int& outQualityLevel, DXGI_FORMAT format)
{
	unsigned int qualityLevel = 0;
	unsigned int ableMultiSample = D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT;
	for(ableMultiSample; ableMultiSample >= 0; ableMultiSample/=2)
	{
		HRESULT hr = _device->CheckMultisampleQualityLevels( format, ableMultiSample, &qualityLevel);

		if( SUCCEEDED(hr) )
			break;
	}

	outAbleMultiSample = ableMultiSample;
	outQualityLevel = qualityLevel;
}

bool DirectX::InitDevice(const Win32* win)
{
	if( CreateDeviceAndSwapChain(win) == false )
		return false;

	if( CreateRenderTargetView() == false )
		return false;

	if( CreateDefaultDepthStencilView(win->GetSize()) == false)
		return false;

	_immediateContext->OMSetRenderTargets(1, &_renderTargetView, _depthStencilView);
	_immediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	CreateViewport(win->GetSize());

	if( CreateRasterize() == false )
		return false;

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