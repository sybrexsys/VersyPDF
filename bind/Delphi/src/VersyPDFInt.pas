unit VersyPDFInt;
{$i vspas.Inc}
interface
uses Windows, ShellAPI, VersyPDFTypes, VersyJpeg;


const
    		drAllDisabled			 = 0;
        drPrint                 = 1;
        drModifyContent         = 2;
        drCopyInformation       = 4;
        drModifyAnnotation      = 8;
        drFillAcroForm          = 16;
        drExtractTextAndImage   = 32;
        drAssembleDocument      = 64;
        drPrintHiResolution     = 128;
		    drAllEnabled			 = 255;

        sffExclude                 = 1;
        sffIncludeNoValueFields    = 2;
		    sffExportFormat            = 4;
        sffGetMethod               = 8;
        sffSubmitCoordinates       = 16;
        sffXML                     = 32;
        sffIncludeAnnotations      = 128;
        sffSubmitPDF               = 256;

        afInvisible		    = 1;
		    afHidden			= 2;
        afPrint			    = 4;
        afNoZoom			= 8;
        afNoRotate		    = 16;
        afNoView			= 32;
        afReadOnly		    = 64;
        afListBox			= 128;
        afToggleNoView	    = 256;

        aofDefault			=0;
        aofRequired		    =1;
        aofReadOnly		    =2;
        aofNoExport		    =4;
        aofMultiLine		=8;
        aofPassword		    =16;
        aofEdit			    =32;
        aofMultiSelect		=64;
        aofDoNotSpellCheck	=128;
        aofDoNotScroll		=256;

        cbsFinger     = 43;
        cbsPen        = 50;
        cbsVmark      = 51;
        cbsNike       = 52;
        cbsCross      = 53;
        cbsX          = 54;
        cbsCheck      = 56;
        cbsPlus       = 58;
        cbsDiamond    = 70;
        cbsStar       = 72;
        cbsFlower     = 95;
        cbsSnow       = 100;
        cbsCircle     = 108;
        cbsRectangle  = 110;
        cbsRhombus    = 117;
type


   TPDFExplicitDest = record
        ExType              :ppInt32;
        Page                :ppInt32;
        a                   :ppReal;
        b                   :ppReal;
        c                   :ppReal;
        d                   :ppReal;
    end;

    PPDFExplicitDest = ^TPDFExplicitDest;

    PDFColor = record
        Device:Integer;
        Comp1:Double;
        Comp2:Double;
        Comp3:Double;
        Comp4:Double;
    end;
    PPDFColor = ^ PDFColor;

    TPDFDocumentConnection = record
        OldDocument: PDFDocHandle;
        NewDocument: PDFDocHandle;
        Size: Cardinal;
        Pages: ^Cardinal;
    end;

    PPDFDocumentConnection = ^TPDFDocumentConnection;



// Exception functions
function VSGetErrorStr ( ErrorCode:Cardinal; Buffer:PChar ):Integer; register; external;
procedure PPCallException ( Lib: PDFLibHandle; ErrorCode: Cardinal);register; external;

type
  ErrorFunc = procedure ( Lib :PDFLibHandle; ErrCode :Integer; Opaque : Pointer);cdecl;
  MallocFunc = function ( Size:Cardinal):Pointer;cdecl;
  ReallocFunc = function ( Buffer:Pointer; Size:Cardinal):Pointer;cdecl;
  FreeFunc = procedure ( Buffer : Pointer);cdecl;

// Library functions
function InitPDFLibrary(UserName, Password: PChar): PDFLibHandle; register; external;
function InitPDFLibraryWithParams ( LicenseName, LicenseKey: PChar;
         MallocProc:MallocFunc; ReallocProc: ReallocFunc;FreeProc:FreeFunc;
         Error: ErrorFunc; Opaque: Pointer ):PDFLibHandle; register; external;
procedure DonePDFLibrary(Lib: Pointer); register; external;
function ULStringToAtom ( Lib:PDFLibHandle; Str: PChar ):Cardinal;register; external;
function ULAtomToString ( Lib: PDFLibHandle; Atom:Cardinal):PChar;register; external;
function ULExistsAtomForString ( Lib: PDFLibHandle; Str: PChar ):Boolean;register; external;



// Stream functions
//function ULStreamMemNew( Lib: PDFLibHandle; Size: Cardinal ): PDFStreamHandle; register; external;
//function ULStreamMemReadOnlyNew( Lib:PDFLibHandle; Buffer: Pointer; Size: Cardinal ): PDFStreamHandle; register; external;
//function ULStreamFileNew( Lib: PDFLibHandle; FileName: PChar; OpenMode: Integer ): PDFStreamHandle; register; external;
function ULStreamCustomNew ( Lib, AReadBuffer, AWriteBuffer, AGetPosition,
                    ASetPosition,AGetSize, AGetChar, ALookChar, AStream:Pointer ):PDFStreamHandle; register;external;
procedure ULStreamClose( Stream : PDFStreamHandle); register; external;
function ULStreamSetPosition( Stream : PDFStreamHandle; NewPosition: Cardinal ): Cardinal; register; external;
function ULStreamGetSize( Stream : PDFStreamHandle ):Cardinal;register; external;
function ULStreamGetPosition( Stream : PDFStreamHandle ):Cardinal; register; external;
function ULStreamSetSize( Stream :PDFStreamHandle; Size: Cardinal ):Cardinal;register; external;
function ULStreamReadBuffer( Stream: PDFStreamHandle; var Buffer; Count: Cardinal ):Cardinal;  register; external;
function ULStreamWriteBuffer( Stream: PDFStreamHandle; const Buffer; Count: Cardinal ):Cardinal;  register; external;
function ULStreamCopyToStream( FromStrm, ToStrm: PDFStreamHandle):ppUns32;  register; external;

