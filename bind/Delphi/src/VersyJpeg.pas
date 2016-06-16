unit VersyJpeg;
{$i vspas.inc}
interface


function PDFImageLoadJPEGFromStream ( P:Pointer ):Pointer; register; external;
procedure PDFImageCompressJPEG( p1,p2:Pointer; Q:Byte); register; external;

procedure VersyPDF_jpeg_abort( cinfo: Pointer );register; external;
function VersyPDF_jpeg_std_error ( P:Pointer ):Pointer;register; external;
procedure VersyPDF_jpeg_CreateCompress( cinfo:Pointer; version: Integer; structsize:Cardinal );register; external;
procedure VersyPDF_jpeg_CreateDecompress( cinfo:Pointer; version: Integer; structsize:Cardinal );register; external;
procedure VersyPDF_jpeg_set_defaults( cinfo:Pointer );register; external;
procedure VersyPDF_jpeg_set_colorspace( cinfo:Pointer; ColorSpace:Integer );register; external;
procedure VersyPDF_jpeg_set_quality( cinfo:Pointer; Quality:Integer; ch:Byte );register; external;
procedure VersyPDF_jpeg_suppress_tables (cinfo:Pointer; ch:Byte );register; external;
procedure VersyPDF_jpeg_finish_compress (cinfo, mstream:Pointer);register; external;
procedure VersyPDF_jpeg_start_compress (cinfo:Pointer; AllTab:Byte);register; external;
procedure VersyPDF_jpeg_write_tables (cinfo:Pointer);register; external;
function VersyPDF_jpeg_write_scanlines (cinfo, ScanLines:Pointer; num: Cardinal;mem:Pointer ):Cardinal;register; external;
function VersyPDF_jpeg_write_raw_data (cinfo, Data:Pointer; num: Cardinal ):Cardinal;register; external;
function VersyPDF_jpeg_read_header ( Cinfo: Pointer; req: Byte;Fmem:Pointer ): Integer;register; external;
function VersyPDF_jpeg_start_decompress( cinfo:Pointer ):Byte;register; external;
function VersyPDF_jpeg_read_scanlines (cinfo, ScanLines:Pointer; maxlines: Cardinal;mem:Pointer ):Cardinal;register; external;
function VersyPDF_jpeg_read_raw_data (cinfo, Data:Pointer; num: Cardinal ):Cardinal;register; external;
function VersyPDF_jpeg_finish_decompress( cinfo:Pointer ):Byte;register; external;
procedure VersyPDF_jpeg_destroy( cinfo: Pointer );register; external;
procedure VersyPDF_jpeg_destroy_compress( cinfo: Pointer );register; external;
procedure VersyPDF_jpeg_destroy_decompress( cinfo: Pointer );register; external;
function VersyPDF_jpeg_resync_to_restart( cinfo:Pointer; desired:Integer;Fmem:Pointer  ):Byte;register; external;
procedure VersyPDF_jpeg_reset_huff_decode( cinfo: Pointer; ref:Pointer );register; external;
function VersyPDF_jpeg_alloc_quant_table( cinfo: Pointer ):Pointer;register; external;
function VersyPDF_jpeg_alloc_huff_table( cinfo: Pointer ):Pointer;register; external;


implementation

uses VersyCommon, VersyZLib, Versypdfint, {$ifdef USENAMESPACE}WinAPI.Windows{$else}Windows{$endif};

//procedure VersyPDF_jpeg_destroy_compress( P:Pointer );cdecl; external;
//procedure VersyPDF_jpeg_destroy_decompress( P:Pointer );cdecl; external;



