//External includes
#include "vld.h"
#include "SDL.h"
#include "SDL_surface.h"
#undef main

//Standard includes
#include <iostream>

//Project includes
#include "ETimer.h"
#include "ERenderer.h"
#include "SceneGraph.h"
#include "PerspectiveCamera.h"
#include "Sphere.h"
#include "Plane.h"
#include "Triangle.h"
#include "TriangleMesh.h"
#include "PointLight.h"
#include "DirectionalLight.h"
#include "LambertMaterial.h"
#include "LambertPhongMaterial.h"
#include "LambertCookTorranceMaterial.h"
#include "MaterialManager.h"
#include "ProjectSettings.h"
#include "SceneManager.h"

void ShutDown(SDL_Window* pWindow)
{
	delete SceneManager::GetInstance();
	delete MaterialManager::GetInstance();
	delete ProjectSettings::GetInstance();
	SDL_DestroyWindow(pWindow);
	SDL_Quit();
}

void InitMaterials()
{
	MaterialManager::GetInstance()->AddMaterial("Mat_LambertCookTorrance_Aluminium", new LambertCookTorranceMaterial(Elite::RGBColor(0.913f, 0.922f, 0.924f), 0.1f, true));
	MaterialManager::GetInstance()->AddMaterial("Mat_LambertCookTorrance_Gold", new LambertCookTorranceMaterial(Elite::RGBColor(1.f, 0.782f, 0.344f), 0.6f, true));
	MaterialManager::GetInstance()->AddMaterial("Mat_LambertCookTorrance_Iron", new LambertCookTorranceMaterial(Elite::RGBColor(0.562f, 0.565f, 0.578f), 1.f, true));
	MaterialManager::GetInstance()->AddMaterial("Mat_LambertCookTorrance_Green", new LambertCookTorranceMaterial(Elite::RGBColor(0.40f, 0.32f, 0.57f), 0.1f, false));
	MaterialManager::GetInstance()->AddMaterial("Mat_LambertCookTorrance_Purple", new LambertCookTorranceMaterial(Elite::RGBColor(0.47f, 0.63f, 0.41f), 0.6f, false));
	MaterialManager::GetInstance()->AddMaterial("Mat_LambertCookTorrance_Red", new LambertCookTorranceMaterial(Elite::RGBColor(0.67f, 0.17f, 0.17f), 1.f, false));
	MaterialManager::GetInstance()->AddMaterial("Mat_LambertPhong_Purple", new LambertPhongMaterial(Elite::RGBColor(0.40f, 0.32f, 0.57f), 1.f, 6));
	MaterialManager::GetInstance()->AddMaterial("Mat_LambertPhong_Green", new LambertPhongMaterial(Elite::RGBColor(0.47f, 0.63f, 0.41f), 0.66f, 4));
	MaterialManager::GetInstance()->AddMaterial("Mat_LambertPhong_Red", new LambertPhongMaterial(Elite::RGBColor(0.67f, 0.17f, 0.17f), 0.33f, 8));
	MaterialManager::GetInstance()->AddMaterial("Mat_LambertCookTorrance_Blue", new LambertCookTorranceMaterial(Elite::RGBColor(0.2f, 0.3f, 0.45f), 0.8f, false));
}

