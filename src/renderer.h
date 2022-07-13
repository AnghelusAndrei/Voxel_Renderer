#pragma once
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_mutex.h"
#include "SDL2/SDL_ttf.h"
#include <bits/stdc++.h>
#include <strstream>
#include "../utils/Cl_utils.h"
#include "./Input.h"
#include "procedural_generation.h"

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif
#include <sys/stat.h>

#define PROGRAM_FILE "./src/pixel.cl"
#define FONT_FILE "./assets/font.ttf"
#define KERNEL_FUNC "compute_pixels_kernel"

#define SCREEN_HEIGHT 600
#define SCREEN_WIDTH 800

#define res_x 600
#define res_y 450

SDL_atomic_t running;
const int FOV = 90;

Octree octree;
Debug console;
CameraHandler Camera;




static int RenderEngine(void *data){
    double start_time = SDL_GetTicks();

    SDL_Window *window;
    SDL_Renderer *renderer;
    window = SDL_CreateWindow("SDL Render Window", 200, 200, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_ShowWindow(window);

    std::cout<<"Initializing OpenCL data"<<std::endl;

    
    cl_device_id device;
    cl_context context;
    cl_program program;
    cl_kernel kernel;
    cl_command_queue queue;
    cl_int err,g_mem=octree.n;
    size_t global_size[3] = {res_x, res_y, 0};

    cl_mem ddata, doutput;
 

    device = create_device();
    context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);

    program = build_program(context, device, PROGRAM_FILE);
    queue = clCreateCommandQueue(context, device, 0, &err);


    ddata = clCreateBuffer(context, CL_MEM_READ_ONLY, (size_t)(octree.OCTREE_LENGTH)*sizeof(cl_uint4), NULL, &err); CheckErr(err);
    doutput = clCreateBuffer(context, CL_MEM_WRITE_ONLY, res_y*res_x*sizeof(cl_uchar3), NULL, &err); CheckErr(err);

    err = clEnqueueWriteBuffer(queue, ddata, CL_TRUE, 0,(size_t)(octree.OCTREE_LENGTH)*sizeof(cl_uint4), octree.octree, 0, NULL, NULL); CheckErr(err);
    kernel = clCreateKernel(program, KERNEL_FUNC, &err);

    cl_int2 res = {res_x,res_y};

    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&ddata); CheckErr(err);// <=====INPUT
    err = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&doutput); CheckErr(err);// <=====OUTPUT
    err = clSetKernelArg(kernel, 2, sizeof(cl_int), (void *)&g_mem); CheckErr(err);
    err = clSetKernelArg(kernel, 3, sizeof(cl_int), (void *)&FOV); CheckErr(err);

    TTF_Init();
    TTF_Font * font = TTF_OpenFont(FONT_FILE, 50);

    double end_time = SDL_GetTicks();
    double FPS = 1000/(end_time-start_time);
    std::cout<<"Initialized all data in "<<end_time-start_time<<" ms"<<std::endl;

    cl_uchar3* houtput = nullptr;
    SDL_Texture* SCREEN = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, res_x, res_y);

    while (SDL_AtomicGet(&running))
    {

        Camera.StartRenderFrame();
        start_time = SDL_GetTicks();

        cl_float3 pos = Camera.Camera.pos.CL();
        cl_float3 vec = Camera.Camera.vector.CL();

        err = clSetKernelArg(kernel, 4, sizeof(cl_float3), (void *)&pos); CheckErr(err);
        err = clSetKernelArg(kernel, 5, sizeof(cl_float3), (void *)&vec); CheckErr(err);
        err = clSetKernelArg(kernel, 6, sizeof(cl_int), (void *)&console.depth); CheckErr(err);
        

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


        SDL_Color color = {255, 255, 255, 255};
        std::stringstream FPS_TEXT;
        FPS_TEXT<<FPS;
        std::string str = "FPS: "+FPS_TEXT.str();
        SDL_Rect text_rect = {0, 0, 80, 30};
        SDL_Surface * surface = TTF_RenderText_Solid(font, str.c_str(), color);
        SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surface);

        SDL_RenderCopy(renderer, texture, NULL, &text_rect);
        SDL_RenderPresent(renderer);

        Camera.EndRenderFrame();
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