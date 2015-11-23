# glSampleIDReproducer
Nvidia OpenGL GLSL bug, which shows problems with gl_SampleID and numSamples > 8

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
