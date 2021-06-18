@echo off

pushd W:\build
    del /Q *.obj *.map *.pdb *.exe *.cpp *.log *.recipe *.txt *.exp *.ilk *.lib *.dll > NUL 2> NUL
popd