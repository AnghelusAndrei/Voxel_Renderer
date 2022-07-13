#pragma once
#include "./src/Data.hcl"
#include "./src/functions.hcl"


const float f_error = 0.001;
const float b_error = 0;


typedef struct ray_data ray_data;
struct ray_data{
  uchar3 color;
  float3 position;
  float3 normal;
}; 



ray_data RayCast(float3 pos, float3 vector, int n, uint4 *octree){

    float3 normal = {0,0,0};
    float3 ray = pos;
    float3 x,y,z;
    float3 v_add;
    int3 found = {1,1,1};
    int3 checked = {0,0,0};

    int q = 0;

    while(q<50) {

        int d = 1e9;
        float xD,yD,zD;

        x=(float3){0,0,0};
        y=(float3){0,0,0};
        z=(float3){0,0,0};
        v_add = (float3){0,0,0};

        while(!inBounds(ray,n,b_error)){

            if(found.x == 1){
                if(ray.x < b_error){
                    if(vector.x <= 0)found.x = 0;
                    else{
                    x.x = b_error+f_error-ray.x;
                    x.y = (x.x*vector.y)/vector.x;
                    x.z = (x.x*vector.z)/vector.x;
                    }
                }
                if(ray.x > n){
                    if(vector.x >= 0)found.x = 0;
                    else{
                    x.x = n-f_error-ray.x;
                    x.y = (x.x*vector.y)/vector.x;
                    x.z = (x.x*vector.z)/vector.x;
                    }
                }
                if(found.x == 1){
                    xD = vec3Distance(ray,ray+x);
                    if(xD < d){d = xD; v_add = x;}
                }
            }


            if(found.y == 1){
                if(ray.y < b_error){
                    if(vector.y <= 0)found.y = 0;
                    else{
                    y.y = b_error+f_error-ray.y;
                    y.x = (y.y*vector.x)/vector.y;
                    y.z = (y.y*vector.z)/vector.y;
                    }
                }
                if(ray.y > n){
                    if(vector.y >= 0)found.y = 0;
                    else{
                    y.y = n-f_error-ray.y;
                    y.x = (y.y*vector.x)/vector.y;
                    y.z = (y.y*vector.z)/vector.y;
                    }
                }
                if(found.y == 1){
                    yD = vec3Distance(ray,ray+y);
                    if(yD < d){d = yD; v_add = y;}
                }
            }


            if(found.z == 1){
                if(ray.z < b_error){
                    if(vector.z <= 0)found.z = 0;
                    else{
                    z.z = b_error+f_error-ray.z;
                    z.x = (z.z*vector.x)/vector.z;
                    z.y = (z.z*vector.y)/vector.z;
                    }
                }
                if(ray.z > n){
                    if(vector.z >= 0)found.z = 0;
                    else{
                    z.z = n-f_error-ray.z;
                    z.x = (z.z*vector.x)/vector.z;
                    z.y = (z.z*vector.y)/vector.z;
                    }
                }
                if(found.z == 1){
                    zD = vec3Distance(ray,ray+z);
                    if(zD < d){d = zD; v_add = z;}
                }
            }

            if((found.x == 0 && found.y == 0 && found.z == 0) || (v_add.x == 0 && v_add.y == 0 && v_add.z == 0)){
                return (ray_data){(uchar3){Gradient((uchar3){0,0,0}, (uchar3){0,0,0}, (vector.z+1)*127)},(float3){-1,0,0},(float3){0,0,0}};
            }
 
            ray = ray+v_add;
            d = 1e9;

            x=(float3){0,0,0};
            y=(float3){0,0,0};
            z=(float3){0,0,0};
            v_add = (float3){0,0,0};
        }


        leaf_data leaf = Lookup(ray,octree,n);

        if(leaf.type == CUBE){
            return (ray_data){
                (uchar3){leaf.data.x,leaf.data.y,leaf.data.z},
                ray,
                normal
            };
        }


        x.x = (leaf.data.x-ray.x) + (int)(vector.x>=0)*leaf.size + semn(vector.x)*f_error;
        x.y = (x.x*vector.y)/vector.x;
        x.z = (x.x*vector.z)/vector.x;
        xD = vec3Distance(ray,ray+x);
        if(xD < d){
            d = xD; 
            v_add = x; 
            normal = (float3){-semn(vector.x),0,0};
        }

        y.y = (leaf.data.y-ray.y) + (int)(vector.y>=0)*leaf.size + semn(vector.y)*f_error;
        y.x = (y.y*vector.x)/vector.y;
        y.z = (y.y*vector.z)/vector.y;
        yD = vec3Distance(ray,ray+y);
        if(yD < d){
            d = yD; 
            v_add = y; 
            normal = (float3){0,-semn(vector.y),0};
        }

        z.z = (leaf.data.z-ray.z) + (int)(vector.z>=0)*leaf.size + semn(vector.z)*f_error;
        z.x = (z.z*vector.x)/vector.z;
        z.y = (z.z*vector.y)/vector.z;
        zD = vec3Distance(ray,ray+z);
        if(zD < d){
            d = zD; 
            v_add = z; 
            normal = (float3){0,0,-semn(vector.z)};
        }
        

        ray += v_add;
        q++;
    }

    return (ray_data){(uchar3){0, 255, 0},(float3){-1,0,0},(float3){0,0,0}};
}
