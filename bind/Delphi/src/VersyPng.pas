unit VersyPng;
{$i vspas.Inc}
interface
uses VersyPDFInt;


function PDFImageLoadPNGFromStream ( P:Pointer ):Pointer; register; external;

implementation

uses VersyCommon, VersyZLib, Windows;


// PNG
{$l obj\versypdf\VSpng.obj}
{$l obj\versypdf\VSExcept.obj}
{$l obj\versypdf\VSBaseA.obj}
{$l obj\versypdf\VSMisc.obj}
{$l obj\versypdf\VSBaseA.obj}
{$l obj\versypdf\VSLibA.obj}
{$l obj\png\pngpread.obj }
{$l obj\png\pngread.obj  }
{$l obj\png\pngget.obj   }
{$l obj\png\pngrio.obj   }
{$l obj\png\pngvcrd.obj  }
{$l obj\png\pngwio.obj   }
{$l obj\png\pngwrite.obj }
{$l obj\png\pngwtran.obj }
{$l obj\png\png.obj}
{$l obj\png\pngwutil.obj }
{$l obj\png\pngrutil.obj }
{$l obj\png\pngrtran.obj }
{$l obj\png\pngtrans.obj }
{$l obj\png\pngset.obj   }
{$l obj\png\pngmem.obj   }
{$l obj\png\pngerror.obj}
end.


