//Image functions
function PDFImageCreate ( Lib: PDFLibHandle; Width, Height:Cardinal;
    Depth:Integer; Device: Integer ):PDFImageHandle; register; external;
function PDFImageLoadFromStream ( Lib: PDFLibHandle; AStream: PDFStreamHandle; Index: Cardinal ):PDFImageHandle; register; external;
function PDFImageLoadFromFile ( Lib: PDFLibHandle; FileName : PChar; Index:Cardinal  ):PDFImageHandle; register; external;
function PDFImageLoadFromBuffer ( Lib: PDFLibHandle; Buffer:Pointer; Size: Cardinal; Index: Cardinal ):PDFImageHandle;  register; external;
function PDFImageLoadFromHandle ( Lib: PDFLibHandle;Handle:HBITMAP ):PDFImageHandle;register; external;
procedure PDFImageFree ( Image : PDFImageHandle  );  register; external;
function PDFImageGetDepth ( Image: PDFImageHandle ):Integer; register; external;
function PDFImageGetColorDevice ( Image: PDFImageHandle ):Integer; register; external;
function PDFImageGetWidth ( Image: PDFImageHandle ):Cardinal;  register; external;
function PDFImageGetHeight ( Image: PDFImageHandle ):Cardinal; register; external;
function PDFImageGetScanLine ( Image: PDFImageHandle; ScanLineIndex: Cardinal ):Pointer; register; external;

function PDFImageGetTIFFCountFromFile ( Lib:PDFLibHandle;FileName: PChar ):Cardinal;  register; external;
function PDFImageGetTIFFCountFromBuffer ( Lib:PDFLibHandle; Buffer:Pointer; Size: Cardinal ):Cardinal; register; external;
function PDFImageAppendToDoc ( Image: PDFImageHandle; Doc:PDFDocHandle; CompressionType:Integer ):Cardinal; register; external;
function PDFImageAppendToDocFromFile ( Doc: PDFDocHandle; FileName: PChar; Index:Cardinal; CompressionType:Integer):Cardinal; register; external;
function PDFImageAppendToDocFromBuffer ( Doc: PDFDocHandle; Buffer: Pointer; BufferSize,
    Index: Cardinal; CompressionType:Integer ):Cardinal; register; external;
function PDFImageAppendToDocFromStream ( Doc: PDFDocHandle; AStream: PDFStreamHandle;
    Index: Cardinal; CompressionType: Integer ):Cardinal; register; external;
function PDFImageAppendToDocAsMask ( Image: PDFImageHandle; Doc: PDFDocHandle; CompressionType:Integer ):Cardinal; register; external;
function PDFImageAppendToDocWithMask ( Image: PDFImageHandle; Doc: PDFDocHandle; CompressionType:Integer;
    MaskImageIndex: Cardinal ):Cardinal; register; external;
procedure PDFImageChange ( Image:PDFImageHandle; NewWidth: Cardinal;
    NewHeight:Cardinal; NewDepth, NewDevice: Integer );register; external;

//
function PDFDocGetOutlineRoot(Doc: PDFDocHandle):PDFOutlineHandle;  register; external;
function PDFOutlineAddNewSibling( Outline: PDFOutlineHandle ):PDFOutlineHandle; register; external;
function PDFOutlineAddNewNext ( Outline: PDFOutlineHandle):PDFOutlineHandle; register; external;
function PDFOutlineAddNewPrev(Outline: PDFOutlineHandle):PDFOutlineHandle;  register; external;
function PDFOutlineAddNewChild(Outline: PDFOutlineHandle):PDFOutlineHandle;  register; external;
function PDFOutlineGetFirstChild(Outline: PDFOutlineHandle):PDFOutlineHandle; register; external;
function PDFOutlineGetLastChild(Outline: PDFOutlineHandle):PDFOutlineHandle;register; external;
function PDFOutlineGetNext(Outline: PDFOutlineHandle):PDFOutlineHandle; register; external;
function PDFOutlineGetPrev(Outline: PDFOutlineHandle):PDFOutlineHandle; register; external;
function PDFOutlineGetParent(Outline: PDFOutlineHandle):PDFOutlineHandle; register; external;
function PDFOutlineHasChildren(Outline: PDFOutlineHandle):Boolean; register; external;
function PDFOutlineGetCount(Outline: PDFOutlineHandle):Integer; register; external;
procedure PDFOutlineSetExpanded(Outline: PDFOutlineHandle; Expanded:Boolean);register; external;
procedure PDFOutlineSetColor(Outline: PDFOutlineHandle; Color:PPDFColor);register; external;
procedure PDFOutlineSetFlags(Outline: PDFOutlineHandle; Flags: Integer);register; external;
procedure PDFOutlineSetTitle(Outline: PDFOutlineHandle; Str:Pointer; Len:Integer); register; external;
procedure PDFOutlineSetDestination(Outline: PDFOutlineHandle;
                                  Destination :PDFDestinationHandle);register; external;
procedure PDFOutlineSetAction(Outline: PDFOutlineHandle; Action:PDFActionHandle );register; external;

// Cos Objects functions


function CosNullNew(PDFDoc: Pointer): Pointer; register; external;

function CosBoolNew(PDFDoc: Pointer; IsIndirect: Boolean; Value: Boolean): Pointer; register; external;
procedure CosBoolSetValue(CosObject: Pointer; Value: Boolean); register; external;
function CosBoolGetValue(CosObject: Pointer): Boolean; register; external;

