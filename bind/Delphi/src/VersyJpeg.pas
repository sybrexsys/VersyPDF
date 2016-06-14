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

uses VersyCommon, VersyZLib, Versypdfint, windows;

//procedure VersyPDF_jpeg_destroy_compress( P:Pointer );cdecl; external;
//procedure VersyPDF_jpeg_destroy_decompress( P:Pointer );cdecl; external;




{$l obj\jdapimin.obj}
{$l obj\jmemmgr.obj}
{$l obj\jdinput.obj}
{$l obj\jdatasrc.obj}
{$l obj\jdapistd.obj}
{$l obj\jdmaster.obj}
{$l obj\jdphuff.obj}
{$l obj\jdhuff.obj}
{$l obj\jdmerge.obj}
{$l obj\jdcolor.obj}
{$l obj\jquant1.obj}
{$l obj\jquant2.obj}
{$l obj\jdmainct.obj}
{$l obj\jdcoefct.obj}
{$l obj\jdpostct.obj}
{$l obj\jddctmgr.obj}
{$l obj\jdsample.obj}
{$l obj\jidctflt.obj}
{$l obj\jidctfst.obj}
{$l obj\jidctint.obj}
{$l obj\jidctred.obj}
{$l obj\jdmarker.obj}
{$l obj\jutils.obj}
{$l obj\jcomapi.obj}
{$l obj\jdatadst.obj}
{$l obj\jcparam.obj}
{$l obj\jcapistd.obj}
{$l obj\jcapimin.obj}
{$l obj\jcinit.obj}
{$l obj\jcmarker.obj}
{$l obj\jcmaster.obj}
{$l obj\jcmainct.obj}
{$l obj\jcprepct.obj}
{$l obj\jccoefct.obj}
{$l obj\jccolor.obj}
{$l obj\jcsample.obj}
{$l obj\jcdctmgr.obj}
{$l obj\jcphuff.obj}
{$l obj\jfdctint.obj}
{$l obj\jfdctfst.obj}
{$l obj\jfdctflt.obj}
{$l obj\jerror.obj}
{$l obj\jchuff.obj}
{$l obj\VSJpeg.obj}
{$l obj\VSExcept.obj}
{$l obj\VSBaseA.obj}
{$l obj\VSMisc.obj}
{$l obj\VSBaseA.obj}
{$l obj\VSLibA.obj}



end.