{$ifndef WIN64}
{$l ..\obj\x32\jdapimin.obj}
{$l ..\obj\x32\jmemmgr.obj}
{$l ..\obj\x32\jdinput.obj}
{$l ..\obj\x32\jdatasrc.obj}
{$l ..\obj\x32\jdapistd.obj}
{$l ..\obj\x32\jdmaster.obj}
{$l ..\obj\x32\jdphuff.obj}
{$l ..\obj\x32\jdhuff.obj}
{$l ..\obj\x32\jdmerge.obj}
{$l ..\obj\x32\jdcolor.obj}
{$l ..\obj\x32\jquant1.obj}
{$l ..\obj\x32\jquant2.obj}
{$l ..\obj\x32\jdmainct.obj}
{$l ..\obj\x32\jdcoefct.obj}
{$l ..\obj\x32\jdpostct.obj}
{$l ..\obj\x32\jddctmgr.obj}
{$l ..\obj\x32\jdsample.obj}
{$l ..\obj\x32\jidctflt.obj}
{$l ..\obj\x32\jidctfst.obj}
{$l ..\obj\x32\jidctint.obj}
{$l ..\obj\x32\jidctred.obj}
{$l ..\obj\x32\jdmarker.obj}
{$l ..\obj\x32\jutils.obj}
{$l ..\obj\x32\jcomapi.obj}
{$l ..\obj\x32\jdatadst.obj}
{$l ..\obj\x32\jcparam.obj}
{$l ..\obj\x32\jcapistd.obj}
{$l ..\obj\x32\jcapimin.obj}
{$l ..\obj\x32\jcinit.obj}
{$l ..\obj\x32\jcmarker.obj}
{$l ..\obj\x32\jcmaster.obj}
{$l ..\obj\x32\jcmainct.obj}
{$l ..\obj\x32\jcprepct.obj}
{$l ..\obj\x32\jccoefct.obj}
{$l ..\obj\x32\jccolor.obj}
{$l ..\obj\x32\jcsample.obj}
{$l ..\obj\x32\jcdctmgr.obj}
{$l ..\obj\x32\jcphuff.obj}
{$l ..\obj\x32\jfdctint.obj}
{$l ..\obj\x32\jfdctfst.obj}
{$l ..\obj\x32\jfdctflt.obj}
{$l ..\obj\x32\jerror.obj}
{$l ..\obj\x32\jchuff.obj}
{$l ..\obj\x32\VSJpeg.obj}
{$l ..\obj\x32\VSExcept.obj}
{$l ..\obj\x32\VSBaseA.obj}
{$l ..\obj\x32\VSMisc.obj}
{$l ..\obj\x32\VSBaseA.obj}
{$l ..\obj\x32\VSLibA.obj}
{$else}
{$l ..\obj\x64\jdapimin.obj}
{$l ..\obj\x64\jmemmgr.obj}
{$l ..\obj\x64\jdinput.obj}
{$l ..\obj\x64\jdatasrc.obj}
{$l ..\obj\x64\jdapistd.obj}
{$l ..\obj\x64\jdmaster.obj}
{$l ..\obj\x64\jdphuff.obj}
{$l ..\obj\x64\jdhuff.obj}
{$l ..\obj\x64\jdmerge.obj}
{$l ..\obj\x64\jdcolor.obj}
{$l ..\obj\x64\jquant1.obj}
{$l ..\obj\x64\jquant2.obj}
{$l ..\obj\x64\jdmainct.obj}
{$l ..\obj\x64\jdcoefct.obj}
{$l ..\obj\x64\jdpostct.obj}
{$l ..\obj\x64\jddctmgr.obj}
{$l ..\obj\x64\jdsample.obj}
{$l ..\obj\x64\jidctflt.obj}
{$l ..\obj\x64\jidctfst.obj}
{$l ..\obj\x64\jidctint.obj}
{$l ..\obj\x64\jidctred.obj}
{$l ..\obj\x64\jdmarker.obj}
{$l ..\obj\x64\jutils.obj}
{$l ..\obj\x64\jcomapi.obj}
{$l ..\obj\x64\jdatadst.obj}
{$l ..\obj\x64\jcparam.obj}
{$l ..\obj\x64\jcapistd.obj}
{$l ..\obj\x64\jcapimin.obj}
{$l ..\obj\x64\jcinit.obj}
{$l ..\obj\x64\jcmarker.obj}
{$l ..\obj\x64\jcmaster.obj}
{$l ..\obj\x64\jcmainct.obj}
{$l ..\obj\x64\jcprepct.obj}
{$l ..\obj\x64\jccoefct.obj}
{$l ..\obj\x64\jccolor.obj}
{$l ..\obj\x64\jcsample.obj}
{$l ..\obj\x64\jcdctmgr.obj}
{$l ..\obj\x64\jcphuff.obj}
{$l ..\obj\x64\jfdctint.obj}
{$l ..\obj\x64\jfdctfst.obj}
{$l ..\obj\x64\jfdctflt.obj}
{$l ..\obj\x64\jerror.obj}
{$l ..\obj\x64\jchuff.obj}
{$l ..\obj\x64\VSJpeg.obj}
{$l ..\obj\x64\VSExcept.obj}
{$l ..\obj\x64\VSBaseA.obj}
{$l ..\obj\x64\VSMisc.obj}
{$l ..\obj\x64\VSBaseA.obj}
{$l ..\obj\x64\VSLibA.obj}
{$endif}


end.


