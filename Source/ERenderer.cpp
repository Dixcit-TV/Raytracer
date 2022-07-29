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

Elite::Renderer::Renderer(SDL_Window* pWindow, PerspectiveCamera* pCamera)
	: m_Threads{ size_t(std::thread::hardware_concurrency()) * 2 }
	, m_TileWorkQueue{}
	, m_TileWorkMutex{}
	, m_Waiter{}
	, m_FrameWaiter{}
	, m_RemainingWork{ 0 }
	, m_ThreadsRunning{ true }
	, m_pWindow{ pWindow }
	, m_pFrontBuffer{ SDL_GetWindowSurface(pWindow) }
	, m_pBackBuffer{ nullptr }
	, m_pBackBufferPixels{ nullptr }
	, m_Width{ 0 }
	, m_Height{ 0 }
{
	int width, height = 0;
	SDL_GetWindowSize(pWindow, &width, &height);
	m_Width = static_cast<uint32_t>(width);
	m_Height = static_cast<uint32_t>(height);
	m_pBackBuffer = SDL_CreateRGBSurface(0, m_Width, m_Height, 32, 0, 0, 0, 0);
	m_pBackBufferPixels = (uint32_t*)m_pBackBuffer->pixels;

	for (uint32_t idx{}; idx < m_Threads.size(); ++idx)
	{
		m_Threads[idx] = std::thread(std::bind(&Renderer::RenderThreadFnc, this, pCamera));
	}
}

Elite::Renderer::~Renderer()
{
	m_ThreadsRunning.store(false);
	m_Waiter.notify_all();

	for (std::thread& t : m_Threads)
	{
		if (t.joinable())
			t.join();
	}

	m_Threads.clear();
}

void Elite::Renderer::Render()
{
	SDL_LockSurface(m_pBackBuffer);

	{
		std::lock_guard lck{ m_TileWorkMutex };
		InitWorkQueue();
		m_RemainingWork.store(int(m_TileWorkQueue.size()));
	}
	m_Waiter.notify_all();

	std::unique_lock lck{ m_TileWorkMutex };
	m_FrameWaiter.wait(lck, [this]() { return m_RemainingWork == 0; });

	//while (m_RemainingWork);

	SDL_UnlockSurface(m_pBackBuffer);
	SDL_BlitSurface(m_pBackBuffer, 0, m_pFrontBuffer, 0);
	SDL_UpdateWindowSurface(m_pWindow);
}

bool Elite::Renderer::SaveBackbufferToImage() const
{
	return SDL_SaveBMP(m_pBackBuffer, "BackbufferRender.bmp");
}

void Elite::Renderer::InitWorkQueue()
{
	float tileXCountF{ float(m_Width) / TILE_SIZE };
	[[maybe_unused]] float decimalX{ tileXCountF - int(tileXCountF) };

	float tileYCountF{ float(m_Height) / TILE_SIZE };
	[[maybe_unused]] float decimaly{ tileYCountF - int(tileYCountF) };

	uint32_t tileXCount{ uint32_t(tileXCountF) };
	uint32_t tileYCount{ uint32_t(tileYCountF) };
	//if (decimal < 0.5f)

	m_TileWorkQueue.resize(size_t(tileXCount) * tileYCount);

	for (uint32_t y{ 0 }; y < tileYCount; ++y)
		for (uint32_t x{ 0 }; x < tileXCount; ++x)
		{
			TileSettings& ts{ m_TileWorkQueue[size_t(tileXCount) * y + x] };
			ts.x = x * TILE_SIZE;
			ts.y = y * TILE_SIZE;
			ts.width = TILE_SIZE;
			ts.height = TILE_SIZE;
		}
}

void Elite::Renderer::RenderThreadFnc(PerspectiveCamera* pCamera)
{
	while (m_ThreadsRunning)
	{
		TileSettings ts{};
		std::unique_lock lck{ m_TileWorkMutex };
		m_Waiter.wait(lck, [this]() { return m_TileWorkQueue.size() > 0 || !m_ThreadsRunning; });
		if (m_TileWorkQueue.size() > 0)
		{
			ts = m_TileWorkQueue.back();
			m_TileWorkQueue.pop_back();

			lck.unlock();

			PartialRender(pCamera, ts);

			lck.lock();
			--m_RemainingWork;
			m_FrameWaiter.notify_one();
		}
	}
}

void Elite::Renderer::PartialRender(PerspectiveCamera* pCamera, const TileSettings& tileSettings)
{
	bool hit{ false };

	SceneGraph& currentScene{ SceneManager::GetInstance()->GetActiveScene() };
	const std::vector<Object*>& pObjects{ currentScene.GetObjects() };
	const std::vector<Light*>& pLights{ currentScene.GetLights() };
	ProjectSettings* pProjectSettings{ ProjectSettings::GetInstance() };
	bool hardShadows{ pProjectSettings->IsHardShadowEnabled() };
	LightRenderMode lightMode{ pProjectSettings->GetLightMode() };
	//Loop over all the pixels
	for (uint32_t r = tileSettings.y; r < tileSettings.y + tileSettings.height; ++r)
	{
		for (uint32_t c = tileSettings.x; c < tileSettings.x + tileSettings.width; ++c)
		{
			hit = false;
			Elite::RGBColor pixelColor{};
			HitRecord hitRecord{ pCamera->CastRay(Elite::IPoint2(c, r), m_Width, m_Height) };

			for (Object* pObject : pObjects)
				hit |= pObject->HitCheck(hitRecord);

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