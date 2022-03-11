#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_mutex.h"
#include "SDL2/SDL_ttf.h"
#include <bits/stdc++.h>
#include <strstream>

#include "../utils/main_utilities.h"
#include "./Input.h"
#include "procedural_generation.h"
#include "renderer.h"



int main()
{

    int density, octaves;
    siv::PerlinNoise::seed_type seed;

    std::cout<<"Please insert a seed value: "; std::cin>>seed;
    std::cout<<"Please insert the number of noise octaves (must be between 1 and 10): "; std::cin>>octaves;
    std::cout<<"Please insert a density value (must be between 0 and 255): "; std::cin>>density;

    float start_time = SDL_GetTicks();
    std::cout<<"Loading Map"<<std::endl;

    GeneratePerlin(world[0], grid, seed, density, octaves, {0,0,0});

    std::cout<<"Loaded map in "<<(float)SDL_GetTicks()-start_time<<"ms"<<std::endl;
    std::cout<<"Initialozing threads & kernel"<<std::endl;

    SDL_AtomicSet(&running, 1);
    SDL_Thread *RenderingThread;
    RenderingThread = SDL_CreateThread(RenderEngine, "render_engine", NULL);
    SDL_Event e;

    while (SDL_PollEvent(&e) || true)
    {

        start_time = SDL_GetTicks();
        if (e.type == SDL_QUIT)
        {
            SDL_AtomicSet(&running, 0);
            break;
        }
            
        Input(e);
        float end_time = SDL_GetTicks();

        PLAYER_SPEED = (end_time-start_time)*PLAYER_SPEED_CONST;
    }

    int status;
    SDL_WaitThread(RenderingThread, &status);

    return 0;
}