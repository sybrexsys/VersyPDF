unit VersyCommon;
{$i vspas.Inc}
interface
uses Windows;


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

  TQSortFunc = function ( Const First:PChar; Const Second: PChar ): Integer; cdecl;


var                                                          
  __turboFloat: LongBool = False;



// Memory functions
function _memset ( Dest:Pointer; C:Integer; Count: Cardinal ):Pointer;cdecl; external 'MSVCRT.DLL' name 'memset';
function _memcpy ( Dest:Pointer; const Src: Pointer; Count: Cardinal ):Pointer;cdecl; external 'MSVCRT.DLL' name 'memcpy';
function _memmove ( Dest:Pointer; const Src: Pointer; Count: Cardinal ):Pointer;cdecl; external 'MSVCRT.DLL' name 'memmove';
function _memcmp ( const Buf1:Pointer; const Buf2: Pointer; Count: Cardinal ):Pointer;cdecl; external 'MSVCRT.DLL' name 'memcmp';
function _malloc ( Count: Cardinal ):Pointer; cdecl; external 'MSVCRT.DLL' name 'malloc';
function _calloc ( Num: Cardinal; Size: Cardinal ):Pointer; cdecl; external 'MSVCRT.DLL' name 'calloc';
function _realloc ( Dest:Pointer; Count: Cardinal ):Pointer; cdecl; external 'MSVCRT.DLL' name 'realloc';
procedure _free ( Dest: Pointer ); cdecl; external 'MSVCRT.DLL' name 'free';
function malloc ( Count: Cardinal ):Pointer; cdecl; external 'MSVCRT.DLL' name 'malloc';


// Process functions
procedure _exit ( Status: Integer ); cdecl; external 'MSVCRT.DLL' name 'exit';
procedure _abort; cdecl; external 'MSVCRT.DLL' name 'abort';
procedure _longjmp ( Dest: Pointer; Value: Integer ); cdecl; external 'MSVCRT.DLL' name 'longjmp';
function _setjmp ( Dest: Pointer ) : Integer; cdecl; external 'MSVCRT.DLL' name '_setjmp';
procedure __assert ( Status: Integer ); cdecl; external 'MSVCRT.DLL' name '_assert';

// string functions

function _strlen ( const Src: PChar): Cardinal;cdecl; external 'MSVCRT.DLL' name 'strlen';
function _strstr ( const Src: PChar; const StrSearch: PChar): PChar;cdecl; external 'MSVCRT.DLL' name 'strstr';
function _strchr ( const Src: PChar; c: Integer): PChar;cdecl; external 'MSVCRT.DLL' name 'strchr';
function _strncmp ( const Str1: PChar; const Str2: PChar; c: Integer): Integer;cdecl; external 'MSVCRT.DLL' name 'strncmp';
function _strcmp ( const Str1: PChar; const Str2: PChar): Integer;cdecl; external 'MSVCRT.DLL' name 'strcmp';
function _sprintf (Buffer: PChar; const format: PChar; other: array of const ): Integer;cdecl; external 'MSVCRT.DLL' name 'sprintf'; varargs;
function _strcat ( Dest:PChar; const Src: PChar): Cardinal;cdecl; external 'MSVCRT.DLL' name 'strcat';
function _strcpy ( Dest:PChar; const Src: PChar): PChar;cdecl; external 'MSVCRT.DLL' name 'strcpy';
function _printf (const format: PChar; other: array of const ): Integer;cdecl; external 'MSVCRT.DLL' name 'printf'; varargs;
type
 PPChar = ^PChar;
function _strtod ( const nptr: PChar;Dest:PPChar): Double;cdecl; external 'MSVCRT.DLL' name 'strcat';


// Debug functions
function __ltolower ( c: integer ):integer; cdecl; external 'MSVCRT.DLL' name 'tolower';
function __ltoupper ( c: integer ):integer; cdecl; external 'MSVCRT.DLL' name 'toupper';
function __ltowlower ( c: integer ):integer; cdecl; external 'MSVCRT.DLL' name 'towlower';
function __ltowupper ( c: integer ):integer; cdecl; external 'MSVCRT.DLL' name 'towupper';

// Math functions
function _fabs ( x: Double ): Double; cdecl; external 'MSVCRT.DLL' name 'fabs';
function _log10 ( x: Double ): Double; cdecl; external 'MSVCRT.DLL' name 'log10';
function _sin ( x: Double ): Double; cdecl; external 'MSVCRT.DLL' name 'sin';
function _cos ( x: Double ): Double; cdecl; external 'MSVCRT.DLL' name 'cos';
function _sqrt ( x: Double ): Double; cdecl; external 'MSVCRT.DLL' name 'sqrt';
function _atan2 ( x: Double ): Double; cdecl; external 'MSVCRT.DLL' name 'atan2';
function _asin ( x: Double ): Double; cdecl; external 'MSVCRT.DLL' name 'asin';
procedure _qsort ( base: Pointer; num: Integer; Width: Integer; Func : TQSortFunc ); cdecl; external 'MSVCRT.DLL' name 'qsort';
function _atol ( const str: PChar ): Integer; cdecl; external 'MSVCRT.DLL' name 'atol';
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
function _fopen ( const filename:PChar; const mode: PChar ): Pointer; cdecl; external 'MSVCRT.DLL' name 'fopen';
function _fclose ( Stream: Pointer ): Integer; cdecl; external 'MSVCRT.DLL' name 'fclose';
function _ftell ( Stream: Pointer ): Integer; cdecl; external 'MSVCRT.DLL' name 'ftell';
function _fseek ( Stream: Pointer; offset: Integer; origin: Integer ): Integer; cdecl; external 'MSVCRT.DLL' name 'fseek';
function _fread ( Buffer:Pointer; Size: Cardinal; Count: Cardinal; Stream: Pointer ): Cardinal; cdecl; external 'MSVCRT.DLL' name 'fread';
function _fwrite ( const Buffer:Pointer; Size: Cardinal; Count: Cardinal; Stream: Pointer ): Cardinal; cdecl; external 'MSVCRT.DLL' name 'fwrite';
function _fflush ( Stream: Pointer ): Integer; cdecl; external 'MSVCRT.DLL' name 'fflush';
function _fprintf (Stream: Pointer; const format: PChar; other: array of const ): Integer;cdecl; external 'MSVCRT.DLL' name 'fprintf'; varargs;
function _fputc ( c:Integer;Stream: Pointer ): Integer; cdecl; external 'MSVCRT.DLL' name 'fputc';


function __ftol: Integer; cdecl;
procedure __lldiv;
procedure __llmul;

implementation

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





end.
