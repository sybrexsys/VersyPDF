unit VersyCommon;
{$i vspas.Inc}
interface
uses {$ifdef USENAMESPACE}WinAPI.Windows{$else}Windows{$endif};


type

  c_tm  = packed record
      tm_sec: Integer;
      tm_min: Integer;
      tm_hour: Integer;
      tm_mday: Integer;
      tm_mon: Integer;
      tm_year: Integer;
      tm_wday: Integer;
      tm_yday: Integer;
      tm_isdst: Integer;
  end;
  Pc_tm       = ^c_tm;

  TQSortFunc = function ( Const First:PAnsiChar; Const Second: PAnsiChar ): Integer; cdecl;


var                                                          
  __turboFloat: LongBool = False;
  _fltused:integer = 0;


{$ifndef WIN64}
// Memory functions

function _memset ( Dest:Pointer; C:Integer; Count: Cardinal ):Pointer;cdecl; external 'MSVCRT.DLL' name 'memset';
function _memcpy ( Dest:Pointer; const Src: Pointer; Count: Cardinal ):Pointer;cdecl; external 'MSVCRT.DLL' name 'memcpy';
function _memmove ( Dest:Pointer; const Src: Pointer; Count: Cardinal ):Pointer;cdecl; external 'MSVCRT.DLL' name 'memmove';
function _memcmp ( const Buf1:Pointer; const Buf2: Pointer; Count: Cardinal ):Pointer;cdecl; external 'MSVCRT.DLL' name 'memcmp';
function _malloc ( Count: Cardinal ):Pointer; cdecl; external 'MSVCRT.DLL' name 'malloc';
function _calloc ( Num: Cardinal; Size: Cardinal ):Pointer; cdecl; external 'MSVCRT.DLL' name 'calloc';
function _realloc ( Dest:Pointer; Count: Cardinal ):Pointer; cdecl; external 'MSVCRT.DLL' name 'realloc';
procedure _free ( Dest: Pointer ); cdecl; external 'MSVCRT.DLL' name 'free';
//function malloc ( Count: Cardinal ):Pointer; cdecl; external 'MSVCRT.DLL' name 'malloc';
// Process functions
procedure _exit ( Status: Integer ); cdecl; external 'MSVCRT.DLL' name 'exit';
procedure _abort; cdecl; external 'MSVCRT.DLL' name 'abort';
procedure _longjmp ( Dest: Pointer; Value: Integer ); cdecl; external 'MSVCRT.DLL' name 'longjmp';
function _setjmp ( Dest: Pointer ) : Integer; cdecl; external 'MSVCRT.DLL' name '_setjmp';
procedure __assert ( Status: Integer ); cdecl; external 'MSVCRT.DLL' name '_assert';


// string functions

function _strlen ( const Src: PAnsiChar): Cardinal;cdecl; external 'MSVCRT.DLL' name 'strlen';
function _strstr ( const Src: PAnsiChar; const StrSearch: PAnsiChar): PAnsiChar;cdecl; external 'MSVCRT.DLL' name 'strstr';
function _strchr ( const Src: PAnsiChar; c: Integer): PAnsiChar;cdecl; external 'MSVCRT.DLL' name 'strchr';
function _strncmp ( const Str1: PAnsiChar; const Str2: PAnsiChar; c: Integer): Integer;cdecl; external 'MSVCRT.DLL' name 'strncmp';
function _strcmp ( const Str1: PAnsiChar; const Str2: PAnsiChar): Integer;cdecl; external 'MSVCRT.DLL' name 'strcmp';
function _sprintf (Buffer: PAnsiChar; const format: PAnsiChar; other: array of const ): Integer;cdecl; external 'MSVCRT.DLL' name 'sprintf'; varargs;
function _strcat ( Dest:PAnsiChar; const Src: PAnsiChar): Cardinal;cdecl; external 'MSVCRT.DLL' name 'strcat';
function _strcpy ( Dest:PAnsiChar; const Src: PAnsiChar): PAnsiChar;cdecl; external 'MSVCRT.DLL' name 'strcpy';
function _printf (const format: PAnsiChar; other: array of const ): Integer;cdecl; external 'MSVCRT.DLL' name 'printf'; varargs;
type
 PPChar = ^PAnsiChar;
function _strtod ( const nptr: PAnsiChar;Dest:PPChar): Double;cdecl; external 'MSVCRT.DLL' name 'strcat';


// Debug functions
function __ltolower ( c: integer ):integer; cdecl; external 'MSVCRT.DLL' name 'tolower';
function __ltoupper ( c: integer ):integer; cdecl; external 'MSVCRT.DLL' name 'toupper';
function __ltowlower ( c: integer ):integer; cdecl; external 'MSVCRT.DLL' name 'towlower';
function __ltowupper ( c: integer ):integer; cdecl; external 'MSVCRT.DLL' name 'towupper';