function CosIntNew(PDFDoc: Pointer; IsIndirect: Boolean; Value: Integer): Pointer; register; external;
procedure CosIntSetValue(CosObject: Pointer; Value: Integer); register; external;
function CosIntGetValue(CosObject: Pointer): Integer; register; external;

function CosRealNew(PDFDoc: Pointer; IsIndirect: Boolean; Value: Double): Pointer; register; external;
procedure CosRealSetValue(CosObject: Pointer; Value: Double); register; external;
function CosRealGetValue(CosObject: Pointer): Double; register; external;

function CosStringNew(PDFDoc: Pointer; IsIndirect: Boolean; CosString: PChar; Length: Integer): Pointer; register; external;
function CosStringGetValue(CosObject: Pointer; Length: PCardinal): PChar; register; external;
procedure CosStringSetValue(CosObject: Pointer; StringVal: PChar; Length: Integer); register; external;

function CosNameNew(PDFDoc: Pointer; IsIndirect: Boolean; Value: Longword): Pointer; register; external;
procedure CosNameSetValue(CosObject: Pointer; Value: Cardinal); register; external;
function CosnameGetValue(CosObject: Pointer): Cardinal; register; external;

function CosArrayNew(PDFDoc: Pointer; IsIndirect: Boolean; Entries: Integer): Pointer; register; external;
function CosArrayAppend(CosObject: Pointer; NewCosObject: Pointer): Integer; register; external;
function CosArrayCount(CosObject: Pointer): Integer; register; external;
function CosArrayInsert(CosObject: Pointer; NewCosObject: Pointer; pos: Integer): Integer; register; external;
procedure CosArrayRemove(CosObject: Pointer; Index: Integer); register; external;
procedure CosArrayClear(CosObject: Pointer); register; external;
function CosArrayItem(CosObject: Pointer; Index: Integer): PDFCosHandle; register; external;


function CosDictNew(PDFDoc: Pointer; IsIndirect: Boolean; Entries: Integer): Pointer; register; external;
procedure CosDictAppend(CosObject: Pointer; Key: Longword; KeyValue: Pointer); register; external;
procedure CosDictClear(CosObject: Pointer); register; external;
procedure CosDictRemoveKey(CosObject: Pointer; Key: Longword); register; external;
function CosDictCount(CosObject: Pointer): Integer; register; external;

function CosStreamNew(PDFDoc: Pointer; IsIndirect: Boolean; Entries: Integer): Pointer; register; external;
function CosStreamGetValue(CosObject: Pointer): Pointer; register; external;

function CosGetFromDoc(PDFDoc: Pointer; ID: Integer): Pointer; register; external;
function CosDictValueByName(CosObject: Pointer; Key: Integer): Pointer; register; external;
function CosCopy(PDFDoc: Pointer; CosObject: Pointer): Pointer; register; external;



procedure CosFree(CosObject: Pointer); register; external;
function CosGetID(CosObject: Pointer): Integer; register; external;
function CosGetGeneration(CosObject: Pointer): Word; register; external;

function CosIsIndirect(CosObject: Pointer): Boolean; register; external;
function CosNumberGetValue(CosObject: Pointer): Double; register; external;

function CosGetType(CosObject: Pointer): Integer; register; external;

// Doc file functions

function PDFDocGetRoot(Doc: PDFDocHandle):PDFCosHandle;  register; external;
function PDFDocGetVersion ( Doc: PDFDocHandle):Integer;register; external;
procedure PDFDocSetVersion ( Doc: PDFDocHandle; Version:Integer);register; external;

function PDFDocCreate ( Lib: PDFLibHandle ): PDFDocHandle; register; external;
procedure PDFDocSaveToFile(Doc: PDFDocHandle; FileName: PChar); register; external;
procedure PDFDocClose(Doc: PDFDocHandle);  register; external;
procedure PDFDestinationNamedNewM(Doc: PDFDocHandle; DestName: PChar; Len:Integer; Dest:PPDFExplicitDest); register; external;

function PDFDocLoadFromFile(Lib: PDFLibHandle; FileName: PChar): PDFDocHandle;  register; external;
function PDFDocLoadFromBuffer ( Lib: PDFLibHandle; Buffer: Pointer; Size: Integer ): PDFDocHandle; register; external;
function PDFDocLoadFromStream( Lib: PDFLibHandle; Stream: PDFStreamHandle ):PDFDocHandle; register; external;
function PDFDocIsCrypted( Doc: PDFDocHandle  ): Boolean; register; external;
function PDFDocCheckPassword( Doc: PDFDocHandle;  Password: PChar ):Integer; register; external;
function PDFDocGetPermission( Doc:PDFDocHandle):Integer; register; external;

procedure PDFDocSetSecurity( Doc:PDFDocHandle; Permission: Integer;
                           KeyLength: Integer;UserPassword,OwnerPassword: PChar );register; external;
procedure PDFDocRemoveSecurity ( Doc: PDFDocHandle ); register; external;
procedure PDFDocSetEncryptMetaData ( Doc: PDFDocHandle;CryptMetadata:Boolean);register; external;

