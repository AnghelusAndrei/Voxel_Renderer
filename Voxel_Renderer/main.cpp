#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_mutex.h"
#include "SDL2/SDL_ttf.h"
#include <cstdint>
#include <cstdlib>
#include <iostream>
#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>
#include <fstream>
#include <strstream>
#include <string>
#include <algorithm>
#include <list>
#include "engine.h"

const uint32_t SCREEN_HEIGHT = 600;
const uint32_t SCREEN_WIDTH = 800;
const uint32_t res_x = 60;
const uint32_t res_y = 60;
SDL_atomic_t running;
const int FOV = 60;

const float PLAYER_SPEED = 0.000002;
cam Camera;

bool FIRST_FRAME_END = false;
int START_ANGLEX;

bool KEY_W = false;
bool KEY_A = false;
bool KEY_S = false;
bool KEY_D = false;
bool KEY_SPACE = false;
bool KEY_SHIFT = false;

bool FULLSCREEN = false;





void Input(SDL_Event e)
{
        if(KEY_W){
                Camera.pos.x += cos((FixAng(Camera.rot)) * M_PI / 180) * PLAYER_SPEED;
                Camera.pos.y += sin((FixAng(Camera.rot)) * M_PI / 180) * PLAYER_SPEED;
        }
        if(KEY_A){
                Camera.pos.x += cos((FixAng(Camera.rot-90)) * M_PI / 180) * PLAYER_SPEED;
                Camera.pos.y += sin((FixAng(Camera.rot-90)) * M_PI / 180) * PLAYER_SPEED;
        }
        if(KEY_S){
                Camera.pos.x += cos((FixAng(Camera.rot-180)) * M_PI / 180) * PLAYER_SPEED;
                Camera.pos.y += sin((FixAng(Camera.rot-180)) * M_PI / 180) * PLAYER_SPEED;
        }
        if(KEY_D){
                Camera.pos.x += cos((FixAng(Camera.rot+90)) * M_PI / 180) * PLAYER_SPEED;
                Camera.pos.y += sin((FixAng(Camera.rot+90)) * M_PI / 180) * PLAYER_SPEED;
        }
        if(KEY_SPACE){
                Camera.pos.z += PLAYER_SPEED;
        }
        if(KEY_SHIFT){
                Camera.pos.z -= PLAYER_SPEED;
        }


    
        
        if (e.type == SDL_KEYDOWN)
        {

            if(e.key.keysym.sym == SDLK_w){
                KEY_W = true;
            }
            if(e.key.keysym.sym == SDLK_a){
                KEY_A = true;
            }
            if(e.key.keysym.sym == SDLK_s){
                KEY_S = true;
            }
            if(e.key.keysym.sym == SDLK_d){
                KEY_D = true;
            }
            if(e.key.keysym.sym == 32){
                KEY_SPACE = true;
            }
            if(e.key.keysym.sym == 1073742049){
                KEY_SHIFT = true;
            }
        }
        else if(e.type == SDL_KEYUP){
            if(e.key.keysym.sym == SDLK_w){
                KEY_W = false;
            }
            if(e.key.keysym.sym == SDLK_a){
                KEY_A = false;
            }
            if(e.key.keysym.sym == SDLK_s){
                KEY_S = false;
            }
            if(e.key.keysym.sym == SDLK_d){
                KEY_D = false;
            }
            if(e.key.keysym.sym == SDLK_d){
                KEY_D = false;
            }
            if(e.key.keysym.sym == 32){
                KEY_SPACE = false;
            }
            if(e.key.keysym.sym == 1073742049){
                KEY_SHIFT = false;
            }
            /*if(e.key.keysym.sym == SDLK_ESCAPE){
                SDL_SetWindowFullscreen(window, 0);
                SDL_SetWindowSize(window, 800, 600);
                SDL_ShowCursor(1);
                FULLSCREEN = false;
            }
            if(e.key.keysym.sym == SDLK_f){
                SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
                SDL_ShowCursor(0);
                FULLSCREEN = true;
            }*/
        }
        else if (e.type == SDL_MOUSEMOTION)
        {   

            if(FIRST_FRAME_END == false){
                FIRST_FRAME_END = true;
                START_ANGLEX = e.motion.x;
            }
            Camera.rot = FixAng(e.motion.x-START_ANGLEX);

            /*if(FULLSCREEN){
                if(e.motion.x > SCREEN_WIDTH-10){
                    START_ANGLEX = 10;
                    SDL_WarpMouseInWindow(window, 10, e.motion.y);
                }
                if(e.motion.x < 10){
                    START_ANGLEX = SCREEN_WIDTH-10;
                    SDL_WarpMouseInWindow(window, SCREEN_WIDTH-10, e.motion.y);
                }
            }*/
        }  
}


static int RenderEngine(void *data){
    SDL_Window *window;
    SDL_Renderer *renderer;
    window = SDL_CreateWindow("SDL Render Window", 200, 200, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    SDL_ShowWindow(window);


    SDL_Color* map = new SDL_Color[10*10*10];
    for(int z = 0; z < 10; z++){
        for(int y = 0; y < 10; y++){
            for(int x = 0; x < 10; x++){
                map[z*100+y*10+x] = (SDL_Color){(Uint8)(rand()%255), (Uint8)(rand()%255), (Uint8)(rand()%255)};
            }
        }
    }

    while (SDL_AtomicGet(&running))
    {

        float start_time = SDL_GetTicks();


        for(int y = 0; y < SCREEN_HEIGHT; y+=SCREEN_HEIGHT/res_y){
            for(int x = 0; x < SCREEN_WIDTH; x+=SCREEN_WIDTH/res_x){
                SDL_Color pixel = FindPixel(x, y, FOV, map, 10, Camera, SCREEN_HEIGHT, SCREEN_WIDTH);

                SDL_SetRenderDrawColor(renderer, pixel.r, pixel.g, pixel.b, pixel.a);
                SDL_Rect pixel_rect{x, y, SCREEN_WIDTH/res_x, SCREEN_HEIGHT/res_y};
                SDL_RenderFillRect(renderer, &pixel_rect);
            }
        }

        float end_time = SDL_GetTicks();
        float FPS = 1000/(end_time-start_time);
        SDL_RenderPresent(renderer);
    }

    return 0;
}


int main()
{

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

        Input(e);
    }

    int status;
    SDL_WaitThread(RenderingThread, &status);

    return 0;
}