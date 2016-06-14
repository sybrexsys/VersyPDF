unit VersyZLib;
{$i vspas.Inc}
interface
function versy_z_inflate( Z:Pointer; f:Integer): Integer; register; external;
function versy_z_inflateEnd( Z:Pointer): Integer; register; external;
function versy_z_inflateSync( Z:Pointer): Integer; register; external;
function versy_z_inflateReset ( Z: Pointer ): Integer;register; external;
function versy_z_inflateInit_( Z:Pointer; const Version: PChar; StreamSize:Integer): Integer; register; external;
function versy_z_crc32 ( crc: Cardinal; const P:Pointer; Size: Cardinal):Cardinal;register; external;

function versy_z_deflate( Z:Pointer; f:Integer): Integer; register; external;
function versy_z_deflateEnd( Z:Pointer): Integer; register; external;
function versy_z_deflateReset ( Z: Pointer ): Integer;register; external;
function versy_z_deflateInit_( Z:Pointer; Level:Integer; const Version: PChar; StreamSize:Integer): Integer; register; external;
function versy_z_deflateParams ( Z: Pointer; Level, Strategy:Integer ): Integer;register; external;


implementation

uses VersyCommon;

{$l obj\crc32.obj}
{$l obj\deflate.obj}
{$l obj\inflate.obj}
{$l obj\inftrees.obj}
{$l obj\trees.obj}
{$l obj\adler32.obj}
{$l obj\infblock.obj}
{$l obj\inftrees.obj}
{$l obj\infcodes.obj}
{$l obj\infutil.obj}
{$l obj\inffast.obj}
{$l obj\zutil.obj}


end.
