#include "DirectX.h"
#include "Utility.h"

using namespace Device;

DirectX::DirectX(void) :
	_device(nullptr), _swapChain(nullptr), _immediateContext(nullptr),
	_renderTargetView(nullptr), _disableCulling(nullptr),
	_opaqueBlend(nullptr), _alphaToCoverageBlend(nullptr), _defaultCulling(nullptr),
	_depthDisableDepthTest(nullptr), _depthLess(nullptr), 
	_depthEqualAndDisableDepthWrite(nullptr), _depthGreater(nullptr),
	_depthGreaterAndDisableDepthWrite(nullptr),  _opaqueBlendDepthOnly(nullptr), _alphaBlend(nullptr), _useMSAA(false),
	_anisotropicSamplerState(nullptr), _linearSamplerState(nullptr), _pointSamplerState(nullptr)
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
		sd.SampleDesc = (*multiSampler);
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

	_useMSAA = sd.SampleDesc.Count > 1;

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

void DirectX::CreateBlendStates(bool isDeferredRender)
{
	D3D11_BLEND_DESC desc;
	memset(&desc, 0, sizeof(D3D11_BLEND_DESC));

	desc.AlphaToCoverageEnable					= false;
	desc.IndependentBlendEnable					= false;

	D3D11_RENDER_TARGET_BLEND_DESC renderTargetBlendDesc;

	renderTargetBlendDesc.BlendEnable			= false;

	renderTargetBlendDesc.BlendOp				= D3D11_BLEND_OP_ADD;
	renderTargetBlendDesc.BlendOpAlpha			= D3D11_BLEND_OP_ADD;

	renderTargetBlendDesc.SrcBlend				= D3D11_BLEND_ONE; 
	renderTargetBlendDesc.SrcBlendAlpha			= D3D11_BLEND_ONE; 

	renderTargetBlendDesc.DestBlend				= D3D11_BLEND_ZERO; 
	renderTargetBlendDesc.DestBlendAlpha		= D3D11_BLEND_ZERO; 

	renderTargetBlendDesc.RenderTargetWriteMask	= D3D11_COLOR_WRITE_ENABLE_ALL;

	desc.RenderTarget[0] = renderTargetBlendDesc;

	if(isDeferredRender)
	{
		desc.RenderTarget[1] = renderTargetBlendDesc;	// specular_fresnel0
		desc.RenderTarget[2] = renderTargetBlendDesc;	// normal_roughness
	}

	if( FAILED(_device->CreateBlendState(&desc, &_opaqueBlend)) )
		ASSERT_MSG("Error!, device cant create opaque blend state");


	renderTargetBlendDesc.RenderTargetWriteMask	= 0;
	if( FAILED(_device->CreateBlendState(&desc, &_opaqueBlendDepthOnly)) )
		ASSERT_MSG("Error!, device cant create _opaqueBlendDepthOnly state");

	desc.AlphaToCoverageEnable = true;
	if( FAILED(_device->CreateBlendState(&desc, &_alphaToCoverageBlend)) )
		ASSERT_MSG("Error!, device cant create alphaToCoverage blend state");

	renderTargetBlendDesc.BlendEnable			= true;

	renderTargetBlendDesc.DestBlend				= D3D11_BLEND_INV_SRC_ALPHA;
	renderTargetBlendDesc.DestBlendAlpha		= D3D11_BLEND_INV_SRC_ALPHA;

	renderTargetBlendDesc.SrcBlend				= D3D11_BLEND_SRC_ALPHA;
	renderTargetBlendDesc.SrcBlendAlpha			= D3D11_BLEND_SRC_ALPHA;

	renderTargetBlendDesc.RenderTargetWriteMask	= D3D11_COLOR_WRITE_ENABLE_ALL;

	desc.RenderTarget[0] = renderTargetBlendDesc;

	if(isDeferredRender)
	{
		desc.RenderTarget[1] = renderTargetBlendDesc;	// specular_fresnel0
		desc.RenderTarget[2] = renderTargetBlendDesc;	// normal_roughness
	}

	if( FAILED(_device->CreateBlendState(&desc, &_alphaBlend)) )
		ASSERT_MSG("Error!, device cant create _alphaBlend blend state");
}

