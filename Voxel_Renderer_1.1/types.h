#pragma once



struct vec3
{
    float x, y, z;
};

struct triangle
{
    vec3 p[3];
};

struct mat4x4
{
    float m[4][4] = { 0 };
};

struct mesh
{
    std::vector<triangle> tris;
};

struct pixel_t
{
    std::uint8_t r, g, b;
};

struct velocity_t
{
    float x, y;
    bool lockW, lockA, lockS, lockD;
};

struct player_t
{
    float x, y, z, mousex, mousey;
    velocity_t velocity;
};

struct coll_block_t
{
    float x, y;
    SDL_Color pixel;
};

struct ray_t
{
    float x, y, z;
};

struct wall_t
{
    std::int32_t x, y, height, width;
    SDL_Color color;
};



struct dist_data_t
{
    std::int32_t f;
    wall_t wall;
    float dist, b;
};


float Distance(float x1, float x2, float y1, float y2){
    return sqrt((x1-x2) * (x1-x2) + (y1-y2)*(y1-y2));
}

bool pointInside(int wallx, int wally, float x, float y)
{
  return wallx - 0.001 < x && wallx + 50 + 0.001 >x && wally - 0.001 <y && wally + 50 + 0.001 > y;
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

uint8_t r(uint8_t n)
{
    return rand() % n;
}


const std::uint32_t MAP_SIZE = 20;
SDL_Color map[MAP_SIZE][MAP_SIZE] = {r(255),r(255),r(255),255};






