@echo off

pushd W:\build
    del /Q *.obj *.map *.pdb *.exe *.cpp *.log *.recipe *.txt *.exp *.ilk *.lib *.dll *.bmp > NUL 2> NUL
popd