unit VersyZLib;
{$i vspas.Inc}
interface
function versy_z_inflate( Z:Pointer; f:Integer): Integer; register; external;
function versy_z_inflateEnd( Z:Pointer): Integer; register; external;
function versy_z_inflateSync( Z:Pointer): Integer; register; external;
function versy_z_inflateReset ( Z: Pointer ): Integer;register; external;
function versy_z_inflateInit_( Z:Pointer; const Version: PAnsiChar; StreamSize:Integer): Integer; register; external;
function versy_z_crc32 ( crc: Cardinal; const P:Pointer; Size: Cardinal):Cardinal;register; external;

function versy_z_deflate( Z:Pointer; f:Integer): Integer; register; external;
function versy_z_deflateEnd( Z:Pointer): Integer; register; external;
function versy_z_deflateReset ( Z: Pointer ): Integer;register; external;
function versy_z_deflateInit_( Z:Pointer; Level:Integer; const Version: PAnsiChar; StreamSize:Integer): Integer; register; external;
function versy_z_deflateParams ( Z: Pointer; Level, Strategy:Integer ): Integer;register; external;


implementation

uses VersyCommon;
{$ifndef WIN64}
{$l ..\obj\x32\crc32.obj}
{$l ..\obj\x32\deflate.obj}
{$l ..\obj\x32\inflate.obj}
{$l ..\obj\x32\inftrees.obj}
{$l ..\obj\x32\trees.obj}
{$l ..\obj\x32\adler32.obj}
{$l ..\obj\x32\infblock.obj}
{$l ..\obj\x32\inftrees.obj}
{$l ..\obj\x32\infcodes.obj}
{$l ..\obj\x32\infutil.obj}
{$l ..\obj\x32\inffast.obj}
{$l ..\obj\x32\zutil.obj}
{$else}
{$l ..\obj\x64\crc32.obj}
{$l ..\obj\x64\deflate.obj}
{$l ..\obj\x64\inflate.obj}
{$l ..\obj\x64\inftrees.obj}
{$l ..\obj\x64\trees.obj}
{$l ..\obj\x64\adler32.obj}
{$l ..\obj\x64\infblock.obj}
{$l ..\obj\x64\inftrees.obj}
{$l ..\obj\x64\infcodes.obj}
{$l ..\obj\x64\infutil.obj}
{$l ..\obj\x64\inffast.obj}
{$l ..\obj\x64\zutil.obj}
{$endif}

end.
