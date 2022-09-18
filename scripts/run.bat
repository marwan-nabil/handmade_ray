@echo off

pushd W:\handmade_ray\data
    ..\build\ray.exe %*
    start test.bmp
popd