// Math functions
function _fabs ( x: Double ): Double; cdecl; external 'MSVCRT.DLL' name 'fabs';
function _tan ( x: Double ): Double; cdecl; external 'MSVCRT.DLL' name 'tan';
function _log10 ( x: Double ): Double; cdecl; external 'MSVCRT.DLL' name 'log10';
function _sin ( x: Double ): Double; cdecl; external 'MSVCRT.DLL' name 'sin';
function _cos ( x: Double ): Double; cdecl; external 'MSVCRT.DLL' name 'cos';
function _sqrt ( x: Double ): Double; cdecl; external 'MSVCRT.DLL' name 'sqrt';
function _atan2 ( x: Double ): Double; cdecl; external 'MSVCRT.DLL' name 'atan2';
function _asin ( x: Double ): Double; cdecl; external 'MSVCRT.DLL' name 'asin';
procedure _qsort ( base: Pointer; num: Integer; Width: Integer; Func : TQSortFunc ); cdecl; external 'MSVCRT.DLL' name 'qsort';
function _atol ( const str: PAnsiChar ): Integer; cdecl; external 'MSVCRT.DLL' name 'atol';
function _pow ( x: Double; y: Double ): Double; cdecl; external 'MSVCRT.DLL' name 'pow';
function _floor ( x: Double ): Double; cdecl; external 'MSVCRT.DLL' name 'floor';
function _exp ( x: Double ): Double; cdecl; external 'MSVCRT.DLL' name 'exp';
function _log ( x: Double ): Double; cdecl; external 'MSVCRT.DLL' name 'log';
function _rand : Integer; cdecl; external 'MSVCRT.DLL' name 'rand';
function _srand: Integer; cdecl external 'MSVCRT.DLL' name 'srand';


// Time functions
function _time ( time: Int64 ): Int64; cdecl; external 'MSVCRT.DLL' name 'time';
function _localtime ( time: Int64 ): Pc_tm; cdecl; external 'MSVCRT.DLL' name 'localtime';
function _gmtime ( time: Int64 ): Pc_tm; cdecl; external 'MSVCRT.DLL' name 'gmtime';
function _mktime (Value: Pc_tm):Int64; cdecl; external 'MSVCRT.DLL' name 'mktime';


// Character functions

function _isdigit ( c: Integer ): Integer; cdecl; external 'MSVCRT.DLL' name 'isdigit';
function _isspace ( c: Integer ): Integer; cdecl; external 'MSVCRT.DLL' name 'isspace';
function _isalnum ( c: Integer ): Integer; cdecl; external 'MSVCRT.DLL' name 'isalnum';
function _isxdigit ( c: Integer ): Integer; cdecl; external 'MSVCRT.DLL' name 'isxdigit';
function _isprint ( c: Integer ): Integer; cdecl; external 'MSVCRT.DLL' name 'isprint';

// I/O functions
function _fopen ( const filename:PAnsiChar; const mode: PAnsiChar ): Pointer; cdecl; external 'MSVCRT.DLL' name 'fopen';
function _fclose ( Stream: Pointer ): Integer; cdecl; external 'MSVCRT.DLL' name 'fclose';
function _ftell ( Stream: Pointer ): Integer; cdecl; external 'MSVCRT.DLL' name 'ftell';
function _fseek ( Stream: Pointer; offset: Integer; origin: Integer ): Integer; cdecl; external 'MSVCRT.DLL' name 'fseek';
function _fread ( Buffer:Pointer; Size: Cardinal; Count: Cardinal; Stream: Pointer ): Cardinal; cdecl; external 'MSVCRT.DLL' name 'fread';
function _fwrite ( const Buffer:Pointer; Size: Cardinal; Count: Cardinal; Stream: Pointer ): Cardinal; cdecl; external 'MSVCRT.DLL' name 'fwrite';
function _fflush ( Stream: Pointer ): Integer; cdecl; external 'MSVCRT.DLL' name 'fflush';
function _fprintf (Stream: Pointer; const format: PAnsiChar; other: array of const ): Integer;cdecl; external 'MSVCRT.DLL' name 'fprintf'; varargs;
function _fputc ( c:Integer;Stream: Pointer ): Integer; cdecl; external 'MSVCRT.DLL' name 'fputc';


{$else}

const

  shell32 = 'shell32.dll';
  kernelbase = 'kernel32.dll';
// Memory functions

