@echo off
del obj\x32\*.obj >nul
c:\bcb\bin\bcc32 +cfg\c.cfg ..\..\sources\freetype\*.c >c32result.log
c:\bcb\bin\bcc32 +cfg\c.cfg ..\..\sources\flate\*.c >>c32result.log
c:\bcb\bin\bcc32 +cfg\c.cfg ..\..\sources\png\*.c >>c32result.log
c:\bcb\bin\bcc32 +cfg\c.cfg ..\..\sources\jpeg\*.c >>c32result.log
c:\bcb\bin\bcc32 +cfg\c.cfg ..\..\sources\tiff\*.c >>c32result.log
c:\bcb\bin\bcc32 +cfg\c.cfg ..\..\sources\versypdf\sources\*.c >>c32result.log

