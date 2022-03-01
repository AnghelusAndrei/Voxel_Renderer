#pragma once
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

struct vec3{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
};

struct cam{
    cl_float3 pos={0,0,0};
    cl_float3 vel;
    int rot;
    int rotz;
};

float min(std::vector<float> num){
    float m = num[0];

    for(int i = 1; i < num.size(); i++){
        if(num[i] < m){
            m = num[i];
        }
    }

    return m;
}


float max(std::vector<float> num){
    float m = num[0];

    for(int i = 0; i < num.size(); i++){
        if(num[i] > m){
            m = num[i];
        }
    }

    return m;
}

int MAX3(int a, int b, int c){
    if(a>=b&&a>=c)return a;
    if(b>=a&&b>=c)return b;
    if(c>=a&&c>=b)return c;
}


float vec2Distance(cl_float3 vec1, cl_float3 vec2){
    return sqrt((vec1.x-vec2.x) * (vec1.x-vec2.x) + (vec1.y-vec2.y)*(vec1.y-vec2.y));
}


float vec3Distance(cl_float3 vec1, cl_float3 vec2){
    float d[] = {abs(vec1.x-vec2.x), abs(vec1.y-vec2.y), abs(vec1.z-vec2.z)};
    if (d[0] < d[1]) std::swap(d[0],d[1]);
    if (d[0] < d[2]) std::swap(d[0],d[2]);
    return d[0] * sqrt(1.0 + d[1]/d[0] + d[2]/d[0]);
}


bool Positive(cl_int4 col, int t1, int t2){
    return col.x > t1 && col.y > t1 && col.z > t1 && col.x < t2 && col.y < t2 && col.z < t2;
}


bool PointInVoxel(cl_float3 vec, cl_float3 voxel){
    return voxel.x <= vec.x && voxel.x + 1 >= vec.x && voxel.y <= vec.y && voxel.y + 1 >= vec.y && voxel.z <= vec.z && voxel.z + 1 >= vec.z;
}


float degToRad(float ang)
{
    return ang * (M_PI/180);;
}


float radToDeg(float rad)
{
    return rad * (180/M_PI);
}


float FixAng(float ang)
{
    if (ang > 359)
    {
        float mult = floor(ang / 360);
        ang = ang - (360 * mult);
    }
    if (ang < 0)
    {
        float ap = abs(ang);
        float mult = ceil(ap / 360);
        ang = (360 * mult) - ap;
    }
    return ang;
}

int LimitColor(int c){
    if(c>255)return 255;
    if(c<0)return 0;
    return c;
}


float inGrid(float x, float grid){
    float y;
    if(x < 0){
        y = 0;
    }else if(x > grid){
        y = grid;
    }else{
        y = x;
    }
    return y;
}


bool inBounds(cl_float3 v, float grid){
    return v.x > 0 && v.x < grid && v.y > 0 && v.y < grid && v.z > 0 && v.z < grid;
}

float roundInBounds(float x, int grid){
    if(x>grid){x=grid;}
    if(x<0){x=0;}
    return x;
}

