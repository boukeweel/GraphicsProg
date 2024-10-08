//External includes
#include "SDL.h"
#include "SDL_surface.h"

//Project includes
#include "Renderer.h"
#include <iostream>
#include "Maths.h"
#include "Matrix.h"
#include "Material.h"
#include "Scene.h"
#include "Utils.h"

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
	auto& materials = pScene->GetMaterials();
	auto& lights = pScene->GetLights();

	const float aspectRatio = static_cast<float>(m_Width) / static_cast<float>(m_Height);
	const float FOV = tan(camera.fovAngle / 2);
	const Matrix cameraToWorld = camera.CalculateCameraToWorld();
	Vector3 rayDirectionVS{};

	for (int px{}; px < m_Width; ++px)
	{
		for (int py{}; py < m_Height; ++py)
		{
			rayDirectionVS.x = (2 * (px + 0.5f) / m_Width - 1) * aspectRatio * FOV;
			rayDirectionVS.y = (1 - 2 * (py + 0.5f) / m_Height) * FOV;
			rayDirectionVS.z = 1;


			Vector3 rayDirectionWS{ cameraToWorld.TransformVector(rayDirectionVS) };

			Ray viewRay{ camera.origin,rayDirectionWS };

			HitRecord closetHit{ };

			pScene->GetClosestHit(viewRay, closetHit);

			//black BackGround
			ColorRGB finalColor{};

			if(closetHit.didHit)
			{
				//if we hit something set the finalcolor to the color of the hit object
				finalColor = materials[closetHit.materialIndex]->Shade();

				const Vector3 HitPointOffset{ closetHit.origin + closetHit.normal * 0.001f };
				//adding shadows
				for (const Light& light : lights)
				{
					Vector3 rayToLight{ LightUtils::GetDirectionToLight(light, HitPointOffset) };
					float distanceToLight{ rayToLight.Magnitude() };

					Ray newRay(HitPointOffset, rayToLight.Normalized(), 0.001f,distanceToLight - 0.001f);
					if (pScene->DoesHit(newRay))
					{
						finalColor *= 0.5f;
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
	}

	//@END
	//Update SDL Surface
	SDL_UpdateWindowSurface(m_pWindow);
}

bool Renderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBuffer, "RayTracing_Buffer.bmp");
}
