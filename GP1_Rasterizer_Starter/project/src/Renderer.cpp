//External includes
#include "SDL.h"
#include "SDL_surface.h"

//Project includes
#include "Renderer.h"

#include <iostream>

#include "Maths.h"
#include "Texture.h"
#include "Utils.h"

using namespace dae;

Renderer::Renderer(SDL_Window* pWindow) :
	m_pWindow(pWindow)
{
	//Initialize
	SDL_GetWindowSize(pWindow, &m_Width, &m_Height);

	//Create Buffers
	m_pFrontBuffer = SDL_GetWindowSurface(pWindow);
	m_pBackBuffer = SDL_CreateRGBSurface(0, m_Width, m_Height, 32, 0, 0, 0, 0);
	m_pBackBufferPixels = (uint32_t*)m_pBackBuffer->pixels;

	//m_pDepthBufferPixels = new float[m_Width * m_Height];

	//Initialize Camera
	m_Camera.Initialize(60.f, { .0f,.0f,-10.f });
}

Renderer::~Renderer()
{
	//delete[] m_pDepthBufferPixels;
}


void Renderer::Update(Timer* pTimer)
{
	m_Camera.Update(pTimer);
}

void Renderer::Render()
{
	SDL_LockSurface(m_pBackBuffer);

	// Clear screen buffer
	SDL_FillRect(m_pBackBuffer, nullptr, SDL_MapRGB(m_pBackBuffer->format, 0, 0, 0));


	SceneWhiteTriangle();


	//@END
	//Update SDL Surface
	SDL_UnlockSurface(m_pBackBuffer);
	SDL_BlitSurface(m_pBackBuffer, 0, m_pFrontBuffer, 0);
	SDL_UpdateWindowSurface(m_pWindow);

	
}

void Renderer::VertexTransformationFunction(const std::vector<Vertex>& vertices_in, std::vector<Vertex>& vertices_out) const
{
	vertices_out.clear();
	vertices_out.reserve(vertices_in.size());

	for (const Vertex& vertex : vertices_in)
	{
		// Transform the vertex position using the view matrix
		Vector3 viewPosition{ m_Camera.viewMatrix.TransformPoint(vertex.position).Normalized() };

		const float aspectRatio = static_cast<float>(m_Width) / static_cast<float>(m_Height);
		const float FOV = -1.f / tan(m_Camera.fovAngle / 2);

		Vector3 PojectedVertex{
			(viewPosition.x / viewPosition.z) / (aspectRatio * FOV),
			(viewPosition.y / viewPosition.z ) / FOV,
			viewPosition.z
		};

		Vertex transformedVertex = vertex;
		transformedVertex.position.x = (PojectedVertex.x + 1.0f) / 2.0f * m_Width;
		transformedVertex.position.y = (1.0f - (PojectedVertex.y + 1.0f) / 2.0f) * m_Height;
		transformedVertex.position.z = PojectedVertex.z; // Keep Z for depth buffering

		// Add the transformed vertex to the output vector
		vertices_out.emplace_back(transformedVertex);
	}
}

void Renderer::SceneWhiteTriangle()
{
	std::vector<Vertex> vertices_ndc = {
		{{ 0.f,  .5f, 1.f }},
		{{.5f, -.5f, 1.f }},
		{{-.5f, -.5f, 1.f }}
	};

	std::vector<Vertex> vertices_Screen;
	VertexTransformationFunction(vertices_ndc, vertices_Screen);

	Vector2 V0 = { vertices_Screen[0].position.x, vertices_Screen[0].position.y };
	Vector2 V1 = { vertices_Screen[1].position.x, vertices_Screen[1].position.y };
	Vector2 V2 = { vertices_Screen[2].position.x, vertices_Screen[2].position.y };

	for (int py = 0; py < m_Height; ++py)
	{
		for (int px = 0; px < m_Width; ++px)
		{
			// Calculate the pixel center point in screen space
			Vector2 P(px + 0.5f, py + 0.5f);

			// Edge vectors
			Vector2 edge0 = V1 - V0;
			Vector2 edge1 = V2 - V1;
			Vector2 edge2 = V0 - V2;

			// Vectors from vertices to the pixel center
			Vector2 vector0 = P - V0;
			Vector2 vector1 = P - V1;
			Vector2 vector2 = P - V2;

			// Calculate signed areas (cross products)
			float cross0 = Vector2::Cross(edge0, vector0);
			float cross1 = Vector2::Cross(edge1, vector1);
			float cross2 = Vector2::Cross(edge2, vector2);

			// Check if all cross products have the same sign
			bool inside = (cross0 >= 0 && cross1 >= 0 && cross2 >= 0) || (cross0 <= 0 && cross1 <= 0 && cross2 <= 0);

			if (inside)
			{
				// Inside the triangle, color the pixel white
				ColorRGB color{ 1.0f, 1.0f, 1.0f };

				// Map color to buffer
				m_pBackBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
					static_cast<uint8_t>(color.r * 255),
					static_cast<uint8_t>(color.g * 255),
					static_cast<uint8_t>(color.b * 255));
			}
		}
	}
}

bool Renderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBackBuffer, "Rasterizer_ColorBuffer.bmp");
}
