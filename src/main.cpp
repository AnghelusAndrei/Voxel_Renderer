#include "SDL.h"
#include "SDL_mutex.h"
#include "SDL_ttf.h"
#include <bits/stdc++.h>
#include <strstream>

#include "Cl_utils.h"
#include "Input.h"
#include "debug.h"
#include "procedural_generation.h"
#include "renderer.h"
#include "octree.h"
#include "model.h"



int main()
{
    console.GetData2D(config);
    octree.Initialize(config.depth);
    GenerateOctree2D(octree, config);

    Camera.light = Vector(octree.n/2, octree.n/2, config.height);

    SDL_AtomicSet(&running, 1);
    SDL_Thread *RenderingThread;
    RenderingThread = SDL_CreateThread(RenderEngine, "render_engine", NULL);

    SDL_Event e;

    while (true)
    {
        SDL_PollEvent(&e);
        
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