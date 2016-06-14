set Files=VersyCommon.pas VersyFreeType.pas VersyJpeg.pas VersyPDF.pas VersyPDFInt.pas VersyPDFTypes.pas VersyPng.pas VersyTiff.pas VersyZLib.pas
del *.dcu
e:\Borland\Delphi6\dcc32.exe %Files% -DDEBUGOFF
copy *.dcu dcu\
del *.dcu
