unit VersyPng;
{$i vspas.Inc}
interface
uses VersyPDFInt;


function PDFImageLoadPNGFromStream ( P:Pointer ):Pointer; register; external;

implementation

uses VersyCommon, VersyZLib, Windows;


// PNG
{$l ..\obj\VSpng.obj}
{$l ..\obj\VSExcept.obj}
{$l ..\obj\VSBaseA.obj}
{$l ..\obj\VSMisc.obj}
{$l ..\obj\VSBaseA.obj}
{$l ..\obj\VSLibA.obj}
{$l ..\obj\pngpread.obj }
{$l ..\obj\pngread.obj  }
{$l ..\obj\pngget.obj   }
{$l ..\obj\pngrio.obj   }
{$l ..\obj\pngvcrd.obj  }
{$l ..\obj\pngwio.obj   }
{$l ..\obj\pngwrite.obj }
{$l ..\obj\pngwtran.obj }
{$l ..\obj\png.obj}
{$l ..\obj\pngwutil.obj }
{$l ..\obj\pngrutil.obj }
{$l ..\obj\pngrtran.obj }
{$l ..\obj\pngtrans.obj }
{$l ..\obj\pngset.obj   }
{$l ..\obj\pngmem.obj   }
{$l ..\obj\pngerror.obj}
end.


















