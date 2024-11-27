#pragma once

#include <cstdint>
#include <vector>

#include "Camera.h"

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

		void VertexTransformationFunction(const std::vector<Vertex>& vertices_in, std::vector<Vertex>& vertices_out) const;
		//void VertexTransformationFunction(Mesh& mesh);

	private:
		void InitializeWeek2();

		void SceneWeek1();
		void SceneWeek2();

		void TriangleSrip(const Mesh& mesh);
		void TriangleList(const Mesh& mesh);
		void Rasteriz(const Mesh& mesh, const size_t v0, const size_t v1, const size_t v2);

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
	};
}
