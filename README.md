# SVO Renderer
## 3D voxel raytracing engine on the GPU using OpenCL for parallel pixel computation (raycasting on an SVO)

### TO do:
  - Loading from files
  - Fixing bugs
  - Delta frames
  - SVO compression

![alt text](https://cdn.discordapp.com/attachments/912320676269015040/950796442446991440/unknown.png)

### requires:
* SDL2
* OpenCL
* SDL_ttf

### Building
```
git clone --recursive https://github.com/https://github.com/MegaSomething/Voxel_Renderer.git
cd Voxel_Renderer

mkdir build
cd build

cmake ..
cmake --build .
```
