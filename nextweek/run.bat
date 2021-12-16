@echo off
if %1 == 0 (
    cmake -G"Visual Studio 16 2019" -S . -Bbuild
    cmake --build build --target raytracer
) else if %1 == 1 (
    cmake --build build --target raytracer
)
set config_name=%2
.\target\Debug\raytracer.exe 300 %config_name%
start image.ppm