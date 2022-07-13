#pragma once


void swap(float a, float b){
    float x = b;
    b=a;
    a=x;}

float vec3Distance(float3 vec1, float3 vec2) {
  float d[] = {fabs(vec1.x - vec2.x), 
              fabs(vec1.y - vec2.y),
              fabs(vec1.z - vec2.z)};

  if (d[0] < d[1])
    swap(d[0], d[1]);
  if (d[0] < d[2])
    swap(d[0], d[2]);
    
  return d[0] * sqrt(1.0 + d[1] / d[0] + d[2] / d[0]);
}


bool inBounds(float3 v, float n, float b) {
  return v.x > b && v.x <= n && v.y > b && v.y <= n && v.z > b &&
         v.z <= n;}


uchar3 Gradient(uchar3 c1, uchar3 c2, int q){
  return (uchar3){
    min(c2.x,c1.x)+((c2.x-c1.x)/255)*q,
    min(c2.y,c1.y)+((c2.y-c1.y)/255)*q,
    min(c2.z,c1.z)+((c2.z-c1.z)/255)*q
  };
}

int semn(float x){
  if(x >= 0)return 1;
  else return -1;
}

