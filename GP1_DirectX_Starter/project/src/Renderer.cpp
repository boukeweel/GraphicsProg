#include "pch.h"
#include "Renderer.h"
#include "Camera.h"
#include "EffectOpaque.h"
#include "EffectBase.h"
#include "EffectPartialCoverage.h"
#include "Texture.h"
#include "MeshSoftware.h"
#include "MeshDirectX.h"

namespace dae {

	Renderer::Renderer(SDL_Window* pWindow) :
		m_pWindow(pWindow)
	{
		//Initialize
		SDL_GetWindowSize(pWindow, &m_Width, &m_Height);


		//initialize software pipeline
		m_pFrontBuffer = SDL_GetWindowSurface(pWindow);
		m_pBackBuffer = SDL_CreateRGBSurface(0, m_Width, m_Height, 32, 0, 0, 0, 0);
		m_pBackBufferPixels = static_cast<uint32_t*>(m_pBackBuffer->pixels);

		m_pDepthBufferPixels = new float[m_Width * m_Height];

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

		InitializeBikeDirectX();
		InitializeBikeSoftware();

		m_pCamera = new Camera{ {0,0,-50.f},45.f,static_cast<float>(m_Width) / static_cast<float>(m_Height) };

		if (m_UseDirectX)
			m_CurrentBackGroundColor = m_HardWareColor;
		else
			m_CurrentBackGroundColor = m_SoftWareColor;
	}

	void Renderer::InitializeBikeDirectX()
	{
		m_pEffectOpaque = new EffectOpaque{ m_pDevice,L"resources/OpaqueShader.fx" };

		m_pEffectOpaque->SetLightingDirection({ 0.577f,-0.577f,0.577f });
		m_pEffectOpaque->SetLightIntensity(7.f);
		m_pEffectOpaque->SetPhongSpecular(1.f);
		m_pEffectOpaque->SetPhongExponent(25.f);
		m_pEffectOpaque->SetAmbientColor({ 0.025f,0.025f,0.025f });
		m_pEffectOpaque->SetUseNormalMap(true);
		m_pEffectOpaque->SetCullMode(m_pDeviceContext, CullMode::None);

		Material* pMaterial = new Material{
			Texture::LoadFromFile(m_pDevice,"resources/vehicle_diffuse.png"),
			Texture::LoadFromFile(m_pDevice,"resources/vehicle_normal.png"),
			Texture::LoadFromFile(m_pDevice,"resources/vehicle_specular.png"),
			Texture::LoadFromFile(m_pDevice,"resources/vehicle_gloss.png")
		};

		MeshDirectX* pMesh = new MeshDirectX{ m_pDevice,m_pEffectOpaque,"resources/vehicle.obj",pMaterial };

		m_pDirectXMeshes.emplace_back(pMesh);

		m_pEffectPartialCoverage = new EffectPartialCoverage{ m_pDevice,L"resources/PartialCoverage.fx" };

		m_pEffectPartialCoverage->SetLightingDirection({ 0.577f,-0.577f,0.577f });
		m_pEffectPartialCoverage->SetCullMode(m_pDeviceContext, CullMode::None);

		pMaterial = new Material{
			Texture::LoadFromFile(m_pDevice,"resources/fireFX_diffuse.png"),
			nullptr,
			nullptr,
			nullptr,
		};

		m_pFireMesh = new MeshDirectX{ m_pDevice,m_pEffectPartialCoverage,"resources/fireFX.obj",pMaterial };

		CycleTextureSampling();
	}

