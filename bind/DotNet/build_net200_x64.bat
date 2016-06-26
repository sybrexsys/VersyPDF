@echo off

set current_dir=%CD%
set builder="%VS80COMNTOOLS%\..\..\VC\vcpackages\VCBuild.exe"

call "%VS80COMNTOOLS%\..\..\VC\bin\vcvars32.bat"

echo Cleaning

%builder% "2.0\versypdfsrc.vcproj" /nologo /clean "Release|x64"
%builder% "2.0\versypdf.vcproj" /nologo /clean "Release|x64"


echo LIBs
%builder% "2.0\versypdfsrc.vcproj" /nologo /rebuild "Release|x64"
%builder% "2.0\versypdf.vcproj" /nologo /rebuild "Release|x64"


