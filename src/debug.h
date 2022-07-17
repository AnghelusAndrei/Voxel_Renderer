#pragma once
#include "PerlinNoise.hpp"


struct config_{
    int density;
    int octaves;
    siv::PerlinNoise::seed_type seed;
    int depth;
    int height;
};

class Debug{
    private:
        float start_time;

    public:

        config_ config;
        int RenderStatus;


        void GetData3D();
        void GetData2D();
        void StartTime();
        void ShowMs();
        float GetTime();
};

void Debug::GetData3D(){
    std::cout<<"Please insert a seed value: "; std::cin>>config.seed;
    std::cout<<"Please insert the number of noise octaves (must be between 1 and 10): "; std::cin>>config.octaves;
    std::cout<<"Please insert a density value (must be between 0 and 255): "; std::cin>>config.density;
    std::cout<<"Please insert a depth value (must be between 0 and 10): "; std::cin>>config.depth;
}


void Debug::GetData2D(){
    std::cout<<"Please insert a seed value: "; std::cin>>config.seed;
    std::cout<<"Please insert the number of noise octaves (must be between 1 and 10): "; std::cin>>config.octaves;
    std::cout<<"Please insert a height value (must be between 50 and 250): "; std::cin>>config.height;
    std::cout<<"Please insert a depth value (must be between 0 and 10): "; std::cin>>config.depth;
}

void Debug::StartTime(){
    start_time = SDL_GetTicks();
}

void Debug::ShowMs(){
    std::cout<<(float)SDL_GetTicks()-start_time<<" ms"<<std::endl;
}

float Debug::GetTime(){
    return (float)SDL_GetTicks()-start_time;
}