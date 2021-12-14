@REM cmake -G"Visual Studio 16 2019" -S . -Bbuild
cmake --build build --target raytracer
.\target\Debug\raytracer.exe 300
@REM .\target\Debug\raytracer.exe 300 texture_config.data
start image.ppm