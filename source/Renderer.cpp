//External includes
#include "SDL.h"
#include "SDL_surface.h"

//Project includes
#include "Renderer.h"
#include "Math.h"
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

	const float width = m_Width;
	const float height = m_Height;
	const float aspectRatio = width / height;
	const float fov = tanf(camera.fovAngle * TO_RADIANS / 2.f);
	camera.CalculateCameraToWorld();

	for (int px{}; px < m_Width; ++px)
	{
		const float cx = ((2 * (px + 0.5f)) / width - 1) * aspectRatio * fov; //This should only be done everytime px changes, so this position is better

		for (int py{}; py < m_Height; ++py)
		{	
			//TODO 1: From Raster to Camera
			const float cy = (1 - (2 * (py + 0.5f)) / height) * fov;

			//Camera to world
			const Vector3 rayDirection = camera.cameraToWorld.TransformVector(Vector3(cx, cy, 1.f)).Normalized();

			const Ray hitRay = Ray{ camera.origin, rayDirection };

			//TODO 6: rendering the scene
			HitRecord closestHit{};
			pScene->GetClosestHit(hitRay, closestHit);

			//ColorRGB finalColor{ rayDirection.x, rayDirection.y, rayDirection.z };
			ColorRGB finalColor{};
			if (closestHit.didHit)
			{
				finalColor = materials[closestHit.materialIndex]->Shade();

				for (const Light& light : lights)
				{
					Vector3 lightDirection{ dae::LightUtils::GetDirectionToLight(light, closestHit.origin) };
					const float lightDistance = lightDirection.Normalize();

					const float offset{ 0.0001f };
					Ray lightRay = Ray{ closestHit.origin + closestHit.normal * offset,
						lightDirection, 
						0.0001f, 
						lightDistance };

					if (pScene->DoesHit(lightRay))
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
