#include "pch.h"

#if defined(_DEBUG)
#include "vld.h"
#endif

#undef main
#include "Renderer.h"

using namespace dae;

void ShutDown(SDL_Window* pWindow)
{
	SDL_DestroyWindow(pWindow);
	SDL_Quit();
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
		"DirectX - ***Bouke Weel/ 2GD11***",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		width, height, 0);

	if (!pWindow)
		return 1;

	//Initialize "framework"
	const auto pTimer = new Timer();
	const auto pRenderer = new Renderer(pWindow);

	std::cout << "------------------------------------------\n";
	std::cout << "BOTH\n";
	std::cout << "Toggle DirectX/Software: F1 \n";
	std::cout << "Toggle Rotation: F2 \n";
	std::cout << "Cycle Culling Mode: F9 \n";
	std::cout << "Toggle Uniform Clear Color: F10 \n";
	std::cout << "Toggle Print Fps: F11 \n";
	std::cout << "------------------------------------------\n";
	std::cout << "HARDWARE\n";
	std::cout << "Toggle Fire: F3 \n";
	std::cout << "Cycle Texture Sampling: F4 \n";
	std::cout << "------------------------------------------\n";
	std::cout << "SOFTWARE\n";
	std::cout << "Cycle Shading Mode: F5 \n";
	std::cout << "Toggle NormalMap: F6 \n";
	std::cout << "Toggle DepthBuffer Visualization: F7 \n";
	std::cout << "Toggle BoundingBox Visualization: F8 \n";
	std::cout << "------------------------------------------\n";

	//Start loop
	pTimer->Start();
	float printTimer = 0.f;
	bool isLooping = true;
	bool ShowFps = false;
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
			case SDL_KEYUP:
				if (e.key.keysym.scancode == SDL_SCANCODE_F1)
				{
					pRenderer->ToggleUseDirectX();
				}
				else if (e.key.keysym.scancode == SDL_SCANCODE_F2)
				{
					pRenderer->ToggleRotation();
				}
				else if (e.key.keysym.scancode == SDL_SCANCODE_F3)
				{
					pRenderer->ToggleFireMesh();
				}
				else if (e.key.keysym.scancode == SDL_SCANCODE_F4)
				{
					pRenderer->CycleTextureSampling();
				}
				else if (e.key.keysym.scancode == SDL_SCANCODE_F5)
				{
					pRenderer->CycleShadingMode();
				}
				else if (e.key.keysym.scancode == SDL_SCANCODE_F6)
				{
					pRenderer->ToggleNormalMap();
				}
				else if (e.key.keysym.scancode == SDL_SCANCODE_F7)
				{
					pRenderer->ToggleDepthBuffer();
				}
				else if (e.key.keysym.scancode == SDL_SCANCODE_F8)
				{
					pRenderer->ToggleBoundingBox();
				}
				else if (e.key.keysym.scancode == SDL_SCANCODE_F9)
				{
					pRenderer->CycleCullMode();
				}
				else if (e.key.keysym.scancode == SDL_SCANCODE_F10)
				{
					pRenderer->ToggleUniformClearColor();
				}
				else if (e.key.keysym.scancode == SDL_SCANCODE_F11)
				{
					ShowFps = !ShowFps;

					std::cout << "Show FPS: " << (ShowFps ? "true" : "false") << "\n";
				}
				break;
			default: ;
			}
		}

		//--------- Update ---------
		pRenderer->Update(pTimer);
		//--------- Render ---------
		pRenderer->Render();

		//--------- Timer ---------
		pTimer->Update();
		if(ShowFps)
		{
			printTimer += pTimer->GetElapsed();
			if (printTimer >= 1.f)
			{
				printTimer = 0.f;
				std::cout << "dFPS: " << pTimer->GetdFPS() << std::endl;
			}
		}
	}
	pTimer->Stop();

	//Shutdown "framework"
	delete pRenderer;
	delete pTimer;

	ShutDown(pWindow);
	return 0;
}