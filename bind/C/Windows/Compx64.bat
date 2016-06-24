@ECHO OFF
REM -------------------------------------------------------------------
set CMP=d:\release\compilers
set path=%CMP%\PSDK\AMD64;%path%
set include=%CMP%\PSDK\Include;%CMP%\vs8\Include;%include%
set lib=%CMP%\PSDK\Lib\AMD64

set includes=/I "..\..\..\sources\versypdf\sources" /I "..\..\..\sources\versypdf" /I "..\..\..\sources\flate" /I "..\..\..\sources\jpeg" /I "..\..\..\sources\png" /I "..\..\..\sources\tiff" /I "..\..\..\sources\png" /I "..\..\..\sources\freetype" 
set defines=/D"_WINDOWS" /D"_WIN32" /D "WIN32"  /D "_CRT_SECURE_NO_WARNINGS" 
set sources=/TC ..\..\..\sources\versypdf\sources\*.c ..\..\..\sources\freetype\*.c ..\..\..\sources\jpeg\*.c ..\..\..\sources\tiff\*.c ..\..\..\sources\flate\*.c ..\..\..\sources\png\*.c   

del tmp\*.obj

cl %includes% %defines% /O2  /FD /EHsc /GS- /Fotmp\  /TC /MP4 /W3 /nologo /c %sources%

link.exe -lib /out:".\out\VersyPDF64.lib"  .\tmp\*.obj

cl %includes% %defines% /O2  /FD /EHsc /Fotmp\ /GS- /TP  /W3 /nologo /c src\*.c*

link.exe /DLL /out:".\Out\VersyPDF64.dll"  .\tmp\*.obj  /INCREMENTAL:NO /SUBSYSTEM:WINDOWS /MACHINE:x64 /OPT:REF /OPT:ICF  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib secur32.lib

del tmp\*.obj
del *.idb



