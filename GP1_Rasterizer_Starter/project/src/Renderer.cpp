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

	m_pDepthBufferPixels = new float[m_Width * m_Height];

	InitializeSpaceBike();
}

void Renderer::InitializeSpaceBike()
{
	m_Camera.Initialize(45.f, { .0f,.0f,.0f }, static_cast<float>(m_Width) / static_cast<float>(m_Height));

	Mesh SpaceBike{};
	Utils::ParseOBJ("resources/vehicle.obj", SpaceBike.vertices, SpaceBike.indices);

	SpaceBike.material.pDiffuse = Texture::LoadFromFile("resources/vehicle_diffuse.png");
	SpaceBike.material.pNormal = Texture::LoadFromFile("resources/vehicle_normal.png");
	SpaceBike.material.pSpecular = Texture::LoadFromFile("resources/vehicle_specular.png");
	SpaceBike.material.pGloss = Texture::LoadFromFile("resources/vehicle_gloss.png");
	SpaceBike.material.DiffuseReflectance = 7.f;

	SpaceBike.primitiveTopology = PrimitiveTopology::TriangleList;
	SpaceBike.Translate({ .0f,.0f,50.f });

	m_PhongSpecular = 0.5f;
	m_Shininess = 25.f;
	m_ShouldRotated = true;

	m_Meshes.emplace_back(SpaceBike);
}


Renderer::~Renderer()
{
	delete[] m_pDepthBufferPixels;
	for (Mesh& mesh : m_Meshes)
	{
		delete mesh.material.pDiffuse;
		delete mesh.material.pNormal;
		delete mesh.material.pSpecular;
		delete mesh.material.pGloss;

		mesh.material.pDiffuse = nullptr;
		mesh.material.pNormal = nullptr;
		mesh.material.pSpecular = nullptr;
		mesh.material.pGloss = nullptr;
	}
}

void Renderer::Update(Timer* pTimer)
{
	m_Camera.Update(pTimer);

	if(m_ShouldRotated)
	{
		m_CurrentRotation += pTimer->GetElapsed();
		for (Mesh& mesh : m_Meshes)
		{
			mesh.RotateY(m_CurrentRotation);
			mesh.UpdateTransforms();
		}
	}
}

void Renderer::Render()
{
	SDL_LockSurface(m_pBackBuffer);

	//fill the whole depth buffer with max values so it can become smaller
	std::fill_n(m_pDepthBufferPixels, m_Width * m_Height, std::numeric_limits<float>::max());
	// Clear screen buffer
	SDL_FillRect(m_pBackBuffer, nullptr, SDL_MapRGB(m_pBackBuffer->format, 100, 100, 100));

	RasterizeMesh();

	//@END
	//Update SDL Surface
	SDL_UnlockSurface(m_pBackBuffer);
	SDL_BlitSurface(m_pBackBuffer, 0, m_pFrontBuffer, 0);
	SDL_UpdateWindowSurface(m_pWindow);
}

void Renderer::VertexTransformationFunction(Mesh& mesh) const
{
	mesh.ResetVertices();

	const Matrix WorldViewProjectionMatrix = mesh.worldMatrix * m_Camera.m_ViewMatrix * m_Camera.m_ProjectionMatrix;

	for (Vertex_Out& vertex : mesh.vertices_out)
	{
		vertex.position =  WorldViewProjectionMatrix.TransformPoint(vertex.position);

		//transform the normal and tagents so its in the correct position
		vertex.normal = WorldViewProjectionMatrix.TransformVector(vertex.normal).Normalized();
		vertex.tangent = WorldViewProjectionMatrix.TransformVector(vertex.tangent).Normalized();

		//get the viewDirection
		vertex.viewDirection = (vertex.position.GetXYZ() - m_Camera.m_Origin).Normalized();

		vertex.position.x /= vertex.position.w;
		vertex.position.y /= vertex.position.w;
		vertex.position.z /= vertex.position.w;

		vertex.position.z = 1.f / vertex.position.z;

		vertex.position.x = (vertex.position.x + 1.f) / 2.f * static_cast<float>(m_Width);
		vertex.position.y = (1.f - vertex.position.y) / 2.f * static_cast<float>(m_Height);
	}
}

void Renderer::RasterizeMesh()
{
	for (Mesh& mesh : m_Meshes)
	{
		VertexTransformationFunction(mesh);

		if(mesh.primitiveTopology == PrimitiveTopology::TriangleStrip)
		{
			TriangleStrip(mesh);
		}
		else
		{
			TriangleList(mesh);
		}
	}
}
void Renderer::TriangleStrip(const Mesh& mesh)
{
	for (size_t i = 0; i < mesh.indices.size() - 2; i++)
	{
		size_t v0;
		size_t v1;
		size_t v2;
		if (i % 2 == 0)
		{
			v0 = mesh.indices[i];
			v1 = mesh.indices[i + 1];
			v2 = mesh.indices[i + 2];
		}
		else
		{
			v0 = mesh.indices[i];
			v1 = mesh.indices[i + 2];
			v2 = mesh.indices[i + 1];
		}

		Rasterize(mesh, v0, v1, v2);
	}
}
void Renderer::TriangleList(const Mesh& mesh)
{
	for (size_t i = 0; i < mesh.indices.size() - 2; i += 3)
	{
		const size_t v0 = mesh.indices[i];
		const size_t v1 = mesh.indices[i + 1];
		const size_t v2 = mesh.indices[i + 2];
		Rasterize(mesh, v0, v1, v2);
	}
}

