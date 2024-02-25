#include <iostream>

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
#include "Utils.h"
#include <random>

Elite::Renderer::Renderer(SDL_Window* pWindow, RenderMode renderMode, size_t threadCount)
	: m_RenderFunction{}
	, m_Threads{}
	, m_WorkQueue{}
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
	, m_RenderMode{ renderMode }
{
	int width, height = 0;
	SDL_GetWindowSize(pWindow, &width, &height);
	m_Width = static_cast<uint32_t>(width);
	m_Height = static_cast<uint32_t>(height);
	m_pBackBuffer = SDL_CreateRGBSurface(0, m_Width, m_Height, 32, 0, 0, 0, 0);
	m_pBackBufferPixels = (uint32_t*)m_pBackBuffer->pixels;

	if (renderMode == RenderMode::Multi_Threaded)
	{
		m_Threads.resize(threadCount);
		for (uint32_t idx{}; idx < threadCount; ++idx)
		{
			m_Threads[idx] = std::thread(std::bind(&Renderer::RenderThreadFnc, this));
		}

		m_RenderFunction = std::bind(&Renderer::MultiThreadedRender, this, std::placeholders::_1);
	}
	else
		m_RenderFunction = std::bind(&Renderer::TileRender, this, std::placeholders::_1, TileSettings{ 0, 0, m_Width, m_Height });
}

Elite::Renderer::~Renderer()
{
	if (m_Threads.size() > 0)
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
}

void Elite::Renderer::Render(PerspectiveCamera* pCamera)
{
	SDL_LockSurface(m_pBackBuffer);

	m_RenderFunction(pCamera);

	SDL_UnlockSurface(m_pBackBuffer);
	SDL_BlitSurface(m_pBackBuffer, 0, m_pFrontBuffer, 0);
	SDL_UpdateWindowSurface(m_pWindow);
}

bool Elite::Renderer::SaveBackbufferToImage() const
{
	return SDL_SaveBMP(m_pBackBuffer, "BackbufferRender.bmp");
}

void Elite::Renderer::InitWorkQueue(PerspectiveCamera* pCamera)
{
	float tileXCountF{ float(m_Width) / TILE_SIZE };
	[[maybe_unused]] float decimalX{ tileXCountF - int(tileXCountF) };

	float tileYCountF{ float(m_Height) / TILE_SIZE };
	[[maybe_unused]] float decimaly{ tileYCountF - int(tileYCountF) };

	uint32_t tileXCount{ uint32_t(tileXCountF) };
	uint32_t tileYCount{ uint32_t(tileYCountF) };
	//if (decimal < 0.5f)

	m_WorkQueue.reserve(size_t(tileXCount) * tileYCount);

	for (uint32_t y{ 0 }; y < tileYCount; ++y)
		for (uint32_t x{ 0 }; x < tileXCount; ++x)
		{
			TileSettings ts{ };
			ts.x = x * TILE_SIZE;
			ts.y = y * TILE_SIZE;
			ts.width = TILE_SIZE;
			ts.height = TILE_SIZE;

			m_WorkQueue.push_back(std::bind(&Renderer::TileRender, this, pCamera, ts));
		}
}

void Elite::Renderer::RenderThreadFnc()
{                            
	while (m_ThreadsRunning)
	{
		std::unique_lock lck{ m_TileWorkMutex };
		m_Waiter.wait(lck, [this]() { return m_WorkQueue.size() > 0 || !m_ThreadsRunning; });
		if (m_WorkQueue.size() > 0)
		{
			std::function<void()> work{ m_WorkQueue.back() };
			m_WorkQueue.pop_back();

			lck.unlock();

			work();

			lck.lock();
			--m_RemainingWork;
			m_FrameWaiter.notify_one();
		}
	}
}

void Elite::Renderer::MultiThreadedRender(PerspectiveCamera* pCamera)
{
	{
		std::lock_guard lck{ m_TileWorkMutex };
		InitWorkQueue(pCamera);
		m_RemainingWork.store(int(m_WorkQueue.size()));
	}
	m_Waiter.notify_all();

	std::unique_lock lck{ m_TileWorkMutex };
	m_FrameWaiter.wait(lck, [this]() { return m_RemainingWork == 0; });
}