function PDFDocAppendPage( Doc:PDFDocHandle; Width, Height:ppReal  ):Integer; register; external;
function PDFDocAppendPage2( Doc:PDFDocHandle; Size: Integer; Orientation: Integer ):Integer; register; external;
function PDFDocGetPageCount( Doc:PDFDocHandle ):Integer;register; external;
procedure PDFDocSaveToStream( Doc:PDFDocHandle; Stream: PDFStreamHandle ); register; external;
function PDFDocGetInfo( Doc:PDFDocHandle; Info: Integer; StrLen: PCardinal ): Pointer;  register; external;
procedure PDFDocSetInfo( Doc:PDFDocHandle; Info: Integer; Str: PChar; StrLen: Cardinal );  register; external;
procedure PDFDocSetPacked( Doc:PDFDocHandle; ispacked: Boolean ); register; external;
procedure PDFDocSetLinearized( Doc:PDFDocHandle; Linearized: Boolean ); register; external;
procedure PDFDocSetRemoveUnUsed( Doc:PDFDocHandle; Remove: Boolean );  register; external;
procedure PDFDocSetJpegImageQuality ( Doc:PDFDocHandle; Quality:Integer ); register; external;
procedure PDFDocSetAutoLaunch( Doc:PDFDocHandle; AutoLaunch: Boolean ); register; external;
procedure PDFDocSetUsedDC( Doc:PDFDocHandle; DC: HDC  ); register; external;
procedure PDFDocSetEMFColorImagesAsJpeg ( Doc:PDFDocHandle; EmfAsJpeg: Boolean ); register; external;
procedure PDFDocSetEMFBWImagesAsJBIG2 ( Doc:PDFDocHandle; AsJBIG2: Boolean ); register; external;

function PDFFontStandardAppend(Doc: PDFDocHandle; Font: Integer; Encode: Integer ):Cardinal; register; external;
function PDFFontTrueTypeAppend(Doc: PDFDocHandle; FontName: PChar; Bold: Boolean;Italic:Boolean ):Cardinal; register; external;
function PDFFontTrueTypeAppendFromFile(Doc: PDFDocHandle;fontfilename : PChar):Cardinal; register; external;
function PDFFontTrueTypeAppendFromStream(Doc: PDFDocHandle; Strm: PDFStreamHandle ):Cardinal; register; external;
function PDFFontType1AppendFromFile ( Doc: PDFDocHandle;fontfilename : PChar):Cardinal; register; external;
function PDFFontType1AppendFromStream(Doc: PDFDocHandle; Strm: PDFStreamHandle ):Cardinal; register; external;


// Canvas functions

procedure PBXArc(PaintBox: Pointer; x1, y1, x2, y2, x3, y3, x4, y4: Double); register; external;
procedure PBXArc2(PaintBox: Pointer; x1, y1, x2, y2, BeginAngle, EndAngle: Double); register; external;
procedure PBXCircle(PaintBox: Pointer; X, Y, R: Double); register; external;
procedure PBXClip(PaintBox: Pointer); register; external;
procedure PBXClose(PaintBox: Pointer; Pack: Boolean); register; external;
procedure PBXClosePath(PaintBox: Pointer); register; external;
procedure PBXCurveTo(PaintBox: Pointer; x1, y1, x2, y2, x3, y3: Double); register; external;
procedure PBXEllipse(PaintBox: Pointer; x1, y1, x2, y2: Double); register; external;
procedure PBXEoClip(PaintBox: Pointer); register; external;
procedure PBXEoFill(PaintBox: Pointer); register; external;
procedure PBXEoFillAndStroke(PaintBox: Pointer); register; external;
procedure PBXFill(PaintBox: Pointer); register; external;
procedure PBXFillAndStroke(PaintBox: Pointer); register; external;
function PBXGetHeight(PaintBox: Pointer): Double; register; external;
function PBXGetTextWidth(PaintBox: Pointer; Text: PChar): Double; register; external;
function PBXGetUnicodeWidth(PaintBox: Pointer; Text: PWord; Len: Integer): Double; register; external;
function PBXGetWidth(PaintBox: Pointer): Double; register; external;
procedure PBXLineTo(PaintBox: Pointer; X, Y: Double); register; external;
procedure PBXMoveTo(PaintBox: Pointer; X, Y: Double); register; external;
procedure PBXNewPath(PaintBox: Pointer); register; external;
procedure PBXNoDash(PaintBox: Pointer); register; external;
procedure PBXPie(PaintBox: Pointer; x1, y1, x2, y2, x3, y3, x4, y4: Double); register; external;
procedure PBXPie2(PaintBox: Pointer; x1, y1, x2, y2, BeginAngle, EndAngle: Double); register; external;
procedure PBXPlayMetaFile(PaintBox: Pointer; Metafile: HGDIOBJ; X, Y, XScale, YScale: Double); register; external;
procedure PBXRectangle(PaintBox: Pointer; x1, y1, x2, y2: Double); register; external;
procedure PBXRectRotated(PaintBox: Pointer; X, Y, W, H, Angle: Double); register; external;
procedure PBXRoundRect(PaintBox: Pointer; x1, y1, x2, y2, x3, y3: Double); register; external;
procedure PBXSetActiveFont(PaintBox: Pointer; Index: Integer; FontSize: Double; UnderLine, StrikeOut: Boolean); register; external;
procedure PBXSetActiveFontWithCharset(PaintBox: Pointer; Index: Integer; FontSize: Double; Charset: Byte; UnderLine, StrikeOut: Boolean); register; external;
procedure PBXSetCharacterSpacing(PaintBox: Pointer; Spacing: Double); register; external;
procedure PBXSetColorM(PaintBox: Pointer; Color: PPDFColor); register; external;
procedure PBXSetDash(PaintBox: Pointer; Dash: PChar); register; external;
procedure PBXSetFillColorM(PaintBox: Pointer; Color: PPDFColor); register; external;
procedure PBXSetFlatness(PaintBox: Pointer; Flatness: Integer); register; external;
procedure PBXSetGState(PaintBox: Pointer; Index: Integer); register; external;
procedure PBXSetHorizontalScaling(PaintBox: Pointer; Scale: Double); register; external;
procedure PBXSetLineCap(PaintBox: Pointer; LineCap: Integer); register; external;
procedure PBXSetLineJoin(PaintBox: Pointer; LineJoin: Integer); register; external;
procedure PBXSetLineWidth(PaintBox: Pointer; LineWidth: Double); register; external;
procedure PBXSetMiterLimit(PaintBox: Pointer; MiterLimit: Double); register; external;
procedure PBXSetStrokeColorM(PaintBox: Pointer; Color: PPDFColor); register; external;
procedure PBXSetTextRenderingMode(PaintBox: Pointer; Mode: Integer); register; external;
procedure PBXSetWordSpacing(PaintBox: Pointer; Spacing: Double); register; external;
procedure PBXShowImage(PaintBox: Pointer; Index: Integer; X, Y, Width, Height, Angle: Double); register; external;
procedure PBXStateRestore(PaintBox: Pointer); register; external;
procedure PBXStateStore(PaintBox: Pointer); register; external;
procedure PBXStroke(PaintBox: Pointer); register; external;
procedure PBXTextOut(PaintBox: Pointer; X, Y, Orientation: Double; TextStr: PChar); register; external;
procedure PBXUnicodeTextOut(PaintBox: Pointer; X, Y, Orientation: Double; Text: PWord; Len: Integer); register; external;
procedure PBXAppendLine(PaintBox: Pointer; LineCode: PChar); register; external;
function PBXGetStreamHandle( PaintBox:Pointer ): PDFStreamHandle;register; external;

