#include "./src/functions.hcl"
#include "./src/Data.hcl"
#include "./src/ray.hcl"


const float reflectionCoefficent = 0;



__kernel void compute_pixels_kernel(__global uint4 *map, __global uchar3 *doutput,
                                    const int n, const int FOV,
                                    const float3 pos, const float3 vec, const float3 light, const int d) {
                                      
    size_t index_x= get_global_id(0);
    size_t index_y = get_global_id(1);
    float2 resolution = {get_num_groups(0) * get_local_size(0), get_num_groups(1) * get_local_size(1)};
    size_t index = index_y * convert_int(resolution.x) + index_x;
    depth = d;

    float f = tan(radians((float)FOV/2));

    float2 PlaneScale;
    PlaneScale.x = 2 * (float)index_x / resolution.x - 1;
    PlaneScale.y = 2 * (float)index_y / resolution.y - 1;

    float aspectRatio = resolution.y/resolution.x;
    float3 v_x,v_y, c_y;

    v_x = PlaneScale.x * normalize(cross((float3){0,0,1}, vec)) * f;
    c_y = normalize(cross((float3){0,0,1}, vec));
    v_y = PlaneScale.y * normalize(cross(c_y, vec)) * aspectRatio * f;

    float3 vector = vec + v_x + v_y;
    vector = normalize(vector);
    
    ray_data voxel;
    
    voxel = RayCast(pos, pos + vector * (float)1e9,vector,n,map,50);

    
    if(voxel.hit){


        ray_data voxel2, illumination;

        float3 reflection_vec = vector - 2*dot(vector,voxel.normal)*voxel.normal;
        
        float3 add_v = {
            voxel.normal.x*f_error*2, 
            voxel.normal.y*f_error*2, 
            voxel.normal.z*f_error*2
        };

        voxel.position = voxel.position + add_v;
        float3 light_vector = normalize(light-voxel.position);

        illumination = RayCast(voxel.position, light, light_vector,n,map,30);

        if(!illumination.hit){

            float lighting = (dot(light_vector, reflection_vec) + 1) / 2 + 1/5;
            float lDistCoefficent = vec3Distance(voxel.position, light)/5;

            if(reflectionCoefficent > 0.01){
                voxel2 = RayCast(voxel.position, voxel.position + reflection_vec * (float)1e9, normalize(reflection_vec),n,map,40);

                float3 raw_color = ( (float3){(float)voxel.color.x,(float)voxel.color.y,(float)voxel.color.z} + (float3){(float)voxel2.color.x * reflectionCoefficent,(float)voxel2.color.y * reflectionCoefficent,(float)voxel2.color.z * reflectionCoefficent}) / ( 1 + reflectionCoefficent );
                uchar3 illuminated_color = Color(raw_color * lighting - lDistCoefficent);

                doutput[index] = illuminated_color;
            }else{
                uchar3 illuminated_color = Color(convert_float3(voxel.color) * lighting - lDistCoefficent);

                doutput[index] = illuminated_color;
            }

        }else{
            doutput[index] = (uchar3){voxel.color.x/5, voxel.color.y/5, voxel.color.z/5};
        }


    }else{
        doutput[index] = voxel.color;
    }
}