function memset ( Dest:Pointer; C:Integer; Count: Cardinal ):Pointer;cdecl; external 'MSVCRT.DLL' name 'memset';
function memcpy ( Dest:Pointer; const Src: Pointer; Count: Cardinal ):Pointer;cdecl; external 'MSVCRT.DLL' name 'memcpy';
function memmove ( Dest:Pointer; const Src: Pointer; Count: Cardinal ):Pointer;cdecl; external 'MSVCRT.DLL' name 'memmove';
function memcmp ( const Buf1:Pointer; const Buf2: Pointer; Count: Cardinal ):Pointer;cdecl; external 'MSVCRT.DLL' name 'memcmp';
function malloc ( Count: Cardinal ):Pointer; cdecl; external 'MSVCRT.DLL' name 'malloc';
function calloc ( Num: Cardinal; Size: Cardinal ):Pointer; cdecl; external 'MSVCRT.DLL' name 'calloc';
function realloc ( Dest:Pointer; Count: Cardinal ):Pointer; cdecl; external 'MSVCRT.DLL' name 'realloc';
procedure free ( Dest: Pointer ); cdecl; external 'MSVCRT.DLL' name 'free';
// Process functions
procedure exit ( Status: Integer ); cdecl; external 'MSVCRT.DLL' name 'exit';
procedure abort; cdecl; external 'MSVCRT.DLL' name 'abort';
procedure longjmp ( Dest: Pointer; Value: Integer ); cdecl; external 'MSVCRT.DLL' name 'longjmp';
function _setjmp ( Dest: Pointer ) : Integer; cdecl; external 'MSVCRT.DLL' name '_setjmp';
procedure _assert ( Status: Integer ); cdecl; external 'MSVCRT.DLL' name '_assert';
procedure _wassert ( P:Pointer;D:Pointer;Status: Integer ); cdecl; external 'MSVCRT.DLL' name '_wassert';

// string functions

function strlen ( const Src: PAnsiChar): Cardinal;cdecl; external 'MSVCRT.DLL' name 'strlen';
function strstr ( const Src: PAnsiChar; const StrSearch: PAnsiChar): PAnsiChar;cdecl; external 'MSVCRT.DLL' name 'strstr';
function strchr ( const Src: PAnsiChar; c: Integer): PAnsiChar;cdecl; external 'MSVCRT.DLL' name 'strchr';
function strncmp ( const Str1: PAnsiChar; const Str2: PAnsiChar; c: Integer): Integer;cdecl; external 'MSVCRT.DLL' name 'strncmp';
function strcmp ( const Str1: PAnsiChar; const Str2: PAnsiChar): Integer;cdecl; external 'MSVCRT.DLL' name 'strcmp';
function sprintf (Buffer: PAnsiChar; const format: PAnsiChar; other: array of const ): Integer;cdecl; external 'MSVCRT.DLL' name 'sprintf'; varargs;
function strcat ( Dest:PAnsiChar; const Src: PAnsiChar): Cardinal;cdecl; external 'MSVCRT.DLL' name 'strcat';
function strcpy ( Dest:PAnsiChar; const Src: PAnsiChar): PAnsiChar;cdecl; external 'MSVCRT.DLL' name 'strcpy';
function printf (const format: PAnsiChar; other: array of const ): Integer;cdecl; external 'MSVCRT.DLL' name 'printf'; varargs;
type
 PPChar = ^PAnsiChar;
function strtod ( const nptr: PAnsiChar;Dest:PPChar): Double;cdecl; external 'MSVCRT.DLL' name 'strcat';


// Debug functions
function _ltolower ( c: integer ):integer; cdecl; external 'MSVCRT.DLL' name 'tolower';
function _ltoupper ( c: integer ):integer; cdecl; external 'MSVCRT.DLL' name 'toupper';
function _ltowlower ( c: integer ):integer; cdecl; external 'MSVCRT.DLL' name 'towlower';
function _ltowupper ( c: integer ):integer; cdecl; external 'MSVCRT.DLL' name 'towupper';
function toupper ( c: integer ):integer; cdecl; external 'MSVCRT.DLL' name 'toupper';

// Math functions
function fabs ( x: Double ): Double; cdecl; external 'MSVCRT.DLL' name 'fabs';
function tan ( x: Double ): Double; cdecl; external 'MSVCRT.DLL' name 'tan';
function log10 ( x: Double ): Double; cdecl; external 'MSVCRT.DLL' name 'log10';
function sin ( x: Double ): Double; cdecl; external 'MSVCRT.DLL' name 'sin';
function cos ( x: Double ): Double; cdecl; external 'MSVCRT.DLL' name 'cos';
function sqrt ( x: Double ): Double; cdecl; external 'MSVCRT.DLL' name 'sqrt';
function atan2 ( x: Double ): Double; cdecl; external 'MSVCRT.DLL' name 'atan2';
function asin ( x: Double ): Double; cdecl; external 'MSVCRT.DLL' name 'asin';
procedure qsort ( base: Pointer; num: Integer; Width: Integer; Func : TQSortFunc ); cdecl; external 'MSVCRT.DLL' name 'qsort';
function atol ( const str: PAnsiChar ): Integer; cdecl; external 'MSVCRT.DLL' name 'atol';
function pow ( x: Double; y: Double ): Double; cdecl; external 'MSVCRT.DLL' name 'pow';
function floor ( x: Double ): Double; cdecl; external 'MSVCRT.DLL' name 'floor';
function exp ( x: Double ): Double; cdecl; external 'MSVCRT.DLL' name 'exp';
function log ( x: Double ): Double; cdecl; external 'MSVCRT.DLL' name 'log';
function rand : Integer; cdecl; external 'MSVCRT.DLL' name 'rand';
function srand: Integer; cdecl external 'MSVCRT.DLL' name 'srand';

