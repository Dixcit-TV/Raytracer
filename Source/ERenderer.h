/*=============================================================================*/
// Copyright 2017-2019 Elite Engine
// Authors: Matthieu Delaere
/*=============================================================================*/
// ERenderer.h: class that holds the surface to render to, does traverse the pixels 
// and traces the rays using a tracer
/*=============================================================================*/
#ifndef ELITE_RAYTRACING_RENDERER
#define	ELITE_RAYTRACING_RENDERER

#include <cstdint>
#include <thread>
#include <vector>
#include <functional>
#include <queue>
#include <mutex>

struct SDL_Window;
struct SDL_Surface;
struct HitRecord;
class PerspectiveCamera;

namespace Elite
{
	struct TileSettings
	{
		uint32_t x, y;
		uint32_t width, height;
	};

	class Renderer final
	{
	public:
		const uint32_t TILE_SIZE = 8;

		Renderer(SDL_Window* pWindow);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void Render(PerspectiveCamera* pCamera);
		bool SaveBackbufferToImage() const;

	private:
		std::vector<std::thread> m_Threads;
		std::vector<std::function<void()>> m_WorkQueue;

		std::mutex m_TileWorkMutex;
		std::condition_variable m_Waiter;
		std::condition_variable m_FrameWaiter;
		std::atomic_int m_RemainingWork;
		std::atomic_bool m_ThreadsRunning;

		SDL_Window* m_pWindow = nullptr;
		SDL_Surface* m_pFrontBuffer = nullptr;
		SDL_Surface* m_pBackBuffer = nullptr;
		uint32_t* m_pBackBufferPixels = nullptr;
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;

		void InitWorkQueue(PerspectiveCamera* pCamera);
		void RenderThreadFnc();
		void PartialRender(PerspectiveCamera* pCamera, TileSettings tileSettings);
		//void Shading(HitRecord hitRecord, size_t pixel);
	};
}

#endif