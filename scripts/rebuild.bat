@echo off
cd /d "%~dp0\.."

echo cleaning old build
rmdir /s /q build

echo configuring project
mkdir build
cd build
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release

echo building & running clean
cmake --build . --target run
cd ..