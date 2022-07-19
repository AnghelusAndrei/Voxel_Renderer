#pragma once
#include "PerlinNoise.hpp"
#include <iostream>
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#define LEAF 0
#define NODE 1
#define CUBE 2

typedef struct leaf_data leaf_data;
struct leaf_data{
    cl_float3 data;
    cl_uint size;
    cl_uint type;
}; 

long OCTREE_INDEX = 8;


class Octree{
    private:

        int Locate(cl_uint3 pos, int depth, int p2){
            int a=n/p2;
            int b=n/(p2*2);

            return (pos.x%a)/b + 
                2*((pos.y%a)/b)+
                4*((pos.z%a)/b);
        }

    public:

        int depth;
        int n;
        long OCTREE_LENGTH = 0;
        cl_uint4 * octree;

        void Initialize(int d);
        void Insert(cl_uint3 pos, cl_uint3 col);
        leaf_data Lookup(cl_float3 pos);
};

void Octree::Initialize(int d){
    depth = d;
    n = 1<<depth;
    int o_m = 8;

    for(int i = 0;i<depth;i++){
        OCTREE_LENGTH += o_m;
        o_m *= 8;
    }

    octree = new cl_uint4[OCTREE_LENGTH];
}


void Octree::Insert(cl_uint3 pos, cl_uint3 col){
    int d=1;
    int offset = 0;
    int p2 = 1;

    while(d<depth){
        int i = offset+Locate(pos,d,p2);

        switch (octree[i].w)
        {
            case LEAF:
                offset = OCTREE_INDEX;
                octree[i].x=offset;
                octree[i].w = NODE;
                OCTREE_INDEX+=8;
                break;
            case NODE:
                offset = octree[i].x;
                break;
        }

        d++;
        p2*=2;
    }

    int i = offset+Locate(pos,d,p2);
    octree[i]={
        col.x,
        col.y,
        col.z,
        CUBE
    };

}
