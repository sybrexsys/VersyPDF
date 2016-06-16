@echo off
del obj\x64\*.obj >nul
set options=-c -Ic:\mvc\include -Ic:\mvc\include\SDK -I..\..\sources\freetype -I..\..\sources\flate -I..\..\sources\png -I..\..\sources\jpeg -I..\..\sources\tiff -I..\..\sources\versypdf -I..\..\sources\versypdf\sources -Foobj\x64\ -nologo /GS- /TC -DDELPHI /Gs999999 /Yd
c:\mvc\cl ..\..\sources\freetype\*.c %options% >c64result.log
c:\mvc\cl ..\..\sources\flate\*.c %options% >>c64result.log
c:\mvc\cl ..\..\sources\png\*.c %options% >>c64result.log
c:\mvc\cl ..\..\sources\jpeg\*.c %options% >>c64result.log
c:\mvc\cl ..\..\sources\tiff\*.c %options% >>c64result.log
c:\mvc\cl ..\..\sources\versypdf\sources\*.c %options% >>c64result.log


