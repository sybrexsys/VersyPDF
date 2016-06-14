unit VersyJpeg;
{$i vspas.inc}
interface


function PDFImageLoadJPEGFromStream ( P:Pointer ):Pointer; register; external;
procedure PDFImageCompressJPEG( p1,p2:Pointer; Q:Byte); register; external;

procedure versyjpeg_abort( cinfo: Pointer );register; external;
function versyjpeg_std_error ( P:Pointer ):Pointer;register; external;
procedure versyjpeg_CreateCompress( cinfo:Pointer; version: Integer; structsize:Cardinal );register; external;
procedure versyjpeg_CreateDecompress( cinfo:Pointer; version: Integer; structsize:Cardinal );register; external;
procedure versyjpeg_set_defaults( cinfo:Pointer );register; external;
procedure versyjpeg_set_colorspace( cinfo:Pointer; ColorSpace:Integer );register; external;
procedure versyjpeg_set_quality( cinfo:Pointer; Quality:Integer; ch:Byte );register; external;
procedure versyjpeg_suppress_tables (cinfo:Pointer; ch:Byte );register; external;
procedure versyjpeg_finish_compress (cinfo, mstream:Pointer);register; external;
procedure versyjpeg_start_compress (cinfo:Pointer; AllTab:Byte);register; external;
procedure versyjpeg_write_tables (cinfo:Pointer);register; external;
function versyjpeg_write_scanlines (cinfo, ScanLines:Pointer; num: Cardinal;mem:Pointer ):Cardinal;register; external;
function versyjpeg_write_raw_data (cinfo, Data:Pointer; num: Cardinal ):Cardinal;register; external;
function versyjpeg_read_header ( Cinfo: Pointer; req: Byte;Fmem:Pointer ): Integer;register; external;
function versyjpeg_start_decompress( cinfo:Pointer ):Byte;register; external;
function versyjpeg_read_scanlines (cinfo, ScanLines:Pointer; maxlines: Cardinal;mem:Pointer ):Cardinal;register; external;
function versyjpeg_read_raw_data (cinfo, Data:Pointer; num: Cardinal ):Cardinal;register; external;
function versyjpeg_finish_decompress( cinfo:Pointer ):Byte;register; external;
procedure versyjpeg_destroy( cinfo: Pointer );register; external;
procedure versyjpeg_destroy_compress( cinfo: Pointer );register; external;
procedure versyjpeg_destroy_decompress( cinfo: Pointer );register; external;
function versyjpeg_resync_to_restart( cinfo:Pointer; desired:Integer;Fmem:Pointer  ):Byte;register; external;
procedure versyjpeg_reset_huff_decode( cinfo: Pointer; ref:Pointer );register; external;
function versyjpeg_alloc_quant_table( cinfo: Pointer ):Pointer;register; external;
function versyjpeg_alloc_huff_table( cinfo: Pointer ):Pointer;register; external;


implementation

uses VersyCommon, VersyZLib, Versypdfint, windows;

//procedure versyjpeg_destroy_compress( P:Pointer );cdecl; external;
//procedure versyjpeg_destroy_decompress( P:Pointer );cdecl; external;




{$l obj\jpeg\jdapimin.obj}
{$l obj\jpeg\jmemmgr.obj}
{$l obj\jpeg\jdinput.obj}
{$l obj\jpeg\jdatasrc.obj}
{$l obj\jpeg\jdapistd.obj}
{$l obj\jpeg\jdmaster.obj}
{$l obj\jpeg\jdphuff.obj}
{$l obj\jpeg\jdhuff.obj}
{$l obj\jpeg\jdmerge.obj}
{$l obj\jpeg\jdcolor.obj}
{$l obj\jpeg\jquant1.obj}
{$l obj\jpeg\jquant2.obj}
{$l obj\jpeg\jdmainct.obj}
{$l obj\jpeg\jdcoefct.obj}
{$l obj\jpeg\jdpostct.obj}
{$l obj\jpeg\jddctmgr.obj}
{$l obj\jpeg\jdsample.obj}
{$l obj\jpeg\jidctflt.obj}
{$l obj\jpeg\jidctfst.obj}
{$l obj\jpeg\jidctint.obj}
{$l obj\jpeg\jidctred.obj}
{$l obj\jpeg\jdmarker.obj}
{$l obj\jpeg\jutils.obj}
{$l obj\jpeg\jcomapi.obj}
{$l obj\jpeg\jdatadst.obj}
{$l obj\jpeg\jcparam.obj}
{$l obj\jpeg\jcapistd.obj}
{$l obj\jpeg\jcapimin.obj}
{$l obj\jpeg\jcinit.obj}
{$l obj\jpeg\jcmarker.obj}
{$l obj\jpeg\jcmaster.obj}
{$l obj\jpeg\jcmainct.obj}
{$l obj\jpeg\jcprepct.obj}
{$l obj\jpeg\jccoefct.obj}
{$l obj\jpeg\jccolor.obj}
{$l obj\jpeg\jcsample.obj}
{$l obj\jpeg\jcdctmgr.obj}
{$l obj\jpeg\jcphuff.obj}
{$l obj\jpeg\jfdctint.obj}
{$l obj\jpeg\jfdctfst.obj}
{$l obj\jpeg\jfdctflt.obj}
{$l obj\jpeg\jerror.obj}
{$l obj\jpeg\jchuff.obj}
{$l obj\versypdf\VSJpeg.obj}
{$l obj\versypdf\VSExcept.obj}
{$l obj\versypdf\VSBaseA.obj}
{$l obj\versypdf\VSMisc.obj}
{$l obj\versypdf\VSBaseA.obj}
{$l obj\versypdf\VSLibA.obj}



end.