// Time functions
function time ( time: Int64 ): Int64; cdecl; external 'MSVCRT.DLL' name 'time';
function _time64 ( time: Int64 ): Int64; cdecl; external 'MSVCRT.DLL' name 'time';
function localtime ( time: Int64 ): Pc_tm; cdecl; external 'MSVCRT.DLL' name 'localtime';
function gmtime ( time: Int64 ): Pc_tm; cdecl; external 'MSVCRT.DLL' name 'gmtime';
function _localtime64( time: Int64 ): Pc_tm; cdecl; external 'MSVCRT.DLL' name 'localtime';
function _gmtime64( time: Int64 ): Pc_tm; cdecl; external 'MSVCRT.DLL' name 'gmtime';

function mktime (Value: Pc_tm):Int64; cdecl; external 'MSVCRT.DLL' name 'mktime';


// Character functions

function isdigit ( c: Integer ): Integer; cdecl; external 'MSVCRT.DLL' name 'isdigit';
function isspace ( c: Integer ): Integer; cdecl; external 'MSVCRT.DLL' name 'isspace';
function isalnum ( c: Integer ): Integer; cdecl; external 'MSVCRT.DLL' name 'isalnum';
function isxdigit ( c: Integer ): Integer; cdecl; external 'MSVCRT.DLL' name 'isxdigit';
function isprint ( c: Integer ): Integer; cdecl; external 'MSVCRT.DLL' name 'isprint';

// I/O functions
function fopen ( const filename:PAnsiChar; const mode: PAnsiChar ): Pointer; cdecl; external 'MSVCRT.DLL' name 'fopen';
function fclose ( Stream: Pointer ): Integer; cdecl; external 'MSVCRT.DLL' name 'fclose';
function ftell ( Stream: Pointer ): Integer; cdecl; external 'MSVCRT.DLL' name 'ftell';
function fseek ( Stream: Pointer; offset: Integer; origin: Integer ): Integer; cdecl; external 'MSVCRT.DLL' name 'fseek';
function fread ( Buffer:Pointer; Size: Cardinal; Count: Cardinal; Stream: Pointer ): Cardinal; cdecl; external 'MSVCRT.DLL' name 'fread';
function fwrite ( const Buffer:Pointer; Size: Cardinal; Count: Cardinal; Stream: Pointer ): Cardinal; cdecl; external 'MSVCRT.DLL' name 'fwrite';
function fflush ( Stream: Pointer ): Integer; cdecl; external 'MSVCRT.DLL' name 'fflush';
function fprintf (Stream: Pointer; const format: PAnsiChar; other: array of const ): Integer;cdecl; external 'MSVCRT.DLL' name 'fprintf'; varargs;
function fputc ( c:Integer;Stream: Pointer ): Integer; cdecl; external 'MSVCRT.DLL' name 'fputc';
function ferror ( P: Pointer ): Integer; cdecl; external 'MSVCRT.DLL' name 'ferror';

function __imp_CloseHandle(hObject: THandle): BOOL; stdcall; external kernelbase name 'CloseHandle';
function __imp_WriteFile(hFile: THandle; const Buffer; nNumberOfBytesToWrite: DWORD;
  var lpNumberOfBytesWritten: DWORD; lpOverlapped: POverlapped): BOOL; stdcall;  external kernel32 name 'WriteFile';

function __imp_GetFileSize(hFile: THandle; lpFileSizeHigh: Pointer): DWORD; stdcall;external kernelbase name 'GetFileSize';
function __imp_SetFilePointer(hFile: THandle; lDistanceToMove: Longint;
  lpDistanceToMoveHigh: Pointer; dwMoveMethod: DWORD): DWORD; stdcall; external kernel32 name 'SetFilePointer';

function __imp_MapViewOfFile(hFileMappingObject: THandle; dwDesiredAccess: DWORD;
  dwFileOffsetHigh, dwFileOffsetLow: DWORD; dwNumberOfBytesToMap: SIZE_T): Pointer; stdcall; external kernelbase name 'MapViewOfFile';
