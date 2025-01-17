#pragma once


namespace dae
{
	class MeshDirectX;
	class MeshSoftware;
}

struct SDL_Window;
struct SDL_Surface;

namespace dae
{
	class Camera;
	class EffectBase;
	class EffectOpaque;
	class EffectPartialCoverage;

	class Renderer final
	{
	public:
		Renderer(SDL_Window* pWindow);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void Update(const Timer* pTimer);
		void Render() const;


		//toggles
		//both
		void ToggleRotation();
		void ToggleUseDirectX();
		void CycleCullMode();
		void ToggleUniformClearColor();
		void ToggleNormalMap();

		//hardware
		void ToggleFireMesh();
		void CycleTextureSampling();
		
		//software
		void CycleShadingMode();
		void ToggleDepthBuffer();
		void ToggleBoundingBox();

	private:
		void InitializeBikeDirectX();
		void InitializeBikeSoftware();

		void RenderDirectX() const;
		void RenderSoftware() const;

		SDL_Window* m_pWindow{};
		Camera* m_pCamera{};

		int m_Width{};
		int m_Height{};

		bool m_IsInitialized{ false };
		

		//toggels
		bool m_ShouldRotated{true};
		bool m_UseDirectX{ true };
		bool m_RenderFire{ true };
		bool m_UseNormalMap{ true };
		bool m_DepthBuffer{ false };
		bool m_ShowBoundingBox{ false };

		int m_TextureSampling = 0;

		//Software
		SDL_Surface* m_pFrontBuffer{ nullptr };
		SDL_Surface* m_pBackBuffer{ nullptr };
		uint32_t* m_pBackBufferPixels{};

		float* m_pDepthBufferPixels{};

		std::vector<MeshSoftware*> m_pSoftwareMeshes;

		//DIRECTX
		HRESULT InitializeDirectX();
		//...
		ID3D11Device* m_pDevice{};
		ID3D11DeviceContext* m_pDeviceContext{};
		IDXGISwapChain* m_pSwapChain{};
		ID3D11Texture2D* m_pDepthStencilBuffer{};
		ID3D11DepthStencilView* m_pDepthStencilView{};

		ID3D11Resource* m_pRenderTargetBuffer{};
		ID3D11RenderTargetView* m_pRenderTargetView{};

		std::vector<MeshDirectX*> m_pDirectXMeshes;
		MeshDirectX* m_pFireMesh;

		EffectBase* m_pEffect{};
		EffectOpaque* m_pEffectOpaque{};
		EffectPartialCoverage* m_pEffectPartialCoverage{};
	};
}
