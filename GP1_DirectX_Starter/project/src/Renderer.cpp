#include "pch.h"
#include "Renderer.h"
#include "Camera.h"
#include "Texture.h"


namespace dae {

	Renderer::Renderer(SDL_Window* pWindow) :
		m_pWindow(pWindow)
	{
		//Initialize
		SDL_GetWindowSize(pWindow, &m_Width, &m_Height);

		//Initialize DirectX pipeline
		const HRESULT result = InitializeDirectX();
		if (result == S_OK)
		{
			m_IsInitialized = true;
			std::cout << "DirectX is initialized and ready!\n";
		}
		else
		{
			std::cout << "DirectX initialization failed!\n";
		}

		//InitializeTextureCubeMesh();
		InitializeBike();
	}

	void Renderer::InitializeTextureCubeMesh()
	{
		m_pEffect = new Effect{ m_pDevice,L"resources/PosCol3D.fx" };

		Material* pMaterial = new Material{
			Texture::LoadFromFile(m_pDevice,"resources/uv_grid_2.png")
		};

		m_pMesh = new Mesh{
		m_pDevice,m_pEffect,
		{
				{{-3.0f,  3.0f, -2.0f},{},{0.f,0.f}}, // V0
				{{ 0.0f,  3.0f, -2.0f},{},{.5f,0.f}}, // V1
				{{ 3.0f,  3.0f, -2.0f},{},{1.f,0.f}}, // V2
				{{-3.0f,  0.0f, -2.0f},{},{0.f,.5f}}, // V3
				{{ 0.0f,  0.0f, -2.0f},{},{.5f,.5f}}, // V4
				{{ 3.0f,  0.0f, -2.0f},{},{1.f,.5f}}, // V5
				{{-3.0f, -3.0f, -2.0f},{},{0.f,1.f}}, // V6
				{{ 0.0f, -3.0f, -2.0f},{},{.5f,1.f}}, // V7
				{{ 3.0f, -3.0f, -2.0f},{},{1.f,1.f}}, // V8
			},
			{
				3,0,4,
				0,1,4,
				4,1,5,
				1,2,5,
				6,3,7,
				3,4,7,
				7,4,5,
				7,5,8
			},pMaterial };


		m_pEffect->SetSampleState(0);

		m_pCamera = new Camera{ {0,0,-10.f},45.f,static_cast<float>(m_Width) / static_cast<float>(m_Height) };
	}

	void Renderer::InitializeBike()
	{
		m_pEffect = new Effect{ m_pDevice,L"resources/PosCol3D.fx" };

		Material* pMaterial = new Material{
			Texture::LoadFromFile(m_pDevice,"resources/vehicle_diffuse.png")
		};

		m_pMesh = new Mesh{ m_pDevice,m_pEffect,"resources/vehicle.obj",pMaterial };

		m_pEffect->SetSampleState(0);

		m_pCamera = new Camera{ {0,0,-50.f},45.f,static_cast<float>(m_Width) / static_cast<float>(m_Height) };
	}


	Renderer::~Renderer()
	{
		if(m_pDeviceContext)
		{
			m_pDeviceContext->ClearState();
			m_pDeviceContext->Flush();
			m_pDeviceContext->Release();
		}

		SafeRelease(m_pDepthStencilView)
		SafeRelease(m_pDepthStencilBuffer)
		SafeRelease(m_pRenderTargetView)
		SafeRelease(m_pRenderTargetBuffer)
		SafeRelease(m_pSwapChain)
		SafeRelease(m_pDeviceContext)
		SafeRelease(m_pDevice)

		delete m_pEffect;
		m_pEffect = nullptr;

		delete m_pMesh;
		m_pMesh = nullptr;

		delete m_pCamera;
		m_pCamera = nullptr;
	}

	void Renderer::Update(const Timer* pTimer)
	{
		m_pCamera->Update(pTimer);


		m_pMesh->AddYawRotation(PI * 2 * pTimer->GetElapsed() * (45.f / 360.f));
	}


	void Renderer::Render() const
	{
		if (!m_IsInitialized)
			return;

		//Clear RTV & DSV
		constexpr float color[4] = { 0.f,0.f,0.3f,1.f };
		m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, color);
		m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_DEPTH, 1.f, 0.f);

		//Invoke Draw Calls
		m_pMesh->Render(m_pDeviceContext,m_pCamera->GetViewProjectionMatrix());


		//Present backbuffer(Swap)
		m_pSwapChain->Present(0, 0);
	}


	HRESULT Renderer::InitializeDirectX()
	{

		//Create device and deviceContext
		D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_1;
		uint32_t createDiviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
		createDiviceFlags != D3D11_CREATE_DEVICE_DEBUG;
#endif

		HRESULT result = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, createDiviceFlags, &featureLevel,
			1, D3D11_SDK_VERSION, &m_pDevice, nullptr, &m_pDeviceContext);

		if (FAILED(result))
			return result;

		//Create DXGI Factory
		IDXGIFactory1* pDxgiFactory{};
		result = CreateDXGIFactory1(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&pDxgiFactory));
		if (FAILED(result))
			return result;

		//Create SwapChain
		DXGI_SWAP_CHAIN_DESC swapChainDesc{};
		swapChainDesc.BufferDesc.Width = m_Width;
		swapChainDesc.BufferDesc.Height = m_Height;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 1;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 60;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 1;
		swapChainDesc.Windowed = true;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags = 0;

		//get the handle from the sdl backbuffer
		SDL_SysWMinfo sysWMInfo{};
		SDL_GetVersion(&sysWMInfo.version);
		SDL_GetWindowWMInfo(m_pWindow, &sysWMInfo);
		swapChainDesc.OutputWindow = sysWMInfo.info.win.window;

		result = pDxgiFactory->CreateSwapChain(m_pDevice, &swapChainDesc, &m_pSwapChain);
		if (FAILED(result))
			return result;

		D3D11_TEXTURE2D_DESC depthStencilDesc{};
		depthStencilDesc.Width = m_Width;
		depthStencilDesc.Height = m_Height;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
		depthStencilViewDesc.Format = depthStencilDesc.Format;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

		result = m_pDevice->CreateTexture2D(&depthStencilDesc, nullptr, &m_pDepthStencilBuffer);
		if (FAILED(result))
			return result;

		result = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilViewDesc, &m_pDepthStencilView);
		if (FAILED(result))
			return result;

		//create RenderTarget & RenderTargetView

		//resource
		result = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&m_pRenderTargetBuffer));
		if (FAILED(result))
			return result;

		//view
		result = m_pDevice->CreateRenderTargetView(m_pRenderTargetBuffer, nullptr, &m_pRenderTargetView);
		if (FAILED(result))
			return result;

		//bind RTV & DSB to output merger stage
		m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

		D3D11_VIEWPORT viewport{};
		viewport.Width = static_cast<float>(m_Width);
		viewport.Height = static_cast<float>(m_Height);
		viewport.TopLeftX = 0.f;
		viewport.TopLeftY = 0.f;
		viewport.MinDepth = 0.f;
		viewport.MaxDepth = 1.f;
		m_pDeviceContext->RSSetViewports(1, &viewport);

		//Realising The Factory after his long work day
		SafeRelease(pDxgiFactory)

		return result;
	}
}
