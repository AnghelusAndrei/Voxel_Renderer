#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_mutex.h"
#include "SDL2/SDL_ttf.h"
#include <bits/stdc++.h>
#include <strstream>

#include "../utils/Cl_utils.h"
#include "./Input.h"
#include "debug.h"
#include "procedural_generation.h"
#include "renderer.h"
#include "octree.h"



int main()
{
    
    console.GetData();
    console.StartTime();

    octree.Initialize(console.depth);
    GenerateOctree(octree,console.seed,console.density,console.octaves);

    console.ShowMs();

    

    SDL_AtomicSet(&running, 1);
    SDL_Thread *RenderingThread;
    RenderingThread = SDL_CreateThread(RenderEngine, "render_engine", NULL);
    SDL_Event e;

    while (SDL_PollEvent(&e) || true)
    {

        if (e.type == SDL_QUIT)
        {
            SDL_AtomicSet(&running, 0);
            break;
        }
            
        Camera.Frame(e,console);
    }

    SDL_WaitThread(RenderingThread, &console.RenderStatus);

    return 0;
}