//Thread functions
function PDFDocGetThreadCount( Doc: PDFDocHandle ): Integer; register; external;
function PDFDocGetThread( Doc: PDFDocHandle; Index: Integer ): PDFThreadHandle;register; external;
function PDFThreadNew( Doc: PDFDocHandle ):PDFThreadHandle;register; external;
procedure PDFThreadDelete( Thread: PDFThreadHandle );register; external;
function PDFThreadGetFirstBead( Thread: PDFThreadHandle ):PDFBeadHandle; register; external;
function PDFThreadGetInfo( Thread: PDFThreadHandle; Info: Integer; StrLen: PCardinal  ):PChar; register; external;
procedure PDFThreadSetInfo( Thread: PDFThreadHandle; Info: Integer; Value: PChar; Len: Cardinal ); register; external;
function PDFThreadAppendBeadM( Thread: PDFThreadHandle; Page: Cardinal; Rect: PPDFRect ):PDFBeadHandle; register; external;
function PDFBeadGetNext( Bead: PDFBeadHandle):PDFBeadHandle; register; external;
function PDFBeadGetPrev( Bead: PDFBeadHandle ):PDFBeadHandle; register; external;
procedure PDFBeadGetRect( Bead: PDFBeadHandle; Rect: PPDFRect );register; external;
function PDFBeadGetPage( Bead: PDFBeadHandle ):Integer;register; external;
function PDFBeadGetThread( Bead: PDFBeadHandle ): PDFThreadHandle;register; external;
procedure PDFBeadSetRectM( Bead: PDFBeadHandle; Rect: PPDFRect );register; external;

// AcroInfo function

function PDFAcroGetCount( Doc: PDFDocHandle  ):Cardinal; register; external;

function PDFAcroGetNameByIndex( Doc: PDFDocHandle; Index:Cardinal; IsUnicode:PByte; Length: PCardinal):Pointer;register; external;
function PDFAcroGetValueByIndex( Doc: PDFDocHandle; Index:Cardinal; Length: PCardinal):PChar;register; external;
function PDFAcroGetTypeByIndex( Doc: PDFDocHandle; Index:Cardinal):TPDFAcroType;register; external;
function PDFAcroGetFlagsByIndex( Doc: PDFDocHandle; Index:Cardinal):Integer;register; external;
function PDFAcroGetIndexByName( Doc: PDFDocHandle; FieldName:PChar; CheckUnicodeNames:Boolean):Integer;register; external;
procedure PDFAcroSetValueByIndex( Doc: PDFDocHandle; Index: Cardinal; NewValue:PChar);register; external;
function PDFAcroGetOptionCountByIndex( Doc: PDFDocHandle; FIndex: Cardinal):Integer;register; external;
function PDFAcroGetOptionValueByIndex( Doc: PDFDocHandle; Index, OptionIndex:Cardinal; Length: PCardinal):PChar;register; external;
function PDFAcroGetObjByIndex( Doc:PDFDocHandle; Ingex:Cardinal): PDFCosHandle;register; external;

// Pages functions

function PDFCreateDocumentConnection(  OldDocument, NewDocument: PDFDocHandle ):PPDFDocumentConnection;register; external;
procedure PDFSelectPageFromSourceDocument( DocumentConnection :PPDFDocumentConnection; PageIndex:Cardinal );register; external;
procedure PDFCopyPagesToDestinationDocument( DocumentConnection:PPDFDocumentConnection );register; external;
procedure PDFFreeDocumentConnection( DocumentConnection: PPDFDocumentConnection );register; external;

// Page Function

