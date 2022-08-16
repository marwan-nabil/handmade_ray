@echo off

pushd F:\ray\data
    ..\build\ray.exe %*
    start test.bmp
popd