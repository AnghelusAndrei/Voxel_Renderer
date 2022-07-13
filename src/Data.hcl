#pragma once


#define LEAF 0
#define NODE 1
#define CUBE 2

int depth;

typedef struct leaf_data leaf_data;
struct leaf_data{
  float3 data;
  uint size;
  uint type;
}; 

int Locate(uint3 pos, int depth, int n, int p2){
    int a=n/p2;
    int b=n/(p2*2);

    return (pos.x%a)/b + 
        2*((pos.y%a)/b)+
        4*((pos.z%a)/b);
}


leaf_data Lookup(float3 pos, uint4 octree[], int m){
    int d=1;
    int offset = 0;
    int p2=1;
    while(d<=depth){
        int i = offset+Locate((uint3){(uint)pos.x,(uint)pos.y,(uint)pos.z},d,m,p2);
        int s = m/(p2*2);

        switch (octree[i].w)
        {
        case LEAF:
            return (leaf_data){
                (float3){floor(pos.x/s)*s,floor(pos.y/s)*s,floor(pos.z/s)*s},
                (uint){s},
                octree[i].w
            };
        case CUBE:
            return (leaf_data){
                (float3){octree[i].x,octree[i].y,octree[i].z},
                (uint){1},
                octree[i].w
            };
        case NODE:
            offset = octree[i].x;
            break;
        }

        d++;
        p2*=2;
    }
}
