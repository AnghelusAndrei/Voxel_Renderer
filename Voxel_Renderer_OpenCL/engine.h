#pragma once

struct vec3{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
};

struct cam{
    vec3 pos={0,0,0};
    vec3 vel;
    int rot;
    int rotz;
};

struct point_data{
    float dist;
    SDL_Color color;
    int offset;
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


float vec2Distance(vec3 vec1, vec3 vec2){
    return sqrt((vec1.x-vec2.x) * (vec1.x-vec2.x) + (vec1.y-vec2.y)*(vec1.y-vec2.y));
}


float vec3Distance(vec3 vec1, vec3 vec2){
    float d[] = {abs(vec1.x-vec2.x), abs(vec1.y-vec2.y), abs(vec1.z-vec2.z)};
    if (d[0] < d[1]) std::swap(d[0],d[1]);
    if (d[0] < d[2]) std::swap(d[0],d[2]);
    return d[0] * sqrt(1.0 + d[1]/d[0] + d[2]/d[0]);
}


bool Positive(SDL_Color col, int t1, int t2){
    return col.r > t1 && col.g > t1 && col.b > t1 && col.r < t2 && col.g < t2 && col.b < t2;
}


bool PointInVoxel(vec3 vec, vec3 voxel){
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


bool inBounds(vec3 v, float grid){
    return v.x > 0 && v.x < grid && v.y > 0 && v.y < grid && v.z > 0 && v.z < grid;
}

float roundInBounds(float x, int grid){
    if(x>grid){x=grid;}
    if(x<0){x=0;}
    return x;
}

SDL_Color FindPixel(float xa, float ya, int FOV, SDL_Color map[], int grid, cam Camera, int SCREEN_HEIGHT, int SCREEN_WIDTH, int res_x, int res_y, int density)
{

            std::vector<point_data> distArray;

        
            bool found = false;
    
            vec3 yCheck;
            vec3 xCheck;
            vec3 zCheck;
    
            float ang_x = FixAng(Camera.rot+(xa*FOV)/SCREEN_WIDTH-FOV/2);
            float ang_y = FixAng(Camera.rotz+(ya*FOV)/SCREEN_HEIGHT-FOV/2);
    
            bool xFound = false;
            bool yFound = false;
            bool zFound = false;
            
    
            for(int offset = 0; offset < 500 && found == false; offset++){
                
                vec3 x,y,z;

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
                        int voxel_rgbSUM=map[int(x.z*grid*grid+x.y*grid+x.x)].r+map[int(x.z*grid*grid+x.y*grid+x.x)].g+map[int(x.z*grid*grid+x.y*grid+x.x)].b;
                        if(voxel_rgbSUM>0){       
                            float dist_x = vec3Distance(Camera.pos, xCheck);
                            xFound = true;
                            distArray.push_back((point_data){dist_x, map[(int)(x.z)*grid*grid+(int)(x.y)*grid+(int)(x.x)], 50});
                        }
                    }

                    if(inBounds(y, grid) && !yFound){
                        int voxel_rgbSUM=map[int(y.z*grid*grid+y.y*grid+y.x)].r+map[int(y.z*grid*grid+y.y*grid+y.x)].g+map[int(y.z*grid*grid+y.y*grid+y.x)].b;
                        if(voxel_rgbSUM>0){
                            float dist_y = vec3Distance(Camera.pos, yCheck);
                            yFound = true;
                            distArray.push_back((point_data){dist_y, map[(int)(y.z)*grid*grid+(int)(y.y)*grid+(int)(y.x)], 25});
                        }
                    }
                
                    if(inBounds(z, grid) && !zFound){
                        int voxel_rgbSUM=map[int(z.z*grid*grid+z.y*grid+z.x)].r+map[int(z.z*grid*grid+z.y*grid+z.x)].g+map[int(z.z*grid*grid+z.y*grid+z.x)].b;
                        if(voxel_rgbSUM>0){
                            float dist_z = vec3Distance(Camera.pos, zCheck);
                            zFound = true;
                            distArray.push_back((point_data){dist_z, map[(int)(z.z)*grid*grid+(int)(z.y)*grid+(int)(z.x)], 0});
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
                return (SDL_Color){0,0,0,0};
            }

    
            point_data point;
            double MIN=1e9;
            for(int i=0;i<distArray.size();i++)
            {
                if(distArray[i].dist<MIN) 
                {
                    MIN=distArray[i].dist;
                    point=distArray[i];
                }
            }


    return (SDL_Color){(Uint8)LimitColor(static_cast<int>(point.color.r-point.dist*1-point.offset)), (Uint8)LimitColor(static_cast<int>(point.color.g-point.dist*1-point.offset)), (Uint8)LimitColor(static_cast<int>(point.color.b-point.dist*1-point.offset)), 255};
}