function __imp_UnmapViewOfFile(lpBaseAddress: Pointer): BOOL; stdcall;external kernelbase name 'UnmapViewOfFile';
function __imp_CreateFileMappingA(hFile: THandle; lpFileMappingAttributes: PSecurityAttributes;
  flProtect, dwMaximumSizeHigh, dwMaximumSizeLow: DWORD; lpName: LPCSTR): THandle; stdcall;external kernelbase name 'CreateFileMappingA';
function __imp_CreateFileA(lpFileName: LPCSTR; dwDesiredAccess, dwShareMode: DWORD;
  lpSecurityAttributes: PSecurityAttributes; dwCreationDisposition, dwFlagsAndAttributes: DWORD;
  hTemplateFile: THandle): THandle; stdcall;external kernelbase name 'CreateFileA';
function __imp_GetStockObject(Index: Integer): HGDIOBJ; stdcall; external gdi32 name 'GetStockObject';
function __imp_SelectObject(DC: HDC; p2: HGDIOBJ): HGDIOBJ; stdcall;external gdi32 name 'SelectObject';
function __imp_GetTextMetricsA(DC: HDC; var TM: TTextMetricA): BOOL; stdcall; external gdi32 name 'GetTextMetricsA';
function __imp_GetDC(hWnd: HWND): HDC; stdcall; external user32 name 'GetDC';
function __imp_ReleaseDC(hWnd: HWND; hDC: HDC): Integer; stdcall; external user32 name 'ReleaseDC';

function __imp_GlobalAlloc(uFlags: UINT; dwBytes: SIZE_T): HGLOBAL; stdcall;external kernel32 name 'GlobalAlloc';
function __imp_GlobalSize(hMem: HGLOBAL): SIZE_T; stdcall;external kernel32 name 'GlobalSize';
function __imp_GlobalFree(hMem: HGLOBAL): HGLOBAL; stdcall;external kernel32 name 'GlobalFree';
function __imp_ReadFile(hFile: THandle; var Buffer; nNumberOfBytesToRead: DWORD;
  var lpNumberOfBytesRead: DWORD; lpOverlapped: POverlapped): BOOL; stdcall;external kernel32 name 'ReadFile';
function __imp_CreateFileW(lpFileName: LPCWSTR; dwDesiredAccess, dwShareMode: DWORD;
  lpSecurityAttributes: PSecurityAttributes; dwCreationDisposition, dwFlagsAndAttributes: DWORD;
  hTemplateFile: THandle): THandle; stdcall;external kernelbase name 'CreateFileW';
function __imp_WideCharToMultiByte(CodePage: UINT; dwFlags: DWORD;
  lpWideCharStr: LPWSTR; cchWideChar: Integer; lpMultiByteStr: LPSTR;
  cchMultiByte: Integer; lpDefaultChar: LPCSTR; lpUsedDefaultChar: PBOOL): Integer; stdcall;external kernel32 name 'WideCharToMultiByte';

function __imp_CreateCompatibleDC(DC: HDC): HDC; stdcall;
 external gdi32 name 'CreateCompatibleDC';
function __imp_DeleteDC(DC: HDC): BOOL; stdcall;
 external gdi32 name 'DeleteDC';
function __imp_FreeLibrary(hLibModule: HMODULE): BOOL; stdcall;
 external kernel32 name 'FreeLibrary';
function __imp_GetProcAddress(hModule: HMODULE; lpProcName: LPCSTR): FARPROC; stdcall;
function __imp_GetProcAddress(hModule: HMODULE; lpProcName: LPCSTR): FARPROC; external kernel32 name 'GetProcAddress';
function __imp_LoadLibraryA(lpLibFileName: LPCSTR): HMODULE;stdcall;
 external kernelbase name 'LoadLibraryA';
function __imp_CreateBitmap(Width, Height: Integer; Planes, BitCount: Longint;
  Bits: Pointer): HBITMAP; stdcall;
 external gdi32 name 'CreateBitmap';
function __imp_CreateBrushIndirect(const p1: TLogBrush): HBRUSH; stdcall;
 external gdi32 name 'CreateBrushIndirect';
function __imp_CreateCompatibleBitmap(DC: HDC; Width, Height: Integer): HBITMAP; stdcall;
 external gdi32 name 'CreateCompatibleBitmap';
function __imp_CreateDIBPatternBrushPt(const p1: Pointer; p2: UINT): HBRUSH; stdcall;
 external gdi32 name 'CreateDIBPatternBrushPt';
function __imp_CreateFontIndirectA(const p1: TLogFontA): HFONT; stdcall;
 external gdi32 name 'CreateFontIndirectA';
