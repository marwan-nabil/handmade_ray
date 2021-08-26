@echo off

pushd W:\data
    ..\build\ray.exe %*
    start test.bmp
popd