	void Renderer::InitializeBikeSoftware()
	{
		Material* pMaterial = new Material{
			Texture::LoadFromFile("resources/vehicle_diffuse.png"),
			Texture::LoadFromFile("resources/vehicle_normal.png"),
			Texture::LoadFromFile("resources/vehicle_specular.png"),
			Texture::LoadFromFile("resources/vehicle_gloss.png")
		};

		MeshSoftware* mesh = new MeshSoftware("resources/vehicle.obj", pMaterial, m_pBackBuffer, m_pBackBufferPixels, m_pDepthBufferPixels, m_Width, m_Height);

		m_pSoftwareMeshes.emplace_back(mesh);
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

		delete m_pEffectOpaque;
		m_pEffectOpaque = nullptr;

		delete m_pEffectPartialCoverage;
		m_pEffectPartialCoverage = nullptr;

		for (MeshDirectX* mesh : m_pDirectXMeshes)
		{
			delete mesh;
			mesh = nullptr;
		}

		for (MeshSoftware* mesh : m_pSoftwareMeshes)
		{
			delete mesh;
			mesh = nullptr;
		}

		delete m_pCamera;
		m_pCamera = nullptr;

		delete[] m_pDepthBufferPixels;
	}

	void Renderer::Update(const Timer* pTimer)
	{
		m_pCamera->Update(pTimer);

		if(m_ShouldRotated)
		{
			float rotation{ PI * 2 * pTimer->GetElapsed() * (45.f / 360.f) };
			for (MeshDirectX* mesh : m_pDirectXMeshes)
			{
				mesh->AddYawRotation(rotation);
			}
			m_pFireMesh->AddYawRotation(rotation);


			for (MeshSoftware* mesh : m_pSoftwareMeshes)
			{
				mesh->AddYawRotation(rotation);
			}
		}
	}


	void Renderer::Render() const
	{
		if(m_UseDirectX)
		{
			RenderDirectX();
		}
		else
		{
			RenderSoftware();
		}
	}

