@echo off

pushd W:\build
    cl -nologo -Zo -Z7 -FC -Oi -Od -GR- -EHa- -MTd -Gm- -fp:fast -fp:except-^
       -W4 -WX -wd4201 -wd4100 -wd4189 -wd4505 -wd4456^
       -DHANDMADE_WIN32=1 -DHANDMADE_SLOW=1 -DHANDMADE_INTERNAL=1 -D_CRT_SECURE_NO_WARNINGS^
       W:\code\ray.cpp^
       /link /incremental:no  /opt:ref^
       user32.lib gdi32.lib winmm.lib opengl32.lib
popd