void InitScene1()
{
	SceneGraph scene1{};
	scene1.AddObject(new Sphere(Elite::MakeTranslation(Elite::FVector3(-2.5f, 2.f, 0.f)), 1.f, MaterialManager::GetInstance()->GetMaterial("Mat_LambertCookTorrance_Green")));
	scene1.AddObject(new Sphere(Elite::MakeTranslation(Elite::FVector3(0.f, 2.f, 0.f)), 1.f, MaterialManager::GetInstance()->GetMaterial("Mat_LambertCookTorrance_Purple")));
	scene1.AddObject(new Sphere(Elite::MakeTranslation(Elite::FVector3(2.5f, 2.f, 0.f)), 1.f, MaterialManager::GetInstance()->GetMaterial("Mat_LambertCookTorrance_Red")));

	scene1.AddObject(new Sphere(Elite::MakeTranslation(Elite::FVector3(-2.5f, 4.5f, 0.f)), 1.f, MaterialManager::GetInstance()->GetMaterial("Mat_LambertCookTorrance_Aluminium")));
	scene1.AddObject(new Sphere(Elite::MakeTranslation(Elite::FVector3(0.f, 4.5f, 0.f)), 1.f, MaterialManager::GetInstance()->GetMaterial("Mat_LambertCookTorrance_Gold")));
	scene1.AddObject(new Sphere(Elite::MakeTranslation(Elite::FVector3(2.5f, 4.5f, 0.f)), 1.f, MaterialManager::GetInstance()->GetMaterial("Mat_LambertCookTorrance_Iron")));

	scene1.AddObject(new Triangle(Elite::FPoint3(-.75f, 1.5f, 0.f), Elite::FPoint3(-.75f, 0.f, 0.f), Elite::FPoint3(.75f, 0.f, 0.f), Elite::MakeTranslation(Elite::FVector3(-2.5f, 6.f, 0.f)), MaterialManager::GetInstance()->GetMaterial("Mat_LambertPhong_Green"), CullMode::FRONTFACE));
	scene1.AddObject(new Triangle(Elite::FPoint3(-.75f, 1.5f, 0.f), Elite::FPoint3(-.75f, 0.f, 0.f), Elite::FPoint3(.75f, 0.f, 0.f), Elite::MakeTranslation(Elite::FVector3(0.f, 6.f, 0.f)), MaterialManager::GetInstance()->GetMaterial("Mat_LambertPhong_Purple"), CullMode::BACKFACE));
	scene1.AddObject(new Triangle(Elite::FPoint3(-.75f, 1.5f, 0.f), Elite::FPoint3(-.75f, 0.f, 0.f), Elite::FPoint3(.75f, 0.f, 0.f), Elite::MakeTranslation(Elite::FVector3(2.5f, 6.f, 0.f)), MaterialManager::GetInstance()->GetMaterial("Mat_LambertPhong_Red")));

	//SceneGraph::GetInstance()->AddObject(new Sphere(Elite::MakeTranslation(Elite::FVector3(-2.5f, 7.f, 0.f)), 1.f, MaterialManager::GetInstance()->GetMaterial("Mat_LambertPhong_Green")));
	//SceneGraph::GetInstance()->AddObject(new Sphere(Elite::MakeTranslation(Elite::FVector3(0.f, 7.f, 0.f)), 1.f, MaterialManager::GetInstance()->GetMaterial("Mat_LambertPhong_Purple")));
	//SceneGraph::GetInstance()->AddObject(new Sphere(Elite::MakeTranslation(Elite::FVector3(2.5f, 7.f, 0.f)), 1.f, MaterialManager::GetInstance()->GetMaterial("Mat_LambertPhong_Red")));

	scene1.AddObject(new Plane(Elite::MakeTranslation(Elite::FVector3(0.f, 0.f, 0.f)), MaterialManager::GetInstance()->GetMaterial("Mat_LambertCookTorrance_Blue")));
	scene1.AddObject(new Plane(Elite::FMatrix4{ Elite::MakeRotationX(float(E_PI_DIV_2)), Elite::FVector3(0.f, 0.f, -6.f) }, MaterialManager::GetInstance()->GetMaterial("Mat_LambertCookTorrance_Blue")));
	scene1.AddObject(new Plane(Elite::FMatrix4{ Elite::MakeRotationZ(float(E_PI_DIV_2)), Elite::FVector3(5.f, 0.f, 0.f) }, MaterialManager::GetInstance()->GetMaterial("Mat_LambertCookTorrance_Blue")));
	scene1.AddObject(new Plane(Elite::FMatrix4{ Elite::MakeRotationZ(float(-E_PI_DIV_2)), Elite::FVector3(-5.f, 0.f, 0.f) }, MaterialManager::GetInstance()->GetMaterial("Mat_LambertCookTorrance_Blue")));

	scene1.AddLight(new PointLight(Elite::FPoint3(0.f, 8.f, -2.f), 25.f, Elite::RGBColor(1.f, 0.84f, 0.77f)));
	scene1.AddLight(new PointLight(Elite::FPoint3(0.f, 5.f, 6.f), 50.f, Elite::RGBColor(1.f, 1.f, 1.f)));
	scene1.AddLight(new DirectionalLight(Elite::FVector3(0.f, -1.f, 0.f), 1.f, Elite::RGBColor(0.79f, 1.f, 1.f)));

	SceneManager::GetInstance()->AddScene(std::move(scene1));
}