	void Renderer::RenderDirectX() const
	{
		if (!m_IsInitialized)
			return;

		//Clear RTV & DSV
		const float color[4] = { m_CurrentBackGroundColor.r,m_CurrentBackGroundColor.g,m_CurrentBackGroundColor.b,1.f };
		m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, color);
		m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_DEPTH, 1.f, 0.f);

		m_pEffectOpaque->SetCamaraOrigin(m_pCamera->GetOrigin());
		m_pEffectPartialCoverage->SetCamaraOrigin(m_pCamera->GetOrigin());

		//Invoke Draw Calls
		for (MeshDirectX* mesh : m_pDirectXMeshes)
		{
			mesh->Render(m_pDeviceContext, m_pCamera->GetViewProjectionMatrix());
		}

		if(m_RenderFire)
			m_pFireMesh->Render(m_pDeviceContext, m_pCamera->GetViewProjectionMatrix());

		//Present backbuffer(Swap)
		m_pSwapChain->Present(0, 0);
	}

	void Renderer::RenderSoftware() const
	{
		SDL_LockSurface(m_pBackBuffer);

		//fill the whole depth buffer with max values so it can become smaller
		std::fill_n(m_pDepthBufferPixels, m_Width * m_Height, std::numeric_limits<float>::max());
		// Clear screen buffer
		SDL_FillRect(m_pBackBuffer, nullptr, SDL_MapRGB(m_pBackBuffer->format, m_CurrentBackGroundColor.r * 255, m_CurrentBackGroundColor.g * 255, m_CurrentBackGroundColor.b * 255));

		for (MeshSoftware* mesh : m_pSoftwareMeshes)
		{
			mesh->Render(m_pCamera);
		}

		//@END
		//Update SDL Surface
		SDL_UnlockSurface(m_pBackBuffer);
		SDL_BlitSurface(m_pBackBuffer, 0, m_pFrontBuffer, 0);
		SDL_UpdateWindowSurface(m_pWindow);
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


	//Toggels

	//both
	void Renderer::ToggleUseDirectX()
	{
		m_UseDirectX = !m_UseDirectX;

		std::cout << "Using DirectX: " << (m_UseDirectX ? "true" : "false") << "\n";

		if(!m_UseUniformColor)
		{
			if (m_UseDirectX)
				m_CurrentBackGroundColor = m_HardWareColor;
			else
				m_CurrentBackGroundColor = m_SoftWareColor;
		}
	}
	void Renderer::ToggleRotation()
	{
		m_ShouldRotated = !m_ShouldRotated;

		std::cout << "Rotating: " << (m_ShouldRotated ? "true" : "false") << "\n";
	}
	void Renderer::CycleCullMode()
	{
		switch (m_CullMode)
		{
		case CullMode::None:
			m_CullMode = CullMode::Front;
			std::wcout << L"Cull Mode: Front\n";
			break;
		case CullMode::Front:
			m_CullMode = CullMode::Back;
			std::wcout << L"Cull Mode: Back\n";
			break;
		case CullMode::Back:
			m_CullMode = CullMode::None;
			std::wcout << L"Cull Mode: None\n";
			break;
		}

		//You can technology only have to call one but this is cleaner 
		m_pEffectOpaque->SetCullMode(m_pDeviceContext, m_CullMode);
		m_pEffectPartialCoverage->SetCullMode(m_pDeviceContext, m_CullMode);

		for (MeshSoftware* mesh : m_pSoftwareMeshes)
		{
			mesh->SetCullMode(m_CullMode);
		}
	}
	void Renderer::ToggleUniformClearColor()
	{
		m_UseUniformColor = !m_UseUniformColor;

		if(m_UseUniformColor)
		{
			m_CurrentBackGroundColor = m_UniformColor;
		}else
		{
			if (m_UseDirectX)
				m_CurrentBackGroundColor = m_HardWareColor;
			else
				m_CurrentBackGroundColor = m_SoftWareColor;
		}
	}

	//hardware
	void Renderer::ToggleFireMesh()
	{
		m_RenderFire = !m_RenderFire;

		std::cout << "Rendering fire: " << (m_RenderFire ? "true" : "false") << "\n";
	}
	void Renderer::CycleTextureSampling()
	{
		switch (m_TextureSampling)
		{
		case 0:
			std::cout << "Sample State: ANISOTROPIC" << std::endl;
			m_pEffectOpaque->SetSampleState(0);
			m_pEffectPartialCoverage->SetSampleState(0);
			m_TextureSampling = 1;
			break;
		case 1:
			std::cout << "Sample State: POINT" << std::endl;
			m_pEffectOpaque->SetSampleState(1);
			m_pEffectPartialCoverage->SetSampleState(1);
			m_TextureSampling = 2;
			break;
		case 2:
			std::cout << "Sample State: LINEAR" << std::endl;
			m_pEffectOpaque->SetSampleState(2);
			m_pEffectPartialCoverage->SetSampleState(2);
			m_TextureSampling = 0;
			break;
		default:
			break;
		}
	}

	//software
	void Renderer::CycleShadingMode()
	{
		for (MeshSoftware* mesh : m_pSoftwareMeshes)
		{
			mesh->CycleShadingMode();
		}
	}
	void Renderer::ToggleDepthBuffer()
	{
		m_DepthBuffer = !m_DepthBuffer;

		std::cout << "DepthBuffer: " << (m_DepthBuffer ? "true" : "false") << "\n";

		for (MeshSoftware* mesh : m_pSoftwareMeshes)
		{
			mesh->ToggleToDepthBuffer();
		}
	}
	void Renderer::ToggleNormalMap()
	{
		m_UseNormalMap = !m_UseNormalMap;

		std::cout << "Using NormalMap: " << (m_UseNormalMap ? "true" : "false") << "\n";

		for (MeshSoftware* mesh : m_pSoftwareMeshes)
		{
			mesh->ToggleNormalMapping();
		}
	}
	void Renderer::ToggleBoundingBox()
	{
		m_ShowBoundingBox = !m_ShowBoundingBox;

		std::cout << "Show BoundingBox: " << (m_ShowBoundingBox ? "true" : "false") << "\n";

		for (MeshSoftware* mesh : m_pSoftwareMeshes)
		{
			mesh->ToggleShowBoundingBox();
		}
	}
}
