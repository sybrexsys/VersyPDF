@echo off

set current_dir=%CD%
set builder="%VS80COMNTOOLS%\..\..\VC\vcpackages\VCBuild.exe"

call "%VS80COMNTOOLS%\..\..\VC\bin\vcvars32.bat"

echo LIBs

%builder% "Libs\flate\flate_vs2005.vcproj" /nologo /rebuild "Release|Win32"

%builder% "Libs\freetype\freetype_vs2005.vcproj" /nologo /rebuild "Release|Win32"

%builder% "Libs\jpeg\jpeg_vs2005.vcproj" /nologo /rebuild "Release|Win32"

%builder% "Libs\png\png_vs2005.vcproj" /nologo /rebuild "Release|Win32"

%builder% "Libs\tiff\Tiff_vs2005.vcproj" /nologo /rebuild "Release|Win32"

%builder% "Libs\versypdf\versypdf_vs2005.vcproj" /nologo /rebuild "Release|Win32"

echo Build PDF SDK VS2005 versions

%builder% "BytescoutPDF_vs2005.vcproj" /nologo /rebuild "Release (DEMO)|Win32"
%builder% "BytescoutPDF_vs2005.vcproj" /nologo /rebuild "Release (FULL)|Win32"