function PDFPageGetContentCount( Doc: PDFDocHandle; Page: Cardinal): Cardinal;register; external;
function PDFPageGetCosObject( Doc: PDFDocHandle; Page: Cardinal): PDFCosHandle;register; external;
procedure PDFPageSetRotateAngle( Doc: PDFDocHandle; Page: Cardinal; Rotate: Integer );register; external;
function PDFPageGetRotateAngle( Doc: PDFDocHandle; Page: Cardinal): Integer;register; external;
function PDFPageGetBox( Doc: PDFDocHandle; Page: Cardinal; BoxType: Integer ): TPDFRect; register; external;
procedure PDFPageSetBoxM( Doc: PDFDocHandle; Page: Cardinal; BoxType:Integer; Rect: PPDFRect );register; external;
function PDFPageAddContent( Doc: PDFDocHandle; Page: Cardinal): Cardinal;register; external;
function PDFPageInsertContent( Doc: PDFDocHandle; Page: Cardinal; Index: Cardinal ): Cardinal;register; external;
procedure PDFPageRemoveContent( Doc: PDFDocHandle; Page: Cardinal; Index: Cardinal  ); register; external;
function PDFPageCreatePaintBox( Doc: PDFDocHandle; Page: Cardinal; Resolution: Cardinal ): PBXHandle;register; external;
function PDFPageCreatePaintBoxFromContent( Doc: PDFDocHandle; Page: Cardinal; Index: Cardinal; Resolution: Cardinal ): PBXHandle;register; external;
function PDFPageCreatePaintBoxAndRepackContents( Doc: PDFDocHandle;Page:Cardinal;Resolution:Cardinal):PBXHandle;register; external;


// AcroForm functions

procedure PDFAcroObjectSetBorderM( Doc: PDFDocHandle; AcroIndex: Cardinal; BorderColor: PPDFColor;
    FillColor: PPDFColor; BorderWidth: ppReal ); register; external;
procedure PDFAcroObjectSetFontM ( Doc: PDFDocHandle; AcroIndex: Cardinal; FontIndex: Cardinal;
    FontSize: ppReal; Color: PPDFColor ); register; external;
procedure PDFAcroObjectSetFlag ( Doc: PDFDocHandle; AcroIndex: Cardinal; Flag: Cardinal ); register; external;
procedure PDFAcroObjectSetAnnotationFlag ( Doc: PDFDocHandle; AcroIndex: Cardinal; Flag: Word ); register; external;

procedure PDFAcroObjectSetCaption ( Doc: PDFDocHandle; AcroIndex: Cardinal; Caption: PChar ); register; external;
procedure PDFAcroObjectAppendItem ( Doc: PDFDocHandle; AcroIndex: Cardinal; Item: PChar ); register; external;
function PDFPageAppendEditBoxM (Doc: PDFDocHandle;  Page: Cardinal; R: PPDFRect; Name: PChar ):Cardinal; register; external;
procedure PDFAcroEditBoxSetMaxLen ( Doc: PDFDocHandle; AcroIndex: Cardinal; MaxLen: Cardinal ); register; external;
procedure PDFAcroEditBoxSetAlign ( Doc: PDFDocHandle; AcroIndex: Cardinal; Align: Integer ); register; external;
function PDFPageAppendComboBoxM ( Doc: PDFDocHandle; Page: Cardinal; R: PPDFRect; Name: PChar):Cardinal; register; external;
function PDFPageAppendListBoxM ( Doc: PDFDocHandle; Page: Cardinal; R: PPDFRect; Name: PChar):Cardinal; register; external;
function PDFPageAppendSignatureBoxM ( Doc: PDFDocHandle; Page: Cardinal; R: PPDFRect; Name: PChar):Cardinal; register; external;
procedure PDFAcroObjectSetStyle ( Doc: PDFDocHandle; AcroIndex: Cardinal; Sign: Integer; Style: Integer ); register; external;
procedure PDFAcroPushButtonSetMiter ( Doc: PDFDocHandle; AcroIndex: Cardinal; Miter: ppReal ); register; external;
function PDFPageAppendPushButtonM ( Doc: PDFDocHandle; Page: Cardinal; R: PPDFRect; Name: PChar  ):Cardinal; register; external;
function PDFPageAppendCheckBoxM ( Doc: PDFDocHandle; Page: Cardinal; R:PPDFRect; Name: PChar; InitialState: Boolean  ):Cardinal; register; external;
function PDFPageAppendRadioButtonM ( Doc: PDFDocHandle; Page: Cardinal; R: PPDFRect; Name: PChar;
    Group: PChar; InitialState: Boolean ):Cardinal; register; external;
procedure PDFAcroObjectAddAction( Doc: PDFDocHandle; AcroIndex: Cardinal; Action: PDFActionHandle; EventType:Integer ); register; external;



procedure PDFDocAppendSignatureFromBuffer ( Doc: PDFDocHandle; Buffer: Pointer; Size: Cardinal; Name: PChar;
											  Reason: PChar; PKCS7: Boolean; Password: PChar ); register; external;
procedure PDFDocAppendSignatureFromFile ( Doc: PDFDocHandle; FileName: PChar; Name: PChar;
											  Reason: PChar; PKCS7: Boolean; Password: PChar ); register; external;
procedure PDFDocAppendSignatureFromStream ( Doc: PDFDocHandle; Stream: PDFStreamHandle; Name: PChar;
											  Reason: PChar; PKCS7: Boolean; Password: PChar ); register; external;

function PDFPagePlaceSignatureM( Doc:PDFDocHandle; PageIndex:Integer;R: PPDFRect;Resolution:Integer):PBXHandle;register; external;


// GState functions