cl_int4 FindPixel(float xa, float ya, int FOV, cl_int4 map[], int grid, cam Camera, int SCREEN_HEIGHT, int SCREEN_WIDTH, int res_x, int res_y, int density)
{

            float minDist = 1e9;
            cl_int4 minVoxel;
            int minShade;
        
            bool found = false;
    
            cl_float3 yCheck;
            cl_float3 xCheck;
            cl_float3 zCheck;
    
            float ang_x = FixAng(Camera.rot+(xa*FOV)/SCREEN_WIDTH-FOV/2);
            float ang_y = FixAng(Camera.rotz+(ya*FOV)/SCREEN_HEIGHT-FOV/2);
    
            bool xFound = false;
            bool yFound = false;
            bool zFound = false;
            
    
            for(int offset = 0; offset < 500 && found == false; offset++){
                
                cl_float3 x,y,z;

                //DDA
                float vec2Diagonal;
                if(ang_y<90){
                    zCheck.z=roundInBounds(floor(Camera.pos.z),grid)-offset;
                    vec2Diagonal=abs(Camera.pos.z-zCheck.z)*tan(degToRad(FixAng(90-abs(ang_y))))/cos(degToRad(abs(FixAng((xa*FOV)/SCREEN_WIDTH-FOV/2))));
                }else{
                    zCheck.z=roundInBounds(ceil(Camera.pos.z),grid)+offset;
                    vec2Diagonal=abs(zCheck.z-Camera.pos.z)*tan(degToRad(FixAng(90-abs(360-ang_y))))/cos(degToRad(abs(FixAng((xa*FOV)/SCREEN_WIDTH-FOV/2))));
                }

                if(ang_x < 90){ //down-right
                    xCheck.y = roundInBounds(ceil(Camera.pos.y),grid)+offset;
                    xCheck.x = Camera.pos.x+(xCheck.y-Camera.pos.y)*tan(degToRad(90-ang_x));

                    yCheck.x = roundInBounds(ceil(Camera.pos.x),grid)+offset;
                    yCheck.y = Camera.pos.y+(yCheck.x-Camera.pos.x)*tan(degToRad(ang_x));

                    zCheck.x=Camera.pos.x+vec2Diagonal*cos(degToRad(ang_x));
                    zCheck.y=Camera.pos.y+vec2Diagonal*sin(degToRad(ang_x));
                    

                    xCheck.z = Camera.pos.z-tan(degToRad(abs(ang_y)))*vec2Distance(Camera.pos, xCheck)*cos(degToRad(abs(FixAng((xa*FOV)/SCREEN_WIDTH-FOV/2))));
                    yCheck.z = Camera.pos.z-tan(degToRad(abs(ang_y)))*vec2Distance(Camera.pos, yCheck)*cos(degToRad(abs(FixAng((xa*FOV)/SCREEN_WIDTH-FOV/2))));


                    x.x = floor(xCheck.x);
                    x.y = xCheck.y;
                    x.z = floor(xCheck.z);

                    y.x = yCheck.x;
                    y.y = floor(yCheck.y);
                    y.z = floor(yCheck.z);
                }else if(ang_x >= 90 && ang_x < 180){ //down-left
                    xCheck.y = roundInBounds(ceil(Camera.pos.y),grid)+offset;
                    xCheck.x = Camera.pos.x-(xCheck.y-Camera.pos.y)*tan(degToRad(FixAng(ang_x-90)));

                    yCheck.x = roundInBounds(floor(Camera.pos.x),grid)-offset;
                    yCheck.y = Camera.pos.y+(Camera.pos.x-yCheck.x)*tan(degToRad(180-ang_x));

                    zCheck.x=Camera.pos.x-vec2Diagonal*sin(degToRad(ang_x-90));
                    zCheck.y=Camera.pos.y+vec2Diagonal*cos(degToRad(ang_x-90));

                    xCheck.z = Camera.pos.z-tan(degToRad(abs(ang_y)))*vec2Distance(Camera.pos, xCheck)*cos(degToRad(abs(FixAng((xa*FOV)/SCREEN_WIDTH-FOV/2))));
                    yCheck.z = Camera.pos.z-tan(degToRad(abs(ang_y)))*vec2Distance(Camera.pos, yCheck)*cos(degToRad(abs(FixAng((xa*FOV)/SCREEN_WIDTH-FOV/2))));

                    x.x = floor(xCheck.x);
                    x.y = xCheck.y;
                    x.z = floor(xCheck.z);

                    y.x = yCheck.x-1;
                    y.y = floor(yCheck.y);
                    y.z = floor(yCheck.z);
                }else if(ang_x >= 180 && ang_x < 270){ //up-left
                    xCheck.y = roundInBounds(floor(Camera.pos.y),grid)-offset;
                    xCheck.x = Camera.pos.x-(Camera.pos.y-xCheck.y)*tan(degToRad(FixAng(270-ang_x)));

                    yCheck.x = roundInBounds(floor((Camera.pos.x)),grid)-offset;
                    yCheck.y = Camera.pos.y-(Camera.pos.x-yCheck.x)*tan(degToRad(FixAng(ang_x-180)));

                    zCheck.x=Camera.pos.x-vec2Diagonal*cos(degToRad(ang_x-180));
                    zCheck.y=Camera.pos.y-vec2Diagonal*sin(degToRad(ang_x-180));
                    

                    xCheck.z = Camera.pos.z-tan(degToRad(abs(ang_y)))*vec2Distance(Camera.pos, xCheck)*cos(degToRad(abs(FixAng((xa*FOV)/SCREEN_WIDTH-FOV/2))));
                    yCheck.z = Camera.pos.z-tan(degToRad(abs(ang_y)))*vec2Distance(Camera.pos, yCheck)*cos(degToRad(abs(FixAng((xa*FOV)/SCREEN_WIDTH-FOV/2))));

                    x.x = floor(xCheck.x);
                    x.y = xCheck.y-1;
                    x.z = floor(xCheck.z);

                    y.x = yCheck.x-1;
                    y.y = floor(yCheck.y);
                    y.z = floor(yCheck.z);
                }else if(ang_x >= 270 && ang_x < 360){ //up-right
                    xCheck.y = roundInBounds(floor(Camera.pos.y),grid)-offset;
                    xCheck.x = Camera.pos.x+(Camera.pos.y-xCheck.y)*tan(degToRad(FixAng(ang_x-270)));

                    yCheck.x = roundInBounds(ceil(Camera.pos.x),grid)+offset;
                    yCheck.y = Camera.pos.y-(yCheck.x-Camera.pos.x)*tan(degToRad(FixAng(360-ang_x)));

                    zCheck.x=Camera.pos.x+vec2Diagonal*sin(degToRad(ang_x-270));
                    zCheck.y=Camera.pos.y-vec2Diagonal*cos(degToRad(ang_x-270));
                    

                    xCheck.z = Camera.pos.z-tan(degToRad(abs(ang_y)))*vec2Distance(Camera.pos, xCheck)*cos(degToRad(abs(FixAng((xa*FOV)/SCREEN_WIDTH-FOV/2))));
                    yCheck.z = Camera.pos.z-tan(degToRad(abs(ang_y)))*vec2Distance(Camera.pos, yCheck)*cos(degToRad(abs(FixAng((xa*FOV)/SCREEN_WIDTH-FOV/2))));

                    x.x = floor(xCheck.x);
                    x.y = xCheck.y-1;
                    x.z = floor(xCheck.z);

                    y.x = yCheck.x;
                    y.y = floor(yCheck.y);
                    y.z = floor(yCheck.z);

                }

                    if(ang_y<90){
                        z.z = zCheck.z-1;
                        z.x = floor(zCheck.x);
                        z.y = floor(zCheck.y); 
                    }else{
                        z.z = zCheck.z;
                        z.x = floor(zCheck.x);
                        z.y = floor(zCheck.y);
                    }
                    
                    if(inBounds(x, grid) && !xFound){
                        int voxel_rgbSUM=map[int(x.z*grid*grid+x.y*grid+x.x)].x+map[int(x.z*grid*grid+x.y*grid+x.x)].y+map[int(x.z*grid*grid+x.y*grid+x.x)].z;
                        if(voxel_rgbSUM>0){       
                            float dist_x = vec3Distance(Camera.pos, xCheck);
                            xFound = true;
                            if(dist_x<minDist){ 
                                minDist=dist_x;
                                minVoxel = map[(int)(x.z)*grid*grid+(int)(x.y)*grid+(int)(x.x)];
                                minShade=50;
                            }
                        }
                    }

                    if(inBounds(y, grid) && !yFound){
                        int voxel_rgbSUM=map[int(y.z*grid*grid+y.y*grid+y.x)].x+map[int(y.z*grid*grid+y.y*grid+y.x)].y+map[int(y.z*grid*grid+y.y*grid+y.x)].z;
                        if(voxel_rgbSUM>0){
                            float dist_y = vec3Distance(Camera.pos, yCheck);
                            yFound = true;
                            if(dist_y<minDist){ 
                                minDist=dist_y;
                                minVoxel = map[(int)(y.z)*grid*grid+(int)(y.y)*grid+(int)(y.x)];
                                minShade=25;
                            }
                        }
                    }
                
                    if(inBounds(z, grid) && !zFound){
                        int voxel_rgbSUM=map[int(z.z*grid*grid+z.y*grid+z.x)].x+map[int(z.z*grid*grid+z.y*grid+z.x)].y+map[int(z.z*grid*grid+z.y*grid+z.x)].z;
                        if(voxel_rgbSUM>0){
                            float dist_z = vec3Distance(Camera.pos, zCheck);
                            zFound = true;
                            if(dist_z<minDist){ 
                                minDist=dist_z;
                                minVoxel = map[(int)(z.z)*grid*grid+(int)(z.y)*grid+(int)(z.x)];
                                minShade=0;
                            }
                        }
                    }

                

                if(floor(Camera.pos.z)-offset<0 && ceil(Camera.pos.z)+offset>grid && ceil(Camera.pos.y)+offset > grid && ceil(Camera.pos.x)+offset > grid && floor(Camera.pos.y)-offset < 0 && floor(Camera.pos.x)-offset < 0){
                    break;
                }
                if(xFound && yFound && zFound){
                    found = true;break;
                }
            
            }

            if(!xFound && !yFound && !zFound){
                return {0,0,0,0};
            }

    return {LimitColor(minVoxel.x-minDist*1-minShade), LimitColor(minVoxel.y-minDist*1-minShade), LimitColor(minVoxel.z-minDist*1-minShade), 255};
}