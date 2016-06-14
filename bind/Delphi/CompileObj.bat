del obj\flate\*.obj
del obj\freetype\*.obj
del obj\png\*.obj
del obj\jpeg\*.obj
del obj\tiff\*.obj
del obj\versypdf\*.obj

c:\bcb\bin\bcc32 +obj\co2.cfg obj\src\freetype\*.c 
copy *.obj obj\freetype
del *.obj
c:\bcb\bin\bcc32 +obj\co2.cfg obj\src\flate\*.c 
copy *.obj obj\flate
del *.obj
c:\bcb\bin\bcc32 +obj\co2.cfg obj\src\png\*.c 
copy *.obj obj\png
del *.obj
c:\bcb\bin\bcc32 +obj\co2.cfg obj\src\jpeg\*.c 
copy *.obj obj\jpeg
del *.obj
c:\bcb\bin\bcc32 +obj\co2.cfg obj\src\tiff\*.c 
copy *.obj obj\tiff
del *.obj
c:\bcb\bin\bcc32 +obj\co2.cfg obj\src\versypdf\sources\*.c 
copy *.obj obj\versypdf
del *.obj

