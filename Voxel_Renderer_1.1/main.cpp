#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_mutex.h"
#include <cstdint>
#include <cstdlib>
#include <chrono>
#include <iostream>
#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>
#include "types.h"

    const std::uint64_t SCREEN_HEIGHT = 600; 
    const std::uint64_t SCREEN_WIDTH = 800;

    
const std::uint8_t RAY_RES = 5;  
float square = 50;
float grid = square*MAP_SIZE;
float division_factor = SCREEN_HEIGHT*square;
const float RGBmultiplyer = 1.1;
bool FIRST_FRAME_END = false;
int START_ANGLEX;


bool KEY_W = false;
bool KEY_A = false;
bool KEY_S = false;
bool KEY_D = false;
bool KEY_SPACE = false;
bool KEY_SHIFT = false;


bool FULLSCREEN = false;
bool MouseXWrap = false;
bool MouseYWrap = false;
bool threaded_FS = false;
bool threaded_FS_closed = false;
int mouseXv;
int mouseYv;

int running;

player_t *player;
float FOV = 60;
const float PLAYER_SPEED = 1;


#define EXIT_IF_ERROR(ptr)           \
    if (!ptr)                        \
    {                                \
        std::cerr << SDL_GetError(); \
        return 1;                    \
    }






/*ray_t DDA_X(float ang_x, float ang_y, player_t* current_player, float grid, int offset){
    ray_t xCheck;
    if(ang_x < 90){ //down-right
        xCheck.y = current_player->y+(grid-current_player->y-floor((grid - current_player->y)))+offset;
        xCheck.x = current_player->x+((grid-current_player->y-floor((grid - current_player->y)))+offset)*abs(tan(degToRad(90-ang_x)));
    }else if(ang_x >= 90 && ang_x < 180){ //down-left
        xCheck.y = current_player->y+(grid-current_player->y-floor((grid - current_player->y)))+offset;
        xCheck.x = current_player->x-((grid-current_player->y-floor((grid - current_player->y)))+offset)*abs(tan(degToRad(FixAng(ang_x-90))));
    }else if(ang_x >= 180 && ang_x < 270){ //up-left
        xCheck.y = current_player->y-(current_player->y-floor((current_player->y)))-offset;
        xCheck.x = current_player->x-((current_player->y-floor((current_player->y)))+offset)*abs(tan(degToRad(FixAng(270-ang_x))));
    }else if(ang_x >= 270 && ang_x < 360){ //up-right
        xCheck.y = current_player->y-(current_player->y-floor((current_player->y)))-offset;
        xCheck.x = current_player->x+((current_player->y-floor((current_player->y)))+offset)*abs(tan(degToRad(FixAng(ang_x-270))));
    }

    

    return xCheck;
}*/


/*ray_t DDA_Y(float ang_x, float ang_y, player_t* current_player, float grid, int offset){
    ray_t yCheck;
    if(ang_x < 90){ //down-right
        yCheck.x = current_player->x+(grid-current_player->x-floor((grid - current_player->x)))+offset;
        yCheck.y = current_player->y+(yCheck.x-current_player->x)*abs(tan(degToRad(ang_x)));
    }else if(ang_x >= 90 && ang_x < 180){ //down-left
        yCheck.x = floor((current_player->x))-offset;
        yCheck.y = current_player->y+(current_player->x-floor((current_player->x))+offset)*abs(tan(degToRad(180-ang_x)));
    }else if(ang_x >= 180 && ang_x < 270){ //up-left
        yCheck.x = floor((current_player->x))-offset;
        yCheck.y = current_player->y-(current_player->x-floor((current_player->x))+offset)*abs(tan(degToRad(FixAng(ang_x-180))));
    }else if(ang_x >= 270 && ang_x < 360){ //up-right
        yCheck.x = current_player->x+(grid-current_player->x-floor((grid - current_player->x)))+offset;
        yCheck.y = current_player->y-((grid-current_player->x-floor((grid - current_player->x)))+offset)*abs(tan(degToRad(FixAng(360-ang_x))));
    }



    return yCheck;
}*/


