@echo off

set LastError=%ERRORLEVEL%
if not %LastError%==0 goto :end

pushd W:\data
    ..\build\ray.exe
    start test.bmp
popd

:end