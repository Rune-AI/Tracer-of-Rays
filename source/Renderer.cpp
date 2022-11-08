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

#include <future>
#include <ppl.h> //parallel_for

using namespace dae;
//#define ASYNC
#define PARALLEL_FOR

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

	/*const float width = m_Width;
	const float height = m_Height;*/
	const float aspectRatio = m_Width / static_cast<float>(m_Height);
	const float fov = tanf(camera.fovAngle * TO_RADIANS / 2.f);
	camera.CalculateCameraToWorld();

	const uint32_t numPixels = m_Width * m_Height;
	
#if defined(ASYNC)
	//Async
	//+++++
	const uint32_t numCores = std::thread::hardware_concurrency();
	std::vector<std::future<void>> async_futures{};
	const uint32_t numPixelsPerTask = numPixels / numCores;
	uint32_t numUnassignedPixels = numPixels % numCores;
	uint32_t currPixelIndex = 0;

	for (uint32_t coreId{0}; coreId < numCores; ++coreId)
	{
		uint32_t taskSize = numPixelsPerTask;
		if (numUnassignedPixels > 0)
		{
			++taskSize;
			--numUnassignedPixels;
		}
		
		async_futures.push_back(std::async(std::launch::async, [=, this]
			{
				//Rander all pixels for this task (currPixelIndex > currPixelIndex + taskSize)
				const uint32_t pixelIndexEnd = currPixelIndex + taskSize;
				for (uint32_t pixelIndex = currPixelIndex; pixelIndex < pixelIndexEnd; ++pixelIndex)
				{
					RenderPixel(pScene, pixelIndex, fov, aspectRatio, camera, lights, materials);
				}
			}));
		
		currPixelIndex += taskSize;
	}
	
	//Wait for async tasks to finish
	for (const std::future<void>& future : async_futures)
	{
		future.wait();
	}
	
#elif defined(PARALLEL_FOR)
	//Parallel For
	//++++++++++++
	concurrency::parallel_for(0u, numPixels, [=, this](uint32_t pixelIndex)
		{
			RenderPixel(pScene, pixelIndex, fov, aspectRatio, camera, lights, materials);
		});
#else
	for (uint32_t i{ 0 }; i < numPixels; ++i)
	{
		RenderPixel(pScene, i, fov, aspectRatio, camera, lights, materials);
	}
#endif

	//@END
	//Update SDL Surface
	SDL_UpdateWindowSurface(m_pWindow);
}

void Renderer::RenderPixel(Scene* pScene, uint32_t pixelIndex, float fov, float aspectRatio, const Camera& camera,
	const std::vector<Light>& lights, const std::vector<Material*>& materials) const
{
	const int px = pixelIndex % m_Width;
	const int py = pixelIndex / m_Width;

	float rx = px + 0.5f;
	float ry = py + 0.5f;

	float cx = ((2 * rx) / float(m_Width) - 1) * aspectRatio * fov;
	float cy = (1 - (2 * ry) / float(m_Height)) * fov;

	const Vector3 rayDirection = camera.cameraToWorld.TransformVector(Vector3(cx, cy, 1.f)).Normalized();

	const Ray viewRay = Ray{ camera.origin, rayDirection };

	//TODO 6: rendering the scene
	HitRecord closestHit{};
	pScene->GetClosestHit(viewRay, closestHit);


	//ColorRGB finalColor{ rayDirection.x, rayDirection.y, rayDirection.z };
	ColorRGB finalColor{};
	if (closestHit.didHit)
	{
		//finalColor = materials[closestHit.materialIndex]->Shade();
		for (const Light& light : lights)
		{
			Vector3 lightDirection{ dae::LightUtils::GetDirectionToLight(light, closestHit.origin) };
			const float lightDistance = lightDirection.Normalize();

			const float offset{ 0.0001f };
			Ray lightRay = Ray{ closestHit.origin + closestHit.normal * offset,
				lightDirection,
				0.00001f,
				lightDistance };

			if (m_ShadowsEnabled && pScene->DoesHit(lightRay)) continue;

			float lambertCosineObserverdArea{ Vector3::Dot(closestHit.normal, lightDirection) };


			switch (m_currentLightingMode)
			{
			case dae::Renderer::LightingMode::observationArea:
				if (lambertCosineObserverdArea < 0) continue;
				finalColor += ColorRGB(lambertCosineObserverdArea, lambertCosineObserverdArea, lambertCosineObserverdArea);
				break;
			case dae::Renderer::LightingMode::Radiance:
				finalColor += LightUtils::GetRadiance(light, closestHit.origin);
				break;
			case dae::Renderer::LightingMode::BRDF:
				if (lambertCosineObserverdArea < 0) continue;
				finalColor += materials[closestHit.materialIndex]->Shade(closestHit, lightDirection, -rayDirection);
				break;
			case dae::Renderer::LightingMode::Combined:
				if (lambertCosineObserverdArea < 0) continue;
				finalColor += LightUtils::GetRadiance(light, closestHit.origin)
					* materials[closestHit.materialIndex]->Shade(closestHit, lightDirection, -rayDirection)
					* lambertCosineObserverdArea;
				break;
			default:
				//for debugging
				finalColor += ColorRGB(rayDirection.x, rayDirection.y, rayDirection.z);
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

void Renderer::KeyboardInputs(const SDL_Event& e)
{
	switch (e.key.keysym.scancode)
	{
	case SDL_SCANCODE_F2:
		m_ShadowsEnabled = !m_ShadowsEnabled;
		break;
	case SDL_SCANCODE_F3:
		m_currentLightingMode = static_cast<LightingMode>((int(m_currentLightingMode) + 1) % 4);
		break;
	}
}

