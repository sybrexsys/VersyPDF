set Files=src\VersyCommon.pas src\VersyFreeType.pas src\VersyJpeg.pas src\VersyPDF.pas src\VersyPDFInt.pas src\VersyPDFTypes.pas src\VersyPng.pas src\VersyTiff.pas src\VersyZLib.pas
c:\Borland\DelphiXE6\dcc32.exe %Files% -DDEBUGOFF -B -Nlib\DelphiXE6\Lib32 >delphiXE6result.log
c:\Borland\DelphiXE6\dcc64.exe %Files% -DDEBUGOFF -B -Nlib\DelphiXE6\Lib64 >>delphiXE6result.log

