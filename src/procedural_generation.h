#pragma once
#include "../utils/PerlinNoise.hpp"
#include <iostream>
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

void GeneratePerlin(cl_int4 map[], int grid, siv::PerlinNoise::seed_type seed, int density, int octaves, cl_int3 ch_pos){
    const siv::PerlinNoise perlin{ seed };
    if(density<100)density=100;
    if(density>200)density=200;

    for(int k=ch_pos.z;k<ch_pos.z+grid;k++){
        for(int i=ch_pos.y;i<ch_pos.y+grid;i++){
            for(int j=ch_pos.x;j<ch_pos.x+grid;j++){
                int x=j,y=i,z=k;
                int noiseData = (int)(perlin.octave3D_01((x*0.01), (y*0.01), (z*0.01), octaves)*255);
                if(noiseData<density){
                    map[z*grid*grid+y*grid+x].x = (int)(perlin.octave3D_01((x*0.01), (y*0.01), (z*0.02), octaves)*255);
                    map[z*grid*grid+y*grid+x].y = (int)(perlin.octave3D_01((x*0.01), (y*0.02), (z*0.01), octaves)*255);
                    map[z*grid*grid+y*grid+x].z = (int)(perlin.octave3D_01((x*0.02), (y*0.01), (z*0.01), octaves)*255);
                }
            }
        }
    }
    
}