void Wall(SDL_Renderer *renderer, player_t *current_player){
        for(float b = 0; b < FOV; b+=FOV/(SCREEN_WIDTH/RAY_RES))
        {
            std::vector<dist_data_t> distArray;

        
            bool found = false;
    
            ray_t ray;
            ray_t yCheck;
            ray_t xCheck;
    
            float ang = FixAng(current_player->mousex-FOV/2+b);
    
            bool xFound = false;
            bool yFound = false;
            
    
            for(int offset = 0; offset < 7 && found == false; offset++){
                ray_t tile;
                tile.x = offset*(square);
                tile.y = offset*(square);
                int wall;
                
                //DDA
            
                if(ang < 90){ //down-right
                    yCheck.x = current_player->x+(grid-current_player->x-floor((grid - current_player->x)/(square))*(square))+tile.x;
                    yCheck.y = current_player->y+(yCheck.x-current_player->x)*abs(tan(degToRad(ang)));
            
                    xCheck.y = current_player->y+(grid-current_player->y-floor((grid - current_player->y)/(square))*(square))+tile.y;
                    xCheck.x = current_player->x+((grid-current_player->y-floor((grid - current_player->y)/(square))*(square))+tile.y)*abs(tan(degToRad(90-ang)));
                }else if(ang >= 90 && ang < 180){ //down-left
                    yCheck.x = floor((current_player->x)/(square))*(square)-tile.x;
                    yCheck.y = current_player->y+(current_player->x-floor((current_player->x)/(square))*(square)+tile.x)*abs(tan(degToRad(180-ang)));
            
                    xCheck.y = current_player->y+(grid-current_player->y-floor((grid - current_player->y)/(square))*(square))+tile.y;
                    xCheck.x = current_player->x-((grid-current_player->y-floor((grid - current_player->y)/(square))*(square))+tile.y)*abs(tan(degToRad(FixAng(ang-90))));
                }else if(ang >= 180 && ang < 270){ //up-left
                    yCheck.x = floor((current_player->x)/(square))*(square)-tile.x;
                    yCheck.y = current_player->y-(current_player->x-floor((current_player->x)/(square))*(square)+tile.x)*abs(tan(degToRad(FixAng(ang-180))));
            
                    xCheck.y = current_player->y-(current_player->y-floor((current_player->y)/(square))*(square))-tile.y;
                    xCheck.x = current_player->x-((current_player->y-floor((current_player->y)/(square))*(square))+tile.y)*abs(tan(degToRad(FixAng(270-ang))));
                }else if(ang >= 270 && ang < 360){ //up-right
                    yCheck.x = current_player->x+(grid-current_player->x-floor((grid - current_player->x)/(square))*(square))+tile.x;
                    yCheck.y = current_player->y-((grid-current_player->x-floor((grid - current_player->x)/(square))*(square))+tile.x)*abs(tan(degToRad(FixAng(360-ang))));
            
                    xCheck.y = current_player->y-(current_player->y-floor((current_player->y)/(square))*(square))-tile.y;
                    xCheck.x = current_player->x+((current_player->y-floor((current_player->y)/(square))*(square))+tile.y)*abs(tan(degToRad(FixAng(ang-270))));
                }
        
                
                //check


                int yStart = floor(current_player->y/square)-5;
                int yEnd = floor(current_player->y/square)+5;
                int xStart = floor(current_player->x/square)-5;
                int xEnd = floor(current_player->x/square)+5;
                if(yStart < 0){
                    yStart = 0;
                }

                if(yStart > 20){
                    yStart = 20;
                }

                if(yEnd < 0){
                    yEnd = 0;
                }

                if(yEnd > 20){
                    yEnd = 20;
                }

                if(xStart < 0){
                    xStart = 0;
                }

                if(xStart > 20){
                    xStart = 20;
                }

                if(xEnd < 0){
                    xEnd = 0;
                }

                if(xEnd > 20){
                    xEnd = 20;
                }

                for(int i=yStart; i<yEnd; i++)
                {
                    for(int j=xStart;j<xEnd;j++)
                    {
                        int colorSUM = map[i][j].r+map[i][j].g+map[i][j].b;
                        if(colorSUM > 153){
                            wall_t wall;
                            wall.x = square * j;
                            wall.y = square * i;
                            wall.height = square;
                            wall.width = square;
                            wall.color = map[i][j];

                            dist_data_t dist_data_x;
                            dist_data_x.dist = 9999;
                            dist_data_x.wall = wall;
                            dist_data_x.f = 20;
                            dist_data_x.b = b;

                            dist_data_t dist_data_y;
                            dist_data_y.dist = 9998;
                            dist_data_y.wall = wall;
                            dist_data_y.f = 0;
                            dist_data_y.b = b;

                            ray_t ray_x;
                            ray_t ray_y;

                            if(pointInside(wall.x, wall.y, xCheck.x, xCheck.y)){
                                ray_x.x = xCheck.x;
                                ray_x.y = xCheck.y;
                                int faceY;
                                if(xCheck.y>wall.y){
                                    faceY = square+1;
                                }else{
                                    faceY = square*(-1)+1;
                                }
                                
                                dist_data_x.dist = Distance(current_player->x,ray_x.x,current_player->y,ray_x.y);
                                dist_data_x.wall = wall;
                                dist_data_x.f = 0;
                                dist_data_x.b = b;
                                
                                xFound = true;
                            }
                            if(pointInside(wall.x, wall.y, yCheck.x, yCheck.y)){
                                ray_y.x = yCheck.x;
                                ray_y.y = yCheck.y;
                                int faceX;
                                if(yCheck.x>wall.x){
                                    faceX = square+1;
                                }else{
                                    faceX = square*(-1)+1;
                                }

                                dist_data_y.dist = Distance(current_player->x,ray_y.x,current_player->y,ray_y.y);
                                dist_data_y.wall = wall;
                                dist_data_y.f = 20;
                                dist_data_y.b = b;
                                
                                yFound = true;
                            }

                            if(dist_data_x.dist < dist_data_y.dist){
                                distArray.push_back(dist_data_x);
                            }else{
                                distArray.push_back(dist_data_y);
                            }
                        }
                    }
                }

    
                if(xFound && yFound){
                    found = true;
                }
            }

    
            dist_data_t dist;
            float MIN=999999;
            for(int i=0;i<distArray.size();i++)
            {
                if(distArray[i].dist<MIN) 
                {
                    MIN=distArray[i].dist;
                    dist=distArray[i];
                }
            }

            if(dist.dist < square*5){
                dist.dist *= cos(degToRad(FOV/2-b));
                int Rdist = floor(255-dist.dist/RGBmultiplyer-dist.f);
                if(Rdist<0){
                    Rdist = 0;
                }
                if(Rdist>255){
                    Rdist = 255;
                }

                

                float pos = SCREEN_HEIGHT/2;


                int SCREEN_X;
                int SCREEN_Y;
                int SCREEN_W;
                int SCREEN_H;

                
                SCREEN_X = (int)floor(b*((SCREEN_WIDTH)/FOV));
                SCREEN_Y = (int)pos;
                SCREEN_W = 1*RAY_RES;
                SCREEN_H = 1*RAY_RES;
                
            


                const SDL_Rect pixel{static_cast<int>(SCREEN_X), static_cast<int>(SCREEN_Y), static_cast<int>(SCREEN_W), static_cast<int>(SCREEN_H)};
                SDL_SetRenderDrawColor(renderer, dist.wall.color.r-dist.f-Rdist, dist.wall.color.g-dist.f-Rdist, dist.wall.color.b-dist.f-Rdist, 255);
                SDL_RenderFillRect(renderer, &pixel);
                
            }
        }
}


