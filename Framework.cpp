#include "framework.h"
#include "KeyInput.h"
#include "scene.h"

Framework* Framework::inst = nullptr;

Framework::~Framework()
{
	/*GetDevice()->Release();
	GetImmediateContext()->Release();
	GetRenderTargetView()->Release();
	GetDepthStencilView()->Release();
	GetRasterizerState0()->Release();
	GetRasterizerState1()->Release();
	GetSamplerState()->Release();*/

	//device->Release();
	//immediateContext->Release();
	//swapChain->Release();
	//renderTargetView->Release();
	//depthStencilView->Release();
	//for (int i = 0; i < 2; i++)
	//{
	//	rasterizerStates[i]->Release();
	//}
	//depthStencilState->Release();
	//noneDepthStencilState->Release();
	//samplerState->Release();
	//samplerCrampState->Release();


	SceneManager::Instance().Finalize();
}

bool Framework::Initialize()
{
	soundManager = std::make_unique<SoundManager>(hwnd);

	HRESULT hr = S_OK;

	RECT rc;
	GetClientRect(hwnd, &rc);
	//UINT screen_width = rc.right - rc.left;
	//UINT screen_height = rc.bottom - rc.top;
	screenWidth = rc.right - rc.left;
	screenHeight = rc.bottom - rc.top;


	// Create Device
	{
		UINT createDeviceFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
		D3D_DRIVER_TYPE driverTypes[] =
		{
			D3D_DRIVER_TYPE_UNKNOWN,
			D3D_DRIVER_TYPE_HARDWARE,
			D3D_DRIVER_TYPE_WARP,
			D3D_DRIVER_TYPE_REFERENCE,
		};
		D3D_FEATURE_LEVEL featureLevels[] =
		{
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_1,
		};
		D3D_FEATURE_LEVEL featureLevel;
		for (D3D_DRIVER_TYPE driverType : driverTypes)
		{
			hr = D3D11CreateDevice(nullptr, driverType, nullptr, createDeviceFlags, featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, device.GetAddressOf(), &featureLevel, immediateContext.GetAddressOf());
			if (SUCCEEDED(hr))
			{
				break;
			}
		}
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		//_ASSERT_EXPR(!(feature_level < D3D_FEATURE_LEVEL_11_0), L"Direct3D Feature Level 11 unsupported.");
	}
	// Create Swap Chain
	BOOL enable_4x_msaa = TRUE;
	{
		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		swapChainDesc.BufferDesc.Width = screenWidth;
		swapChainDesc.BufferDesc.Height = screenHeight;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		UINT msaa_quality_level;
		device->CheckMultisampleQualityLevels(swapChainDesc.BufferDesc.Format, 4, &msaa_quality_level);
		swapChainDesc.SampleDesc.Count = enable_4x_msaa ? 4 : 1;
		swapChainDesc.SampleDesc.Quality = enable_4x_msaa ? msaa_quality_level - 1 : 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 1;
		swapChainDesc.OutputWindow = hwnd;
		swapChainDesc.Windowed = TRUE;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags = 0;
#if 0
		Microsoft::WRL::ComPtr<IDXGIFactory> dxgiFactory;
		CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(dxgiFactory.GetAddressOf()));
#else
		Microsoft::WRL::ComPtr<IDXGIFactory1> dxgiFactory;
		Microsoft::WRL::ComPtr<IDXGIDevice> dxgi_device;

		HRESULT hr = device.Get()->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(dxgi_device.GetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		Microsoft::WRL::ComPtr<IDXGIAdapter> adapter;
		hr = dxgi_device->GetAdapter(adapter.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		hr = adapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(dxgiFactory.GetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
#endif
		hr = dxgiFactory->CreateSwapChain(device.Get(), &swapChainDesc, swapChain.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	// Create Render Target View
	D3D11_TEXTURE2D_DESC backBufferDesc;
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
		hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		hr = device->CreateRenderTargetView(backBuffer.Get(), NULL, renderTargetView.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		backBuffer->GetDesc(&backBufferDesc);
	}
	// create rasterizer state : solid mode
	{
		D3D11_RASTERIZER_DESC rasterizerDesc = {};
		rasterizerDesc.FillMode = D3D11_FILL_SOLID; //D3D11_FILL_WIREFRAME, D3D11_FILL_SOLID
		rasterizerDesc.CullMode = D3D11_CULL_BACK; //D3D11_CULL_NONE, D3D11_CULL_FRONT, D3D11_CULL_BACK   
		rasterizerDesc.FrontCounterClockwise = FALSE;
		rasterizerDesc.DepthBias = 0;
		rasterizerDesc.DepthBiasClamp = 0;
		rasterizerDesc.SlopeScaledDepthBias = 0;
		rasterizerDesc.DepthClipEnable = TRUE;
		rasterizerDesc.ScissorEnable = FALSE;
		rasterizerDesc.MultisampleEnable = FALSE;
		rasterizerDesc.AntialiasedLineEnable = FALSE;
		hr = device->CreateRasterizerState(&rasterizerDesc, rasterizerStates[0].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	// create rasterizer state : wireframe mode
	{
		D3D11_RASTERIZER_DESC rasterizerDesc = {};
		rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME; //D3D11_FILL_WIREFRAME, D3D11_FILL_SOLID
		rasterizerDesc.CullMode = D3D11_CULL_BACK; //D3D11_CULL_NONE, D3D11_CULL_FRONT, D3D11_CULL_BACK   
		rasterizerDesc.FrontCounterClockwise = FALSE;
		rasterizerDesc.DepthBias = 0;
		rasterizerDesc.DepthBiasClamp = 0;
		rasterizerDesc.SlopeScaledDepthBias = 0;
		rasterizerDesc.DepthClipEnable = TRUE;
		rasterizerDesc.ScissorEnable = FALSE;
		rasterizerDesc.MultisampleEnable = FALSE;
		rasterizerDesc.AntialiasedLineEnable = FALSE;
		hr = device->CreateRasterizerState(&rasterizerDesc, rasterizerStates[1].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	// create depth stencil state
	{
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
		depthStencilDesc.DepthEnable = TRUE;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		//depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
		depthStencilDesc.StencilEnable = FALSE;
		depthStencilDesc.StencilReadMask = 0xFF;
		depthStencilDesc.StencilWriteMask = 0xFF;
		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		hr = device->CreateDepthStencilState(&depthStencilDesc, depthStencilState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		
	}
	// create none depth stencil state
	{
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
		depthStencilDesc.DepthEnable = FALSE;
		//depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
		depthStencilDesc.StencilEnable = FALSE;
		depthStencilDesc.StencilReadMask = 0xFF;
		depthStencilDesc.StencilWriteMask = 0xFF;
		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		hr = device->CreateDepthStencilState(&depthStencilDesc, noneDepthStencilState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	{
		// create sampler state
		D3D11_SAMPLER_DESC samplerDesc = {};
		samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MipLODBias = 0;
		samplerDesc.MaxAnisotropy = 16;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		memcpy(samplerDesc.BorderColor, &DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f), sizeof(DirectX::XMFLOAT4));
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		hr = device->CreateSamplerState(&samplerDesc, samplerState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	{
		// create sampler cramp state
		D3D11_SAMPLER_DESC samplerDescCramp = {};
		samplerDescCramp.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDescCramp.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDescCramp.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDescCramp.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDescCramp.MipLODBias = 0;
		samplerDescCramp.MaxAnisotropy = 16;
		samplerDescCramp.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		memcpy(samplerDescCramp.BorderColor, &DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f), sizeof(DirectX::XMFLOAT4));
		samplerDescCramp.MinLOD = 0;
		samplerDescCramp.MaxLOD = D3D11_FLOAT32_MAX;
		hr = device->CreateSamplerState(&samplerDescCramp, samplerCrampState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	// Create Depth Stencil View
	D3D11_TEXTURE2D_DESC depthStencilBufferDesc = backBufferDesc;
	//Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
	ID3D11Texture2D* depthStencilBuffer{};
	{
		depthStencilBufferDesc.MipLevels = 1;
		depthStencilBufferDesc.ArraySize = 1;
		depthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	//DXGI_FORMAT_D32_FLOAT
		depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilBufferDesc.CPUAccessFlags = 0;
		depthStencilBufferDesc.MiscFlags = 0;
		hr = device->CreateTexture2D(&depthStencilBufferDesc, NULL, &depthStencilBuffer);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		depthStencilViewDesc.Format = depthStencilBufferDesc.Format;
		depthStencilViewDesc.ViewDimension = enable_4x_msaa ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Flags = 0;
		depthStencilViewDesc.Texture2D.MipSlice = 0;
		hr = device->CreateDepthStencilView(depthStencilBuffer, &depthStencilViewDesc, depthStencilView.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = static_cast<FLOAT>(screenWidth);
	viewport.Height = static_cast<FLOAT>(screenHeight);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	immediateContext->RSSetViewports(1, &viewport);

	font = std::make_unique<Sprite>(device.Get(), L"./fonts/font4.png");
	blend = std::make_unique<Blender>(device.Get());

	// タイトルシーンへ
	//SceneManager::Instance().ChangeScene(new SceneTitle());
	SceneManager::Instance().ChangeScene(new SceneTitle());

	depthStencilBuffer->Release();
	return true;
}
void Framework::Update(float elapsedTime/*Elapsed seconds from last frame*/)
{
	// 入力更新処理
	input.Update();

	// キー情報更新
	KeyInput::Update();

	// シーン更新
	SceneManager::Instance().Update(elapsedTime);

}
void Framework::Render(float elapsedTime/*Elapsed seconds from last frame*/)
{
	//immediateContext->OMSetDepthStencilState(depthStencilState.Get(), 1);
	immediateContext->RSSetState(rasterizerStates[0].Get());
	//immediateContext->PSSetSamplers(0, 1, samplerState.GetAddressOf());
	// シーン描画
	SceneManager::Instance().Render();

	// 画面のフリップ
	swapChain->Present(1, 0);
}

void Framework::SetViewPort(float width, float height)
{
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)width;
	vp.Height = (FLOAT)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	immediateContext->RSSetViewports(1, &vp);
}

void Framework::SetSampler(int slot)
{
	immediateContext->PSSetSamplers(slot, 1, samplerState.GetAddressOf());
}

void Framework::SetSamplerCramp(int slot)
{
	immediateContext->PSSetSamplers(slot, 1, samplerCrampState.GetAddressOf());
}

void Framework::Clear(float color[4])
{
	immediateContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());

	immediateContext->ClearRenderTargetView(renderTargetView.Get(), color);
	immediateContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	immediateContext->OMSetDepthStencilState(depthStencilState.Get(), 1);
}