function PDFExtGraphicStateNew(Doc: PDFDocHandle):Cardinal; register; external;
procedure PDFExtGraphicStateSetLineWidth(Doc: PDFDocHandle; GState: Cardinal; LineWidth: ppReal );register; external;
procedure PDFExtGraphicStateSetLineCap(Doc: PDFDocHandle; GState: Cardinal; LineCap: Integer );register; external;
procedure PDFExtGraphicStateSetLineJoin(Doc: PDFDocHandle; GState: Cardinal; LineJoin: Integer );register; external;
procedure PDFExtGraphicStateSetMitterLimit(Doc: PDFDocHandle; GState: Cardinal; MitterLimit: ppReal );register; external;
procedure PDFExtGraphicStateSetDashPattern(Doc: PDFDocHandle; GState: Cardinal; DashPattern: PDFCosHandle );register; external;
procedure PDFExtGraphicStateSetAlphaIsShape(Doc: PDFDocHandle; GState: Cardinal; AlphaIsShape: Boolean );register; external;
procedure PDFExtGraphicStateSetAlphaStroke(Doc: PDFDocHandle; GState: Cardinal; AlphaStroke: ppReal );register; external;
procedure PDFExtGraphicStateSetAlphaFill(Doc: PDFDocHandle; GState: Cardinal; AlphaFill: ppReal );register; external;
procedure PDFExtGraphicStateSetTextKnockout(Doc: PDFDocHandle; GState: Cardinal; TextKnockout: Boolean );register; external;
procedure PDFExtGraphicStateSetOverprintStroke(Doc: PDFDocHandle; GState: Cardinal; OverprintStroke: Boolean );register; external;
procedure PDFExtGraphicStateSetOverprintFill(Doc: PDFDocHandle; GState: Cardinal; OverprintFill: Boolean );register; external;
procedure PDFExtGraphicStateSetOverprintMode(Doc: PDFDocHandle; GState: Cardinal; OverprintMode: Integer );register; external;
procedure PDFExtGraphicStateSetFlatness(Doc: PDFDocHandle; GState: Cardinal; Flatness: ppReal );register; external;
procedure PDFExtGraphicStateSetRenderingIntent(Doc: PDFDocHandle; GState: Cardinal; Intent: Integer );register; external;
procedure PDFExtGraphicStateSetBlendMode(Doc: PDFDocHandle; GState: Cardinal; BlendMode: Integer );register; external;
procedure PDFExtGraphicStateSetSmoothness(Doc: PDFDocHandle; GState: Cardinal; Smoothness: ppReal );register; external;
procedure PDFExtGraphicStateSetStrokeAdjustment(Doc: PDFDocHandle; GState: Cardinal; StrokeAdjustment:Boolean );register; external;
procedure PDFExtGraphicStateSetCTM(Doc: PDFDocHandle; GState: Cardinal; a,b,c,d,e,f: ppReal );register; external;

// Destination functions
function PDFDestinationFromString( Doc: PDFDocHandle; Str: PChar; Len: Integer; IsIndirect: Boolean ): PDFDestinationHandle ;register; external;
function PDFDestinationFromExplitM( Doc: PDFDocHandle; Dest: PPDFExplicitDest; OtherDocument, IsIndirect: Boolean ):PDFDestinationHandle ;register; external;

//Annotation functions
procedure PDFAnnotationSetFlag( Annotation: PDFAnnotationHandle; AnnotationFlag:Word ); register; external;
procedure PDFAnnotationSetAlphaBlending( Annotation: PDFAnnotationHandle; AlphaBlending: ppReal );register; external;
procedure PDFAnnotationSetTitle ( Annotation: PDFAnnotationHandle; Title: PChar; TitleLength:Cardinal );register; external;
procedure PDFAnnotationSetName ( Annotation: PDFAnnotationHandle; Name: PChar; NameLength: Cardinal  );register; external;
procedure PDFAnnotationSetBorderStyle ( Annotation: PDFAnnotationHandle; Width: ppReal;Style: Integer; Dash: PDFCosHandle );register; external;
procedure PDFAnnotationSetColorM ( Annotation: PDFAnnotationHandle ; Color: PPDFColor  );register; external;
procedure PDFAnnotationSetAction ( Annotation: PDFAnnotationHandle; Action: PDFActionHandle );register; external;
function PDFPageAppendAnnotationTextM ( Doc: PDFDocHandle; Page: Cardinal; Rect: PPDFRect; Content: PChar;
	ContentLength: Cardinal; IsOpened: Boolean; Name: Integer ):PDFAnnotationHandle;register; external;
function PDFPageAppendAnnotationLinkWithDestM ( Doc: PDFDocHandle; Page: Cardinal; Rect: PPDFRect ;
	Destination: PDFDestinationHandle; Mode:Integer; Visible: Boolean ): PDFAnnotationHandle;register; external;
function PDFPageAppendAnnotationLinkWithActionM ( Doc: PDFDocHandle; Page: Cardinal; Rect: PPDFRect ;
	Action: PDFActionHandle; Mode:Integer; Visible: Boolean ): PDFAnnotationHandle;register; external;
function PDFPageAppendAnnotationStampM ( Doc: PDFDocHandle; Page: Cardinal; Rect: PPDFRect; Content: PChar;
	ContentLength: Cardinal; Name: Integer ): PDFAnnotationHandle;register; external;
function PDFPageAppendAnnotationStampWithDrawBoxM ( Doc: PDFDocHandle; Page: Cardinal; Rect: PPDFRect; Content: PChar;
	ContentLength: Cardinal; Name: Integer; Resolution:Cardinal; PBX: PPointer ): PDFAnnotationHandle;register; external;


// Action functions
procedure PDFActionSetNext( Action: PDFActionHandle ; Next: PDFActionHandle );register; external;
function PDFActionNewGoToDestination(Doc: PDFDocHandle; Destination: PDFDestinationHandle ):PDFActionHandle;register; external;
function PDFActionNewGoToRemote(Doc: PDFDocHandle; FileName: PChar;
        FileNameLength: Integer; Destination: PDFDestinationHandle ; InNewWindow: Boolean ):PDFActionHandle;register; external;
