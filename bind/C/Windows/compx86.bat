@ECHO OFF
REM -------------------------------------------------------------------
set CMP=d:\release\compilers
set path=%CMP%\msvstk\Bin;%path%
set include=%CMP%\msvstk\Include;%CMP%\PSDK\Include;%include%
set lib=%CMP%\msvstk\Lib;%CMP%\PSDK\Lib;%lib%

set includes=/I "..\..\..\sources\versypdf\sources" /I "..\..\..\sources\versypdf" /I "..\..\..\sources\flate" /I "..\..\..\sources\jpeg" /I "..\..\..\sources\png" /I "..\..\..\sources\tiff" /I "..\..\..\sources\png" /I "..\..\..\sources\freetype" 
set defines=/D"_WINDOWS" /D"_WIN32" /D "WIN32"  /D "_CRT_SECURE_NO_WARNINGS" 
set sources=/TC ..\..\..\sources\versypdf\sources\*.c ..\..\..\sources\freetype\*.c ..\..\..\sources\jpeg\*.c ..\..\..\sources\tiff\*.c ..\..\..\sources\flate\*.c ..\..\..\sources\png\*.c   

del tmp\*.obj

cl %includes% %defines% /O2  /EHsc /Fotmp\  /TC /MP4 /W3 /nologo /c %sources%

link.exe -lib /out:".\out\VersyPDF32.lib"  .\tmp\*.obj
                             
cl %includes% %defines% /O2  /EHsc /Fotmp\ /TP  /W3 /nologo /c src\*.c*

link.exe /DLL /out:".\Out\VersyPDF32.dll"  .\tmp\*.obj  /INCREMENTAL:NO /SUBSYSTEM:WINDOWS /MACHINE:X86 /OPT:ICF  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib secur32.lib

del tmp\*.obj