function __imp_CreateFontIndirectW(const p1: TLogFontW): HFONT; stdcall;
 external gdi32 name 'CreateFontIndirectW';
function __imp_CreatePen(Style, Width: Integer; Color: COLORREF): HPEN; stdcall;
 external gdi32 name 'CreatePen';
function __imp_CreateSolidBrush(p1: COLORREF): HBRUSH; stdcall;
 external gdi32 name 'CreateSolidBrush';
function __imp_DeleteObject(p1: HGDIOBJ): BOOL; stdcall;
 external gdi32 name 'DeleteObject';
function __imp_EnumFontFamiliesExA(DC: HDC; const p2: TLogFontA;
  p3: TFNFontEnumProcA; p4: LPARAM; p5: DWORD): BOOL; stdcall;
 external gdi32 name 'EnumFontFamiliesExA';
function __imp_GetBkColor(hDC: HDC): COLORREF; stdcall;
 external gdi32 name 'GetBkColor';
function __imp_GetBkMode(hDC: HDC): Integer; stdcall;
 external gdi32 name 'GetBkMode';
function __imp_GetCurrentObject(DC: HDC; p2: UINT): HGDIOBJ; stdcall;
 external gdi32 name 'GetCurrentObject';
function __imp_GetDeviceCaps(DC: HDC; Index: Integer): Integer; stdcall;
 external gdi32 name 'GetDeviceCaps';
function __imp_GetMapMode(DC: HDC): Integer; stdcall;
 external gdi32 name 'GetMapMode';
function __imp_GetObjectType(h: HGDIOBJ): DWORD; stdcall;
 external gdi32 name 'GetObjectType';
function __imp_GetPaletteEntries(Palette: HPALETTE; StartIndex, NumEntries: UINT;
  var PaletteEntries): UINT; stdcall;
 external gdi32 name 'GetPaletteEntries';
function __imp_GetPolyFillMode(DC: HDC): Integer; stdcall;
 external gdi32 name 'GetPolyFillMode';
function __imp_GetStretchBltMode(DC: HDC): Integer; stdcall;
 external gdi32 name 'GetStretchBltMode';
function __imp_GetTextAlign(DC: HDC): UINT; stdcall;
 external gdi32 name 'GetTextAlign';
function __imp_GetTextColor(DC: HDC): COLORREF; stdcall;
 external gdi32 name 'GetTextColor';
function __imp_GetTextExtentPoint32A(DC: HDC; Str: LPCSTR; Count: Integer;
  var Size: TSize): BOOL; external gdi32 name 'GetTextExtentPoint32A';
function __imp_GetViewportExtEx(DC: HDC; var Size: TSize): BOOL; stdcall;
 external gdi32 name 'GetViewportExtEx';
function __imp_GetViewportOrgEx(DC: HDC; var Point: TPoint): BOOL; stdcall;
 external gdi32 name 'GetViewportOrgEx';
function __imp_GetWindowExtEx(DC: HDC; var Size: TSize): BOOL; stdcall;
 external gdi32 name 'GetWindowExtEx';
function __imp_GetWindowOrgEx(DC: HDC; var Point: TPoint): BOOL; stdcall;
 external gdi32 name 'GetWindowOrgEx';
function __imp_PatBlt(DC: HDC; X, Y, Width, Height: Integer; Rop: DWORD): BOOL; stdcall;
 external gdi32 name 'PatBlt';
function __imp_RestoreDC(DC: HDC; SavedDC: Integer): BOOL; stdcall;
 external gdi32 name 'RestoreDC';
function __imp_SaveDC(DC: HDC): Integer; stdcall;
 external gdi32 name 'SaveDC';
function __imp_SetBkColor(DC: HDC; Color: COLORREF): COLORREF; stdcall;
 external gdi32 name 'SetBkColor';
function __imp_SetBkMode(DC: HDC; BkMode: Integer): Integer; stdcall;
 external gdi32 name 'SetBkMode';
function __imp_SetDIBitsToDevice(DC: HDC; DestX, DestY: Integer; Width, Height: DWORD;
  SrcX, SrcY: Integer; nStartScan, NumScans: UINT; Bits: Pointer;
  var BitsInfo: TBitmapInfo; Usage: UINT): Integer; stdcall;
 external gdi32 name 'SetDIBitsToDevice';
function __imp_SetGraphicsMode(hdc: HDC; iMode: Integer): Integer; stdcall;
 external gdi32 name 'SetGraphicsMode';
function __imp_SetMapMode(DC: HDC; p2: Integer): Integer; stdcall;
 external gdi32 name 'SetMapMode';
function __imp_SetPolyFillMode(DC: HDC; PolyFillMode: Integer): Integer; stdcall;
 external gdi32 name 'SetPolyFillMode';
