//External includes
#include "SDL.h"
#include "SDL_surface.h"

//Project includes
#include "Renderer.h"

#include <execution>
#include <iostream>
#include "Maths.h"
#include "Matrix.h"
#include "Material.h"
#include "Scene.h"
#include "Utils.h"

#define PARALLEL_EXECUTION

using namespace dae;

Renderer::Renderer(SDL_Window * pWindow) :
	m_pWindow(pWindow),
	m_pBuffer(SDL_GetWindowSurface(pWindow))
{
	//Initialize
	SDL_GetWindowSize(pWindow, &m_Width, &m_Height);
	m_pBufferPixels = static_cast<uint32_t*>(m_pBuffer->pixels);
}

void Renderer::Render(Scene* pScene) const
{
	Camera& camera = pScene->GetCamera();

	const float aspectRatio = static_cast<float>(m_Width) / static_cast<float>(m_Height);
	const float FOV = tan(camera.fovAngle / 2);
	const Matrix cameraToWorld = camera.CalculateCameraToWorld();

	const uint32_t amountPixels{ uint32_t(m_Width * m_Height) };

#if defined(PARALLEL_EXECUTION)

	std::vector<uint32_t> pixelIndices{};

	pixelIndices.reserve(amountPixels);
	for (uint32_t index = 0; index < amountPixels; ++index)
	{
		pixelIndices.emplace_back(index);
	}

	std::for_each(std::execution::par, pixelIndices.begin(), pixelIndices.end(), [&](int i) {
		RenderPixel(pScene, i, FOV, aspectRatio, cameraToWorld, camera.origin);
		});

#else
	for (uint32_t i = 0; i < amountPixels; ++i)
	{
		RenderPixel(pScene, i, FOV, aspectRatio, cameraToWorld, camera.origin);
	}
#endif

	//@END
	//Update SDL Surface
	SDL_UpdateWindowSurface(m_pWindow);
}

void dae::Renderer::RenderPixel(Scene* pScene, uint32_t pixelIndex, float fov, float aspectRatio, const Matrix cameraToWorld, const Vector3 cameraOrigin) const
{
	auto materials{ pScene->GetMaterials() };
	auto& lights{ pScene->GetLights() };

	const uint32_t px{ pixelIndex % m_Width }, py{ pixelIndex / m_Width };

	float rx{ px + 0.5f },ry{py + 0.5f};
	float cx{ (2 * (rx / float(m_Width)) - 1) * aspectRatio * fov };
	float cy{ (1 - (2 * (ry / float(m_Height)))) * fov };

	Vector3 rayDirectionWS{ cameraToWorld.TransformVector({cx,cy,1}) };

	Ray viewRay{ cameraOrigin,rayDirectionWS.Normalized() };

	HitRecord closestHit{ };

	pScene->GetClosestHit(viewRay, closestHit);

	//black BackGround
	ColorRGB finalColor{};

	if (closestHit.didHit)
	{
		const Vector3 hitPointOffset{ closestHit.origin + closestHit.normal * 0.001f };
		const Vector3 v{ -viewRay.direction };
		//adding shadows
		for (const Light& light : lights)
		{
			Vector3 rayToLight{ LightUtils::GetDirectionToLight(light, hitPointOffset) };
			float distanceToLight{ rayToLight.Magnitude() };
			Vector3 l = rayToLight.Normalized();

			//Create HardShadow
			if (m_ShadowsEnabled)
			{
				Ray shadowRay(hitPointOffset, rayToLight.Normalized(), 0.001f, distanceToLight - 0.001f);
				if (pScene->DoesHit(shadowRay))
				{
					//not sure why it works, but it works so super cool
					continue;
				}

			}
			const float cosineLaw = std::max(0.f, Vector3::Dot(closestHit.normal, l));

			switch (m_CurrentLightingMode)
			{
			case LightingMode::ObservedArea:
				finalColor += ColorRGB(1.f, 1.f, 1.f) * cosineLaw;
				break;
			case LightingMode::Radiance:
				finalColor += LightUtils::GetRadiance(light, closestHit.origin);
				break;
			case LightingMode::BRDF:
				finalColor += materials[closestHit.materialIndex]->Shade(closestHit, l, v);
				break;
			case LightingMode::Combined:
				finalColor += LightUtils::GetRadiance(light, closestHit.origin)
					* materials[closestHit.materialIndex]->Shade(closestHit, l, v)
					* cosineLaw;
				break;
			}
		}
	}

	//Update Color in Buffer
	finalColor.MaxToOne();

	m_pBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBuffer->format,
		static_cast<uint8_t>(finalColor.r * 255),
		static_cast<uint8_t>(finalColor.g * 255),
		static_cast<uint8_t>(finalColor.b * 255));

}

bool Renderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBuffer, "RayTracing_Buffer.bmp");
}

void dae::Renderer::CycleLightingMode()
{
	std::cout << std::endl;
	switch (m_CurrentLightingMode)
	{
	case LightingMode::ObservedArea:
		m_CurrentLightingMode = LightingMode::Radiance;
		std::cout << "Current Lighting mode Radiance" << std::endl;
		break;
	case LightingMode::Radiance:
		m_CurrentLightingMode = LightingMode::BRDF;
		std::cout << "Current Lighting mode BRDF" << std::endl;
		break;
	case LightingMode::BRDF:
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