void Elite::Renderer::TileRender(PerspectiveCamera* pCamera, TileSettings tileSettings)
{
	//Loop over all the pixels
	for (uint32_t r = tileSettings.y; r < tileSettings.y + tileSettings.height; ++r)
	{
		for (uint32_t c = tileSettings.x; c < tileSettings.x + tileSettings.width; ++c)
		{
			HitRecord hitRecord{ pCamera->CastRay(Elite::IPoint2{ int(c), int(r) }, m_Width, m_Height) };

			Elite::RGBColor pixelColor{ Elite::Renderer::Trace(hitRecord, 0) };

			//Fill the pixels - pixel access demo
			m_pBackBufferPixels[c + (r * m_Width)] = GetSDL_ARGBColor(GammaCorrection(pixelColor));
		}
	}
}

Elite::RGBColor Elite::Renderer::Trace(HitRecord hitRecord, uint8_t bounce)
{
	if (bounce > MAX_BOUNCE)
		return Elite::RGBColor{};

	ProjectSettings* pProjectSettings{ ProjectSettings::GetInstance() };
	bool hardShadows{ pProjectSettings->IsHardShadowEnabled() };
	LightRenderMode lightMode{ pProjectSettings->GetLightMode() };

	SceneGraph& currentScene{ SceneManager::GetInstance()->GetActiveScene() };
	const std::vector<Object*>& pObjects{ currentScene.GetObjects() };
	const size_t objectCount{ std::size(pObjects) };
	const std::vector<Light*>& pLights{ currentScene.GetLights() };
	const size_t lightCount{ std::size(pLights) };

	bool hit = false;
	Elite::RGBColor directLightColor{ 0.f, 0.f, 0.f };
	Elite::RGBColor indirectLightColor{ 0.f, 0.f, 0.f };
	Elite::RGBColor matColor{ 0.f, 0.f, 0.f };

	for (Object* pObject : pObjects)
		hit |= pObject->HitCheck(hitRecord);

	if (!hit)
		return Elite::RGBColor{};

	for (Light* pLight : pLights)
	{
		if (pLight->IsOn() && !pLight->IsOutOfRange(hitRecord.hitPosition))
		{
			bool hasLight{ true };
			float rayTMax{ FLT_MAX };
			Elite::FVector3 hPointLightDir{ -pLight->GetDirection(hitRecord.hitPosition) };
			switch (pLight->GetType())
			{
			case LightType::POINTLIGHT:
				rayTMax = Normalize(hPointLightDir);
				break;
			case LightType::DIRECTIONAL:
				break;
			}

			HitRecord shadowHitRecord{ Ray(Math::GetRayOriginOffset(hitRecord.hitPosition, hitRecord.normal), hPointLightDir, 0.f, rayTMax) };
			for (size_t objIdx{}; objIdx < objectCount && hasLight && hardShadows; ++objIdx)
			{
				hasLight = !pObjects[objIdx]->HitCheck(shadowHitRecord, true);
			}

			directLightColor += pLight->CalculateIllumination(hitRecord.hitPosition)
				* hasLight
				* std::max(Dot(hitRecord.normal, hPointLightDir), 0.f)
				* ((int(lightMode) & int(LightRenderMode::LIGHTSOURCETONLY)) != 0);

			matColor += hitRecord.pMaterial->Shade(hitRecord, hPointLightDir)
				* ((int(lightMode) & int(LightRenderMode::BRDFONLY)) != 0);
		}
	}

	directLightColor /= static_cast<float>(lightCount);
	matColor /= static_cast<float>(lightCount);

	//const uint8_t samples{ 1 };
	//for (uint8_t i{}; i < samples; ++i)
	//{
	//	std::default_random_engine gen;
	//	std::uniform_real_distribution<float> distribution(0.f, 1.f);

	//	Elite::FVector3 sampDir{ Math::GetONB(hitRecord.normal) * Math::GetCosineWeightedPoint(distribution(gen), distribution(gen)) };
	//	HitRecord reflectHitRecord{ Ray(Math::GetRayOriginOffset(hitRecord.hitPosition, hitRecord.normal), sampDir) };

	//	indirectLightColor += Trace(reflectHitRecord, bounce + 1);
	//}
	//indirectLightColor /= samples;

	Elite::RGBColor pixelColor{ (directLightColor + indirectLightColor) * matColor };

	return pixelColor;
}