@echo off
subst W: /D
subst W: C:\Users\Marwan\Documents\Repos\handmade_ray
W:
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
set path="W:\scripts";%path%
cls