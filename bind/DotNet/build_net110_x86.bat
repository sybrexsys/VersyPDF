@echo off

set current_dir=%CD%
rem "D:\Program Files (x86)\Microsoft Visual Studio .NET 2003\Common7\IDE\"devenv.exe 
set builder="D:\Program Files (x86)\Microsoft Visual Studio .NET 2003\Common7\IDE\devenv.exe"

call "D:\Program Files (x86)\Microsoft Visual Studio .NET 2003\Common7\Tools\vsvars32.bat"

echo Cleaning...

%builder% /clean "Release" /project "VersyPDF" VersyPDFNET_vs2003.sln


echo Building LIBs

%builder% /build "Release" /project "VersyPDF" VersyPDFNET_vs2003.sln


