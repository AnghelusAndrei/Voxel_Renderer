
#pragma OPENCL EXTENSION cl_khr_fp64 : enable
const sampler_t simple_sampler=CLK_ADDRESS_REPEAT;

__kernel void compute_pixels_kernel(__global double *ddata, __global uint *doutput, const uint grid, const uint2 resolution, const uint SCREEN_WIDTH, const uint SCREEN_HEIGHT, const float3 pos, const float2 ang)
{
    const size_t x=get_global_id(0);
}