function PDFActionNewLaunch(Doc: PDFDocHandle; FileName, DefaultDir,
    Operation, Params: PChar; InNewWindow: Boolean):PDFActionHandle;register; external;
function PDFActionNewURI(Doc: PDFDocHandle; URI: PChar; Len: Integer; IsMap: Boolean):PDFActionHandle;register; external;
function PDFActionNewHide(Doc: PDFDocHandle; IsHide: Boolean ):PDFActionHandle;register; external;
procedure PDFActionHideAddAnnotation(Action:PDFActionHandle; Annotation: PDFAnnotationHandle );register; external;
procedure PDFActionHideAddAnnotationName(Action: PDFActionHandle; AnnotationName: PChar; Len:Integer );register; external;
function PDFActionNewNamed(Doc: PDFDocHandle; NamedType:Integer ):PDFActionHandle;register; external;
function PDFActionNewSubmitForm(Doc: PDFDocHandle; URI: PChar; Len: Integer;
        Flags: Integer ):PDFActionHandle;register; external;
procedure PDFActionSubmitFormAddAnnotation(Action: PDFActionHandle ;
        Annotation: PDFAnnotationHandle );register; external;
procedure PDFActionSubmitFormAddAnnotationName(Action: PDFActionHandle ;
        AnnotationName: PChar; Len: Integer);register; external;
function PDFActionNewResetForm(Doc: PDFDocHandle; Exclude: Boolean):PDFActionHandle;register; external;
procedure PDFActionResetFormAddAnnotation(Action: PDFActionHandle ;
        Annotation: PDFAnnotationHandle );register; external;
procedure PDFActionResetFormAddAnnotationName(Action: PDFActionHandle ;
        AnnotationName: PChar; Len: Integer );register; external;
function PDFActionNewImportData(Doc: PDFDocHandle; FileName: PChar; Len:Integer ):PDFActionHandle; register; external;
function PDFActionNewJavaScript(Doc: PDFDocHandle; JavaScript: PChar; Length: Integer):PDFActionHandle; register; external;
function PDFActionNewJavaScriptStream(Doc: PDFDocHandle; JavaScript: PDFCosHandle):PDFActionHandle;register; external;


//Other
function ULCreateFileID (Doc, InStr: Pointer):Pointer; register; external;
procedure ULCreateKey(Data, Owner, User:Pointer);register; external;
function ULStrmSetCapacity(Strm:Pointer;NW:Cardinal):Integer ;register; external;

// Misc
procedure FFree( C:Pointer);

implementation

uses VersyPng, VersyCommon, VersyTiff, VersyZLib, VersyFreeType;

procedure free ( Dest: Pointer ); cdecl; external 'MSVCRT.DLL' name 'free';

procedure FFree( C:Pointer);
begin
  free( C );
end;



// Internal

function VS_PKCS7_Crypt_EncodeDigestInfo ( Di: Pointer; var pbuf: PByte ) : Integer; cdecl;external;
function VS_PKCS7_Crypt_DecodeDigestInfo ( Lib:Pointer; var Di: Pointer; var pbuf: PByte; D: Int64 ) : Pointer; cdecl;external;
procedure VS_PKCS7_Crypt_DisposeDigestInfo ( Di: Pointer; var pbuf: PByte ); cdecl;external;


procedure PDFEMFExecute ( PaintBox: Pointer; Metafile: HGDIOBJ ); cdecl;external;

procedure PDFDocSavePackedLinearized( Doc:Pointer; Strm: Pointer);cdecl;external;
procedure PDFDocSaveUnPackedLinearized( Doc:Pointer;Strm:Pointer);cdecl;external;
procedure PDFDocSavePackedUnLinearized( Doc:Pointer; Strm:Pointer);cdecl;external;
procedure PDFDocSaveUnPackedUnLinearized( Doc:Pointer; Strm:Pointer);cdecl;external;



{$l obj\VSSaveL.obj}
{$l obj\VSSaveUL.obj}
{$l obj\VSOutlineA.obj}
{$l obj\VSPages.obj}

{$l obj\VSDocA.obj}
{$l obj\VSThreadA.obj}
{$l obj\VSAcroFormA.obj}
{$l obj\VSAcroObjects.obj}
{$l obj\VSAcroInfoA.obj}
{$l obj\VSAcroDraw.obj}
{$l obj\VSAnnotA.obj}
{$l obj\VSActionA.obj}
{$l obj\VSNameTree.obj}
{$l obj\VSGStateA.obj}
{$l obj\VSPageA.obj}
{$l obj\VSSignA.obj}
{$l obj\VSkPKCS12.obj}
{$l obj\VSkPKCS7.obj}
{$l obj\VSkX509.obj}
{$l obj\VSkRSA.obj}
{$l obj\VSkASN.obj}
{$l obj\VSEmf.obj}
{$l obj\VSCanvasA.obj}
{$l obj\VSFontA.obj}
{$l obj\VSTtf.obj}
{$l obj\VSImageA.obj}
{$l obj\VSJbig2.obj}
{$l obj\VSCCITT.obj}
{$l obj\VSBMP.obj}
//{$l obj\sources\VStiff.obj}

{$l obj\VSXRef.obj}
{$l obj\VSParse.obj}
{$l obj\VSCosA.obj}
{$l obj\VSCosDocA.obj}
{$l obj\VSFilter.obj}

{$l obj\VSBaseA.obj}
{$l obj\VSMisc.obj}
{$l obj\VSCrypt.obj}
{$l obj\VSPattern.obj}

{$l obj\VSExcept.obj}
{$l obj\VSLibA.obj}
{$l obj\VSAes.obj}




end.

