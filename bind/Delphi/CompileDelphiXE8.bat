set Files=src\VersyCommon.pas src\VersyFreeType.pas src\VersyJpeg.pas src\VersyPDF.pas src\VersyPDFInt.pas src\VersyPDFTypes.pas src\VersyPng.pas src\VersyTiff.pas src\VersyZLib.pas
c:\Borland\DelphiXE8\dcc32.exe %Files% -DDEBUGOFF -B -Nlib\DelphiXE8\Lib32 >delphiXE8result.log
c:\Borland\DelphiXE8\dcc64.exe %Files% -DDEBUGOFF -B -Nlib\DelphiXE8\Lib64 >>delphiXE8result.log