bool DirectX::InitDevice(const Win32* win, bool isDeferredRender)
{
	if( CreateDeviceAndSwapChain(win) == false )
		return false;

	if( CreateRenderTargetView() == false )
		return false;

	_immediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	CreateViewport(win->GetSize());

	//Create rasterizer State
	{
		D3D11_RASTERIZER_DESC desc;
		desc.FillMode				= D3D11_FILL_SOLID;
		desc.CullMode				= D3D11_CULL_BACK;
		desc.FrontCounterClockwise	= true;
		desc.DepthBias				= 0;
		desc.DepthBiasClamp			= 0.0f;
		desc.SlopeScaledDepthBias	= 0.0f;
		desc.DepthClipEnable		= true;
		desc.ScissorEnable			= false;
		desc.MultisampleEnable		= false;
		desc.AntialiasedLineEnable	= false;

		if( FAILED(_device->CreateRasterizerState(&desc, &_defaultCulling)) )
			ASSERT_MSG("Error!, device cant create rasterizer state");

		desc.CullMode				= D3D11_CULL_NONE;		//ÄÃ¸µ ²û
		if( FAILED(_device->CreateRasterizerState(&desc, &_disableCulling)) )
			ASSERT_MSG("Error!, device cant create rasterizer state");
	}
	
	//Create Blend State
	CreateBlendStates(isDeferredRender);

	//Create Depth State
	//using inverted 32bit float depth
	{
		D3D11_DEPTH_STENCIL_DESC desc;
		desc.DepthEnable		= true; 
		desc.DepthWriteMask		= D3D11_DEPTH_WRITE_MASK_ALL; 
		desc.DepthFunc			= D3D11_COMPARISON_GREATER; //invvvvvverrreeettted
		desc.StencilEnable		= false; 
		desc.StencilReadMask	= D3D11_DEFAULT_STENCIL_READ_MASK; 
		desc.StencilWriteMask	= D3D11_DEFAULT_STENCIL_WRITE_MASK; 
		if( FAILED(_device->CreateDepthStencilState( &desc, &_depthGreater)) )
			ASSERT_MSG("Error!, device cant create lessEqual dpeth state");

		//disable depth test write
		desc.DepthWriteMask		= D3D11_DEPTH_WRITE_MASK_ZERO; 
		
		//disable depth test
		desc.DepthEnable = false;
		if( FAILED(_device->CreateDepthStencilState( &desc, &_depthDisableDepthTest)) )
			ASSERT_MSG("Error!, device cant create _depthDisableDepthTest");

		desc.DepthEnable = true;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		desc.DepthFunc = D3D11_COMPARISON_GREATER; //inverted
		if( FAILED(_device->CreateDepthStencilState( &desc, &_depthGreaterAndDisableDepthWrite)) )
			ASSERT_MSG("Error!, device cant create _depthGreaterAndDisableDepthWrite");

		desc.DepthFunc = D3D11_COMPARISON_EQUAL;
		if( FAILED(_device->CreateDepthStencilState( &desc, &_depthEqualAndDisableDepthWrite)) )
			ASSERT_MSG("Error!, device cant create _depthEqualAndDisableDepthWrite");

		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D11_COMPARISON_LESS;
		if( FAILED(_device->CreateDepthStencilState( &desc, &_depthLess)) )
			ASSERT_MSG("Error!, device cant create _depthEqualAndDisableDepthWrite");
	}

	//sampler
	{
		D3D11_SAMPLER_DESC sampDesc;
		ZeroMemory( &sampDesc, sizeof(sampDesc) );
		sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.MaxAnisotropy = 16;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

		HRESULT hr = _device ->CreateSamplerState( &sampDesc, &_anisotropicSamplerState );
		ASSERT_COND_MSG(SUCCEEDED(hr), "Error!, device cant create sampler state");

		sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		hr = _device ->CreateSamplerState( &sampDesc, &_linearSamplerState );
		ASSERT_COND_MSG(SUCCEEDED(hr), "Error!, device cant create sampler state");

		sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		hr = _device ->CreateSamplerState( &sampDesc, &_pointSamplerState );
		ASSERT_COND_MSG(SUCCEEDED(hr), "Error!, device cant create sampler state");
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

void DirectX::ClearDeviceContext()
{
    ID3D11ShaderResourceView* pSRVs[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
    ID3D11RenderTargetView* pRTVs[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
    ID3D11DepthStencilView* pDSV = NULL;
    ID3D11Buffer* pBuffers[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
    ID3D11SamplerState* pSamplers[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

    // Constant buffers
    _immediateContext->VSSetConstantBuffers( 0, 14, pBuffers );
    _immediateContext->PSSetConstantBuffers( 0, 14, pBuffers );
    _immediateContext->CSSetConstantBuffers( 0, 14, pBuffers );

    // Resources
    _immediateContext->VSSetShaderResources( 0, 16, pSRVs );
    _immediateContext->PSSetShaderResources( 0, 16, pSRVs );
    _immediateContext->CSSetShaderResources( 0, 16, pSRVs );

    // Samplers
    _immediateContext->VSSetSamplers( 0, 16, pSamplers );
    _immediateContext->PSSetSamplers( 0, 16, pSamplers );
    _immediateContext->CSSetSamplers( 0, 16, pSamplers );

    // Render targets
    _immediateContext->OMSetRenderTargets( 8, pRTVs, pDSV );

    // States
    FLOAT BlendFactor[4] = { 0,0,0,0 };
    _immediateContext->OMSetBlendState( NULL, BlendFactor, 0xFFFFFFFF );
	_immediateContext->OMSetDepthStencilState( _depthGreater, 0x00 );  // we are using inverted 32-bit float depth for better precision
    _immediateContext->RSSetState( NULL );
}