set Files=src\VersyCommon.pas src\VersyFreeType.pas src\VersyJpeg.pas src\VersyPDF.pas src\VersyPDFInt.pas src\VersyPDFTypes.pas src\VersyPng.pas src\VersyTiff.pas src\VersyZLib.pas
c:\Borland\Delphi10\dcc32.exe %Files% -DDEBUGOFF -B -Nlib\Delphi10\Lib32 >delphi10result.log
c:\Borland\Delphi10\dcc64.exe %Files% -DDEBUGOFF -B -Nlib\Delphi10\Lib64 >>delphi10result.log

