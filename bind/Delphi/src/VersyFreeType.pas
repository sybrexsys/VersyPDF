unit VersyFreeType;
{$i vspas.Inc}
interface
uses VersyPDFInt, {$ifdef USENAMESPACE}WinAPI.Windows{$else}Windows{$endif};

function PDFType1GetFontName ( A,B: Pointer):Integer;register; external;
procedure PDFType1GetFontInfo ( a,b,c:Pointer; L1,l2,l3: PInteger); register; external;

implementation

uses VersyCommon;

// FreeType objects
{$ifndef win64}
{$l ..\obj\x32\VSType1.obj}
{$l ..\obj\x32\VSBaseA.obj}
{$l ..\obj\x32\VSMisc.obj}
{$l ..\obj\x32\VSBaseA.obj}
{$l ..\obj\x32\VSExcept.obj}
{$l ..\obj\x32\VSLibA.obj}
{$l ..\obj\x32\vsftsystem.obj }
{$l ..\obj\x32\ftinit.obj }
{$l ..\obj\x32\t1afm.obj    }
{$l ..\obj\x32\t1driver.obj }
{$l ..\obj\x32\t1objs.obj   }
{$l ..\obj\x32\cidriver.obj }
{$l ..\obj\x32\psmodule.obj }
{$l ..\obj\x32\pshmod.obj   }
{$l ..\obj\x32\pshrec.obj   }
{$l ..\obj\x32\psauxmod.obj   }
{$l ..\obj\x32\t1cmap.obj   }
{$l ..\obj\x32\t1gload.obj  }
{$l ..\obj\x32\t1load.obj   }
{$l ..\obj\x32\t1parse.obj  }
{$l ..\obj\x32\t1decode.obj }
{$l ..\obj\x32\psobjs.obj   }
{$l ..\obj\x32\pshalgo.obj  }
{$l ..\obj\x32\pshglob.obj  }
{$l ..\obj\x32\cidobjs.obj }
{$l ..\obj\x32\cidgload.obj }
{$l ..\obj\x32\psauxmod.obj   }
{$l ..\obj\x32\cidload.obj }
{$l ..\obj\x32\cidparse.obj }
{$l ..\obj\x32\ftobjs.obj }
{$l ..\obj\x32\ftapi.obj }
{$l ..\obj\x32\ftbbox.obj }
{$l ..\obj\x32\ftbdf.obj }
{$l ..\obj\x32\ftinit.obj }
{$l ..\obj\x32\ftlzw.obj }
{$l ..\obj\x32\ftmm.obj }
{$l ..\obj\x32\ftotval.obj }
{$l ..\obj\x32\ftpfr.obj }
{$l ..\obj\x32\ftrfork.obj }
{$l ..\obj\x32\ftstroke.obj }
{$l ..\obj\x32\ftsynth.obj }
{$l ..\obj\x32\ftglyph.obj }
{$l ..\obj\x32\ftbitmap.obj }
{$l ..\obj\x32\ftdbgmem.obj }
{$l ..\obj\x32\ftdebug.obj }
{$l ..\obj\x32\ftoutln.obj }
{$l ..\obj\x32\ftobjs.obj }
{$l ..\obj\x32\ftgloadr.obj }
{$l ..\obj\x32\ftsystem.obj }
{$l ..\obj\x32\ftcalc.obj }
{$l ..\obj\x32\fttrigon.obj }
{$l ..\obj\x32\fttype1.obj  }
{$l ..\obj\x32\t1gload.obj  }
{$l ..\obj\x32\t1load.obj   }
{$l ..\obj\x32\ftstream.obj }
{$l ..\obj\x32\ftutil.obj   }
{$else}
{$l ..\obj\x64\VSType1.obj}
{$l ..\obj\x64\VSBaseA.obj}
{$l ..\obj\x64\VSMisc.obj}
{$l ..\obj\x64\VSBaseA.obj}
{$l ..\obj\x64\VSExcept.obj}
{$l ..\obj\x64\VSLibA.obj}
{$l ..\obj\x64\vsftsystem.obj }
{$l ..\obj\x64\ftinit.obj }
{$l ..\obj\x64\t1afm.obj    }
{$l ..\obj\x64\t1driver.obj }
{$l ..\obj\x64\t1objs.obj   }
{$l ..\obj\x64\cidriver.obj }
{$l ..\obj\x64\psmodule.obj }
{$l ..\obj\x64\pshmod.obj   }
{$l ..\obj\x64\pshrec.obj   }
{$l ..\obj\x64\psauxmod.obj   }
{$l ..\obj\x64\t1cmap.obj   }
{$l ..\obj\x64\t1gload.obj  }
{$l ..\obj\x64\t1load.obj   }
{$l ..\obj\x64\t1parse.obj  }
{$l ..\obj\x64\t1decode.obj }
{$l ..\obj\x64\psobjs.obj   }
{$l ..\obj\x64\pshalgo.obj  }
{$l ..\obj\x64\pshglob.obj  }
{$l ..\obj\x64\cidobjs.obj }
{$l ..\obj\x64\cidgload.obj }
{$l ..\obj\x64\psauxmod.obj   }
{$l ..\obj\x64\cidload.obj }
{$l ..\obj\x64\cidparse.obj }
{$l ..\obj\x64\ftobjs.obj }
{$l ..\obj\x64\ftapi.obj }
{$l ..\obj\x64\ftbbox.obj }
{$l ..\obj\x64\ftbdf.obj }
{$l ..\obj\x64\ftinit.obj }
{$l ..\obj\x64\ftlzw.obj }
{$l ..\obj\x64\ftmm.obj }
{$l ..\obj\x64\ftotval.obj }
{$l ..\obj\x64\ftpfr.obj }
{$l ..\obj\x64\ftrfork.obj }
{$l ..\obj\x64\ftstroke.obj }
{$l ..\obj\x64\ftsynth.obj }
{$l ..\obj\x64\ftglyph.obj }
{$l ..\obj\x64\ftbitmap.obj }
{$l ..\obj\x64\ftdbgmem.obj }
{$l ..\obj\x64\ftdebug.obj }
{$l ..\obj\x64\ftoutln.obj }
{$l ..\obj\x64\ftobjs.obj }
{$l ..\obj\x64\ftgloadr.obj }
{$l ..\obj\x64\ftsystem.obj }
{$l ..\obj\x64\ftcalc.obj }
{$l ..\obj\x64\fttrigon.obj }
{$l ..\obj\x64\fttype1.obj  }
{$l ..\obj\x64\t1gload.obj  }
{$l ..\obj\x64\t1load.obj   }
{$l ..\obj\x64\ftstream.obj }
{$l ..\obj\x64\ftutil.obj   }

{$endif}

end.




