@echo off

set current_dir=%CD%
set builder="%VS80COMNTOOLS%\..\..\VC\vcpackages\VCBuild.exe"

call "%VS80COMNTOOLS%\..\..\VC\bin\vcvars32.bat"

echo Cleaning

%builder% "2.0\versypdfsrc.vcproj" /nologo /clean "Release|Win32"
%builder% "2.0\versypdf.vcproj" /nologo /clean "Release|Win32"


echo LIBs
%builder% "2.0\versypdfsrc.vcproj" /nologo /rebuild "Release|Win32"
%builder% "2.0\versypdf.vcproj" /nologo /rebuild "Release|Win32"