function __imp_StretchBlt(DestDC: HDC; X, Y, Width, Height: Integer; SrcDC: HDC;
  XSrc, YSrc, SrcWidth, SrcHeight: Integer; Rop: DWORD): BOOL; stdcall;
 external gdi32 name 'StretchBlt';
function __imp_StretchDIBits(DC: HDC; DestX, DestY, DestWidth, DestHeight, SrcX,
  SrcY, SrcWidth, SrcHeight: Integer; Bits: Pointer; var BitsInfo: TBitmapInfo;
  Usage: UINT; Rop: DWORD): Integer; stdcall;
 external gdi32 name 'StretchDIBits';
function __imp_SetStretchBltMode(DC: HDC; StretchMode: Integer): Integer; stdcall;
 external gdi32 name 'SetStretchBltMode';
function __imp_SetTextColor(DC: HDC; Color: COLORREF): COLORREF; stdcall;
 external gdi32 name 'SetTextColor';
function __imp_SetTextAlign(DC: HDC; Flags: UINT): UINT; stdcall;
 external gdi32 name 'SetTextAlign';
function __imp_CloseEnhMetaFile(DC: HDC): HENHMETAFILE; stdcall;
 external gdi32 name 'CloseEnhMetaFile';
function __imp_CreateEnhMetaFileA(DC: HDC; FileName: LPCSTR; Rect: PRect; Desc: LPCSTR): HDC; stdcall;
 external gdi32 name 'CreateEnhMetaFileA';
function __imp_DeleteEnhMetaFile(p1: HENHMETAFILE): BOOL; stdcall;
 external gdi32 name 'DeleteEnhMetaFile';
function __imp_EnumEnhMetaFile(DC: HDC; Emf: HENHMETAFILE; EnhMetaFunc: TFNEnhMFEnumProc; Data: Pointer; const Rect: TRect): BOOL; stdcall;
 external gdi32 name 'EnumEnhMetaFile';
function __imp_GetEnhMetaFileHeader(p1: HENHMETAFILE; p2: UINT; p3: PEnhMetaHeader): UINT; stdcall;
 external gdi32 name 'GetEnhMetaFileHeader';
function __imp_GetWorldTransform(DC: HDC; var p2: TXForm): BOOL; stdcall;
 external gdi32 name 'GetWorldTransform';
function __imp_SetWorldTransform(DC: HDC; const p2: TXForm): BOOL; stdcall;
 external gdi32 name 'SetWorldTransform';
function __imp_ModifyWorldTransform(DC: HDC; const p2: TXForm; p3: DWORD): BOOL; stdcall;
 external gdi32 name 'ModifyWorldTransform';
function __imp_SetArcDirection(DC: HDC; Direction: Integer): Integer; stdcall;
 external gdi32 name 'SetArcDirection';
function __imp_GetArcDirection(DC: HDC): Integer; stdcall;
 external gdi32 name 'GetArcDirection';
function __imp_GetObjectA(p1: HGDIOBJ; p2: Integer; p3: Pointer): Integer; stdcall;
 external gdi32 name 'GetObjectA';
function __imp_SetViewportExtEx(DC: HDC; XExt, YExt: Integer; Size: PSize): BOOL; stdcall;
 external gdi32 name 'SetViewportExtEx';
function __imp_SetViewportOrgEx(DC: HDC; X, Y: Integer; Point: PPoint): BOOL; stdcall;
 external gdi32 name 'SetViewportOrgEx';
function __imp_SetWindowExtEx(DC: HDC; XExt, YExt: Integer; Size: PSize): BOOL; stdcall;
 external gdi32 name 'SetWindowExtEx';
function __imp_SetWindowOrgEx(DC: HDC; X, Y: Integer; Point: PPoint): BOOL; stdcall;
 external gdi32 name 'SetWindowOrgEx';
function __imp_GdiFlush: BOOL; stdcall; external gdi32 name 'GdiFlush';
function __imp_MultiByteToWideChar(CodePage: UINT; dwFlags: DWORD;
  const lpMultiByteStr: LPCSTR; cchMultiByte: Integer;
  lpWideCharStr: LPWSTR; cchWideChar: Integer): Integer; stdcall;
 external kernel32 name 'MultiByteToWideChar';
function __imp_GetCharABCWidthsA(DC: HDC; FirstChar, LastChar: UINT; const ABCStructs): BOOL; stdcall;
 external gdi32 name 'GetCharABCWidthsA';
function __imp_GetFontData(DC: HDC; p2, p3: DWORD; p4: Pointer; p5: DWORD): DWORD; stdcall;
 external gdi32 name 'GetFontData';
function __imp_GetDIBits(DC: HDC; Bitmap: HBitmap; StartScan, NumScans: UINT;
  Bits: Pointer; var BitInfo: TBitmapInfo; Usage: UINT): Integer; stdcall;
 external gdi32 name 'GetDIBits';
