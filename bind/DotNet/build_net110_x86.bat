@echo off
set current_dir=%CD%
set sources=..\..\sources\versypdf\sources\*.c ..\..\sources\freetype\*.c ..\..\sources\jpeg\*.c ..\..\sources\tiff\*.c ..\..\sources\flate\*.c ..\..\sources\png\*.c   
rem "D:\Program Files (x86)\Microsoft Visual Studio .NET 2003\Common7\IDE\"devenv.exe 
set builder="D:\Program Files (x86)\Microsoft Visual Studio .NET 2003\Common7\IDE\devenv.exe"
call "D:\Program Files (x86)\Microsoft Visual Studio .NET 2003\Common7\Tools\vsvars32.bat"

echo Cleaning...
%builder% /clean "Release" /project "VersyPDFSrc" "1.1\VersyPDFNET_vs2003.sln"
%builder% /clean "Release" /project "VersyPDF" "1.1\VersyPDFNET_vs2003.sln"

echo Building LIBs
%builder% /build "Release" /project "VersyPDFSrc" "1.1\VersyPDFNET_vs2003.sln"
%builder% /build "Release" /project "VersyPDF" "1.1\VersyPDFNET_vs2003.sln"


