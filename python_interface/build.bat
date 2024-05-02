@echo off

rem 设置SWIG的路径
set SWIG_PATH=D:\Program Files\swigwin-4.2.1\swig.exe

rem 设置头文件目录路径
set INCLUDE_PATH=..\HingeFramework

rem 设置输出文件路径
set SOURCE_PATH=.\src

rem 设置输出文件路径
set OUTPUT_PATH=.\build

rem 设置SETUP.PY的路径
set SETUP_PATH=.\setup.py

rem 执行SWIG命令
swig -python -c++ -I%INCLUDE_PATH% -outdir %OUTPUT_PATH% -o %OUTPUT_PATH%\hinge_framework.cxx %SOURCE_PATH%\hinge_framework.i

echo SWIG wrapper code generated successfully!

rem python %SETUP_PATH% build_ext

rem echo Python module compiled successfully!

pause