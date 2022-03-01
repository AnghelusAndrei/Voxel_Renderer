#pragma OPENCL EXTENSION cl_khr_fp64 : enable


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

float degToRad(float ang) {
  return ang * (M_PI / 180);
  ;}

float radToDeg(float rad) { return rad * (180 / M_PI); }
float Fiindex_xng(float ang) {
  if (ang > 359) {
    float mult = floor(ang / 360);
    ang = ang - (360 * mult);
  }
  if (ang < 0) {
    float ap = fabs(ang);
    float mult = ceil(ap / 360);
    ang = (360 * mult) - ap;
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



__kernel void compute_pixels_kernel(__global int4 *map, __global int4 *doutput,
                                    const int grid, const int2 resolution,
                                    const int SCREEN_WIDTH,
                                    const int SCREEN_HEIGHT, const int FOV,
                                    const float3 pos, const float2 ang) {
    const size_t index = get_global_id(0);
    int index_x = index % resolution.x;
    int index_y = index / resolution.y;

    if(index==0)
    {
        printf("Position: %f %f\n", pos.x, pos.y);
        printf("Direction: %f %f\n", ang.x, ang.y);
        printf("Resolution: %f %f\n", resolution.x, resolution.y);
    }

    float minDist = 1e9;
    int4 minVoxel;
    int minShade;

    bool found = false;

    float3 yCheck;
    float3 xCheck;
    float3 zCheck;

    float ang_x = Fiindex_xng(ang.x + (index_x * FOV) / resolution.x - FOV / 2);
    float ang_y = Fiindex_xng(ang.y + (index_y * FOV) / resolution.y - FOV / 2);

    bool xFound = false;
    bool yFound = false;
    bool zFound = false;

    for (int offset = 0; offset < 500 && found == false; offset++) {

        float3 x, y, z;

        // DDA
        float vec2Diagonal;
        if (ang_y < 90) {
        zCheck.z = roundInBounds(floor(pos.z), grid) - offset;
        vec2Diagonal =
            fabs(pos.z - zCheck.z) * tan(degToRad(Fiindex_xng(90 - fabs(ang_y)))) /
            half_cos(degToRad(fabs(Fiindex_xng((index_x * FOV) / SCREEN_WIDTH - FOV / 2))));
        } else {
        zCheck.z = roundInBounds(ceil(pos.z), grid) + offset;
        vec2Diagonal =
            fabs(zCheck.z - pos.z) * tan(degToRad(Fiindex_xng(90 - fabs(360 - ang_y)))) /
            half_cos(degToRad(fabs(Fiindex_xng((index_x * FOV) / SCREEN_WIDTH - FOV / 2))));
        }

        if (ang_x < 90) { // down-right
        xCheck.y = roundInBounds(ceil(pos.y), grid) + offset;
        xCheck.x = pos.x + (xCheck.y - pos.y) * tan(degToRad(90 - ang_x));

        yCheck.x = roundInBounds(ceil(pos.x), grid) + offset;
        yCheck.y = pos.y + (yCheck.x - pos.x) * tan(degToRad(ang_x));

        zCheck.x = pos.x + vec2Diagonal * half_cos(degToRad(ang_x));
        zCheck.y = pos.y + vec2Diagonal * half_sin(degToRad(ang_x));

        xCheck.z =pos.z -tan(degToRad(fabs(ang_y))) * vec2Distance(pos, xCheck) *half_cos(degToRad(fabs(Fiindex_xng((index_x * FOV) / SCREEN_WIDTH - FOV / 2))));
        yCheck.z =pos.z -tan(degToRad(fabs(ang_y))) * vec2Distance(pos, yCheck) *half_cos(degToRad(fabs(Fiindex_xng((index_x * FOV) / SCREEN_WIDTH - FOV / 2))));

        x.x = floor(xCheck.x);
        x.y = xCheck.y;
        x.z = floor(xCheck.z);

        y.x = yCheck.x;
        y.y = floor(yCheck.y);
        y.z = floor(yCheck.z);
        } else if (ang_x >= 90 && ang_x < 180) { // down-left
        xCheck.y = roundInBounds(ceil(pos.y), grid) + offset;
        xCheck.x = pos.x - (xCheck.y - pos.y) * tan(degToRad(Fiindex_xng(ang_x - 90)));

        yCheck.x = roundInBounds(floor(pos.x), grid) - offset;
        yCheck.y = pos.y + (pos.x - yCheck.x) * tan(degToRad(180 - ang_x));

        zCheck.x = pos.x - vec2Diagonal * half_sin(degToRad(ang_x - 90));
        zCheck.y = pos.y + vec2Diagonal * half_cos(degToRad(ang_x - 90));

        xCheck.z =
            pos.z -tan(degToRad(fabs(ang_y))) * vec2Distance(pos, xCheck) *half_cos(degToRad(fabs(Fiindex_xng((index_x * FOV) / SCREEN_WIDTH - FOV / 2))));
        yCheck.z =
            pos.z -tan(degToRad(fabs(ang_y))) * vec2Distance(pos, yCheck) *half_cos(degToRad(fabs(Fiindex_xng((index_x * FOV) / SCREEN_WIDTH - FOV / 2))));

        x.x = floor(xCheck.x);
        x.y = xCheck.y;
        x.z = floor(xCheck.z);

        y.x = yCheck.x - 1;
        y.y = floor(yCheck.y);
        y.z = floor(yCheck.z);
        } else if (ang_x >= 180 && ang_x < 270) { // up-left
        xCheck.y = roundInBounds(floor(pos.y), grid) - offset;
        xCheck.x =
            pos.x - (pos.y - xCheck.y) * tan(degToRad(Fiindex_xng(270 - ang_x)));

        yCheck.x = roundInBounds(floor((pos.x)), grid) - offset;
        yCheck.y =
            pos.y - (pos.x - yCheck.x) * tan(degToRad(Fiindex_xng(ang_x - 180)));

        zCheck.x = pos.x - vec2Diagonal * half_cos(degToRad(ang_x - 180));
        zCheck.y = pos.y - vec2Diagonal * half_sin(degToRad(ang_x - 180));

        xCheck.z =
            pos.z -
            tan(degToRad(fabs(ang_y))) * vec2Distance(pos, xCheck) *
                half_cos(degToRad(fabs(Fiindex_xng((index_x * FOV) / SCREEN_WIDTH - FOV / 2))));
        yCheck.z =
            pos.z -
            tan(degToRad(fabs(ang_y))) * vec2Distance(pos, yCheck) *
                half_cos(degToRad(fabs(Fiindex_xng((index_x * FOV) / SCREEN_WIDTH - FOV / 2))));

        x.x = floor(xCheck.x);
        x.y = xCheck.y - 1;
        x.z = floor(xCheck.z);

        y.x = yCheck.x - 1;
        y.y = floor(yCheck.y);
        y.z = floor(yCheck.z);
        } else if (ang_x >= 270 && ang_x < 360) { // up-right
        xCheck.y = roundInBounds(floor(pos.y), grid) - offset;
        xCheck.x =
            pos.x + (pos.y - xCheck.y) * tan(degToRad(Fiindex_xng(ang_x - 270)));

        yCheck.x = roundInBounds(ceil(pos.x), grid) + offset;
        yCheck.y =
            pos.y - (yCheck.x - pos.x) * tan(degToRad(Fiindex_xng(360 - ang_x)));

        zCheck.x = pos.x + vec2Diagonal * half_sin(degToRad(ang_x - 270));
        zCheck.y = pos.y - vec2Diagonal * half_cos(degToRad(ang_x - 270));

        xCheck.z =
            pos.z -
            tan(degToRad(fabs(ang_y))) * vec2Distance(pos, xCheck) *
                half_cos(degToRad(fabs(Fiindex_xng((index_x * FOV) / SCREEN_WIDTH - FOV / 2))));
        yCheck.z =
            pos.z -
            tan(degToRad(fabs(ang_y))) * vec2Distance(pos, yCheck) *
                half_cos(degToRad(fabs(Fiindex_xng((index_x * FOV) / SCREEN_WIDTH - FOV / 2))));

        x.x = floor(xCheck.x);
        x.y = xCheck.y - 1;
        x.z = floor(xCheck.z);

        y.x = yCheck.x;
        y.y = floor(yCheck.y);
        y.z = floor(yCheck.z);
        }

        if (ang_y < 90) {
        z.z = zCheck.z - 1;
        z.x = floor(zCheck.x);
        z.y = floor(zCheck.y);
        } else {
        z.z = zCheck.z;
        z.x = floor(zCheck.x);
        z.y = floor(zCheck.y);
        }

        if (inBounds(x, grid) && !xFound) {
        int voxel_rgbSUM = map[(int)(x.z * grid * grid + x.y * grid + x.x)].x +
                            map[(int)(x.z * grid * grid + x.y * grid + x.x)].y +
                            map[(int)(x.z * grid * grid + x.y * grid + x.x)].z;
        if (voxel_rgbSUM > 0) {
            float dist_x = vec3Distance(pos, xCheck);
            xFound = true;
            if (dist_x < minDist) {
            minDist = dist_x;
            minVoxel =
                map[(int)(x.z) * grid * grid + (int)(x.y) * grid + (int)(x.x)];
            minShade = 50;
            }
        }
        }

        if (inBounds(y, grid) && !yFound) {
        int voxel_rgbSUM = map[(int)(y.z * grid * grid + y.y * grid + y.x)].x +
                            map[(int)(y.z * grid * grid + y.y * grid + y.x)].y +
                            map[(int)(y.z * grid * grid + y.y * grid + y.x)].z;
        if (voxel_rgbSUM > 0) {
            float dist_y = vec3Distance(pos, yCheck);
            yFound = true;
            if (dist_y < minDist) {
            minDist = dist_y;
            minVoxel =
                map[(int)(y.z) * grid * grid + (int)(y.y) * grid + (int)(y.x)];
            minShade = 25;
            }
        }
        }

        if (inBounds(z, grid) && !zFound) {
        int voxel_rgbSUM = map[(int)(z.z * grid * grid + z.y * grid + z.x)].x +
                            map[(int)(z.z * grid * grid + z.y * grid + z.x)].y +
                            map[(int)(z.z * grid * grid + z.y * grid + z.x)].z;
        if (voxel_rgbSUM > 0) {
            float dist_z = vec3Distance(pos, zCheck);
            zFound = true;
            if (dist_z < minDist) {
            minDist = dist_z;
            minVoxel =
                map[(int)(z.z) * grid * grid + (int)(z.y) * grid + (int)(z.x)];
            minShade = 0;
            }
        }
        }

        if (floor(pos.z) - offset < 0 && ceil(pos.z) + offset > grid &&
            ceil(pos.y) + offset > grid && ceil(pos.x) + offset > grid &&
            floor(pos.y) - offset < 0 && floor(pos.x) - offset < 0) {
        break;
        }
        if (xFound && yFound && zFound) {
        found = true;
        break;
        }
    }

    if (!xFound && !yFound && !zFound) {
        doutput[index] = (int4){0, 0, 0, 0};
    }

    doutput[index] = (int4){LimitColor(minVoxel.x - minDist * 1 - minShade),
            LimitColor(minVoxel.y - minDist * 1 - minShade),
            LimitColor(minVoxel.z - minDist * 1 - minShade), 255};
}