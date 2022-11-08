#pragma once

#include <cstdint>
#include <vector>

struct SDL_Window;
struct SDL_Surface;

namespace dae
{
	class Timer;
	class Scene;
	class Camera;
	class Light;
	class Material;

	class Renderer final
	{
	public:
		Renderer(SDL_Window* pWindow);
		~Renderer() = default;

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void Render(Scene* pScene) const;
		void RenderPixel(Scene* pScene, uint32_t pixelIndex, float fov, float aspectRatio, const Camera& camera,
			const std::vector<Light>& lights, const std::vector<Material*>& materials) const;
		bool SaveBufferToImage() const;
		
		void KeyboardInputs(const SDL_Event& e);

	private:
		SDL_Window* m_pWindow{};

		SDL_Surface* m_pBuffer{};
		uint32_t* m_pBufferPixels{};

		int m_Width{};
		int m_Height{};
		
		enum class LightingMode
		{
			observationArea, //Labert cosine law
			Radiance, //Incident Radiance
			BRDF, //Scattering the light
			Combined, //ObservedArea*Radiance*BRDF
		};

		LightingMode m_currentLightingMode{ LightingMode::Combined };
		bool m_ShadowsEnabled{ true };
	};
}
