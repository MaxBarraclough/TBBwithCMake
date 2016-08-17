
REM Copy DLL to the folder containing this batch file,
REM using http://stackoverflow.com/a/19303478
REM COPY "C:\Users\mb\Downloads\tbb44\tbb44_20160526oss\bin\intel64\vc14\tbb_debug.dll" "%~dp0\tbb_debug.dll"

REM Create a symlink rather than copying.
REM Note that the args are in opposite order.
mklink "%~dp0\tbb_debug.dll" "C:\Users\mb\Downloads\tbb44\tbb44_20160526oss\bin\intel64\vc14\tbb_debug.dll"

mklink "%~dp0\tbb.dll" "C:\Users\mb\Downloads\tbb44\tbb44_20160526oss\bin\intel64\vc14\tbb.dll"

