set Files=src\VersyCommon.pas src\VersyFreeType.pas src\VersyJpeg.pas src\VersyPDF.pas src\VersyPDFInt.pas src\VersyPDFTypes.pas src\VersyPng.pas src\VersyTiff.pas src\VersyZLib.pas
c:\Borland\DelphiXE7\dcc32.exe %Files% -DDEBUGOFF -B -Nlib\DelphiXE7\Lib32 >delphiXE7result.log
c:\Borland\DelphiXE7\dcc64.exe %Files% -DDEBUGOFF -B -Nlib\DelphiXE7\Lib64 >>delphiXE7result.log