void InitScene2()
{
	SceneGraph scene2{};
	scene2.AddObject(new TriangleMesh("Resources/lowpoly_bunny.obj", Elite::MakeTranslation(Elite::FVector3(0.f, 0.f, 0.f)), MaterialManager::GetInstance()->GetMaterial("Mat_LambertCookTorrance_Gold"), true));

	scene2.AddObject(new Plane(Elite::MakeTranslation(Elite::FVector3(0.f, 0.f, 0.f)), MaterialManager::GetInstance()->GetMaterial("Mat_LambertCookTorrance_Blue")));
	scene2.AddObject(new Plane(Elite::FMatrix4{ Elite::MakeRotationX(float(E_PI_DIV_2)), Elite::FVector3(0.f, 0.f, -6.f) }, MaterialManager::GetInstance()->GetMaterial("Mat_LambertCookTorrance_Blue")));
	scene2.AddObject(new Plane(Elite::FMatrix4{ Elite::MakeRotationZ(float(E_PI_DIV_2)), Elite::FVector3(5.f, 0.f, 0.f) }, MaterialManager::GetInstance()->GetMaterial("Mat_LambertCookTorrance_Blue")));
	scene2.AddObject(new Plane(Elite::FMatrix4{ Elite::MakeRotationZ(float(-E_PI_DIV_2)), Elite::FVector3(-5.f, 0.f, 0.f) }, MaterialManager::GetInstance()->GetMaterial("Mat_LambertCookTorrance_Blue")));

	scene2.AddLight(new PointLight(Elite::FPoint3(0.f, 8.f, -2.f), 25.f, Elite::RGBColor(1.f, 0.84f, 0.77f)));
	scene2.AddLight(new PointLight(Elite::FPoint3(0.f, 5.f, 6.f), 50.f, Elite::RGBColor(1.f, 1.f, 1.f)));
	scene2.AddLight(new DirectionalLight(Elite::FVector3(0.f, -1.f, 0.f), 1.f, Elite::RGBColor(0.79f, 1.f, 1.f)));

	SceneManager::GetInstance()->AddScene(std::move(scene2));
}

void DisplayInfo()
{
	std::cout << "----------RAYTRACER----------\n";
	std::cout << "	WASD + Left/right mouse button down: moving camera\n";
	std::cout << "	Left/Right mouse button down: Rotating camera\n";
	std::cout << "	J: Toggle directional lights\n";
	std::cout << "	K: Toggle Point Light 1\n";
	std::cout << "	L: Toggle Point Light 2\n";
	std::cout << "	J: Toggle Hard Shadows\n";
	std::cout << "	T: Toggle All/BRDF only/Light Only\n";
	std::cout << "	space: switch scene\n";
}

int main(int argc, char* args[])
{
	//Unreferenced parameters
	(void)argc;
	(void)args;

	//Create window + surfaces
	SDL_Init(SDL_INIT_VIDEO);

	const uint32_t width = 640;
	const uint32_t height = 480;
	SDL_Window* pWindow = SDL_CreateWindow(
		"RayTracer - Thomas Vincent",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		width, height, 0);

	if (!pWindow)
		return 1;

	//Initialize "framework"
	Elite::Timer* pTimer = new Elite::Timer();
	Elite::Renderer* pRenderer = new Elite::Renderer(pWindow);
	PerspectiveCamera* pCamera = new PerspectiveCamera(Elite::FPoint3(0.f, 3.f, 10.f), Elite::FVector3(0.f, 0.f, 1.f), float(width) / height, 45.f);
	InitMaterials();
	InitScene1();
	InitScene2();
	DisplayInfo();

	//Start loop
	pTimer->Start();
	float printTimer = 0.f;
	bool isLooping = true;
	bool takeScreenshot = false;
	while (isLooping)
	{
		//--------- Get input events ---------
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				isLooping = false;
				break;
			case SDL_KEYDOWN:
				ProjectSettings::GetInstance()->OnKeyDownEvent(e);
				break;
			case SDL_KEYUP:
				if(e.key.keysym.scancode == SDL_SCANCODE_X)
					takeScreenshot = true;
				break;
			}
		}
		//---------CameraUpdate----------//
		pCamera->Update(pTimer->GetElapsed());

		//---------SceneUpdate----------//
		SceneManager::GetInstance()->GetActiveScene().Update(pTimer->GetElapsed());

		//--------- Render ---------
		pRenderer->Render(pCamera);

		//--------- Timer ---------
		pTimer->Update();
		//printTimer += pTimer->GetElapsed();
		//if (printTimer >= 1.f)
		//{
		//	printTimer = 0.f;
		//	std::cout << "FPS: " << pTimer->GetFPS() << std::endl;
		//}

		//Save screenshot after full render
		if (takeScreenshot)
		{
			if (!pRenderer->SaveBackbufferToImage())
				std::cout << "Screenshot saved!" << std::endl;
			else
				std::cout << "Something went wrong. Screenshot not saved!" << std::endl;
			takeScreenshot = false;
		}
	}
	pTimer->Stop();

	//Shutdown "framework"
	delete pCamera;
	delete pRenderer;
	delete pTimer;

	ShutDown(pWindow);
	return 0;
}