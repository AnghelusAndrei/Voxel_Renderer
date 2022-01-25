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
};

struct point_data{
    float dist;
    SDL_Color color;
    SDL_Color offset;
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


vec3 DDA_X(float ang_x, float ang_y, cam Camera, float grid, int offset){
    vec3 xCheck;
    if(ang_x < 90){ //down-right
        xCheck.y = Camera.pos.y+(grid-Camera.pos.y-floor((grid - Camera.pos.y)))+offset;
        xCheck.x = Camera.pos.x+((grid-Camera.pos.y-floor((grid - Camera.pos.y)))+offset)*abs(tan(degToRad(90-ang_x)));
    }else if(ang_x >= 90 && ang_x < 180){ //down-left
        xCheck.y = Camera.pos.y+(grid-Camera.pos.y-floor((grid - Camera.pos.y)))+offset;
        xCheck.x = Camera.pos.x-((grid-Camera.pos.y-floor((grid - Camera.pos.y)))+offset)*abs(tan(degToRad(FixAng(ang_x-90))));
    }else if(ang_x >= 180 && ang_x < 270){ //up-left
        xCheck.y = Camera.pos.y-(Camera.pos.y-floor((Camera.pos.y)))-offset;
        xCheck.x = Camera.pos.x-((Camera.pos.y-floor((Camera.pos.y)))+offset)*abs(tan(degToRad(FixAng(270-ang_x))));
    }else if(ang_x >= 270 && ang_x < 360){ //up-right
        xCheck.y = Camera.pos.y-(Camera.pos.y-floor((Camera.pos.y)))-offset;
        xCheck.x = Camera.pos.x+((Camera.pos.y-floor((Camera.pos.y)))+offset)*abs(tan(degToRad(FixAng(ang_x-270))));
    }

    

    return xCheck;
}


vec3 DDA_Y(float ang_x, float ang_y, cam Camera, float grid, int offset){
    vec3 yCheck;
    if(ang_x < 90){ //down-right
        yCheck.x = Camera.pos.x+(grid-Camera.pos.x-floor((grid - Camera.pos.x)))+offset;
        yCheck.y = Camera.pos.y+(yCheck.x-Camera.pos.x)*abs(tan(degToRad(ang_x)));
    }else if(ang_x >= 90 && ang_x < 180){ //down-left
        yCheck.x = floor((Camera.pos.x))-offset;
        yCheck.y = Camera.pos.y+(Camera.pos.x-floor((Camera.pos.x))+offset)*abs(tan(degToRad(180-ang_x)));
    }else if(ang_x >= 180 && ang_x < 270){ //up-left
        yCheck.x = floor((Camera.pos.x))-offset;
        yCheck.y = Camera.pos.y-(Camera.pos.x-floor((Camera.pos.x))+offset)*abs(tan(degToRad(FixAng(ang_x-180))));
    }else if(ang_x >= 270 && ang_x < 360){ //up-right
        yCheck.x = Camera.pos.x+(grid-Camera.pos.x-floor((grid - Camera.pos.x)))+offset;
        yCheck.y = Camera.pos.y-((grid-Camera.pos.x-floor((grid - Camera.pos.x)))+offset)*abs(tan(degToRad(FixAng(360-ang_x))));
    }



    return yCheck;
}

vec3 DDA_Z(float ang_x, float ang_y, cam Camera, float grid, int offset){
    vec3 zCheck;
    float vec2Diagonal;
    if(ang_y>=180){
        zCheck.z=ceil(Camera.pos.z)+offset;
        vec2Diagonal=(ceil(Camera.pos.z)-Camera.pos.z+offset)*tan(degToRad(90-ang_y));
    }else{
        zCheck.z=floor(Camera.pos.z)-offset;
        vec2Diagonal=(Camera.pos.z-floor(Camera.pos.z)+offset)*tan(degToRad(90-ang_y));
    }

    if(ang_x >= 180 && ang_x < 270){ //down-right
        zCheck.x=Camera.pos.x+vec2Diagonal*cos(degToRad(ang_x));
        zCheck.y=Camera.pos.y+vec2Diagonal*cos(degToRad(90-ang_x));
    }else if(ang_x >= 270 && ang_x < 360){ //down-left
        zCheck.x=Camera.pos.x-vec2Diagonal*cos(degToRad(180-ang_x));
        zCheck.y=Camera.pos.y+vec2Diagonal*cos(degToRad(ang_x-90));
    }else if(ang_x < 90){ //up-left
        zCheck.x=Camera.pos.x-vec2Diagonal*cos(degToRad(ang_x-180));
        zCheck.y=Camera.pos.y-vec2Diagonal*cos(degToRad(270-ang_x));
    }else if(ang_x >= 90 && ang_x < 180){ //up-right
        zCheck.x=Camera.pos.x+vec2Diagonal*cos(degToRad(360-ang_x));
        zCheck.y=Camera.pos.y-vec2Diagonal*cos(degToRad(ang_x+270));
    }

    return zCheck;
}



SDL_Color FindPixel(int x, int y, int FOV, SDL_Color map[], int grid, cam Camera, int SCREEN_HEIGHT, int SCREEN_WIDTH)
{

            std::vector<point_data> distArray;

        
            bool found = false;
    
            vec3 yCheck;
            vec3 xCheck;
            vec3 zCheck;
    
            float ang_x = FixAng(Camera.rot+(x*FOV)/SCREEN_WIDTH-FOV/2);
            float ang_y = FixAng((y*FOV)/SCREEN_HEIGHT-FOV/2);
    
            bool xFound = false;
            bool yFound = false;
            bool zFound = false;
            
    
            for(int offset = 0; offset < 20 && found == false; offset++){
                
                //DDA


                xCheck = DDA_X(ang_x, ang_y, Camera, grid, offset);
                yCheck = DDA_Y(ang_x, ang_y, Camera, grid, offset);
                zCheck = DDA_Z(ang_x, ang_y, Camera, grid, offset);

                xCheck.z = Camera.pos.z-tan(degToRad(ang_y))*vec2Distance(Camera.pos, xCheck);
                yCheck.z = Camera.pos.z-tan(degToRad(ang_y))*vec2Distance(Camera.pos, yCheck);

                //check

                vec3 x[2], y[2], z[2];

                x[0].x = floor(xCheck.x);
                x[0].y = xCheck.y;
                x[0].z = floor(xCheck.z);

                x[1].x = floor(xCheck.x);
                x[1].y = xCheck.y-1;
                x[1].z = floor(xCheck.z);

                y[0].x = yCheck.x;
                y[0].y = floor(yCheck.y);
                y[0].z = floor(yCheck.z);

                y[1].x = yCheck.x-1;
                y[1].y = floor(yCheck.y);
                y[1].z = floor(xCheck.z);

                z[0].x = zCheck.z;
                z[0].y = floor(zCheck.x);
                z[0].z = floor(zCheck.y);

                z[1].x = zCheck.z-1;
                z[1].y = floor(zCheck.x);
                z[1].z = floor(zCheck.y);


                for(int i=0; i<2; i++)
                {
                    if(inBounds(x[i], grid)){
                        int voxel_rgbSUM=map[int(x[i].z*100+x[i].y*10+x[i].x)].r+map[int(x[i].z*100+x[i].y*10+x[i].x)].g+map[int(x[i].z*100+x[i].y*10+x[i].x)].b;
                        if(voxel_rgbSUM<400 && voxel_rgbSUM>380){
                            if(PointInVoxel(xCheck, x[i])){
                                        
                                float dist_x = vec3Distance(Camera.pos, xCheck);
                                xFound = true;
                                distArray.push_back((point_data){dist_x, map[(int)(x[i].z)*grid*grid+(int)(x[i].y)*grid+(int)(x[i].x)], (SDL_Color){0, 0, 50}});
                            }
                        }
                    }
                }

                for(int i=0; i<2; i++)
                {
                    if(inBounds(y[i], grid)){
                    int voxel_rgbSUM=map[int(y[i].z*100+y[i].y*10+y[i].x)].r+map[int(y[i].z*100+y[i].y*10+y[i].x)].g+map[int(y[i].z*100+y[i].y*10+y[i].x)].b;
                        if(voxel_rgbSUM<400 && voxel_rgbSUM>380){
                            if(PointInVoxel(yCheck, y[i])){
                                
                                float dist_y = vec3Distance(Camera.pos, yCheck);
                                yFound = true;
                                distArray.push_back((point_data){dist_y, map[(int)(y[i].z)*grid*grid+(int)(y[i].y)*grid+(int)(y[i].x)], (SDL_Color){0, 50, 0}});
                            }
                        }
                    }
                }

                for(int i=0; i<2; i++)
                {
                    if(inBounds(z[i], grid)){
                        int voxel_rgbSUM=map[int(z[i].z*100+z[i].y*10+z[i].x)].r+map[int(z[i].z*100+z[i].y*10+z[i].x)].g+map[int(z[i].z*100+z[i].y*10+z[i].x)].b;
                        if(voxel_rgbSUM<400 && voxel_rgbSUM>380){
                            if(PointInVoxel(zCheck, z[i])){
                                        
                                float dist_z = vec3Distance(Camera.pos, zCheck);
                                zFound = true;
                                distArray.push_back((point_data){dist_z, map[(int)(z[i].z)*grid*grid+(int)(z[i].y)*grid+(int)(z[i].x)], (SDL_Color){50, 0, 0}});
                            }
                        }
                    }
                }

    
                if(xFound && yFound && zFound){
                    found = true;break;
                }
            }

            if(!xFound && !yFound && !zFound){
                return (SDL_Color){0,0,0,0};
            }

    
            point_data point;
            double MIN=grid*sqrt(3);
            for(int i=0;i<distArray.size();i++)
            {
                if(distArray[i].dist<MIN) 
                {
                    MIN=distArray[i].dist;
                    point=distArray[i];
                }
            }


    return (SDL_Color){static_cast<Uint8>(point.color.r), static_cast<Uint8>(point.color.g), static_cast<Uint8>(point.color.b), 255};
}