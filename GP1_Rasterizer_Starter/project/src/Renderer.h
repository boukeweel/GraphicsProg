#pragma once

#include <cstdint>
#include <iostream>
#include <vector>

#include "Camera.h"
#include "DataTypes.h"

struct SDL_Window;
struct SDL_Surface;

namespace dae
{
	class Texture;
	struct Mesh;
	struct Vertex;
	class Timer;
	class Scene;

	

	class Renderer final
	{
	public:
		Renderer(SDL_Window* pWindow);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void Update(Timer* pTimer);
		void Render();

		bool SaveBufferToImage() const;

		void ToggleToDepthBuffer()
		{
			m_DepthToggle = !m_DepthToggle;
			if (m_DepthToggle)
				std::cout << "Depth On\n";
			else
				std::cout << "Depth Off\n";
		}
		void ToggleRotation()
		{
			m_ShouldRotated = !m_ShouldRotated;
			if (m_ShouldRotated)
				std::cout << "Rotating Object\n";
			else
				std::cout << "Stopped Rotating\n";
		}
		void ToggleNormalMapping()
		{
			m_NormalMapActive = !m_NormalMapActive;
			if (m_NormalMapActive)
				std::cout << "Normals map on\n";
			else
				std::cout << "Normals map off\n";
		}
		void CycleLightingMode();

	private:
		void InitializeSpaceBike();

		void RasterizeMesh(); 

		void VertexTransformationFunction(Mesh& mesh) const;

		void TriangleStrip(const Mesh& mesh);
		void TriangleList(const Mesh& mesh);
		void Rasterize(const Mesh& mesh, const size_t v0, const size_t v1, const size_t v2);

		void PixelShading(const Material* pMaterial, const int pixelIndex, const Vertex_Out& vertex_out) const;

		float Remap(float value, float fromMin, float fromMax, float toMin = 0.0f, float toMax = 1.0f);

		SDL_Window* m_pWindow{};

		SDL_Surface* m_pFrontBuffer{ nullptr };
		SDL_Surface* m_pBackBuffer{ nullptr };
		uint32_t* m_pBackBufferPixels{};

		float* m_pDepthBufferPixels{};

		Matrix m_WorldSpace{};

		std::vector<Mesh> m_Meshes;
		Texture* m_pTexture;

		Camera m_Camera{};

		int m_Width{};
		int m_Height{};

		Vector3 m_LightDirection{ .577f,-.577f,.577f };
		float m_Shininess{ 25.f };
		float m_PhongSpecular{ 0.5f };

		float m_CurrentRotation{ 0.f };

		//toggles
		enum class LightingMode
		{
			ObservedArea,	//Lambert cosine law
			Diffuse,		//Incident radiance
			Specular,		//Scattering of the light
			Combined		//ObservedArea*Radiance*BRDF
		};

		LightingMode m_CurrentLightingMode{ LightingMode::Combined };
		bool m_ShouldRotated{ true };
		bool m_NormalMapActive{ true };
		bool m_DepthToggle{false};
	};
}
