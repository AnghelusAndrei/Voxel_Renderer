#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_mutex.h"
#include "SDL2/SDL_ttf.h"
#include <bits/stdc++.h>
#include <strstream>
#define _USE_MATH_DEFINES
#include "../utils/engine.h"
#include "procedural_generation.h"

#define __CL_HAS_ANON_STRUCT__ 1
#define CL_TARGET_OPENCL_VERSION 120
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif
#include <sys/stat.h>

#define PROGRAM_FILE "compute_pixels_kernel.cl"
#define KERNEL_FUNC "compute_pixels_kernel"

#define fatal(text) \
    fprintf(stderr, "Error: %d\n", text);   \
    exit(EXIT_FAILURE);

#define debug(text) fprintf(stderr, text "\n");

#define SCREEN_HEIGHT 600
#define SCREEN_WIDTH 800
#define res_x 400
#define res_y 300

SDL_atomic_t running;
const int FOV = 90;

const float PLAYER_SPEED_CONST= 0.06;
float PLAYER_SPEED = 0.000004;
cam Camera;
const int tree_num = 5;
const int grid = 256;

cl_int3* map= new cl_int3[grid*grid*grid];

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


void CheckErr(cl_int err){
    if(err != CL_SUCCESS){
        fatal(err);
    }
}

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


static int RenderEngine(void *data){
    float start_time = SDL_GetTicks();

    SDL_Window *window;
    SDL_Renderer *renderer;
    window = SDL_CreateWindow("SDL Render Window", 200, 200, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    SDL_ShowWindow(window);


    std::cout<<"Initializing OpenCL structure data"<<std::endl;

    
    cl_device_id device;
    cl_context context;
    cl_program program;
    cl_kernel kernel;
    cl_command_queue queue;
    cl_int err;
    size_t global_size[3] = {res_x, res_y, 0};

    cl_mem ddata, doutput;
 

    device = create_device();
    context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);

    program = build_program(context, device, PROGRAM_FILE);
    queue = clCreateCommandQueue(context, device, 0, &err);


    ddata = clCreateBuffer(context, CL_MEM_READ_ONLY, grid*grid*grid*sizeof(cl_int3), NULL, &err); CheckErr(err);
    doutput = clCreateBuffer(context, CL_MEM_WRITE_ONLY, res_y*res_x*sizeof(cl_uchar3), NULL, &err); CheckErr(err);

    err = clEnqueueWriteBuffer(queue, ddata, CL_TRUE, 0, grid*grid*grid*sizeof(cl_int3), map, 0, NULL, NULL); CheckErr(err);
    kernel = clCreateKernel(program, KERNEL_FUNC, &err);

    cl_int2 res = {res_x,res_y};
    cl_float rr = (float)SCREEN_HEIGHT/SCREEN_WIDTH;

    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&ddata); CheckErr(err);// <=====INPUT
    err = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&doutput); CheckErr(err);// <=====OUTPUT
    err = clSetKernelArg(kernel, 2, sizeof(int), (void *)&grid); CheckErr(err);
    err = clSetKernelArg(kernel, 3, sizeof(int), (void *)&FOV); CheckErr(err);

    TTF_Init();
    TTF_Font * font = TTF_OpenFont("../utils/font.ttf", 50);

    float end_time = SDL_GetTicks();
    float FPS = 1000/(end_time-start_time);
    std::cout<<"Initialized all data structures in "<<end_time-start_time<<" ms"<<std::endl;

    cl_uchar3* houtput = nullptr;
    SDL_Texture* SCREEN = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, res_x, res_y);

    while (SDL_AtomicGet(&running))
    {

        start_time = SDL_GetTicks();

        cl_float3 pos = {Camera.pos.x,Camera.pos.y,Camera.pos.z};
        cl_float2 ang = {Camera.rot,Camera.rotz};

        err = clSetKernelArg(kernel, 4, sizeof(cl_float3), (void *)&pos); CheckErr(err);
        err = clSetKernelArg(kernel, 5, sizeof(cl_float2), (void *)&ang); CheckErr(err);
        

        //rendering
        int pitch = 0;
        SDL_LockTexture(SCREEN, NULL, reinterpret_cast<void**>(&houtput), &pitch);

        err = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, global_size, NULL, 0, NULL, NULL); CheckErr(err);
        err = clFinish(queue); CheckErr(err);
        err = clEnqueueReadBuffer(queue, doutput, CL_TRUE, 0, res_x*res_y*sizeof(cl_uchar3), houtput, 0, NULL, NULL); CheckErr(err);

        SDL_UnlockTexture(SCREEN);
        SDL_RenderCopy(renderer, SCREEN, NULL, NULL);
        //----------


        end_time = SDL_GetTicks();
        int FPS = 1000/(end_time-start_time);

        PLAYER_SPEED = (end_time-start_time)*PLAYER_SPEED_CONST;

        SDL_Color color = {255, 255, 255, 255};
        std::stringstream FPS_TEXT;
        FPS_TEXT<<FPS;
        std::string str = "FPS: "+FPS_TEXT.str();
        SDL_Rect text_rect = {0, 0, 80, 30};
        SDL_Surface * surface = TTF_RenderText_Solid(font, str.c_str(), color);
        SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surface);

        SDL_RenderCopy(renderer, texture, NULL, &text_rect);
        SDL_RenderPresent(renderer);
    }

    clReleaseKernel(kernel);
    clReleaseMemObject(ddata);
    clReleaseMemObject(doutput);
    clReleaseCommandQueue(queue);
    clReleaseProgram(program);
    clReleaseContext(context);

    TTF_CloseFont(font);
    TTF_Quit();

    return 0;
}


int main()
{

    int density, octaves;
    siv::PerlinNoise::seed_type seed;

    std::cout<<"Please insert a seed value: "; std::cin>>seed;
    std::cout<<"Please insert the number of noise octaves (must be between 1 and 10): "; std::cin>>octaves;
    std::cout<<"Please insert a density value (must be between 0 and 255): "; std::cin>>density;

    std::cout<<"Loading Map"<<std::endl;

    GeneratePerlin(map, grid, seed, density, octaves);


    std::cout<<"Initialozing threads & kernel"<<std::endl;

    SDL_AtomicSet(&running, 1);
    SDL_Thread *RenderingThread;
    RenderingThread = SDL_CreateThread(RenderEngine, "render_engine", NULL);
    SDL_Event e;

    while (SDL_PollEvent(&e) || true)
    {

        float start_time = SDL_GetTicks();
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