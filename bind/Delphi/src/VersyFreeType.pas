unit VersyFreeType;
{$i vspas.Inc}
interface
uses VersyPDFInt, Windows;

function PDFType1GetFontName ( A,B: Pointer):Integer;register; external;
procedure PDFType1GetFontInfo ( a,b,c:Pointer; L1,l2,l3: PInteger); register; external;

implementation

uses VersyCommon;

// FreeType objects
{$l obj\versypdf\VSType1.obj}



{$l obj\versypdf\VSBaseA.obj}
{$l obj\versypdf\VSMisc.obj}
{$l obj\versypdf\VSBaseA.obj}
{$l obj\versypdf\VSExcept.obj}
{$l obj\versypdf\VSLibA.obj}

{$l obj\freetype\vsftsystem.obj }
{$l obj\freetype\ftinit.obj }
{$l obj\freetype\t1afm.obj    }
{$l obj\freetype\t1driver.obj }
{$l obj\freetype\t1objs.obj   }
{$l obj\freetype\cidriver.obj }
{$l obj\freetype\psmodule.obj }
{$l obj\freetype\pshmod.obj   }
{$l obj\freetype\pshrec.obj   }
{$l obj\freetype\psauxmod.obj   }
{$l obj\freetype\t1cmap.obj   }
{$l obj\freetype\t1gload.obj  }
{$l obj\freetype\t1load.obj   }
{$l obj\freetype\t1parse.obj  }
{$l obj\freetype\t1decode.obj }
{$l obj\freetype\psobjs.obj   }
{$l obj\freetype\pshalgo.obj  }
{$l obj\freetype\pshglob.obj  }
{$l obj\freetype\cidobjs.obj }
{$l obj\freetype\cidgload.obj }
{$l obj\freetype\psauxmod.obj   }
{$l obj\freetype\cidload.obj }
{$l obj\freetype\cidparse.obj }
{$l obj\freetype\ftobjs.obj }
{$l obj\freetype\ftapi.obj }
{$l obj\freetype\ftbbox.obj }
{$l obj\freetype\ftbdf.obj }
{$l obj\freetype\ftinit.obj }
{$l obj\freetype\ftlzw.obj }
{$l obj\freetype\ftmm.obj }
{$l obj\freetype\ftotval.obj }
{$l obj\freetype\ftpfr.obj }
{$l obj\freetype\ftrfork.obj }
{$l obj\freetype\ftstroke.obj }
{$l obj\freetype\ftsynth.obj }
{$l obj\freetype\ftglyph.obj }
{$l obj\freetype\ftbitmap.obj }
{$l obj\freetype\ftdbgmem.obj }
{$l obj\freetype\ftdebug.obj }
{$l obj\freetype\ftoutln.obj }
{$l obj\freetype\ftobjs.obj }
{$l obj\freetype\ftgloadr.obj }
{$l obj\freetype\ftsystem.obj }
{$l obj\freetype\ftcalc.obj }
{$l obj\freetype\fttrigon.obj }
{$l obj\freetype\fttype1.obj  }
{$l obj\freetype\t1gload.obj  }
{$l obj\freetype\t1load.obj   }
{$l obj\freetype\ftstream.obj }
{$l obj\freetype\ftutil.obj   }

end.




