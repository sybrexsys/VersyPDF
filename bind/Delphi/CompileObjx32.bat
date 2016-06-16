@echo off
del obj\x32\*.obj >nul
c:\bcb\bin\bcc32 +cfg\co2.cfg ..\..\sources\freetype\*.c >c32result.log
c:\bcb\bin\bcc32 +cfg\co2.cfg ..\..\sources\flate\*.c >>c32result.log
c:\bcb\bin\bcc32 +cfg\co2.cfg ..\..\sources\png\*.c >>c32result.log
c:\bcb\bin\bcc32 +cfg\co2.cfg ..\..\sources\jpeg\*.c >>c32result.log
c:\bcb\bin\bcc32 +cfg\co2.cfg ..\..\sources\tiff\*.c >>c32result.log
c:\bcb\bin\bcc32 +cfg\co2.cfg ..\..\sources\versypdf\sources\*.c >>c32result.log