void Renderer::Rasterize(const Mesh& mesh, const size_t v0, const size_t v1, const size_t v2)
{
	const Vector2 V0 = { mesh.vertices_out[v0].position.x, mesh.vertices_out[v0].position.y };
	const Vector2 V1 = { mesh.vertices_out[v1].position.x, mesh.vertices_out[v1].position.y };
	const Vector2 V2 = { mesh.vertices_out[v2].position.x, mesh.vertices_out[v2].position.y };

	const float w0 = mesh.vertices_out[v0].position.w;
	const float w1 = mesh.vertices_out[v1].position.w;
	const float w2 = mesh.vertices_out[v2].position.w;

	//dont know if this should be here but it fixes a chrash
	if(w0 < 0 || w1 < 0 || w2 < 0) return;

	const BoundaryBox boundaryBox{
		{
			std::clamp(std::min({V0.x, V1.x, V2.x}), 0.0f, static_cast<float>(m_Width - 1)),
			std::clamp(std::min({V0.y, V1.y, V2.y}), 0.0f, static_cast<float>(m_Height - 1))
		},
		{
			std::clamp(std::max({V0.x, V1.x, V2.x}), 0.0f, static_cast<float>(m_Width - 1)),
			std::clamp(std::max({V0.y, V1.y, V2.y}), 0.0f, static_cast<float>(m_Height - 1))
		}
	};

	for (int py = boundaryBox.Smallest.y; py <= boundaryBox.Biggest.y; ++py)
	{
		for (int px = boundaryBox.Smallest.x; px <= boundaryBox.Biggest.x; ++px)
		{
			// Calculate the pixel center point in screen space
			const Vector2 P(px + 0.5f, py + 0.5f);

			// Calculate sub-areas for barycentric coordinates
			float W0 = Vector2::Cross(V2 - V1, P - V1); // Opposite V0
			if (W0 <= 0) continue;
			float W1 = Vector2::Cross(V0 - V2, P - V2); // Opposite V1
			if (W1 <= 0) continue;
			float W2 = Vector2::Cross(V1 - V0, P - V0); // Opposite V2
			if (W2 <= 0) continue;
			
			const float totalArea = W0 + W1 + W2;
			const Vector3 weights
			{
				W0 / totalArea,
				W1 / totalArea,
				W2 / totalArea
			};

			//get the depth of triangle
			const float nonlinearDepth = 1.0f / (
				weights.x / (1.f / mesh.vertices_out[v0].position.z) +
				weights.y / (1.f / mesh.vertices_out[v1].position.z) +
				weights.z / (1.f / mesh.vertices_out[v2].position.z) );

			//get the index where in screen this pixel is
			const int pixelIndex = px + py * m_Width;

			if(nonlinearDepth < 0 || nonlinearDepth > 1)
				continue;

			if (nonlinearDepth < m_pDepthBufferPixels[pixelIndex])
			{
				m_pDepthBufferPixels[pixelIndex] = nonlinearDepth;

				const Vector2 uv0 = mesh.vertices_out[v0].uv;
				const Vector2 uv1 = mesh.vertices_out[v1].uv;
				const Vector2 uv2 = mesh.vertices_out[v2].uv;

				const float linearDepth = 1.0f / (
					weights.x / w0 +
					weights.y / w1 +
					weights.z / w2 );

				const Vector2 interpolatedUV = linearDepth * (
					uv0 / w0 * weights.x +
					uv1 / w1 * weights.y +
					uv2 / w2 * weights.z );

				const Vector3 InterpolatedNormal = linearDepth * (
					mesh.vertices_out[v0].normal / w0 * weights.x +
					mesh.vertices_out[v1].normal / w1 * weights.y +
					mesh.vertices_out[v2].normal / w2 * weights.z );

				const Vector3 InterpolatedTangent = linearDepth * (
					mesh.vertices_out[v0].tangent / w0 * weights.x +
					mesh.vertices_out[v1].tangent / w1 * weights.y +
					mesh.vertices_out[v2].tangent / w2 * weights.z );

				const Vector3 InterpolatedViewDirection = linearDepth * (
					mesh.vertices_out[v0].viewDirection / w0 * weights.x +
					mesh.vertices_out[v1].viewDirection / w1 * weights.y +
					mesh.vertices_out[v2].viewDirection / w2 * weights.z );

				const float remappedDepth = std::clamp(Remap(nonlinearDepth, 0.985f, 1.0f), 0.0f, 1.0f);
				const ColorRGB InterpolatedColor = { remappedDepth, remappedDepth, remappedDepth };

				const Vertex_Out vertex
				{
					{0,0,0,0},
					InterpolatedColor,
					interpolatedUV,
					InterpolatedNormal,
					InterpolatedTangent,
					InterpolatedViewDirection
				};

				PixelShading(&mesh.material, pixelIndex, vertex);
			}
		}
	}
}

