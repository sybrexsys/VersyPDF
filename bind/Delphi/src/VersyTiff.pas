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

{$l obj\versypdf\VStiff.obj}
{$l obj\versypdf\VSExcept.obj}
{$l obj\versypdf\VSLibA.obj}
{$l obj\tiff\tif_getimage.obj}
{$l obj\tiff\tif_codec.obj   }
{$l obj\tiff\tif_aux.obj     }
{$l obj\tiff\tif_close.obj   }
{$l obj\tiff\tif_open.obj    }
{$l obj\tiff\tif_dirread.obj }
{$l obj\tiff\tif_dirwrite.obj}
{$l obj\tiff\tif_fax3.obj    }
{$l obj\tiff\tif_jpeg.obj    }
{$l obj\tiff\tif_dir.obj     }
{$l obj\tiff\tif_dumpmode.obj}
{$l obj\tiff\tif_luv.obj     }
{$l obj\tiff\tif_lzw.obj     }
{$l obj\tiff\tif_ojpeg.obj   }
{$l obj\tiff\tif_packbits.obj}
{$l obj\tiff\tif_pixarlog.obj}
{$l obj\tiff\tif_write.obj   }
{$l obj\tiff\tif_zip.obj     }
{$l obj\tiff\tif_win32.obj   }
{$l obj\tiff\tif_flush.obj   }
{$l obj\tiff\tif_dirwrite.obj}
{$l obj\tiff\tif_compress.obj}
{$l obj\tiff\tif_codec.obj   }
{$l obj\tiff\tif_dirinfo.obj }
{$l obj\tiff\fax3sm_winnt.obj}
{$l obj\tiff\tif_next.obj    }
{$l obj\tiff\tif_predict.obj }
{$l obj\tiff\tif_print.obj   }
{$l obj\tiff\tif_read.obj    }
{$l obj\tiff\tif_strip.obj   }
{$l obj\tiff\tif_swab.obj    }
{$l obj\tiff\tif_thunder.obj }
{$l obj\tiff\tif_tile.obj    }
{$l obj\tiff\tif_version.obj }
{$l obj\tiff\tif_close.obj   }
{$l obj\tiff\tif_open.obj    }


end.