function __imp_GetPixel(DC: HDC; X, Y: Integer): COLORREF; stdcall;
 external gdi32 name 'GetPixel';
function __imp_CreateDIBSection(DC: HDC; const p2: TBitmapInfo; p3: UINT;
  var p4: Pointer; p5: THandle; p6: DWORD): HBITMAP; stdcall;
 external gdi32 name 'CreateDIBSection';

function __imp_ShellExecuteA(hWnd: HWND; Operation, FileName, Parameters,
  Directory: LPSTR; ShowCmd: Integer): HINST; stdcall; external shell32 name 'ShellExecuteA';


{$endif}



{$ifndef WIN64}
function __ftol: Integer; cdecl;
procedure __lldiv;
procedure __llmul;
{$endif}

implementation
{$ifndef WIN64}

function __ftol: Integer; cdecl;
var
  f: double;
begin
  asm
    lea    eax, f             //  BC++ passes floats on the FPU stack
    fstp  qword ptr [eax]     //  Delphi passes floats on the CPU stack
  end;
  Result := Integer(Trunc(f));
end;
 
procedure __llmul;
asm
        push  edx
        push  eax

  // Param2 : [ESP+16]:[ESP+12]  (hi:lo)
  // Param1 : [ESP+4]:[ESP]      (hi:lo)

        mov   eax, [esp+16]
        mul   dword ptr [esp]
        mov   ecx, eax

        mov   eax, [esp+4]
        mul   dword ptr [esp+12]
        add   ecx, eax

        mov   eax, [esp]
        mul   dword ptr [esp+12]
        add   edx, ecx

        pop   ecx
        pop   ecx

        ret     8
end;


procedure __lldiv;
asm
        push    ebp
        push    ebx
        push    esi
        push    edi
        xor     edi,edi

        mov     ebx,20[esp]             // get the divisor low dword
        mov     ecx,24[esp]             // get the divisor high dword

        or      ecx,ecx
        jnz     @__lldiv@slow_ldiv      // both high words are zero

        or      edx,edx
        jz      @__lldiv@quick_ldiv

        or      ebx,ebx
        jz      @__lldiv@quick_ldiv     // if ecx:ebx == 0 force a zero divide
          // we don't expect this to actually
          // work

@__lldiv@slow_ldiv:

//
//               Signed division should be done.  Convert negative
//               values to positive and do an unsigned division.
//               Store the sign value in the next higher bit of
//               di (test mask of 4).  Thus when we are done, testing
//               that bit will determine the sign of the result.
//
        or      edx,edx                 // test sign of dividend
        jns     @__lldiv@onepos
        neg     edx
        neg     eax
        sbb     edx,0                   // negate dividend
        or      edi,1

@__lldiv@onepos:
        or      ecx,ecx                 // test sign of divisor
        jns     @__lldiv@positive
        neg     ecx
        neg     ebx
        sbb     ecx,0                   // negate divisor
        xor edi,1

@__lldiv@positive:
        mov     ebp,ecx
        mov     ecx,64                  // shift counter
        push    edi                     // save the flags
//
//       Now the stack looks something like this:
//
//               24[esp]: divisor (high dword)
//               20[esp]: divisor (low dword)
//               16[esp]: return EIP
//               12[esp]: previous EBP
//                8[esp]: previous EBX
//                4[esp]: previous ESI
//                 [esp]: previous EDI
//
        xor     edi,edi                 // fake a 64 bit dividend
        xor     esi,esi

@__lldiv@xloop:
        shl     eax,1                   // shift dividend left one bit
        rcl     edx,1
        rcl     esi,1
        rcl     edi,1
        cmp     edi,ebp                 // dividend larger?
        jb      @__lldiv@nosub
        ja      @__lldiv@subtract
        cmp     esi,ebx                 // maybe
        jb      @__lldiv@nosub

@__lldiv@subtract:
        sub     esi,ebx
        sbb     edi,ebp                 // subtract the divisor
        inc     eax                     // build quotient

@__lldiv@nosub:
        loop    @__lldiv@xloop
//
//       When done with the loop the four registers values' look like:
//
//       |     edi    |    esi     |    edx     |    eax     |
//       |        remainder        |         quotient        |
//
        pop     ebx                     // get control bits
        test    ebx,1                   // needs negative
        jz      @__lldiv@finish
        neg     edx
        neg     eax
        sbb     edx,0                   // negate

@__lldiv@finish:
        pop     edi
        pop     esi
        pop     ebx
        pop     ebp
        ret     8

@__lldiv@quick_ldiv:
        div     ebx                     // unsigned divide
        xor     edx,edx
        jmp     @__lldiv@finish
end;
{$else}




{$endif}



end.
