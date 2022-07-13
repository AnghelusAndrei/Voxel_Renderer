#pragma once
#include <bits/stdc++.h>
#include "../src/octree.h"

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

void GenerateOctree(Octree octree, siv::PerlinNoise::seed_type seed, int density, int octaves){
    const siv::PerlinNoise perlin{ seed };
    if(density<100)density=100;
    if(density>200)density=200;
    
    int q = 0;

    for(int k=0;k<octree.n;k++){
        if((int)(((float)k/(float)octree.n)*100)>q){q=(int)(((float)k/(float)octree.n)*100);std::cout<<q<<'%'<<std::endl;}

        for(int i=0;i<octree.n;i++){
            for(int j=0;j<octree.n;j++){
                int x=j,y=i,z=k;
                int noiseData = (int)(perlin.octave3D_01((x*0.01f), (y*0.01f), (z*0.01f), octaves)*255);
                if(noiseData<density){

                    cl_uint3 col = {
                        (int)(perlin.octave3D_01((x*0.01f), (y*0.01f), (z*0.02f), octaves)*255),
                        (int)(perlin.octave3D_01((x*0.01f), (y*0.02f), (z*0.01f), octaves)*255),
                        (int)(perlin.octave3D_01((x*0.02f), (y*0.01f), (z*0.01f), octaves)*255)
                    };
                    
                    octree.Insert((cl_uint3){x,y,z}, col);
                }
            }
        }
    }
    
}