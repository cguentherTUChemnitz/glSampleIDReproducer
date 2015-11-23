#!/bin/sh

#create toplevel build directory containing release and debug build
mkdir -p ../glSampleIDReproEclipse/Release
mkdir -p ../glSampleIDReproEclipse/Debug

#the DCMAKE_CXX_COMPILER_ARG1=-std=c++14 option is necessary for correct eclipse indexer C++14 recognition
cmake -E chdir ../glSampleIDReproEclipse/Release/ cmake -G"Eclipse CDT4 - Unix Makefiles" -DCMAKE_ECLIPSE_VERSION=4.5 -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER_ARG1=-std=c++14 ../../glSampleIDReproducer

cmake -E chdir ../glSampleIDReproEclipse/Debug/ cmake -G"Eclipse CDT4 - Unix Makefiles" -DCMAKE_ECLIPSE_VERSION=4.5 -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER_ARG1=-std=c++14 ../../glSampleIDReproducer


