unit VersyTiff;
{$i vspas.Inc}
interface
uses VersyPDFTypes;

function PDFImageGetTIFFCountFromStream ( Lib:PDFLibHandle; AStream: PDFStreamHandle ):Cardinal;  register; external;
function PDFImageLoadTIFFFromStream( Strm:Pointer; Index:Cardinal):Pointer;register; external;

implementation

uses VersyCommon, {$ifdef USENAMESPACE}WinAPI.Windows{$else}Windows{$endif}, VersyZLib, VersyPDFInt, VersyJpeg;


{$ifndef WIN64}
procedure _vspdft_TIFFError( const module, fmt : PAnsiChar;other: array of const); cdecl;external;varargs;
procedure _vspdft_TIFFWarning( const module, fmt : PAnsiChar;other: array of const); cdecl;external;varargs;

{$l ..\obj\x32\VStiff.obj}
{$l ..\obj\x32\VSExcept.obj}
{$l ..\obj\x32\VSLibA.obj}
{$l ..\obj\x32\tif_getimage.obj}
{$l ..\obj\x32\tif_codec.obj   }
{$l ..\obj\x32\tif_aux.obj     }
{$l ..\obj\x32\tif_close.obj   }
{$l ..\obj\x32\tif_open.obj    }
{$l ..\obj\x32\tif_dirread.obj }
{$l ..\obj\x32\tif_dirwrite.obj}
{$l ..\obj\x32\tif_fax3.obj    }
{$l ..\obj\x32\tif_jpeg.obj    }
{$l ..\obj\x32\tif_dir.obj     }
{$l ..\obj\x32\tif_dumpmode.obj}
{$l ..\obj\x32\tif_luv.obj     }
{$l ..\obj\x32\tif_lzw.obj     }
{$l ..\obj\x32\tif_ojpeg.obj   }
{$l ..\obj\x32\tif_packbits.obj}
{$l ..\obj\x32\tif_pixarlog.obj}
{$l ..\obj\x32\tif_write.obj   }
{$l ..\obj\x32\tif_zip.obj     }
{$l ..\obj\x32\tif_win32.obj   }
{$l ..\obj\x32\tif_flush.obj   }
{$l ..\obj\x32\tif_dirwrite.obj}
{$l ..\obj\x32\tif_compress.obj}
{$l ..\obj\x32\tif_codec.obj   }
{$l ..\obj\x32\tif_dirinfo.obj }
{$l ..\obj\x32\fax3sm_winnt.obj}
{$l ..\obj\x32\tif_next.obj    }
{$l ..\obj\x32\tif_predict.obj }
{$l ..\obj\x32\tif_print.obj   }
{$l ..\obj\x32\tif_read.obj    }
{$l ..\obj\x32\tif_strip.obj   }
{$l ..\obj\x32\tif_swab.obj    }
{$l ..\obj\x32\tif_thunder.obj }
{$l ..\obj\x32\tif_tile.obj    }
{$l ..\obj\x32\tif_version.obj }
{$l ..\obj\x32\tif_close.obj   }
{$l ..\obj\x32\tif_open.obj    }
{$else}

procedure vspdft_TIFFError( const module, fmt : PAnsiChar;other: array of const); cdecl;external;varargs;
procedure vspdft_TIFFWarning( const module, fmt : PAnsiChar;other: array of const); cdecl;external;varargs;


{$l ..\obj\x64\VStiff.obj}
{$l ..\obj\x64\VSExcept.obj}
{$l ..\obj\x64\VSLibA.obj}
{$l ..\obj\x64\tif_getimage.obj}
{$l ..\obj\x64\tif_codec.obj   }
{$l ..\obj\x64\tif_aux.obj     }
{$l ..\obj\x64\tif_close.obj   }
{$l ..\obj\x64\tif_open.obj    }
{$l ..\obj\x64\tif_dirread.obj }
{$l ..\obj\x64\tif_dirwrite.obj}
{$l ..\obj\x64\tif_fax3.obj    }
{$l ..\obj\x64\tif_jpeg.obj    }
{$l ..\obj\x64\tif_dir.obj     }
{$l ..\obj\x64\tif_dumpmode.obj}
{$l ..\obj\x64\tif_luv.obj     }
{$l ..\obj\x64\tif_lzw.obj     }
{$l ..\obj\x64\tif_ojpeg.obj   }
{$l ..\obj\x64\tif_packbits.obj}
{$l ..\obj\x64\tif_pixarlog.obj}
{$l ..\obj\x64\tif_write.obj   }
{$l ..\obj\x64\tif_zip.obj     }
{$l ..\obj\x64\tif_win32.obj   }
{$l ..\obj\x64\tif_flush.obj   }
{$l ..\obj\x64\tif_dirwrite.obj}
{$l ..\obj\x64\tif_compress.obj}
{$l ..\obj\x64\tif_codec.obj   }
{$l ..\obj\x64\tif_dirinfo.obj }
{$l ..\obj\x64\fax3sm_winnt.obj}
{$l ..\obj\x64\tif_next.obj    }
{$l ..\obj\x64\tif_predict.obj }
{$l ..\obj\x64\tif_print.obj   }
{$l ..\obj\x64\tif_read.obj    }
{$l ..\obj\x64\tif_strip.obj   }
{$l ..\obj\x64\tif_swab.obj    }
{$l ..\obj\x64\tif_thunder.obj }
{$l ..\obj\x64\tif_tile.obj    }
{$l ..\obj\x64\tif_version.obj }
{$l ..\obj\x64\tif_close.obj   }
{$l ..\obj\x64\tif_open.obj    }
{$endif}

end.





