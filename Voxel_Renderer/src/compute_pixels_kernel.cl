


float vec2Distance(float3 vec1, float3 vec2) {
  return sqrt((vec1.x - vec2.x) * (vec1.x - vec2.x) +
              (vec1.y - vec2.y) * (vec1.y - vec2.y));}

void swap(float a, float b){
    float x = b;
    b=a;
    a=x;}

float vec3Distance(float3 vec1, float3 vec2) {
  float d[] = {fabs(vec1.x - vec2.x), fabs(vec1.y - vec2.y),
               fabs(vec1.z - vec2.z)};
  if (d[0] < d[1])
    swap(d[0], d[1]);
  if (d[0] < d[2])
    swap(d[0], d[2]);
  return d[0] * sqrt(1.0 + d[1] / d[0] + d[2] / d[0]);}

bool Positive(int4 col, int t1, int t2) {
  return col.x > t1 && col.y > t1 && col.z > t1 && col.x < t2 && col.y < t2 &&
         col.z < t2;}

bool PointInVoxel(float3 vec, float3 voxel) {
  return voxel.x <= vec.x && voxel.x + 1 >= vec.x && voxel.y <= vec.y &&
         voxel.y + 1 >= vec.y && voxel.z <= vec.z && voxel.z + 1 >= vec.z;}


float radToDeg(float rad) { return rad * (180 / M_PI); }
float Fix_ang(float ang) {
  if (ang > 360) {
    return ang - (360 * floor(ang / 360));
  }
  if (ang < 0) {
    return (360 * ceil(fabs(ang) / 360)) - fabs(ang);
  }
  return ang;}
int LimitColor(int c) {
  if (c > 255)
    return 255;
  if (c < 0)
    return 0;
  return c;}

float inGrid(float x, float grid) {
  float y;
  if (x < 0) {
    y = 0;
  } else if (x > grid) {
    y = grid;
  } else { 
    y = x;
  }
  return y;}

bool inBounds(float3 v, float grid) {
  return v.x > 0 && v.x < grid && v.y > 0 && v.y < grid && v.z > 0 &&
         v.z < grid;}

float roundInBounds(float x, int grid) {if (x > grid) {x = grid;}if (x < 0) { x = 0;}return x;}


