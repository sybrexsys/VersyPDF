unit VersyTiff;
{$i vspas.Inc}
interface
uses VersyPDFTypes;

function PDFImageGetTIFFCountFromStream ( Lib:PDFLibHandle; AStream: PDFStreamHandle ):Cardinal;  register; external;
function PDFImageLoadTIFFFromStream( Strm:Pointer; Index:Cardinal):Pointer;register; external;

implementation

uses VersyCommon, Windows, VersyZLib, VersyPDFInt, VersyJpeg;

procedure _vspdft_TIFFError( const module, fmt : PChar;other: array of const); cdecl;external;varargs;
procedure _vspdft_TIFFWarning( const module, fmt : PChar;other: array of const); cdecl;external;varargs;

{$l obj\VStiff.obj}
{$l obj\VSExcept.obj}
{$l obj\VSLibA.obj}
{$l obj\tif_getimage.obj}
{$l obj\tif_codec.obj   }
{$l obj\tif_aux.obj     }
{$l obj\tif_close.obj   }
{$l obj\tif_open.obj    }
{$l obj\tif_dirread.obj }
{$l obj\tif_dirwrite.obj}
{$l obj\tif_fax3.obj    }
{$l obj\tif_jpeg.obj    }
{$l obj\tif_dir.obj     }
{$l obj\tif_dumpmode.obj}
{$l obj\tif_luv.obj     }
{$l obj\tif_lzw.obj     }
{$l obj\tif_ojpeg.obj   }
{$l obj\tif_packbits.obj}
{$l obj\tif_pixarlog.obj}
{$l obj\tif_write.obj   }
{$l obj\tif_zip.obj     }
{$l obj\tif_win32.obj   }
{$l obj\tif_flush.obj   }
{$l obj\tif_dirwrite.obj}
{$l obj\tif_compress.obj}
{$l obj\tif_codec.obj   }
{$l obj\tif_dirinfo.obj }
{$l obj\fax3sm_winnt.obj}
{$l obj\tif_next.obj    }
{$l obj\tif_predict.obj }
{$l obj\tif_print.obj   }
{$l obj\tif_read.obj    }
{$l obj\tif_strip.obj   }
{$l obj\tif_swab.obj    }
{$l obj\tif_thunder.obj }
{$l obj\tif_tile.obj    }
{$l obj\tif_version.obj }
{$l obj\tif_close.obj   }
{$l obj\tif_open.obj    }


end.





