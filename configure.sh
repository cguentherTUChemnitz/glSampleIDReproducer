#!/bin/sh

#create toplevel build directory containing release and debug build
mkdir -p ../glSampleIDReporEclipse/Release
mkdir -p ../glSampleIDReporEclipse/Debug

#the DCMAKE_CXX_COMPILER_ARG1=-std=c++14 option is necessary for correct eclipse indexer C++14 recognition
cmake -E chdir ../glSampleIDReporEclipse/Release/ cmake -G"Eclipse CDT4 - Unix Makefiles" -DCMAKE_ECLIPSE_VERSION=4.5 -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER_ARG1=-std=c++14 ../../glSampleIDRepro

cmake -E chdir ../glSampleIDReporEclipse/Debug/ cmake -G"Eclipse CDT4 - Unix Makefiles" -DCMAKE_ECLIPSE_VERSION=4.5 -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER_ARG1=-std=c++14 ../../glSampleIDRepro


