set Files=src\VersyCommon.pas src\VersyFreeType.pas src\VersyJpeg.pas src\VersyPDF.pas src\VersyPDFInt.pas src\VersyPDFTypes.pas src\VersyPng.pas src\VersyTiff.pas src\VersyZLib.pas
c:\Borland\DelphiXE2\dcc32.exe %Files% -DDEBUGOFF -B -Nlib\DelphiXE2\Lib32 >delphiXE2result.log
c:\Borland\DelphiXE2\dcc64.exe %Files% -DDEBUGOFF -B -Nlib\DelphiXE2\Lib64 >>delphiXE2result.log

