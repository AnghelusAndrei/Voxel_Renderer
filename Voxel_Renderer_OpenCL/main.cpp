#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_mutex.h"
#include "SDL2/SDL_ttf.h"
#include <bits/stdc++.h>
#define _USE_MATH_DEFINES
#include "engine.h"

#define __CL_HAS_ANON_STRUCT__ 1
#define CL_TARGET_OPENCL_VERSION 120
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif
#include <sys/stat.h>

#define PROGRAM_FILE "compute_pixels_kernel.cl"
#define KERNEL_FUNC "compute_pixels"

#define fatal(text) \
    printf(text);   \
    exit(EXIT_FAILURE);

#define debug(text) fprintf(stderr, text "\n");

const uint32_t SCREEN_HEIGHT = 600;
const uint32_t SCREEN_WIDTH = 800;
const uint32_t res_x = 100;
const uint32_t res_y = 100;
SDL_atomic_t running;
const int FOV = 60;

const float PLAYER_SPEED = 0.000004;
cam Camera;
int grid = 20;

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


cl_device_id create_device() {

   cl_platform_id platform;
   cl_device_id dev;
   int err;

   /* Identify a platform */
   err = clGetPlatformIDs(1, &platform, NULL);
   if(err < 0) {
      perror("Couldn't identify a platform");
      exit(1);
   } 

   // Access a device
   // GPU
   err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &dev, NULL);
   if(err == CL_DEVICE_NOT_FOUND) {
      // CPU
      err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &dev, NULL);
   }
   if(err < 0) {
      perror("Couldn't access any devices");
      exit(1);   
   }

   return dev;
}
cl_program build_program(cl_context ctx, cl_device_id dev, const char* filename) {

   cl_program program;
   FILE *program_handle;
   char *program_buffer, *program_log;
   size_t program_size, log_size;
   int err;

   program_handle = fopen(filename, "r");
   if(program_handle == NULL) {
      perror("Couldn't find the program file");
      exit(1);
   }
   fseek(program_handle, 0, SEEK_END);
   program_size = ftell(program_handle);
   rewind(program_handle);
   program_buffer = (char*)malloc(program_size + 1);
   program_buffer[program_size] = '\0';
   fread(program_buffer, sizeof(char), program_size, program_handle);
   fclose(program_handle);

   program = clCreateProgramWithSource(ctx, 1, 
      (const char**)&program_buffer, &program_size, &err);
   if(err < 0) {
      perror("Couldn't create the program");
      exit(1);
   }
   free(program_buffer);

   err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
   if(err < 0) {

      clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, 
            0, NULL, &log_size);
      program_log = (char*) malloc(log_size + 1);
      program_log[log_size] = '\0';
      clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, 
            log_size + 1, program_log, NULL);
      printf("%s\n", program_log);
      free(program_log);
      exit(1);
   }

   return program;
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
                START_ANGLEX = e.motion.y;
            }
            Camera.rot = FixAng(e.motion.x-START_ANGLEX);
            //Camera.rotz = FixAng(e.motion.y-START_ANGLEY);
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

    int density;

    std::cout<<"Please insert a density value: "; std::cin>>density;
    std::cout<<std::endl<<"Initializing Renderer & SDL Render Window"<<std::endl;
    float start_time = SDL_GetTicks();

    SDL_Window *window;
    SDL_Renderer *renderer;
    window = SDL_CreateWindow("SDL Render Window", 200, 200, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
    SDL_ShowWindow(window);


    std::cout<<"Loading Map"<<std::endl;

    cl_int4* map= new cl_int4[grid*grid*grid];
    for(int z = 0; z < grid; z++){
        for(int y = 0; y < grid; y++){
            for(int x = 0; x < grid; x++){
                if(rand()%density==1){
                    map[z*grid*grid+y*grid+x] = (cl_int4){(rand()%255), (rand()%255), (rand()%255)};
                }
            }   
        }
    }


    std::cout<<"Initializing OpenCL structure data"<<std::endl;

    
    cl_device_id device;
    cl_context context;
    cl_program program;
    cl_kernel kernel;
    cl_command_queue queue;
    cl_int i, err;
    size_t global_size[3] = {res_x, res_y, 0};

    cl_mem ddata, doutput;
 

    device = create_device();
    context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);

    program = build_program(context, device, PROGRAM_FILE);
    queue = clCreateCommandQueue(context, device, 0, &err);

    ddata = clCreateBuffer(context, CL_MEM_READ_ONLY, grid*grid*grid*sizeof(cl_int4), NULL, NULL);
    doutput = clCreateBuffer(context, CL_MEM_WRITE_ONLY, SCREEN_HEIGHT*SCREEN_WIDTH*sizeof(cl_int4), NULL, NULL);

    err = clEnqueueWriteBuffer(queue, ddata, CL_TRUE, 0, grid*grid*grid*sizeof(cl_int4), map, 0, NULL, NULL);
    kernel = clCreateKernel(program, KERNEL_FUNC, &err);

    cl_int2 res = {res_x,res_y};
    cl_float3 pos = {Camera.pos.x,Camera.pos.y,Camera.pos.z};
    cl_float2 ang = {Camera.rot,Camera.rotz};

    //astea la initializare
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &ddata); // <=====INPUT
    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &doutput); // <=====OUTPUT
    err |= clSetKernelArg(kernel, 2, sizeof(int), &grid);
    err |= clSetKernelArg(kernel, 3, sizeof(cl_int2), &res);
    err |= clSetKernelArg(kernel, 4, sizeof(int), &SCREEN_WIDTH);
    err |= clSetKernelArg(kernel, 5, sizeof(int), &SCREEN_HEIGHT);
    //astea doua pe frame
    err |= clSetKernelArg(kernel, 6, sizeof(cl_float3), &pos);
    err |= clSetKernelArg(kernel, 7, sizeof(cl_float2), &ang);

    TTF_Init();
    TTF_Font * font = TTF_OpenFont("arial.ttf", 50);

    float end_time = SDL_GetTicks();
    float FPS = 1000/(end_time-start_time);
    std::cout<<"Initialized all data structures in "<<end_time-start_time<<" ms"<<std::endl;



    while (SDL_AtomicGet(&running))
    {

        start_time = SDL_GetTicks();


        for(float y = 0; y < SCREEN_HEIGHT; y+=SCREEN_HEIGHT/res_y){
            for(float x = 0; x < SCREEN_WIDTH; x+=SCREEN_WIDTH/res_x){
                cl_int4 pixel = FindPixel(x, y, FOV, map, grid, Camera, SCREEN_HEIGHT, SCREEN_WIDTH, res_x, res_y, density);

                SDL_SetRenderDrawColor(renderer, pixel.x, pixel.y, pixel.z, 255);
                SDL_Rect pixel_rect{x, y, SCREEN_WIDTH/res_x, SCREEN_HEIGHT/res_y};
                SDL_RenderFillRect(renderer, &pixel_rect);
            }
        }

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
    std::cout<<"Initialozing threads & kernel"<<std::endl;

    SDL_AtomicSet(&running, 1);
    SDL_Thread *RenderingThread;
    RenderingThread = SDL_CreateThread(RenderEngine, "render_engine", NULL);
    SDL_Event e;
    Camera.pos={float(grid),float(grid),float(grid)};

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