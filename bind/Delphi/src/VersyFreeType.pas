unit VersyFreeType;
{$i vspas.Inc}
interface
uses VersyPDFInt, Windows;

function PDFType1GetFontName ( A,B: Pointer):Integer;register; external;
procedure PDFType1GetFontInfo ( a,b,c:Pointer; L1,l2,l3: PInteger); register; external;

implementation

uses VersyCommon;

// FreeType objects
{$l obj\VSType1.obj}



{$l obj\VSBaseA.obj}
{$l obj\VSMisc.obj}
{$l obj\VSBaseA.obj}
{$l obj\VSExcept.obj}
{$l obj\VSLibA.obj}

{$l obj\vsftsystem.obj }
{$l obj\ftinit.obj }
{$l obj\t1afm.obj    }
{$l obj\t1driver.obj }
{$l obj\t1objs.obj   }
{$l obj\cidriver.obj }
{$l obj\psmodule.obj }
{$l obj\pshmod.obj   }
{$l obj\pshrec.obj   }
{$l obj\psauxmod.obj   }
{$l obj\t1cmap.obj   }
{$l obj\t1gload.obj  }
{$l obj\t1load.obj   }
{$l obj\t1parse.obj  }
{$l obj\t1decode.obj }
{$l obj\psobjs.obj   }
{$l obj\pshalgo.obj  }
{$l obj\pshglob.obj  }
{$l obj\cidobjs.obj }
{$l obj\cidgload.obj }
{$l obj\psauxmod.obj   }
{$l obj\cidload.obj }
{$l obj\cidparse.obj }
{$l obj\ftobjs.obj }
{$l obj\ftapi.obj }
{$l obj\ftbbox.obj }
{$l obj\ftbdf.obj }
{$l obj\ftinit.obj }
{$l obj\ftlzw.obj }
{$l obj\ftmm.obj }
{$l obj\ftotval.obj }
{$l obj\ftpfr.obj }
{$l obj\ftrfork.obj }
{$l obj\ftstroke.obj }
{$l obj\ftsynth.obj }
{$l obj\ftglyph.obj }
{$l obj\ftbitmap.obj }
{$l obj\ftdbgmem.obj }
{$l obj\ftdebug.obj }
{$l obj\ftoutln.obj }
{$l obj\ftobjs.obj }
{$l obj\ftgloadr.obj }
{$l obj\ftsystem.obj }
{$l obj\ftcalc.obj }
{$l obj\fttrigon.obj }
{$l obj\fttype1.obj  }
{$l obj\t1gload.obj  }
{$l obj\t1load.obj   }
{$l obj\ftstream.obj }
{$l obj\ftutil.obj   }

end.




