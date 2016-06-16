set Files=src\VersyCommon.pas src\VersyFreeType.pas src\VersyJpeg.pas src\VersyPDF.pas src\VersyPDFInt.pas src\VersyPDFTypes.pas src\VersyPng.pas src\VersyTiff.pas src\VersyZLib.pas
c:\Borland\DelphiXE4\dcc32.exe %Files% -DDEBUGOFF -B -Nlib\DelphiXE4\Lib32 >delphiXE4result.log
c:\Borland\DelphiXE4\dcc64.exe %Files% -DDEBUGOFF -B -Nlib\DelphiXE4\Lib64 >>delphiXE4result.log

