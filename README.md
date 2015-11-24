# glSampleIDReproducer
Nvidia OpenGL GLSL bug, which shows problems with gl_SampleID and numSamples > 8

## Idea:
- use multi-sampling with n samples
- render one pixel sized window
- sum up the gl_SampleIDs using an atomic_counter
- compare to CPU calculated reference value of summed sampleIDs --> they are not equal for number of samples > 8

## Dependencies
- cmake
- glfw
- OpenGL
- glu
- glew
- C++11 compiler

## Build
```sh
git clone https://github.com/cguentherTUChemnitz/glSampleIDReproducer
cd ./glSampleIDReproducer
mkdir build
cd ./build
cmake ..
make all
./glSampleIDRepro
```
