#pragma once
#include "../utils/PerlinNoise.hpp"


class Debug{
    private:
        float start_time;

    public:
        int density;
        int octaves;
        siv::PerlinNoise::seed_type seed;
        int depth;

        int RenderStatus;


        void GetData();
        void StartTime();
        void ShowMs();
        float GetTime();
};

void Debug::GetData(){
    std::cout<<"Please insert a seed value: "; std::cin>>seed;
    std::cout<<"Please insert the number of noise octaves (must be between 1 and 10): "; std::cin>>octaves;
    std::cout<<"Please insert a density value (must be between 0 and 255): "; std::cin>>density;
    std::cout<<"Please insert a depth value (must be between 0 and 10): "; std::cin>>depth;
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