static int Raycaster(void *data)
{
    SDL_Window *window;
    SDL_Renderer *renderer;


    window = SDL_CreateWindow("SDL Render Window", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);



    SDL_ShowWindow(window);
    std::cout << "created window" << std::endl;


    while (running)
    {
        player_t *current_player = (player_t*)SDL_AtomicGetPtr((void**)&player);

        int start_time = SDL_GetTicks();

        if(threaded_FS){
            SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
            threaded_FS=false;
        }
        if(threaded_FS_closed){
            SDL_SetWindowFullscreen(window, 0);
            SDL_SetWindowSize(window, 800, 600);
            threaded_FS_closed=false;
        }
        if(MouseXWrap){
            SDL_WarpMouseInWindow(window, 10, mouseXv);
            MouseXWrap=false;
        }
        if(MouseYWrap){
            SDL_WarpMouseInWindow(window, SCREEN_WIDTH-10, mouseYv);
            MouseYWrap=false;
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);


        Wall(renderer, current_player);

        SDL_RenderPresent(renderer);

        int end_time = SDL_GetTicks();
        int FPS;
        if(end_time-start_time==0){FPS=999999;}
        else{FPS = 1000/(end_time-start_time);}

    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    
    return 0;
}


int main()
{
    for(int i = 0; i < MAP_SIZE; i++){
        for(int j = 0; j < MAP_SIZE; j++){
            map[i][j].r=r(255);
            map[i][j].g=r(255);
            map[i][j].b=r(255);
            map[i][j].a=255;
        }
    }

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0)
    {
        std::cerr << SDL_GetError() << std::endl;
        return 1;
    }
    std::atexit(SDL_Quit);

    player = (player_t *)malloc(sizeof(player_t));
    player->x = grid/2;
    player->y = grid/2;
    player->z = 25;
    player->mousex = 0;

    running = 1;

    SDL_Thread *raycasterThread;

    raycasterThread = SDL_CreateThread(Raycaster, "raycaster", NULL);

    std::cout << "initialized multithreading" << std::endl;






    SDL_Event e;
    while (SDL_PollEvent(&e) || true)
    {


        player_t *current_player =(player_t*)SDL_AtomicGetPtr((void**)&player);

        if (e.type == SDL_QUIT)
        {

            running = 0;
            break;
        }

        if(current_player->x >= grid || current_player->x <= 0 || current_player->y >= grid || current_player->y <= 0){
            current_player->x = grid/2;
            current_player->y = grid/2;
        }


        if(KEY_W){
                current_player->velocity.x += cos((FixAng(current_player->mousex)) * M_PI / 180) * PLAYER_SPEED;
                current_player->velocity.y += sin((FixAng(current_player->mousex)) * M_PI / 180) * PLAYER_SPEED;
        }
        if(KEY_A){
                current_player->velocity.x += cos((FixAng(current_player->mousex-90)) * M_PI / 180) * PLAYER_SPEED;
                current_player->velocity.y += sin((FixAng(current_player->mousex-90)) * M_PI / 180) * PLAYER_SPEED;
        }
        if(KEY_S){
                current_player->velocity.x += cos((FixAng(current_player->mousex-180)) * M_PI / 180) * PLAYER_SPEED;
                current_player->velocity.y += sin((FixAng(current_player->mousex-180)) * M_PI / 180) * PLAYER_SPEED;
        }
        if(KEY_D){
                current_player->velocity.x += cos((FixAng(current_player->mousex+90)) * M_PI / 180) * PLAYER_SPEED;
                current_player->velocity.y += sin((FixAng(current_player->mousex+90)) * M_PI / 180) * PLAYER_SPEED;
        }
        if(KEY_SPACE){
                current_player->z += 1;
        }
        if(KEY_SHIFT){
                current_player->z -= 1;
        }


        if(current_player->velocity.lockW == false && current_player->velocity.y < 0){
            current_player->y += current_player->velocity.y;
        }
        if(current_player->velocity.lockD == false && current_player->velocity.x > 0){
            current_player->x += current_player->velocity.x;
        }
        if(current_player->velocity.lockS == false && current_player->velocity.y > 0){
            current_player->y += current_player->velocity.y;
        }
        if(current_player->velocity.lockA == false && current_player->velocity.x < 0){
            current_player->x += current_player->velocity.x;
        }

        current_player->velocity.x = 0;
        current_player->velocity.y = 0;

    
        
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
            if(e.key.keysym.sym == SDLK_ESCAPE){
                SDL_ShowCursor(1);
                FULLSCREEN = false;
                threaded_FS_closed = true;
            }
            if(e.key.keysym.sym == SDLK_f){
                SDL_ShowCursor(0);
                FULLSCREEN = true;
                threaded_FS = true;
            }
        }
        else if (e.type == SDL_MOUSEMOTION)
        {   

            if(FIRST_FRAME_END == false){
                FIRST_FRAME_END = true;
                START_ANGLEX = e.motion.x;
            }
            current_player->mousex = FixAng(e.motion.x-START_ANGLEX);
            current_player->mousey = e.motion.y;

            if(FULLSCREEN){
                if(e.motion.x > SCREEN_WIDTH-10){
                    START_ANGLEX = 10;
                    MouseXWrap=true;
                    mouseXv=e.motion.y;
                }
                if(e.motion.x < 10){
                    START_ANGLEX = SCREEN_WIDTH-10;
                    MouseYWrap=true;
                    mouseYv=e.motion.x;
                }
            }
        }


        SDL_AtomicSetPtr((void**)&player, (void*)current_player);
        SDL_Delay(7);

    }

    int status;

    SDL_WaitThread(raycasterThread, &status);

    return 0;
}





//g++ main.cpp -lSDL2 -lSDL2_image --std=c++17
//./a.out