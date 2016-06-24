@echo off


IF NOT EXIST "c:\program files (x86)\" GOTO NotX64

set current_dir=%CD%
set builder="%VS80COMNTOOLS%\..\..\VC\vcpackages\VCBuild.exe"

call "%VS80COMNTOOLS%\..\..\VC\vcvarsall.bat" x86_amd64

echo LIBs

%builder% "Libs\flate\flate_vs2005.vcproj" /nologo /rebuild "Release|x64"

%builder% "Libs\freetype\freetype_vs2005.vcproj" /nologo /rebuild "Release|x64"

%builder% "Libs\jpeg\jpeg_vs2005.vcproj" /nologo /rebuild "Release|x64"

%builder% "Libs\png\png_vs2005.vcproj" /nologo /rebuild "Release|x64"

%builder% "Libs\tiff\Tiff_vs2005.vcproj" /nologo /rebuild "Release|x64"

%builder% "Libs\versypdf\versypdf_vs2005.vcproj" /nologo /rebuild "Release|x64"

echo Build PDF SDK VS2005 versions

%builder% "BytescoutPDF_vs2005.vcproj" /nologo /rebuild "Release (DEMO)|x64"
%builder% "BytescoutPDF_vs2005.vcproj" /nologo /rebuild "Release (FULL)|x64"

:NotX64
echo NOT x64 WINDOWS! BUILD ABORTED
echo 