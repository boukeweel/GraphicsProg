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
	m_Camera.Initialize(60.f, { .0f,.0f,-25.f });
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


	SceneWeek1();


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

void Renderer::SceneWeek1()
{
	std::vector<Vertex> vertices_ndc = {
		{{ 0.f,  4.f, 2.f },{1,0,0}},
		{{3.f, -2.f, 2.f },{0,1,0}},
		{{-3.f, -2.f, 2.f },{0,0,1}}
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

			// Calculate sub-areas for barycentric coordinates
			float W0 = Vector2::Cross(V2 - V1, P - V1) / 2; // Opposite V0
			float W1 = Vector2::Cross(V0 - V2, P - V2) / 2; // Opposite V1
			float W2 = Vector2::Cross(V1 - V0, P - V0) / 2; // Opposite V2

			if (W0 >= 0 && W1 >= 0 && W2 >= 0)
			{
				float totalArea = W0 + W1 + W2;

				float colorr = W0 / totalArea;
				float colorg = W1 / totalArea;
				float colorb = W2 / totalArea;

				ColorRGB vertexColor0 = vertices_Screen[0].color;
				ColorRGB vertexColor1 = vertices_Screen[1].color;
				ColorRGB vertexColor2 = vertices_Screen[2].color;

				ColorRGB color{
				colorr * vertexColor0.r + colorg * vertexColor1.r + colorb * vertexColor2.r,
				colorr* vertexColor0.g + colorg * vertexColor1.g + colorb * vertexColor2.g,
				colorr* vertexColor0.b + colorg * vertexColor1.b + colorb * vertexColor2.b };

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
