@echo off

if not exist "build" (
    mkdir build
    echo "Directory 'build' is created"
)

rem Building a project
cd build
cmake -G "MinGW Makefiles" ..
mingw32-make

rem Run project
cd ..
call run_server_and_client.bat