void Renderer::PixelShading(const Material* pMaterial, const int pixelIndex, const Vertex_Out& vertex_out) const
{
	Vector3 SampeldNormal { vertex_out.normal };
	ColorRGB sampleDDiffuseColor { pMaterial->DiffuseColor };
	float PhongSpecular { m_PhongSpecular };
	float PhongExponent { m_Shininess };

	if (pMaterial->pDiffuse)
		sampleDDiffuseColor = pMaterial->pDiffuse->Sample(vertex_out.uv);

	if (pMaterial->pSpecular)
		PhongSpecular *= pMaterial->pSpecular->Sample(vertex_out.uv).r; //only one is needed can be r,g or b

	if (pMaterial->pGloss)
		PhongExponent *= pMaterial->pGloss->Sample(vertex_out.uv).r;	//only one is needed can be r,g or b

	if(pMaterial->pNormal && m_NormalMapActive)
	{
		Vector3 binormal = Vector3::Cross(vertex_out.normal, vertex_out.tangent);

		Matrix tangentSpaceAxis{
			vertex_out.tangent,
			binormal,
			vertex_out.normal,
			Vector3::Zero
		};

		ColorRGB sampleNormalColor = pMaterial->pNormal->Sample(vertex_out.uv);
		Vector3 sampledNormalMapped{
			2.f * sampleNormalColor.r - 1.f,
			2.f * sampleNormalColor.g - 1.f,
			2.f * sampleNormalColor.b - 1.f
		};

		SampeldNormal = tangentSpaceAxis.TransformPoint(sampledNormalMapped);
	}

	ColorRGB LambertDiffuse = sampleDDiffuseColor * pMaterial->DiffuseReflectance / PI;

	const float observedArea = std::max(0.0f, Vector3::Dot(SampeldNormal, -m_LightDirection));

	//phong
	Vector3 reflect{ Vector3::Reflect(m_LightDirection, SampeldNormal) };
	float cosAlpa{std::max(0.f,Vector3::Dot(reflect,vertex_out.viewDirection))};
	ColorRGB PhongColor = colors::White * PhongSpecular * pow(cosAlpa, PhongExponent);

	ColorRGB finalColor{};

	if(m_DepthToggle)
	{
		finalColor = vertex_out.color;
	}
	else
	{
		switch (m_CurrentLightingMode)
		{
		case LightingMode::Diffuse:
			finalColor = LambertDiffuse;
			break;
		case LightingMode::ObservedArea:
			finalColor += colors::White * observedArea;
			break;
		case LightingMode::Specular:
			finalColor = colors::White * PhongColor;
			break;
		case LightingMode::Combined:
			finalColor = PhongColor + LambertDiffuse * observedArea;
			break;
		}
	}

	finalColor.MaxToOne();

	m_pBackBufferPixels[pixelIndex] = SDL_MapRGB(m_pBackBuffer->format,
		static_cast<uint8_t>(finalColor.r * 255),
		static_cast<uint8_t>(finalColor.g * 255),
		static_cast<uint8_t>(finalColor.b * 255));
}

float Renderer::Remap(float value, float fromMin, float fromMax, float toMin, float toMax)
{
	return (value - fromMin) / (fromMax - fromMin) * (toMax - toMin) + toMin;
}

void Renderer::CycleLightingMode()
{
	std::cout << std::endl;
	switch (m_CurrentLightingMode)
	{
	case LightingMode::ObservedArea:
		m_CurrentLightingMode = LightingMode::Diffuse;
		std::cout << "Current Lighting mode Diffuse" << std::endl;
		break;
	case LightingMode::Diffuse:
		m_CurrentLightingMode = LightingMode::Specular;
		std::cout << "Current Lighting mode Specular" << std::endl;
		break;
	case LightingMode::Specular:
		m_CurrentLightingMode = LightingMode::Combined;
		std::cout << "Current Lighting mode Combined" << std::endl;
		break;
	case LightingMode::Combined:
		m_CurrentLightingMode = LightingMode::ObservedArea;
		std::cout << "Current Lighting mode ObservedArea" << std::endl;
		break;
	}
	std::cout << std::endl;
}

bool Renderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBackBuffer, "Rasterizer_ColorBuffer.bmp");
}
