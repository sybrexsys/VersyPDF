set Files=src\VersyCommon.pas src\VersyFreeType.pas src\VersyJpeg.pas src\VersyPDF.pas src\VersyPDFInt.pas src\VersyPDFTypes.pas src\VersyPng.pas src\VersyTiff.pas src\VersyZLib.pas
c:\Borland\DelphiXE3\dcc32.exe %Files% -DDEBUGOFF -B -Nlib\DelphiXE3\Lib32 >delphiXE3result.log
c:\Borland\DelphiXE3\dcc64.exe %Files% -DDEBUGOFF -B -Nlib\DelphiXE3\Lib64 >>delphiXE3result.log

