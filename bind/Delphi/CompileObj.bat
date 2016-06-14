@echo off
del obj\*.obj >nul
c:\bcb\bin\bcc32 +cfg\co2.cfg ..\..\sources\freetype\*.c >cresult.log
c:\bcb\bin\bcc32 +cfg\co2.cfg ..\..\sources\flate\*.c >cresult.log
c:\bcb\bin\bcc32 +cfg\co2.cfg ..\..\sources\png\*.c >cresult.log
c:\bcb\bin\bcc32 +cfg\co2.cfg ..\..\sources\jpeg\*.c >cresult.log
c:\bcb\bin\bcc32 +cfg\co2.cfg ..\..\sources\tiff\*.c >cresult.log
c:\bcb\bin\bcc32 +cfg\co2.cfg ..\..\sources\versypdf\sources\*.c >cresult.log

