//External includes
#include "SDL.h"
#include "SDL_surface.h"

//Project includes
#include "ERenderer.h"
#include "ERGBColor.h"
#include "Structs.h"
#include "SceneGraph.h"
#include "Object.h"
#include "Light.h"
#include "PerspectiveCamera.h"
#include "Material.h"
#include "ProjectSettings.h"
#include "SceneManager.h"

Elite::Renderer::Renderer(SDL_Window* pWindow)
{
	//Initialize
	m_pWindow = pWindow;
	m_pFrontBuffer = SDL_GetWindowSurface(pWindow);
	int width, height = 0;
	SDL_GetWindowSize(pWindow, &width, &height);
	m_Width = static_cast<uint32_t>(width);
	m_Height = static_cast<uint32_t>(height);
	m_pBackBuffer = SDL_CreateRGBSurface(0, m_Width, m_Height, 32, 0, 0, 0, 0);
	m_pBackBufferPixels = (uint32_t*)m_pBackBuffer->pixels;

	int threadCount{ int(std::thread::hardware_concurrency()) };
	m_Threads.resize(threadCount);
}

Elite::Renderer::~Renderer()
{
	for (std::thread& t : m_Threads)
	{
		if (t.joinable())
			t.join();
	}

	m_Threads.clear();
}

void Elite::Renderer::Render(PerspectiveCamera* pCamera)
{
	SDL_LockSurface(m_pBackBuffer);

	int threadCount{ int(m_Threads.size()) };
	int rowPerThreads{ int(m_Height) / threadCount };

	for (int idx{}; idx < threadCount; ++idx)
	{
		bool IsLast{ (idx == threadCount - 1) };
		int endRow{ IsLast * int(m_Height) + !IsLast * ((idx + 1) * rowPerThreads) };
		m_Threads[idx] = std::thread(std::bind(&Renderer::PartialRender, this, pCamera, idx * rowPerThreads, endRow));
	}

	for (std::thread& t : m_Threads)
	{
		if (t.joinable())
			t.join();
	}

	SDL_UnlockSurface(m_pBackBuffer);
	SDL_BlitSurface(m_pBackBuffer, 0, m_pFrontBuffer, 0);
	SDL_UpdateWindowSurface(m_pWindow);
}

bool Elite::Renderer::SaveBackbufferToImage() const
{
	return SDL_SaveBMP(m_pBackBuffer, "BackbufferRender.bmp");
}

void Elite::Renderer::PartialRender(PerspectiveCamera* pCamera, int startRow, int endRow)
{
	bool hit{ false };

	SceneGraph& currentScene{ SceneManager::GetInstance()->GetActiveScene() };
	const std::vector<Object*>& pObjects{ currentScene.GetObjects() };
	const std::vector<Light*>& pLights{ currentScene.GetLights() };
	ProjectSettings* pProjectSettings{ ProjectSettings::GetInstance() };
	bool hardShadows{ pProjectSettings->IsHardShadowEnabled() };
	LightRenderMode lightMode{ pProjectSettings->GetLightMode() };
	//Loop over all the pixels
	for (uint32_t r = startRow; r < uint32_t(endRow); ++r)
	{
		for (uint32_t c = 0; c < m_Width; ++c)
		{
			hit = false;
			Elite::RGBColor pixelColor{};
			HitRecord hitRecord{ pCamera->CastRay(Elite::IPoint2(c, r), m_Width, m_Height) };

			for (Object* pObject : pObjects)
			{
				if (pObject->HitCheck(hitRecord))
					hit = true;
			}

			if (hit)
			{
				Elite::FPoint3 hitPoint{ hitRecord.ray.GetPoint(hitRecord.t) };
				for (Light* pLight : pLights)
				{
					if (pLight->IsOn())
					{
						bool hasLight{ true };
						float rayTMax{ FLT_MAX };
						Elite::FVector3 hPointLightDir{ -pLight->GetDirection(hitPoint) };
						switch (pLight->GetType())
						{
							case LightType::POINTLIGHT:
								rayTMax = Magnitude(hPointLightDir);
								hPointLightDir /= rayTMax;
								break;
							case LightType::DIRECTIONAL:
								break;
						}

						if (hardShadows)
						{
							HitRecord shadowHitRecord{ Ray(hitPoint, hPointLightDir, 0.001f, rayTMax) };
							for (Object* pObject : pObjects)
							{
								if (pObject->HitCheck(shadowHitRecord, true))
								{
									hasLight = false;
									break;
								}
							}
						}

						if (hasLight)
						{
							float dot{ Dot(hitRecord.normal, hPointLightDir) };

							if (dot > 0.f)
							{
								switch (lightMode)
								{
								case LightRenderMode::ALL:
									pixelColor += hitRecord.pMaterial->Shade(hitRecord, hPointLightDir) * pLight->CalculateIllumination(hitPoint) * dot;
									break;
								case LightRenderMode::BRDFONLY:
									pixelColor += hitRecord.pMaterial->Shade(hitRecord, hPointLightDir) * dot;
									break;
								case LightRenderMode::LIGHTSOURCETONLY:
									pixelColor += pLight->CalculateIllumination(hitPoint) * dot;
									break;
								}
							}
						}
					}
				}
				pixelColor.MaxToOne();
			}

			//Fill the pixels - pixel access demo
			m_pBackBufferPixels[c + (r * m_Width)] = GetSDL_ARGBColor(pixelColor);
		}
	}
}