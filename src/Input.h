#pragma once
#include "SDL2/SDL.h"
#include <bits/stdc++.h>
#include "../utils/main_utilities.h"


bool FIRST_FRAME_END = false;
int START_ANGLEX;
int START_ANGLEY;

bool KEY_W = false;
bool KEY_A = false;
bool KEY_S = false;
bool KEY_D = false;
bool KEY_SPACE = false;
bool KEY_SHIFT = false;

bool FULLSCREEN = false;

const float PLAYER_SPEED_CONST= 0.06;
float PLAYER_SPEED = 0.000004;
cam Camera;


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
            START_ANGLEY = e.motion.y;

            Camera.rot = 0;
            Camera.rotz = 0;
        }else{
            Camera.rot += FixAng(e.motion.x - START_ANGLEX);
            float yAng = e.motion.y - START_ANGLEY;
            //Camera.rotz += yAng;
            //Camera.rotz = FixY(Camera.rotz);

            START_ANGLEX = e.motion.x;
            START_ANGLEY = e.motion.y;
        }
            //std::cout<<FixAng(Camera.rot+FOV/2)<<std::endl;

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
