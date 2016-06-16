unit VersyPng;
{$i vspas.Inc}
interface
uses VersyPDFInt,{$ifdef USENAMESPACE}WinAPI.Windows{$else}Windows{$endif};


function PDFImageLoadPNGFromStream ( P:Pointer ):Pointer; register; external;

implementation

uses VersyCommon, VersyZLib;



// PNG
{$ifndef WIN64}
{$l ..\obj\x32\VSpng.obj}
{$l ..\obj\x32\VSExcept.obj}
{$l ..\obj\x32\VSBaseA.obj}
{$l ..\obj\x32\VSMisc.obj}
{$l ..\obj\x32\VSBaseA.obj}
{$l ..\obj\x32\VSLibA.obj}
{$l ..\obj\x32\pngpread.obj }
{$l ..\obj\x32\pngread.obj  }
{$l ..\obj\x32\pngget.obj   }
{$l ..\obj\x32\pngrio.obj   }
{$l ..\obj\x32\pngvcrd.obj  }
{$l ..\obj\x32\pngwio.obj   }
{$l ..\obj\x32\pngwrite.obj }
{$l ..\obj\x32\pngwtran.obj }
{$l ..\obj\x32\png.obj}
{$l ..\obj\x32\pngwutil.obj }
{$l ..\obj\x32\pngrutil.obj }
{$l ..\obj\x32\pngrtran.obj }
{$l ..\obj\x32\pngtrans.obj }
{$l ..\obj\x32\pngset.obj   }
{$l ..\obj\x32\pngmem.obj   }
{$l ..\obj\x32\pngerror.obj}
{$else}
{$l ..\obj\x64\VSpng.obj}
{$l ..\obj\x64\VSExcept.obj}
{$l ..\obj\x64\VSBaseA.obj}
{$l ..\obj\x64\VSMisc.obj}
{$l ..\obj\x64\VSBaseA.obj}
{$l ..\obj\x64\VSLibA.obj}
{$l ..\obj\x64\pngpread.obj }
{$l ..\obj\x64\pngread.obj  }
{$l ..\obj\x64\pngget.obj   }
{$l ..\obj\x64\pngrio.obj   }
{$l ..\obj\x64\pngvcrd.obj  }
{$l ..\obj\x64\pngwio.obj   }
{$l ..\obj\x64\pngwrite.obj }
{$l ..\obj\x64\pngwtran.obj }
{$l ..\obj\x64\png.obj}
{$l ..\obj\x64\pngwutil.obj }
{$l ..\obj\x64\pngrutil.obj }
{$l ..\obj\x64\pngrtran.obj }
{$l ..\obj\x64\pngtrans.obj }
{$l ..\obj\x64\pngset.obj   }
{$l ..\obj\x64\pngmem.obj   }
{$l ..\obj\x64\pngerror.obj}
{$endif}
end.


