__kernel void compute_pixels_kernel(__global int3 *map, __global uchar3 *doutput,
                                    const int grid, const int FOV,
                                    const float3 pos, const float2 ang) {
                                      
    size_t index_x= get_global_id(0);
    size_t index_y = get_global_id(1);
    float2 resolution = {get_num_groups(0) * get_local_size(0), get_num_groups(1) * get_local_size(1)};
    size_t index = index_y * convert_int(resolution.x) + index_x;


    float Depth = 1e9;
    int3 voxel;
    int normal = 0;

    bool found = false;

    float3 yCheck;
    float3 xCheck;
    float3 zCheck;

    float f = resolution.x/tan(radians(Fix_ang(FOV/2)));
    float FOV_Z = 2*degrees(atan(resolution.y/f));

    float projection_plane_dist = (resolution.x/2)/tan(radians((float)(FOV)/2));
    
    int neg_poz_x=1, neg_poz_y=1;
    if((float)index_x-resolution.x/2 < 0){neg_poz_x=-1;}
    if((float)index_y-resolution.y/2 < 0){neg_poz_y=-1;}

    float local_ang_x = degrees(atan(fabs((float)index_x-resolution.x/2)/projection_plane_dist))*neg_poz_x;
    float local_ang_y = degrees(atan(fabs((float)index_y-resolution.y/2)/projection_plane_dist))*neg_poz_y;

    float ang_x = Fix_ang(ang.x + local_ang_x);
    float ang_y = Fix_ang(ang.y + local_ang_y);

    if(ang_x == 90 || ang_x == 270)
    {
      ang_x+=1;
    }

    bool xFound = false;
    bool yFound = false;
    bool zFound = false;

    int3 offset = {0,0,0};

    while(!found) {

        float3 x, y, z;

        // DDA
        float vec2Diagonal;
        if (ang_y < 180) {
          zCheck.z = roundInBounds(floor(pos.z), grid) - offset.z;
          vec2Diagonal = fabs(pos.z - zCheck.z) * tan(radians(90 - fabs(ang_y))) / cos(radians(fabs(Fix_ang(local_ang_x))));
        } else {
        zCheck.z = roundInBounds(ceil(pos.z), grid) + offset.z;
        vec2Diagonal =
            fabs(zCheck.z - pos.z) * tan(radians(90 - fabs(360 - ang_y))) /
            cos(radians(fabs(Fix_ang(local_ang_x))));
        }

        if (ang_x <= 90) { // down-right
        xCheck.y = roundInBounds(ceil(pos.y), grid) + offset.x;
        xCheck.x = pos.x + (xCheck.y - pos.y) * tan(radians(90 - ang_x));

        yCheck.x = roundInBounds(ceil(pos.x), grid) + offset.y;
        yCheck.y = pos.y + (yCheck.x - pos.x) * tan(radians(ang_x));

        zCheck.x = pos.x + vec2Diagonal * cos(radians(ang_x));
        zCheck.y = pos.y + vec2Diagonal * sin(radians(ang_x));

        xCheck.z =pos.z -tan(radians(fabs(ang_y))) * vec2Distance(pos, xCheck) *cos(radians(fabs(Fix_ang(local_ang_x))));
        yCheck.z =pos.z -tan(radians(fabs(ang_y))) * vec2Distance(pos, yCheck) *cos(radians(fabs(Fix_ang(local_ang_x))));

        x.x = floor(xCheck.x);
        x.y = xCheck.y;
        x.z = floor(xCheck.z);

        y.x = yCheck.x;
        y.y = floor(yCheck.y);
        y.z = floor(yCheck.z);
        }
        if (ang_x > 90 && ang_x <= 180) { // down-left
        xCheck.y = roundInBounds(ceil(pos.y), grid) + offset.x;
        xCheck.x = pos.x - (xCheck.y - pos.y) * tan(radians(Fix_ang(ang_x - 90)));

        yCheck.x = roundInBounds(floor(pos.x), grid) - offset.y;
        yCheck.y = pos.y + (pos.x - yCheck.x) * tan(radians(180 - ang_x));

        zCheck.x = pos.x - vec2Diagonal * sin(radians(ang_x - 90));
        zCheck.y = pos.y + vec2Diagonal * cos(radians(ang_x - 90));

        xCheck.z =
            pos.z -tan(radians(fabs(ang_y))) * vec2Distance(pos, xCheck) *cos(radians(fabs(Fix_ang(local_ang_x))));
        yCheck.z =
            pos.z -tan(radians(fabs(ang_y))) * vec2Distance(pos, yCheck) *cos(radians(fabs(Fix_ang(local_ang_x))));

        x.x = floor(xCheck.x);
        x.y = xCheck.y;
        x.z = floor(xCheck.z);

        y.x = yCheck.x - 1;
        y.y = floor(yCheck.y);
        y.z = floor(yCheck.z);
        }
        if (ang_x > 180 && ang_x <= 270) { // up-left
        xCheck.y = roundInBounds(floor(pos.y), grid) - offset.x;
        xCheck.x =
            pos.x - (pos.y - xCheck.y) * tan(radians(Fix_ang(270 - ang_x)));

        yCheck.x = roundInBounds(floor((pos.x)), grid) - offset.y;
        yCheck.y =
            pos.y - (pos.x - yCheck.x) * tan(radians(Fix_ang(ang_x - 180)));

        zCheck.x = pos.x - vec2Diagonal * cos(radians(ang_x - 180));
        zCheck.y = pos.y - vec2Diagonal * sin(radians(ang_x - 180));

        xCheck.z =
            pos.z -
            tan(radians(fabs(ang_y))) * vec2Distance(pos, xCheck) *
                cos(radians(fabs(Fix_ang(local_ang_x))));
        yCheck.z =
            pos.z -
            tan(radians(fabs(ang_y))) * vec2Distance(pos, yCheck) *
                cos(radians(fabs(Fix_ang(local_ang_x))));

        x.x = floor(xCheck.x);
        x.y = xCheck.y - 1;
        x.z = floor(xCheck.z);

        y.x = yCheck.x - 1;
        y.y = floor(yCheck.y);
        y.z = floor(yCheck.z);
        }
        if (ang_x > 270 && ang_x <= 360) { // up-right
        xCheck.y = roundInBounds(floor(pos.y), grid) - offset.x;
        xCheck.x =
            pos.x + (pos.y - xCheck.y) * tan(radians(Fix_ang(ang_x - 270)));

        yCheck.x = roundInBounds(ceil(pos.x), grid) + offset.y;
        yCheck.y =
            pos.y - (yCheck.x - pos.x) * tan(radians(Fix_ang(360 - ang_x)));

        zCheck.x = pos.x + vec2Diagonal * sin(radians(ang_x - 270));
        zCheck.y = pos.y - vec2Diagonal * cos(radians(ang_x - 270));

        xCheck.z =
            pos.z -
            tan(radians(fabs(ang_y))) * vec2Distance(pos, xCheck) *
                cos(radians(fabs(Fix_ang(local_ang_x))));
        yCheck.z =
            pos.z -
            tan(radians(fabs(ang_y))) * vec2Distance(pos, yCheck) *
                cos(radians(fabs(Fix_ang(local_ang_x))));

        x.x = floor(xCheck.x);
        x.y = xCheck.y - 1;
        x.z = floor(xCheck.z);

        y.x = yCheck.x;
        y.y = floor(yCheck.y);
        y.z = floor(yCheck.z);
        }

        if (ang_y < 180) {
        z.z = zCheck.z - 1;
        z.x = floor(zCheck.x);
        z.y = floor(zCheck.y);
        } else {
        z.z = zCheck.z;
        z.x = floor(zCheck.x);
        z.y = floor(zCheck.y);
        }

        if (inBounds(x, grid) && !xFound) {
        int voxel_rgbSUM = map[convert_int(x.z * grid * grid + x.y * grid + x.x)].x +
                            map[convert_int(x.z * grid * grid + x.y * grid + x.x)].y +
                            map[convert_int(x.z * grid * grid + x.y * grid + x.x)].z;
        if (voxel_rgbSUM > 0) {
            float dist_x = vec3Distance(pos, xCheck);
            xFound = true;
            if (dist_x < Depth) {
            Depth = dist_x;
            voxel =
                map[convert_int(x.z) * grid * grid + convert_int(x.y) * grid + convert_int(x.x)];
            normal = 50;
            }
        }
        }

        if (inBounds(y, grid) && !yFound) {
        int voxel_rgbSUM = map[convert_int(y.z * grid * grid + y.y * grid + y.x)].x +
                            map[convert_int(y.z * grid * grid + y.y * grid + y.x)].y +
                            map[convert_int(y.z * grid * grid + y.y * grid + y.x)].z;
        if (voxel_rgbSUM > 0) {
            float dist_y = vec3Distance(pos, yCheck);
            yFound = true;
            if (dist_y < Depth) {
            Depth = dist_y;
            voxel =
                map[convert_int(y.z) * grid * grid + convert_int(y.y) * grid + convert_int(y.x)];
            normal = 25;
            }
        }
        }

        if (inBounds(z, grid) && !zFound) {
        int voxel_rgbSUM = map[convert_int(z.z * grid * grid + z.y * grid + z.x)].x +
                            map[convert_int(z.z * grid * grid + z.y * grid + z.x)].y +
                            map[convert_int(z.z * grid * grid + z.y * grid + z.x)].z;
        if (voxel_rgbSUM > 0) {
            float dist_z = vec3Distance(pos, zCheck);
            zFound = true;
            if (dist_z < Depth) {
            Depth = dist_z;
            voxel =
                map[convert_int(z.z) * grid * grid + convert_int(z.y) * grid + convert_int(z.x)];
            normal = 0;
            }
        }
        }

        if (floor(pos.z) - offset.z < 0 && ceil(pos.z) + offset.z > grid &&
            ceil(pos.y) + offset.y > grid && ceil(pos.x) + offset.x > grid &&
            floor(pos.y) - offset.y < 0 && floor(pos.x) - offset.x < 0) {
        break;
        }
        if (xFound && yFound && zFound) {
        found = true;
        break;
        }
        offset.x++;
        offset.y++;
        offset.z++;
    }

    if (!xFound && !yFound && !zFound) {
        doutput[index] = (uchar3){0, 0, 0};
    }else{
      doutput[index] = (uchar3){LimitColor(voxel.x),
              LimitColor(voxel.y),
              LimitColor(voxel.z)};
    }
}