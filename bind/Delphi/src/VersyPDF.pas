unit VersyPDF;
{$i vspas.Inc}
interface
uses
{$ifdef USENAMESPACE}
  WinAPI.Windows,System.SysUtils, System.Classes, Vcl.Graphics
{$else}
  Windows, SysUtils, Classes,Graphics
{$endif}, 
VersyPDFTypes, versyPDFInt;

{$ifdef WIN64}
Sorry but at currnt time x64 version is not available becaus I cannot debug x64 object files in delphi ide
{$endif}

type
{
    EPDFException is the exception class for errors that occur when creating/reading PDF document.
}

    EPDFException = class ( Exception );
    TPDFDestination = THandle;
    TPDFAction = class;
    TPDFThread = class;
    TPDFPaintBox = class;
    TPDFAcroInformation = class;
    TPDFPage = class;
{
    Some functions returns PDFString. It's mean result can be unicode or ansi string.
}
    PDFString = array of Byte;
{
	TPDFLibrary contains global library initialization, registration,
	configuration, and termination methods.
Note:
	there is only a single, static instance of TPDFLibrary class. Initialization and termination
	methods need to be called only once per application session.
}
    TPDFLibrary = class
    private
        FLib : PDFLibHandle;
        property Handle : PDFLibHandle read FLib;
    public
{
	Description
		Initialize VersyPDF library for current username and
		registration key. Prepare and fill in all library structures.
	Parameters
		LicenseName: [in] License Name for check license information.
		LicenseKey:  [in] License key for check license information.
}
        constructor Create(LicenseName : AnsiString; LicenseKey : AnsiString);
        destructor Destroy;override;
{
	Description
		Gets atom key by Str in PDF Library.
	Parameters
		Str :  [in] Text String Name.
	Returns
		Atom Key. Name Index in PDF Library
}
        function StringToAtom (Str : AnsiString) : ppAtom;
{
	Description
		Gets Text String Name by Atom Key in PDF Library.
	Parameters
		Atom :  [in] Atom Key.
	Returns
		Text String Name in PDF Library
}
        function AtomToString (Atom : ppAtom) : AnsiString;
{
	Description
		Tests if atom exists in PDF Library for searching text string.
	Parameters
		Str :  [in] Text String Name.
	Returns
		Boolean : true - exists, false - name not found.
}
        function ExistsAtomForString ( Str : AnsiString) : Boolean;
    end;

{
	TPDFStream is a base class used for work with file or memory buffer.
}
    TPDFStream = class(TStream)
    private
        FStream : PDFStreamHandle;
        FIsReadStream: Boolean;
    protected
        procedure SetSize(NewSize: Longint); overload; override;
        procedure SetSize(const NewSize: Int64); overload; override;
    public
        constructor Create(AStream :  PDFStreamHandle);
{
	Description
		Reads from PDF Stream to memory buffer some count of the bytes.
	Parameters
		Buffer :  [in] Memory buffer for data.
		Count :   [in] Count of bytes which we want to read from Stream.
	Returns
		Count of bytes which read from Stream.
}
        function Read(var Buffer; Count: Longint): Longint; override;
{
	Description
		Writes from memory buffer to PDF Stream some count of the bytes.
	Parameters
		Buffer :  [in] Memory buffer with data.
		Count :   [in] Count of bytes which we want to write in	Stream.
	Returns
		Count of written bytes in Stream.
}
        function Write(const Buffer; Count: Longint): Longint; override;
{
    Description
        Use Seek to move the current position within the stream by the indicated offset.
    Parameters
        Offset: [in] Moves the current position of the stream by Offset bytes
        Origin: [in] Origin of the seek
    Return
        Seek returns the new value of the Position property.
}
        function Seek(const Offset: Int64; Origin: TSeekOrigin): Int64; override;
    end;

{
	TPDFImage is an class provides common methods for working with images.
}
    TPDFImage = class
    private
        FImage : PDFImageHandle;
        function GetHeight : Cardinal;
        function GetWidth : Cardinal;
        function GetDepth : TPDFComponentDepth;
        function GetColorSpace : TPDFColorSpace;
        function GetScanLine ( Row : Cardinal) : Pointer;
        property Handle :PDFImageHandle read FImage;
    public
{
	Description
		Create PDF image.
	Parameters:
		ALibrary                 - [ in ] APDF Library object.
		Width               - [ in ] Specifies the width in pixels of the image.
		Height              - [ in ] Specifies the height in pixels of the image.
		Depth               - [ in ] Indicates the bit count per component of the image.
		Space               - [ in ] Indicates the color space in which image samples are specified.
}
        constructor Create(ALibrary : TPDFLibrary; Width : Cardinal; Height : Cardinal;
            Depth : TPDFComponentDepth; Space : TPDFColorSpace);
        destructor Destroy;override;
        {
	Description:
		Create PDF image from image file stored in memory buffer.
	Parameters:
		ALibrary                  - [ in ] PDF Library object.
		Buffer            	- [ in ] Storage location for data.
		Size           - [ in ] Size of the storage.
		Index                - [ in ] Index of the image in file ( Used only for Tiff files )
}
        constructor CreateFromBuffer(ALibrary : TPDFLibrary; Buffer : Pointer; Size : Cardinal; Index : Cardinal);
{
	Description:
		Create image from bitmap handle.
	Parameters:
		ALibrary                  - [ in ] PDF Library object.
		BitmapHandle             	- [ in ] Bitmap handle.
}
        constructor CreateFromHandle(ALibrary : TPDFLibrary; BitmapHandle : HBITMAP );
{
	Description:
		Create PDF image from image file stored in the Stream.
	Parameters:
		ALibrary                  - [ in ] PDF Library object.
		AStream             	- [ in ] Stream object where stored image file.
		Index                - [ in ] Index of the image in file ( Used only for Tiff files )
}
        constructor CreateFromStream(ALibrary : TPDFLibrary; AStream : TStream; Index : Cardinal);
{
	Description:
		Create PDF image from image file.
	Parameters:
		ALibrary                 - [ in ] PDF Library object.
		FileName            - [ in ] Filename of the file where stored image
		Index               - [ in ] Index of the image in file ( Used only for Tiff files )
}
        constructor CreateFromFile(ALibrary : TPDFLibrary; FileName : AnsiString; Index : Cardinal);

        {
	Description:
		Gets count of the images in the tiff file stored in memory buffer.
	Parameters:
		ALibrary                  - [ in ] PDF Library object.
		Buffer            	- [ in ] Storage location for data.
		Size           - [ in ] Size of the storage.
	Returns:
		Count of the images.
}
        class function GetTIFFImagesCount (ALibrary : TPDFLibrary; Buffer : Pointer; Size : Cardinal) : Cardinal;overload;
{
	Description:
		Gets count of the images in the tiff file.
	Parameters:
		ALibrary                 - [ in ] PDF Library object.
		FileName            	- [ in ] Filename of the file where stored image.
	Returns:
		Count of the images.
}
        class function GetTIFFImagesCount (ALibrary : TPDFLibrary; FileName : AnsiString) : Cardinal;overload;
{
	Description:
		Gets count of the images in the tiff file opened with PDF Stream.
	Parameters:
		ALibrary                  - [ in ] PDF Library object.
		AStream             	- [ in ] PDF Stream object.
	Returns:
		Count of the images.
}
        class function GetTIFFImagesCount (ALibrary : TPDFLibrary; AStream : TStream) : Cardinal;overload;
{
	Description:
		Gets the color space in which image samples are specified.
	Returns:
		Color space.
}
        property ColorSpace : TPDFColorSpace read GetColorSpace;
{
	Description:
		Provides indexed access to each line of samples.
	Parameters:
		Row        - [ in ] Index of line of the pixels.
	Returns:
		Pointer to line of the samples.
}
        property ScanLine[Row: Cardinal] :Pointer  read GetScanLine;
{
	Description:
		Gets the bit count per component of the image.
	Returns:
		The bit count.
}
        property Depth : TPDFComponentDepth read GetDepth;
{
	Description:
		Gets the height of the image.
	Returns:
		Height in samples of the bitmap.
}
        property Height : Cardinal read GetHeight;
{
	Description:
		Gets the width of the image.
	Returns:
		Width in samples of the bitmap.
}
        property Width : Cardinal read GetWidth;
    end;

{
	PDFOutline it's class for work with outlines in PDF document
}
    TPDFOutline = class
    private
        FOutline : PDFOutlineHandle;
        FParent:TPDFOutline;
        FCount:Cardinal;
        FList: array of TPDFOutline;
        FNext: TPDFOutline;
        FPrev: TPDFOutline;
        FIndex: Integer;
        property Handle: PDFOutlineHandle  read FOutline;
        function GetCount : Integer;
        procedure SetExpanded (Value : Boolean);
        procedure SetDestination (Value : TPDFDestination);
        procedure SetColor (Value : TPDFRGB);
        procedure SetAction (Value : TPDFAction);
        function GetNext : TPDFOutline;
        function GetFirstChild : TPDFOutline;
        function GetParent : TPDFOutline;
        function GetLastChild : TPDFOutline;
        function GetPrev : TPDFOutline;
        procedure SetFlags (Value : Integer);
        function GetHasChildren : Boolean;
        function GetChild ( Index : Cardinal ): TPDFOutline ;
    public
{
    Description:
        Do not call the constructor for TPDFOutline directly.
        Please use TPDFDocument.GetOutlineRoot, AddNewSibling, AddNewChild, AddNewNext, AddNewPrev
}
         constructor Create(AHandle : PDFOutlineHandle );
{
	Description:
		Sets title of outline.
	Parameters:
		Title      - [ in ] Outline title.
        Charset -[in] Charset of the outline's title
	Returns:
		None.
}
        procedure SetTitle (Title : AnsiString;Charset:TFontCharset = 0 );overload;
{
	Description:
		Sets title of outline.
	Parameters:
		Title      - [ in ] Outline title.
	Returns:
		None.
}
        procedure SetTitle (Title : WideString );overload;
{
	Description
		Creates new outline which will be parallel for current outline.
	Returns
		Newly object to created outline.
}
        function AddNewSibling : TPDFOutline;
{
	Description:
		Creates outline as child for current outline.
	Returns:
		Newly object to created outline.
}
        function AddNewChild : TPDFOutline;
{
	Description
		Adds new outline which will be next for current outline.
	Returns
		Newly object to created outline.
}
        function AddNewNext : TPDFOutline;
{
	Description
		Adds new outline which will be previous for current outline.
		Parameters
	Returns
		Newly object to created outline.
}
        function AddNewPrev : TPDFOutline;
{
	Description:
		Returns previous outline.
	Returns:
		Object to previous outline.
}
        property Prev : TPDFOutline read GetPrev;
{
	Description
		\Returns following outline.
	Returns
		Object to following outline.
}
        property Next : TPDFOutline read GetNext;
{
	Description
		\Returns parent outline.
	Returns
		Object to parent outline.
}
        property Parent : TPDFOutline read GetParent;
{
	Description:
		Links outline to destination.
	Returns:
		None.
    Note:
        You can receive destination handle with TPDFDocument.CreateDestination
}
        property Destination : TPDFDestination write SetDestination;
{
	Description
		\Returns the first outline child.
	Returns
		Object to the first outline child.
}
        property FirstChild : TPDFOutline read GetFirstChild;
{
	Description
		\Returns the last outline child.
	Returns
		Newly object to the last outline child.
}
        property LastChild : TPDFOutline read GetLastChild;
{
	Description
		Inspects current outline on children presence.
	Returns
		If current outline has children - true, else - false.
}
        property HasChildren : Boolean read GetHasChildren;
{
    Description
        Use Childs to access a child node in the outline. Each item is a TPDFOutline object.
    Parameters:
        Index: [in] Index is a number specifying the child to return
    Returns:
        TPDFOutline object.
}
        property Childs[Index :Cardinal ]: TPDFOutline read GetChild;
{
	Description:
		Sets color of outline.
	Returns:
		None.
}
        property Color : TPDFRGB write SetColor;
{
	Description
		\Links action to outline.
	Parameters
		Value :   [ in ] Linked action.
	Returns
		None.
}
        property Action : TPDFAction write SetAction;
{
	Description
		Calculates count of the children.
	Returns
		Count of children.
}
        property Count : Integer read GetCount;
{
	Description
		Sets current outline on expanding.
	Returns
		None.
}
        property Expanded : Boolean write SetExpanded;
{
	Description:
		Sets the outline flags.
	Parameters:
		Flags    - [ in ] Outline flags.
	Returns:
		None.
}
        property Flags : Integer write SetFlags;
    end;



{
	TPDFDocument is a class provides common methods for working with PDF documents.
}
    TPDFDocument = class
    private
        FAcroInfo:TPDFAcroInformation;
        FOutlineRoot:TPDFOutline;
        FPackContent: Boolean;
        FPages: array of TPDFPage;
        FPageCount:Integer;
        FResolution: Cardinal;
        FThreadCount: Integer;
        FThreads: array of TPDFThread;
        FDoc : PDFDocHandle;
        FLibrary : TPDFLibrary;
        FLoadedFromStream: Boolean;
        FStream: PDFStreamHandle;
        property Owner : TPDFLibrary read FLibrary;
        procedure SetUsedDC (DC : HDC );
        procedure SetAutoLaunch ( AAutoLaunch : Boolean);
        procedure SetEMFColorImagesAsJpeg ( AsJpeg : Boolean);
        function GetThreadCount: Cardinal;
        procedure SetEMFBWImagesAsJBIG2 ( AsJBIG2 : Boolean);
        procedure SetPacked ( IsPacked : Boolean);
        function GetPageCount : Cardinal;
        procedure SetLinearized (ALinearized : Boolean);
        procedure SetJpegImageQuality (Quality : Integer);
        procedure SetRemoveUnUsed ( ARemoveUnUsed : Boolean);
        function GetAcroInformation : TPDFAcroInformation;
        function GetIsCrypted : Boolean;
        function GetPermission : TPDFDocumentRestrictions;
        function GetThreads(Index:Cardinal): TPDFThread;
        function GetPages(Index: Cardinal): TPDFPage;
        procedure BeforeSave;
        procedure ClearAll;
    function GetVersion: TPDFVersion;
    procedure SetVersion(const Value: TPDFVersion);
    public

{
	Description :
		LoadFromBuffer New PDF Document.
	Parameters :
		ALibrary                 - [in] PDF Library object.
}
        constructor Create(ALibrary : TPDFLibrary);
        destructor Destroy; override;
{
	Description
		Load PDF Document from Memory buffer.
	Parameters
		ALibrary :     [in] PDF Library object.
		Buffer :  [in] Pointer on PDF document beginning.
		Size :  [in] Length of buffer in bytes.
}
        procedure LoadFromBuffer(Buffer: Pointer; Size: Integer);
{
	Description
		Load PDF Document from memory stream or file stream.
	Parameters
		ALibrary :     [in] PDF Library object.
		AStream :  [in] PDF Stream object.
}
        procedure LoadFromStream(AStream : TStream);
{
	Description
		Load PDF Document from file.
	Parameters
		ALibrary :       [in] PDF Library object.
		FileName :  [in] Name of input file.
}
        procedure LoadFromFile(FileName : AnsiString);
{
	Description
		Save PDF Document in file.
	Parameters
		FileName :  [in] Name of output file, text string.
	Returns
		None.
}
        procedure SaveToFile (FileName : AnsiString);overload;
{
	Description
		Save PDF Document in memory stream or file stream.
	Parameters
		AStream :  [out] PDF Stream Handle.
	Returns
		None.
}
        procedure SaveToStream (AStream : TStream);overload;

        procedure Clear;
{
	Description
		Save information in Document description.
	Parameters
		Info :   [in] Type of Description Information.
		Value :  [in] Value which will associated with description information.
        Charset: [in] Charset of the information
	Returns
		None.
}
        procedure SetDocumentInformation (Info : TPDFInformation; Value : AnsiString; CharSet: TFontCharset = 0); overload;
{
	Description
		Save information in Document description.
	Parameters
		Info :   [in] Type of Description Information.
		Value :  [in] Value which will associated with description information.
	Returns
		None.
}
        procedure SetDocumentInformation (Info : TPDFInformation; Value : WideString); overload;

{
	Description
		Get Information from Document Description.
	Parameters
		Info:  [in] Type of Description Information.
	Returns
		Information string.
}
        function GetDocumentInformation (Info : TPDFInformation) : PDFString;
{
	Description :
		Add Pages in PDF Document.
	Parameters :
		Size                - [in] Type of Pages Size.
		Orientation         - [in] Orientation of Pages.
	Returns :
		Index of New Pages in Document.
}
        function AppendPage (Size : TPDFPageSize; Orientation : TPDFPageOrientation) : Cardinal;overload;
{
	Description :
		Add Pages in PDF Document.
	Parameters :
		Width               - [in] Width of Pages.
		Height              - [in] Height of Pages.
	Returns :
		Index of New Pages in Document.
}
        function AppendPage (Width : Double; Height : Double) : Cardinal;overload;
{
	Description:
		Returns root of outlines.
	Returns:
		Object of document outlines root.
}
        function GetOutlineRoot : TPDFOutline;

{ 
	Description
		Append one of 14 standard fonts to document.
	Parameters
		Font :    [ in ] One of standard font.
		Encode :  [ in ] Font encoding type.
	Returns
		Font index in PDF document.                                        
}
        function FontAppendStandard (Font : TPDFStdandardFont; Encode : TPDFEncodingType) : Cardinal;
{ 
	Description
		Append true type font to document from stream.
	Parameters
		AStream :  [ in ] Font stream.
	Returns
		Font index in PDF document.                    
}
        function FontAppendTrueType (AStream : TStream) : Cardinal;overload;
{
	Description
		Append true type font to document by name.
	Parameters
		FontName :  [ in ] Font name.
		Bold :      [ in ] Is bold style.
		Italic :    [ in ] Is italic style.
	Returns
		Font index in PDF document.                
}
        function FontAppendTrueType (FontName : AnsiString; Bold : Boolean; Italic : Boolean) : Cardinal;overload;
{ 
	Description
		Append true type font to document from file.
	Parameters
		FontFileName :  [ in ] Font filename.
	Returns
		Font index in PDF document.                  
}
        function FontAppendTrueType (FontFileName : AnsiString) : Cardinal;overload;
{ 
	Description
		Append Type1 font to document from stream.
	Parameters
		AStream :  [ in ] Font stream.
	Returns
		Font index in PDF document.
}
        function FontAppendType1 (AStream : TStream) : Cardinal;overload;
{
	Description
		Append Type1 font to document from file.
	Parameters
		FontFileName :  [ in ] Font filename.
	Returns
		Font index in PDF document.
}
        function FontAppendType1 (FontFileName : AnsiString) : Cardinal;overload;


{
	Description :
		Checking the PDF Document on Validity of Password.
	Parameters :
		Password            - [in] Password text string.
	Returns :
		Password Validity Type. If Invalid then Password is invalid.
}
        function CheckPassword (Password : AnsiString) : TKeyValidType;


{
	Description
		Set PDF Document Security.
	Parameters
		Enabled :  [in] If true set document security. If false
									remove security from PDF document.
		Permission :         [in] Document Permission's Flags.
		KeyLength :          [in] 40\-bits or 128\-bits Security Key Length.
		UserPassword :       [in] User Password text string.
		OwnerPassword :      [in] Owner Password text string.
	Returns
		None.
}
        procedure SetSecurity ( Permission : TPDFDocumentRestrictions;
            KeyLength : TPDFProtectionType; UserPassword : AnsiString; OwnerPassword : AnsiString);
        procedure RemoveSecurity;
        procedure SetEncryptMetaData ( IsEncrypt: Boolean );

{
	Description:
		Appends image to document with specified compression.
	Parameters:
		Image                - [ in ] PDF Image handle.
		Compression	    - [ in ] Image compression type.
	Returns:
		Index of the image in PDF document.
}
        function ImageAppend (Image : TPDFImage; Compression : TPDFImageCompressionType) : Cardinal;overload;
{
	Description:
		Appends image to document with specified compression from memory buffer.
	Parameters:
		Buffer            	- [ in ] Storage location for data.
		BufferSize           - [ in ] Size of the storage.
		Index                - [ in ] Index of the image if file ( Used only for Tiff files )
		Compression		    - [ in ] Image compression type.
	Returns:
		Index of the image in PDF document.
}
        function ImageAppend (Buffer : Pointer;  Size : Cardinal; Index : Cardinal; Compression : TPDFImageCompressionType) : Cardinal;overload;
{
	Description:
		Appends image to document with specified compression from stream.
	Parameters:
		AStream              - [ in ] Stream where stored image
		Index                - [ in ] Index of the image if file ( Used only for Tiff files )
		Compression	    - [ in ] Image compression type.
	Returns:
		Index of the image in PDF document.
}
        function ImageAppend (AStream : TStream; Index : Cardinal; Compression : TPDFImageCompressionType) : Cardinal;overload;
{
	Description:
		Appends image to document with specified compression from file.
	Parameters:
		FileName             - [ in ] Filename of file where stored image
		Index                - [ in ] Index of the image if file ( Used only for Tiff files )
		Compression	    - [ in ] Image compression type.
	Returns:
		Index of the image in PDF document.
}
        function ImageAppend (FileName : AnsiString; Index : Cardinal; Compression : TPDFImageCompressionType) : Cardinal;overload;
{
	Description:
		Appends b/w image to document with specified compression as transparent mask.
	Parameters:
		Image                - [ in ] PDF Image handle.
		Compression		    - [ in ] Image compression type.
	Returns:
		Index of the image in PDF document.
}
        function ImageAppendAsMask (Image : TPDFImage; Compression : TPDFImageCompressionType) : Cardinal;
{
	Description:
		Appends image to document with specified compression with transparent mask.
	Parameters:
		Image                - [ in ] PDF Image handle.
		Compression	    - [ in ] Image compression type.
	Returns:
		Index of the image in PDF document.
}
        function ImageAppendWithMask (Image : TPDFImage; Compression : TPDFImageCompressionType; Mask : Integer) : Cardinal;
{ 
	Description
       Creates new destination name in name table and assigns to it
       explicit destination
    Parameters
       Name :       [in] Specifies the name which will be appended to
                      name table.
       Destination :  [in] Explicit destination which will be respected
                      to this name
       Charset: [in] Charset of the name.
    Returns
         None.
}
        procedure AppendNamedDestination ( Destination : TPDFExplicitDestination; Name : AnsiString; Charset: TFontCharset );overload;
{
	Description
       Creates new destination name in name table and assigns to it
       explicit destination
    Parameters
       Name :       [in] Specifies the name which will be appended to
                      name table.
       Destination :  [in] Explicit destination which will be respected
                      to this name
    Returns
         None.
}
        procedure AppendNamedDestination ( Destination : TPDFExplicitDestination; Name : WideString );overload;
{
	Description
		Appends digital signature to PDF document stored in memory buffer.
	Parameters
		Buffer :			[in] Memory buffer where stored PFX Personal Signature file.
		BufferSize :		[in] Size of the memory buffer.
		Name :				[in] Name of the digital signature acro object field.
		Reason:				[in] Reason of Sign this document. For example "I agree..."
		PKCS7:				[in] Boolean flag of coding type :
									true    - 'Adobe.PPKMS' and 'adbe.pkcs7.sha1' crypt system sub filter
									false   - 'Adobe.PPKLite' and 'adbe.x509.rsa_sha1' crypt system sub filter
		Password:			[in] Owner Password for Personal Signature.
		Returns
		None.
}

        function CreateDestination( IsIndirect : Boolean; Str : AnsiString; Charset: TFontCharset= 0):TPDFDestination; overload;
        function CreateDestination( IsIndirect : Boolean; Str : WideString ):TPDFDestination; overload;
        function CreateDestination(IsIndirect: Boolean; Destination:
            TPDFExplicitDestination; IsOtherDocument: Boolean): TPDFDestination; overload;

{
	Description
		Appends digital signature to PDF document stored in file.
	Parameters
		FileName :			[in] Filename of the file where stored PFX Personal Signature.
		Name :				[in] Name of the digital signature acro object field.
		Reason:				[in] Reason of Sign this document. For example "I agree..."
		PKCS7:				[in] Boolean flag of coding type :
									true    - 'Adobe.PPKMS' and 'adbe.pkcs7.sha1' crypt system sub filter
									false   - 'Adobe.PPKLite' and 'adbe.x509.rsa_sha1' crypt system sub filter
		Password:			[in] Owner Password for Personal Signature.
	Returns
		None.
}

        procedure AppendSignature (Buffer : Pointer; Size : Integer; Name : AnsiString; Reason : AnsiString;
            PKCS7 : Boolean; Password : AnsiString);overload;
{
	Description
		Appends digital signature to PDF document stored in file.
	Parameters
		FileName :			[in] Filename of the file where stored PFX Personal Signature.
		Name :				[in] Name of the digital signature acro object field.
		Reason:				[in] Reason of Sign this document. For example "I agree..."
		PKCS7:				[in] Boolean flag of coding type :
									true    - 'Adobe.PPKMS' and 'adbe.pkcs7.sha1' crypt system sub filter
									false   - 'Adobe.PPKLite' and 'adbe.x509.rsa_sha1' crypt system sub filter
		Password:			[in] Owner Password for Personal Signature.
	Returns
		None.
}
        procedure AppendSignature (FileName : AnsiString; Name : AnsiString; Reason : AnsiString;
            PKCS7 : Boolean; Password : AnsiString);overload;
{
	Description
		Appends digital signature to PDF document stored in PDF stream.
	Parameters
		AStream :			[in] PDF Stream where stored PFX Personal Signature.
		Name :				[in] Name of the digital signature acro object field.
		Reason:				[in] Reason of Sign this document. For example "I agree..."
		PKCS7:				[in] Boolean flag of coding type :
									true    - 'Adobe.PPKMS' and 'adbe.pkcs7.sha1' crypt system sub filter
									false   - 'Adobe.PPKLite' and 'adbe.x509.rsa_sha1' crypt system sub filter
		Password:			[in] Owner Password for Personal Signature.
	Returns
		None.
}
        procedure AppendSignature (AStream: TStream; Name : AnsiString; Reason : AnsiString;
            PKCS7 : Boolean; Password : AnsiString);overload;

{
    Description:
        Place appended digital signature to page.
    Parameters:
		   Page					- [ in ] The page  object where will located created signature box.
		   Rect					- [ in ] The signature rectangle, defining the location on the page.
		   Resolution			- [ in ] The resolution of the paint box
    Returns:
        Paint box object for drawing signature.
}
  function PlaceSignature ( Page:TPDFPage; Box : TPDFRect; Resolution: Cardinal):TPDFPaintBox;

{
    Description
        LoadFromBuffer new article in PDF document and return object for work with it
    Returns:
        Newly created PDFThread object.
}
        function ThreadAppend: TPDFThread;
{
	Description
		Set whether PDF document is stored as packed document or not.
	Parameters
		Packed :  [in] if true use packed save method. If false don't use it.
	Returns
		None.                                                         
}
        property Pack : Boolean write SetPacked;
{
	Description :
		Get Pages Count of PDF Document.
	Returns :
		Pages Count of PDF Document.
}
        property PageCount : Cardinal read GetPageCount;
{ 
	Description
		Remove all unused objects from PDF Document before save it.
	Parameters
		Remove :  [in] If true remove unused ( unlink ) objects. If	false don't remove.
	Returns
		None.
}
        property Pages[Index: Cardinal]: TPDFPage read GetPages;
{
    Description
        Sets remove unused object stored in PDF document when Save methods will call.
}

        property RemoveUnUsed : Boolean write SetRemoveUnUsed;
{
	Description
		Set whether PDF document is stored as linearized document or not.
	Parameters
		Linearized :  [in] if true use linearized save method. If false	non linearized method.
	Returns
		None.
}
        property Linearized : Boolean write SetLinearized;

{
	Description:
		Returns object for work with acroform objects values.
	Returns:
		Object of PDFAcroInformation.
}
        property AcroInformation : TPDFAcroInformation read GetAcroInformation;
{
	Description :
		Get Document Permission's Flags.
	Returns :
		Document Permission's Flags.
}
        property Permission : TPDFDocumentRestrictions read GetPermission;
{
	Description
		Checking the PDF Document on Security.
	Returns
		Boolean value : true - if document is crypted, false is not	crypted.
}
        property IsCrypted : Boolean read GetIsCrypted;

{
	Description
		Gets Threads Count in PDF Document.
	Returns
		Number of Threads Items.
}
        property ThreadCount: Cardinal read GetThreadCount;
{
	Description
		Set "Emf BW Images as JBIG2" option for PDF Document.
	Parameters
		AsJBIG2 :       [in] If value has set to true, all black/white images stored in the
							EMF file will be saved with JBIG2 compression.<p/>If value has
							set to false, all images stored in the EMF file will be
							saved with CCITT compression.
	Returns
		None.
}
        property EMFBWImagesAsJBIG2 : Boolean write SetEMFBWImagesAsJBIG2;
{
	Description
		Gets Thread of PDF Document according to Index.
	Parameters
		index :  [in] Index of Thread in PDF Document.
	Returns
		Thread object.
}
        property Threads[Index:Cardinal]: TPDFThread read GetThreads;
{
	Description
		Set AutoLaunch option for PDF Document, it launches after
		saving (only Windows Platform).
	Parameters
		AutoLaunch :  [in] If true launch PDF document, If false don't.
	Returns
		None.
}
        property AutoLaunch : Boolean write SetAutoLaunch;
{ 
	Description
		Set "Jpeg Images Quality" for jpeg images stored in PDF	Document.
	Parameters
		Quality :  [in] Value from 0 to 100\: 0 \- worst quality,
					smallest size. 100 \- best quality, biggest size.
	Returns
		None.
}
        property JpegImageQuality : Integer write SetJpegImageQuality;
{ 
	Description
		Set "Emf color Images as jpeg" option for PDF Document.
	Parameters
		EmfAsJpeg :  [in] If value has set to true, all color images stored in the
						EMF file will be saved as JPEG images.<p/>If value has
						set to false, all color images stored in the EMF file will be
						saved with flate compression.
	Returns
		None.
}
        property EMFColorImagesAsJpeg : Boolean write SetEMFColorImagesAsJpeg;
{
    Description
        Sets pack paintboxes after finish work with its or not.
}
        property PackContent: Boolean  read FPackContent write FPackContent;
{
    Description
        Sets default resolution for all newly created paintboxes
}
        property Resolution: Cardinal read FResolution write FResolution;
{
	Description
		Set hDC concerning which EMF images will be parsed.
	Parameters
		DC :   [in] Handle of Device Context.
	Returns
		None.
}
        property UsedDC :HDC  write SetUsedDC;

        property Version:TPDFVersion read GetVersion write SetVersion;
        property Handle : PDFDocHandle read FDoc;
    end;
{
	TPDFCosObject is an abstract base class for work with cos objects.
}
    TPDFCosObject = class
    protected
        Obj : PDFCosHandle;
        FDoc : PDFDocHandle;
    private
        constructor Create(AHandle : PDFCosHandle);
        property Handle : PDFCosHandle read Obj;
        function GetID : Cardinal;
        function GetGeneration : Word;
        function GetIsIndirect : Boolean;
    public
{
	Description
		Gets free Cos object. If it's a composite object (array, dictionary or stream) :
		* all the direct objects in it will be automatically destroyed
		* the indirect objects in it will be not destroyed
	Returns
		None.
}
        procedure FreeObj;
{
	Description
		Creates new Cos object and copies all data from source Cos
		\object excluding indirect information.
	Parameters
		Document  :  [in] Document for which need create new cos object.
	Returns
		New Cos object which has all infomation from source Cos	\object.
}

        function CreateCopy (Document : TPDFDocument) : TPDFCosObject;
{
	Description
		Tests object if it's indirect or direct.
	Returns
		True if Cos Object is indirect, false if Cos Object is	direct.
}

        property IsIndirect : Boolean read GetIsIndirect;
{
	Description
		Gets the generation number of an indirect Cos object.
	Returns
		The generation number of CosObj.
}
        property Generation : Word read GetGeneration;

{
	Description
		Gets the index for an indirect object.
	Returns
		The ID of CosObj.
}
        property ID : Cardinal read GetID;
    end;
{
	TPDFCosNull is class for work with cos null object.
}
    TPDFCosNull = class (TPDFCosObject)
    private
        constructor Create(AHandle : PDFCosHandle);overload;
    public
{
	Description: 
		Creates a new direct null object.
	Parameters:
		Document         - [in]  The document in which the null object is used.
}
        constructor Create(Document : TPDFDocument);overload;
    end;
{
	TPDFCosBool is class for work with cos boolean object.
}
    TPDFCosBoolean = class (TPDFCosObject)
    private
        constructor Create(AHandle : PDFCosHandle);overload;
        procedure SetValue ( Value : Boolean);
        function GetValue : Boolean;
    public
{
	Description
		Creates a new boolean object and sets the specified value.
	Parameters
		Document :         [in] The document in which the boolean is used.
		IsIndirect :  [in] If true, creates the boolean object as an indirect \object.
		Value :       [in] The value which new boolean will have.
}
        constructor Create(Document : TPDFDocument; IsIndirect : Boolean; Value : Boolean);overload;
{
	Description:
		Sets the value of the specified boolean object.
	Parameters: 
		Value       - [in]  New value of the Cos boolean object.
	Returns:
		None
}
        property Value : Boolean read GetValue write SetValue;
    end;

{
	TPDFCosReal is class for work with cos real objects.
}
    TPDFCosReal = class (TPDFCosObject)
    private
        procedure SetValue (Value : Double);
        function GetValue : Double;
        constructor Create(AHandle : PDFCosHandle);overload;
    public
{ 
	Description
		Creates a new real object and sets the specified value.
	Parameters
		Document :         [in] The document in which the real is used.
		IsIndirect :  [in] If true, creates the real object as an indirect object.
		Value :       [in] The value the new real will have.
}
        constructor Create(Document : TPDFDocument; IsIndirect : Boolean; Value : Double);overload;
{
	Description: 
		Gets the value of the specified real object.
	Returns:
		Value of the real Cos object.
}
        property Value : Double read GetValue write SetValue;
    end;
{
	TPDFCosInteger is class for work with cos integer objects.
}
    TPDFCosInteger = class (TPDFCosObject)
    private
        procedure SetValue (Value : Integer);
        function GetValue : Integer;
        constructor Create(AHandle : PDFCosHandle);overload;
    public
{ 
	Description
		Creates a new integer object and sets the specified value.
	Parameters
		Document :         [in] The document in which the integer is used.
		IsIndirect :  [in] If true, creates the integer object as an indirect object.
		Value :       [in] The value the new integer will have.
}
        constructor Create(Document : TPDFDocument; IsIndirect : Boolean; Value : Integer); overload;
{
	Description: 
		Gets the value of the specified integer object.
	Returns:
		Value of the integer Cos object.
}
        property Value : Integer read GetValue write SetValue;
    end;

{
	TPDFCosName is class for work with cos name object.
}
    TPDFCosName = class (TPDFCosObject)
    private
        function GetValue : Cardinal;
        procedure SetValue (Value : Cardinal);
        constructor Create(AHandle : PDFCosHandle);overload;
    public
{ 
	Description
		Creates a new name object and sets the specified value.
	Parameters
		Document :         [in] The document in which the name object is used.
		IsIndirect :  [in] If true, creates the name object as an indirect object.
		Value :       [in] The value the new name will have.
}
        constructor Create(Document : TPDFDocument; IsIndirect : Boolean; Value : Cardinal);overload;
{ 
	Description
		Creates a new name object and sets the specified value.
	Parameters
		Document :         [in] The document in which the name object is used.
		IsIndirect :  [in] If true, creates the name object as an indirect object.
		Value :       [in] The value the new name will have.
}
        constructor Create(Document : TPDFDocument; IsIndirect : Boolean; Value : AnsiString); overload;
{
	Description: 
		Gets the value of the specified name object.
	Returns:
		Value of the name Cos object.
}
        property Value : Cardinal read GetValue write SetValue;
    end;

{
	TPDFCosString is class for work with cos string object.
}
    TPDFCosString = class (TPDFCosObject)
    private
        constructor Create(AHandle : PDFCosHandle);overload;
    public
{ 
	Description
		Creates a new string object and sets the specified value.
	Parameters
		Document :         [in] The document in which the string is used.
		IsIndirect :  [in] If true, creates the string object as an indirect object.
		Value :      [in] The value that the new string will have. 
}
        constructor Create(Document : TPDFDocument; IsIndirect : Boolean; Value : AnsiString; Charset: TFontCharset = 0 );overload;
{ 
	Description
		Creates a new string object and sets the specified value.
	Parameters
		Document :         [in] The document in which the string is used.
		IsIndirect :  [in] If true, creates the string object as an indirect object.
		Value :      [in] The value that the new string will have. 
}
        constructor Create(Document : TPDFDocument; IsIndirect : Boolean; Value : WideString );overload;
{ 
	Description
		Creates a new string object and sets the specified value.
	Parameters
		Document :         [in] The document in which the string is used.
		IsIndirect :  [in] If true, creates the string object as an indirect object.
		Buffer :      [in] The value that the new string will have. 
		Length :      [in] The length of String.
}
        constructor Create(Document : TPDFDocument; IsIndirect : Boolean; Buffer : Pointer; Len : Cardinal);overload;
{ 
	Description
		Gets the value of string Cos object.
	Returns
		The value of string Cos object.
}
        function GetValueString : PDFString;
{ 
	Description
		Sets the new value for string Cos object.
	Parameters
		Value :     [in] The new value that the string Cos object will have.
	Returns
		None.
}
        procedure SetValueString (Value : AnsiString; Charset: TFontCharset );overload;
{ 
	Description
		Sets the new value for string Cos object.
	Parameters
		Value :     [in] The new value that the string Cos object will have.
	Returns
		None.
}
        procedure SetValueString (Value : WideString );overload;
        procedure SetValue (Value : Pointer; Len: Cardinal );
    end;

{
	TPDFCosArray is class for work with cos array object.
}
    TPDFCosArray = class (TPDFCosObject)
    private
        function GetCount : Integer;
        constructor Create(AHandle : PDFCosHandle); overload;
    public
{
	Description: 
		Creates and returns a new array Cos object.
	Parameters: 
		Document    - [in]  The document in which the array is used.
		IsIndirect  - [in]  If true, creates the array object as an indirect object.  
		Entries     - [in]  The number of elements that will be in the array. This value
							only a hint; Cos arrays grow dynamically as needed.
}
        constructor Create(Document : TPDFDocument; IsIndirect : Boolean; Entries : Integer);overload;
{ 
	Description
		Appends an cos object into an array.
	Parameters
		CosObj :  [in] The object to append.
	Returns
		Position in which Cos object was inserted.
}
        function Append (CosObj : TPDFCosObject) : Integer;
{ 
	Description
		Inserts an cos object into an array.
	Parameters
		CosObj :  [in] The object to insert.
		Index :      [in] The location in the array to insert the cos
		object. The cos object is inserted before the specified
		location. The first element in an array has a pos of
		zero. If pos \>= Count ( CosObject ), it
		appends obj to array (increasing the array count by 1).
	Returns
		Position in which Cos object was inserted.
}
        function Insert (CosObj : TPDFCosObject; Index : Integer) : Integer;
{ 
	Summary
		Removes element from array.
	Description
		Checks whether the position is within the array bounds and
		then removes it from the array and moves each subsequent
		element to the slot with the next smaller Index and
		decrements the array's length by 1. Removed element will be
		free.
	Parameters
		Index :      [in] The Index for the array member to remove. Array indices start at 0.
	Returns
		None
}
        procedure Remove (Index : Integer);
{
	Description
		Gets the specified element from an array.
	Parameters
		Index :      [in] The Index for the array member to obtain. Array indices start at 0.
	Returns
		The Cos object occupying the index element of array. Returns
		a null Cos object if Index is outside the array bounds. If
		specified element is referenced Cos object function returns
		Cos object with ID equal to value of referenced Cos object.
}
        function GetItem (index : Integer) : TPDFCosObject;
{ 
	Description
		Clears and gets free all elements from an array.
	Returns
		None
}
        procedure Clear;
{
	Description
		Gets the number of elements in array.
	Returns
		The number of elements in array.
}
        property Count : Integer read GetCount;
    end;
{
	PDFCosDictionary is class for work with cos dictionary object.
}
    TPDFCosDictionary = class (TPDFCosObject)
    private
        constructor Create(AHandle : PDFCosHandle);overload;
    public
{
	Description: 
		Creates a new dictionary.
	Parameters: 
		Document    - [in]  The document in which the dictionary is used.
		IsIndirect  - [in]  If true, creates the dictionary object as an indirect object.
		Entries     - [in]  Number of entries in the dictionary. This value is
							only a hint - Cos dictionaries grow dynamically as needed.
}
        constructor Create(Document : TPDFDocument; IsIndirect : Boolean; Entries : Integer);overload;
{
	Description
		Sets the value of a dictionary key, adding the key to the dictionary.
	Parameters
		index :        [in] The key which value is set.
		CosObject :      [in] The value to set.
	Returns
		None
}
        procedure SetValue (index : Cardinal; CosObject : TPDFCosObject);
{
	Description
		Gets the value of the specified key in the specified dictionary.
	Parameters
		index :        [in] The key whose value is obtained.
	Returns
		The object associated with the specified key. Returns a null
		Cos object if key is not present. If value is referenced Cos
		\object returns Cos object with ID equal to value of
		referenced Cos object.
}
        function GetValue (index : Cardinal) : TPDFCosObject;
{
	Description
		Removes and gets free a key-value pair from a dictionary.
	Parameters
		Key :        [in] The key to remove.
	Returns
		None
}
        procedure RemoveKey (Key : Cardinal);
{
	Description
		Clears and gets free all keys and values from the dictionary \or stream.
	Returns
		None
}
        procedure Clear;
    end;

{
	TPDFCosStream is class for work with cos stream objects.
}
    TPDFCosStream = class (TPDFCosDictionary)
    private
        constructor Create(AHandle : PDFCosHandle);overload;
    public
{
	Description:
		Creates a new stream.
	Parameters:
		Document         - [in]  The document in which the dictionary is used.
		Entries     - [in]  Number of entries in the attribute dictionary. This value
					is only a hint - Cos dictionaries grow dynamically as needed.
}
        constructor Create(Document : TPDFDocument; Entries : Integer);overload;
{
	Description:
		Gets a stream's value.
	Parameters:
		CosObject   - [in]  The stream whose attributes value is obtained.
	Returns:
		The value of the stream Cos object.
}
        function GetStream : TPDFStream;
    end;

{
	PDFAction is the base class for all actions.
}
    TPDFAction = class
    private
        FAction: PDFActionHandle;
        property Handle:PDFActionHandle read FAction;
        constructor Create( AHandle: PDFActionHandle );
    public
{ 
	Description
		This function sets action which will be executed after current action.
	Parameters
		AAction :    [in] Next action object.
	Returns
		None.
}
        procedure AppendNextAction (AAction : TPDFAction);
    end;
{
	TPDFAnnotation is base class for each annotation in PDF document.
}
    TPDFAnnotation = class
    protected
        FAnnot : PDFAnnotationHandle;
        FDoc : PDFDocHandle;
     private
        constructor Create;
        property Handle : PDFAnnotationHandle  read FAnnot;
        procedure SetAction (Value : TPDFAction);
        procedure SetColor (Value : TPDFRGB);
        procedure SetFlags (Value : TPDFAnnotationFlags);
        procedure SetAlphaBlending (Value : Double);
        procedure SetName (Value : AnsiString);
    public
{
	Description:
		Sets Title for PDF Annotation.
	Parameters:
		Value				- [ in ] Specifies annotation's title.
	Returns:
		None.
}
        procedure SetTitle (Value : AnsiString; Charset: TFontCharset = 0);overload;
{
	Description:
		Sets Title for PDF Annotation.
	Parameters:
		Value				- [ in ] Specifies annotation's title.
	Returns:
		None.
}
        procedure SetTitle (Value : WideString);overload;
{
	Description:
		Sets border style for PDF Annotation.
	Parameters:
		Width				- [ in ] Specifies border's width.
		Style				- [ in ] Specifies border's style.
		Dash				- [ in ] Specifies dash of the border. Used only with style equal to Dashed.
	Returns:
		None.
}
        procedure SetBorderStyle (Width : Double; Style : TPDFBorderStyle; Dash : TPDFCosObject);
{
	Description:
		Sets alpha blending for PDF Annotation.
	Parameters:
		Value		- [ in ] Specifies annotation's alpha blending.
	Returns:
		None.
}
        property AlphaBlending : Double write SetAlphaBlending;
{
	Description:
		Sets flag for PDF Annotation.
	Parameters:
		Value      - [ in ] Specifies annotation's flags.
	Returns:
		None.
}
        property Flag : TPDFAnnotationFlags write SetFlags;
{
	Description:
		Sets action for PDF Annotation which will executed when annotation is activated.
	Parameters:
		Value				- [ in ] Specifies annotation's action.
	Returns:
		None.
}
        property Action : TPDFAction write SetAction;
{
	Description:
		Sets color for PDF Annotation.
	Parameters:
		Value				- [ in ] Specifies annotation's name.
	Returns:
		None.
}
        property Color : TPDFRGB write SetColor;
{
	Description:
		Sets Name for PDF Annotation.
	Parameters:
		Value					- [ in ] Specifies annotation's name.
	Returns:
		None.
}
        property Name : AnsiString write SetName;
    end;
{            
    A TPDFAnnotationText represents a “sticky note” attached to a point in the PDF document.
}
    TPDFAnnotationText = class (TPDFAnnotation)
    public
{
	Description:
		Create new text annotation.
	Parameters:
		Page					- [ in ] Page where will located created text annotation.
		Box					- [ in ] The annotation rectangle, defining the location of the annotation on the page
		Content				- [ in ] Text to be displayed for the annotation	
		IsOpened				- [ in ] Specifying whether the annotation should initially be displayed open.
		Name					- [ in ] The name of an icon to be used in displaying the annotation.
}
        constructor Create(Page : TPDFPage; Box : TPDFRect; IsOpened : Boolean;
            Name : TPDFAnnotationTextName; Content: AnsiString; Charset: TFontCharset = 0);overload;
{
	Description:
		Create new text annotation.
	Parameters:
		Page					- [ in ] Page where will located created text annotation.
		Box					- [ in ] The annotation rectangle, defining the location of the annotation on the page
		Content				- [ in ] Text to be displayed for the annotation	
		IsOpened				- [ in ] Specifying whether the annotation should initially be displayed open.
		Name					- [ in ] The name of an icon to be used in displaying the annotation.
}
        constructor Create(Page : TPDFPage; Box : TPDFRect; IsOpened : Boolean;
            Name : TPDFAnnotationTextName; Content: WideString);overload;

    end;
{
    A TPDFAnnotationLink represents either a hypertext link to a destination elsewhere in
    the document or an action to be performed.
}
    TPDFAnnotationLink = class (TPDFAnnotation)
    public
{
	Description:
		Create new link annotation.
	Parameters:
		Page					- [ in ] Page where will located created link annotation.
		Box					- [ in ] The annotation rectangle, defining the location of the annotation on the page.
		Destination			- [ in ] A destination to be displayed when the annotation is activated.
		Mode					- [ in ] The annotation’s highlighting mode.
		Visible				- [ in ] No border is drawn if set to false.
    Note:
        You can receive destination handle with TPDFDocument.CreateDestination
}
        constructor Create(Page : TPDFPage; Box : TPDFRect; Destination : TPDFDestination; Mode : TPDFHighLightMode;
            Visible : Boolean );overload;
{
	Description:
		Create new link annotation.
	Parameters:
		Page					- [ in ] Page where will located created link annotation.
		Box					- [ in ] The annotation rectangle, defining the location of the annotation on the page.
		Action				- [ in ] An action to be performed when the annotation is activated.
		Mode					- [ in ] The annotation’s highlighting mode.
		Visible				- [ in ] No border is drawn if set to false.
	Returns:
		Link annotation handle.
}
        constructor Create(Page : TPDFPage; Box : TPDFRect; Action : TPDFAction; Mode : TPDFHighLightMode;
            Visible : Boolean);overload;
    end;

{
    A TPDFAnnotationStamp displays text or graphics intended to look as
if they were stamped on the page with a rubber stamp.
}
    TPDFAnnotationStamp = class (TPDFAnnotation)
    public
{
	Description:
		Create new stamp annotaion.
	Parameters:
		Page					- [ in ] Page where will located created stamp annotation.
		Box					- [ in ] The annotation rectangle, defining the location of the annotation on the page.
		Content				- [ in ] Text to be displayed for the annotation
		Name					- [ in ] The name of an icon to be used in displaying the annotation.
}
        constructor Create(Page : TPDFPage; Box : TPDFRect; Name : TPDFAnnotationStampName; Content : AnsiString; Charset: TFontCharset = 0);overload;
{
	Description:
		Create new stamp annotaion.
	Parameters:
		Page					- [ in ] Page where will located created stamp annotation.
		Box					- [ in ] The annotation rectangle, defining the location of the annotation on the page.
		Content				- [ in ] Text to be displayed for the annotation
		Name					- [ in ] The name of an icon to be used in displaying the annotation.
}
        constructor Create(Page : TPDFPage; Box : TPDFRect; Name : TPDFAnnotationStampName; Content : WideString);overload;

{
	Description:
		Create new stamp annotaion.
	Parameters:
		Page					- [ in ] Page where will located created stamp annotation.
		Box					- [ in ] The annotation rectangle, defining the location of the annotation on the page.
		Content				- [ in ] Text to be displayed for the annotation
		Name					- [ in ] The name of an icon to be used in displaying the annotation.

}
        constructor Create(Page : TPDFPage; Box : TPDFRect; Name : TPDFAnnotationStampName; Resolution:Cardinal; var PaintBox: TPDFPaintBox; Content : AnsiString; Charset: TFontCharset = 0);overload;
{
	Description:
		Create new stamp annotaion.
	Parameters:
		Page					- [ in ] Page where will located created stamp annotation.
		Box					- [ in ] The annotation rectangle, defining the location of the annotation on the page.
		Content				- [ in ] Text to be displayed for the annotation
		Name					- [ in ] The name of an icon to be used in displaying the annotation.
}
        constructor Create(Page : TPDFPage; Box : TPDFRect; Name : TPDFAnnotationStampName; Resolution:Cardinal; var PaintBox: TPDFPaintBox;Content : WideString);overload;

    end;

{
	TPDFBead is class for work with beads.
}
    TPDFBead = class
    private
        FBead : PDFBeadHandle;
        FParent: TPDFThread;
        procedure SetRectangle (Rect : TPDFRect);
        function GetRectangle : TPDFRect;
        function GetPage : Integer;
    public
        constructor Create(Parent: TPDFThread; AHandle: PDFBeadHandle); overload;
{
	Description
		Navigates to the next bead item.
	Returns
		Newly object of next bead.
}
        function GetNext : TPDFBead;
{ 
	Description
		Navigates to previous bead item.
	Returns
		Newly object of previous bead.
}
        function GetPrev : TPDFBead;
{
	Description
		Gets an Index of Page on which this bead appears.
	Returns
		Index of Page on which this bead appears.
}
        property Page : Integer read GetPage;
{
	Description
		Gets a object of Bead's Thread.
	Returns
		Object of Bead's Thread.
}
        property Parent: TPDFThread read FParent;
{
	Description
		Gets a rectangle specifying the location of this bead on the page.
	Returns
		Rectangle specifying the location of this bead	on the page.
}
        property Rectangle : TPDFRect read GetRectangle write SetRectangle;
        
    end;


{
	TPDFThread is a class for work with PDF articles.
}
    TPDFThread = class
    private
        FThread:PDFThreadHandle;
        FBeadCount:Integer;
        FBeads:array of TPDFBead;
        function GetCount: Cardinal;
        function CalcBeads: Integer ;
        function GetBeads(Index: Cardinal): TPDFBead;
    public

        constructor Create(AThread: PDFThreadHandle);
        destructor Destroy; override;
{
    Description
        Create new bead for thread
    Parameters:
        Page: [in] Page where need append new bead
        Rect: [in] Rectangle specifies new bead
}
        function AppendBead(Page: TPDFPage; Rect: TPDFRect): TPDFBead;
{
	Description
		Gets information from Thread according to Name.
	Parameters
		Info :		[in] Requesting information identifier.
	Returns
		Requested information string.
}
        function GetInfo (Info : TPDFInformation) : PDFString;
{
	Description
		Sets information to Thread according to property name.
	Parameters
		Info :  [in] Identifier of setting information.
		Value :    [in] Text string information value.
	Returns
		None.
}
        procedure SetInfo (Info : TPDFInformation; Value : AnsiString; Charset: TFontCharset = 0 );overload;
{
	Description
		Sets information to Thread according to property name.
	Parameters
		Info :  [in] Identifier of setting information.
		Value :    [in] Text string information value.
	Returns
		None.
}
        procedure SetInfo (Info : TPDFInformation; Value : WideString );overload;
{
	Description
		Gets Bead object by index in Thread.
    Parameters
        Index: [in] Order number of the required bead
	Returns
		Bead object by index in Thread.
}
        property Beads[Index: Cardinal]: TPDFBead read GetBeads;
{
    Description
        Gets count of the beads in PDF article.
}
        property Count: Cardinal read GetCount;
    end;

{
	PDFActionGoTo is class for work with action "Go to a destination in the current document".
}
    TPDFActionGoTo = class (TPDFAction)
    public
{
	Description
		Creates new "GoTo" action and sets destination to explicit destination.
	Parameters
		Document :          [in] PDF Document object
		Destination :  [in] Destination object.
    Note:
        You can receive destination handle with TPDFDocument.CreateDestination
}
        constructor Create(Document : TPDFDocument; Destination : TPDFDestination);
    end;


{
	PDFActionGoToRemote is class for work with action "Go to a destination in another document".
}
    TPDFActionGoToRemote = class (TPDFAction)
    public
{ 
	Description
		Creates new "GoToRemote" action and sets destination to name destination.
	Parameters
		Document :             [in] PDF Document object
		AFileName :        [in] Filename of the PDF document which need open
		Destination:      [in] Destination object.
		InNewWindow :     [in] Specifying whether to open the destination
							document in a new window. If this flag is false,
							the destination document will replace the current
							document in the same window.
    Note:
        You can receive destination handle with TPDFDocument.CreateDestination
}

        constructor Create(Document : TPDFDocument; AFileName : AnsiString; Destination : TPDFDestination;
            AInNewWindow : Boolean);
    end;

{
	PDFActionLaunch is class for work with action "Launch an application".
}
    TPDFActionLaunch = class (TPDFAction)
    public
{
	Description:
		Creates new "Launch" action. 
	Parameters:
		Document         - [in]  PDF Document object
		FileName			- [in] The application to	be launched or the document to be opened or printed.
		DefaultDir		- [in] A string specifying the default directory in standard DOS syntax.
		Operation		- [in] A string specifying the operation to perform:
								* open  Open a document.
								* print Print a document.
		Params			- [in] A parameter string to be passed to the application.
		InNewWindow - [in]  Specifying whether to open the destination document
							in a new window. If this flag is false, the destination document will
							replace the current document in the same window.
}
        constructor Create(Document : TPDFDocument; FileName : AnsiString; DefaultDir : AnsiString;
            Operation : AnsiString; Params : AnsiString;
            InNewWindow : Boolean);
    end;

{
	PDFActionURL is class for work with action "Resolve a uniform resource identifier".
}
    TPDFActionURL = class (TPDFAction)
    public
{
	Description:
		Creates new "URI" action. 
	Parameters:
		Document    - [in]  PDF Document object.
		AURL        - [in]  The uniform resource identifier to resolve, encoded in 7-bit ASCII.
		IsMap       - [in]  A flag specifying whether to track the mouse 
							position when the URI is resolved.
}
        constructor Create(Document : TPDFDocument; AURL : AnsiString; AIsMap : Boolean);
    end;

{
	PDFActionHide is class for work with action "Set an annotation’s Hidden flag".
}
    TPDFActionHide = class (TPDFAction)
    public
{ 
	Description
		Creates new "Hide" action.
	Parameters
		Document :     [in] PDF Document object.
		IsHide :  [in] Type of the action. Execution of this action will
				hide selected annotations if value sets in "true". In
				other case selected annotations will be shown.
}
        constructor Create(Document : TPDFDocument; AIsHide : Boolean);
{ 
	Description
		Appends annotation to list in the hide action.
	Parameters
		Annotation :  [in] Annotation object which is needed to	be appended to list.
	Returns
		None.
	Note
		Operation will be performed for all annotations in the PDF
		document if any annotation for this action is not selected.    
}
        procedure Append (Annotation : TPDFAnnotation);overload;
{ 
	Description
		Appends annotation to list in the hide action.
	Parameters
		AnnotationName :  [in] Name of the annotation which is needed to be	appended to the list.
	Returns
		None.
	Note
		Operation will be performed for all annotations in the PDF
		document if any annotation for this action is not selected.         
}
        procedure Append (AnnotationName : AnsiString);overload;
    end;

{
	PDFActionNamed is class for work with action "Execute an action predefined by the viewer application".
}
    TPDFActionNamed = class (TPDFAction)
    public
{
	Description:
		Creates new "Named" action. 
	Parameters:
		Document         - [in]  PDF Document object.
		ANamedType   - [in]  Operation for the named action.
}
        constructor Create(Document : TPDFDocument; ANamedType : TPDFNamedActionType);
    end;

{
	PDFActionSubmitForm is class for work with action "Send data to a uniform resource locator".
}
    TPDFActionSubmitForm = class (TPDFAction)
    public
{
	Description:
		Creates new "SubmitForm" action. 
	Parameters:
		Document     - [in]  PDF Document object.
		AURL         - [in]  A URL file specification giving the uniform resource locater of the script
							at the Web server that will process the submission.
		AFlags       - [in]  A set of flags specifying various characteristics	of the action.
}
        constructor Create(Document : TPDFDocument; AURL : AnsiString; AFlags : TPDFSubmitFormFlags);
{ 
	Description
		Appends annotation to list in the submitform action.
	Parameters
		Annotation :  [in] Annotation object which need append	to list.
	Returns
		None.
	Note
		Operation will be performed for all acroform object in the
		PDF document (flag PDF_SUBMIT_FORM_FLAG_EXCLUDE not used ) if
		its not selected any annotation for this action.              
}
        procedure Append (Annotation : TPDFAnnotation);overload;
{ 
	Description
		Appends annotation to list in the submitform action.
	Parameters
		AnnotationName :  [in] Name of the annotation which is needed to be
							appended to the list.
	Returns
		None.
	Note
		Operation will be performed for all acroform object in the
		PDF document (flag PDF_SUBMIT_FORM_FLAG_EXCLUDE not used ) if
		its not selected any annotation for this action.                    
}
        procedure Append (AnnotationName : AnsiString);overload;
    end;

{
	PDFActionResetForm is class for work with action "Set fields to their default values".
}
    TPDFActionResetForm = class (TPDFAction)
    public
{ 
	Description
		Creates new "Reset" action.
	Parameters
		Document :      [in] PDF Document object.
		Exclude :  [in] If false, the list specifies which fields to reset.
			If true, the list of the acroform objects informs which
			fields to be excluded from resetting; all fields in the
			document’s interactive form are reset excepting those
			listed.
}
        constructor Create(Document : TPDFDocument; Exclude : Boolean);
{ 
	Description
		Appends annotation to list in the resetform action.
	Parameters
		Annotation :  [in] Annotation object which is needed to	be appended to list.
	Returns
		None.
	Note
		Reset action will be performed for all annotations in the PDF
		document if its not selected any annotation for this action.   
}
        procedure Append (Annotation : TPDFAnnotation);overload;
{ 
	Description
		Appends annotation to list in the resetform action.
	Parameters
		AnnotationName :  [in] Name of the annotation which is needed to be
							appended to the list.
	Returns
		None.
	Note
		Reset action will be performed for all annotations in the PDF
		document if its not selected any annotation for this action.        
}
        procedure Append (AnnotationName : AnsiString);overload;
    end;

{
	PDFActionImportData is class for work with action "Import field values from a file".
}
    TPDFActionImportData = class (TPDFAction)
    public
{
	Description:
		Creates new "Import Data" action. 
	Parameters:
		Document         - [in]  PDF Document object.
		AFileName    - [in]  The FDF filename from which to import the data.
	Returns: 
		The return value is a handle to a PDF action.
}
        constructor Create(Document : TPDFDocument; AFileName : AnsiString);
    end;

{
	PDFActionJavaScript is class for work with action "Execute a JavaScript script".
}
    TPDFActionJavaScript = class (TPDFAction)
    public
{ 
	Description
		Creates new "JavaScript" action from string.
	Parameters
		Document :         [in] PDF Document object
		AJavaScriptString :  [in] JavaScript string which will be executed
}
        constructor Create(Document : TPDFDocument; AJavaScriptString : AnsiString);overload;
{
	Description
		Creates new "JavaScript" action from CosStream where this javascript is stored.
	Parameters
		Document :         [in] PDF Document object
		AJavaScriptStream :  [in] Cos Stream where this JavaScript is stored
}
        constructor Create(Document : TPDFDocument; AJavaScriptStream : TPDFCosStream);overload;
    end;


{
	TPDFPagesConnector is class for copy pages from one file to other.
}
    TPDFPagesConnector = class
    private
        Connection : PPDFDocumentConnection;
    public
{
	Description :
		Create source and destination documents connection and initialize
		page numbers array and its size.
	Parameters :
		ASource     - [in] Source Document where from take pages.
		ATarget     - [in] Destination Document where to put pages.
}
        constructor Create ( ASource, ATarget: TPDFDocument );
        destructor Destroy; override;
        {
	Description :
		Copy Pages from source to destination document and reset page
		numbers array, but document connection remains linked with
		documents.
	Returns :
		None.
}
        procedure Execute;
{
	Description :
		Add page index ( number as from 0 ) to page numbers array.
	Parameters:
		PageIndex           - [in]  integer index of page from Source Document, as from 0
	Returns:
		None.
}
        procedure AppendPage (PageIndex : Cardinal);

    end;


{
	PDFExtGraphicState is class for work with PDF graphics state.
}
    TPDFExtGraphicState = class
    private
        FDoc : PDFDocHandle;
        Index : Integer;
        property Idx : Integer read Index;
        procedure SetRenderingIntent (ARenderingIntent : TPDFRenderingIntents);
        procedure SetBlendMode (ABlendMode : TPDFBlendMode);
        procedure SetOverprintMode (AOverprintMode : Integer);
        procedure SetFlatness (AFlatness : Double);
        procedure SetSmoothness (ASmoothness : Double);
        procedure SetLineWidth (ALineWidth : Double);
        procedure SetStrokeAdjustment ( AStrokeAdjustment : Boolean);
        procedure SetDashPattern (DashPattern : TPDFCosObject);
        procedure SetMiterLimit (AMiterLimit : Double);
        procedure SetAlphaIsShape (AAlphaIsShape : Boolean);
        procedure SetLineCap (ALineCap : TPDFLineCap);
        procedure SetLineJoin (ALineJoin : TPDFLineJoin);
        procedure SetAlphaStroke (AAlphaStroke : Double);
        procedure SetOverprintStroke ( AOverprintStroke : Boolean);
        procedure SetOverprintFill (AOverprintFill : Boolean);
        procedure SetAlphaFill (AAlphaFill : Double);
        procedure SetTextKnockout ( AKnockout : Boolean);

    public
{
	Description:
		Creates graphic state in document.
	Parameters:
		Document                  - [ in ] Current PDF document.
}
        constructor Create(Document : TPDFDocument);
{
	Description
		Setting CTM ( current transformation matrix ) width for current graphic state.
	Parameters
		a :  [ in ] A part of the CTM.
		b :  [ in ] B part of the CTM.
		c :  [ in ] C part of the CTM.
		d :  [ in ] D part of the CTM.
		e :  [ in ] E part of the CTM.
		f :  [ in ] F part of the CTM.
	Returns
		None.
}
        procedure SetCTM (a,b,c,d,e,f : Double);
{
	Description
		Setting "alpha is shape" for current graphic state.
	Parameters
		AAlphaIsShape :  [ in ] Alpha is shape.
	Returns
		None.                                               
}
        property AlphaIsShape : Boolean write SetAlphaIsShape;
{ 
	Description
		Setting mitter limit for current graphic state.
	Parameters
		AMitterLimit :  [ in ] Mitter limit.
   Returns
			   None.                                            
}
        property MiterLimit : Double write SetMiterLimit;
{ 
	Description
		Setting alpha fill for current graphic state.
	Parameters
		AAlphaFill :  [ in ] Alpha fill.
	Returns
		None.                                          
}
        property AlphaFill : Double write SetAlphaFill;
{ 
	Description
		Setting alpha stroke for current graphic state.
	Parameters
		AAlphaStroke :  [ in ] Alpha stroke.
	Returns
		None.                                             
}
        property AlphaStroke : Double write SetAlphaStroke;
{
	Description
		Setting line width for current graphic state.
	Parameters
		ALineWidth :  [ in ] Line width.
	Returns
		None.
}
        property LineWidth : Double write SetLineWidth;
{ 
	Description
		Setting dash pattern for current graphic state.
	Parameters
		DashPattern :  [ in ] Dash pattern.
	Returns
		None.                                            
}
        property DashPattern : TPDFCosObject write SetDashPattern;
{ 
	Description
		Setting line cap for current graphic state.
	Parameters
		ALineJoin :  [ in ] Line join.
	Returns
		None.                                         
}
        property LineJoin : TPDFLineJoin write SetLineJoin;
{ 
	Description
		Setting line cap for current graphic state.
	Parameters
		ALineCap :  [ in ] Line cap.
	Returns
		None.                                        
}
        property LineCap : TPDFLineCap write SetLineCap;
{ 
	Description
		Setting text knockout for current graphic state.
	Parameters
		ATextKnockout :  [ in ] Text knockout.
	Returns
		None.
}
        property TextKnockout : Boolean write SetTextKnockout;
{ 
	Description
		Setting blending mode for current graphic state.
	Parameters
		ABlendMode :  [ in ] Blending mode.
	Returns
		None.                                            
}
        property BlendMode : TPDFBlendMode write SetBlendMode;
{
	Description
		Setting graphic state rendering intents.
	Parameters
		ARenderingIntent :  [ in ] Rendering intents.
	Returns
		None.
}
        property RenderingIntent : TPDFRenderingIntents write SetRenderingIntent;
{ 
	Description
		Setting stroke adjustment for current graphic state.
	Parameters
		AStrokeAdjustment :  [ in ] Stroke adjustment.
	Returns
		None.                                                 
}
        property StrokeAdjustment : Boolean write SetStrokeAdjustment;
{ 
	Description
		Setting smoothness for current graphic state.
	Parameters
		ASmoothness :  [ in ] Smoothness value.
	Returns
		None.                                           
}
        property Smoothness : Double write SetSmoothness;
{ 
	Description
		Setting overprint fill for current graphic state.
	Parameters
		AOverprintFill :  [ in ] Overprint fill.
	Returns
		None.                                              
}
        property OverprintFill : Boolean write SetOverprintFill;
{
	Description
		Setting overprint stroke for current graphic state.
	Parameters
		AOverprintStroke :  [ in ] Overprint stroke.
	Returns
		None.                                                
}
        property OverprintStroke : Boolean write SetOverprintStroke;
{ 
	Description
		Setting flatness for current graphic state.
	Parameters
		AFlatness :  [ in ] Graphic state flatness.
	Returns
		None.
}
        property Flatness : Double write SetFlatness;
{
	Description
		Setting overprint mode for current graphic state.
	Parameters
		AOverprintMode :  [ in ] Overprint mode.
	Returns
		None.                                              
}
        property OverprintMode : Integer write SetOverprintMode;
    end;

{
	TPDFPaintBox is class which can be used to create new page content.
}
    TPDFPaintBox = class
    private
        PB : PBXHandle;
        FPage: TPDFPage;
        function GetWidth:Double;
        function GetHeight: Double;
    public
{
    Description:
        Do not call the constructor for TPDFPaintBox directly.
}
        constructor Create( Parent:TPDFPage; AHandle : Pointer );
        destructor Destroy;override;
{
	Description
		Append misc line to content..
	Parameters
		LineCode        - [in]  Line of the code which need append to content.
	Returns
			None.
}
        procedure AppendLine (LineCode : AnsiString);
{
	Description
		This function sets stroking color to the specified values.
	Parameters
		Color :     [in] The color of the stroke
	Returns
			None.                                                      
}
        procedure SetStrokeColor (Color : TPDFColor);
{ 
	Description
		This function sets filling color to the specified values .
	Parameters
		Color :     [in] The color of the filling
	Returns
		None.                                                      
}
        procedure SetFillColor (Color : TPDFColor);
{ 
	Description
		This function sets both filling and stroking color to the
		specified values.
	Parameters
		Color :     [in] The color of the filling and stroking
	Returns
		None.
}
        procedure SetColor (Color : TPDFColor);
{
	Description:
		The SetLineJoin method sets the line join for this PaintBox.
	Parameters:
		LineJoin    - [in]  Element of the PDFLineJoin enumeration that specifies 
					the join style used at the end of a line segment that 
					meets another line segment.
	Returns: 
		None.
}
        procedure SetLineJoin (LineJoin : TPDFLineJoin);
{ 
	Description
		Specifies line cap style.
	Parameters
		LineCap :   [in] Element of the PDFLineCap enumeration that	specifies the line cap.
	Returns
		None.                                                        
}
        procedure SetLineCap (LineCap : TPDFLineCap);
{
	Description:
		This procedure sets the current line width to the value specified in points.
	Parameters:
		LineWidth   - [in]  Current line width.
	Returns: 
		None.
}
        procedure SetLineWidth (LineWidth : Double);
{ 
	Description
		The flatness tolerance controls the maximum permitted
		distance in device pixels between the mathematically correct
		path and an approximation constructed with straight line
		segments.
	Parameters
		Flatness :  [in] The flatness tolerance value.
	Returns
		None.                                                        
}
        procedure SetFlatness (Flatness : Integer);
{
	Description:
		This function pushes a copy of the entire graphics state onto the stack.
	Returns: 
		None.
}
        procedure StateStore;
{ 
	Description
		Adds a rectangle with rounded corners to path. The rectangle
		will have edges defined by the points ( x1, y1 ), ( x2, y1 ),
		( x2, y2 ), ( x1, y2 ), but the corners will be shaved to
		create a rounded appearance. The curve of the rounded corners
		matches the curvature of an ellipse with width x3 and height y3
	Parameters
		X1 :        [in] The x\-coordinate of the upper\-left corner of the
						rectangle.
		Y1 :        [in] The y\-coordinate of the upper\-left corner of the
						rectangle.
		X2 :        [in] The x\-coordinate of the lower\-right corner of the
						rectangle.
		Y2 :        [in] The y\-coordinate of the lower\-right corner of the
						rectangle.
		X3 :        [in] Specifies the width of the ellipse used to draw the
						rounded corners.
		Y3 :        [in] Specifies the height of the ellipse used to draw
						the rounded corners.
	Returns
		None.                                                                
}
        procedure RoundRect (X1 : Double; Y1 : Double; X2 : Double; Y2 : Double; X3 : Double; Y3 : Double);
{
	Description:
		This function draws a rectangle of size ( w, h ) with one corner at ( x, y ), 
		with an orientation argument, angle, specified in degrees.
	Parameters:
		X           - [in]  The x-coordinate of the lower-left corner of the rectangle. 
		Y           - [in]  The y-coordinate of the lower-left corner of the rectangle. 
		W           - [in]  The width of the rectangle. 
		H           - [in]  The height of the rectangle. 
		Angle       - [in]  Specifies the angle, in degrees, between the escapement vector and the x-axis of the device. 
	Returns: 
		None.
}
        procedure RectRotated (X : Double; Y : Double; W : Double; H : Double; Angle : Double);
{
	Description:
		The line dash pattern controls the pattern of dashes and gaps used to 
		stroke paths.
	Parameters:
		Dash        - [in]  Dash pattern style.
	Returns: 
		None.
}
        procedure SetDash (Dash : AnsiString);
{
	Description:
		This function resets the dash pattern back to none, i.e., solid line.
	Returns: 
		None.
}
        procedure NoDash;
{
	Description:
		This function restores the entire graphics state to its former value by
		popping it from the stack.
	Returns:
		None.
}
        procedure StateRestore;
{
	Description:
		Writes a character string at the specified location using the currently selected font.
	Parameters:
		X           - [in]  Specifies the x-coordinate of the starting point of the text.
		Y           - [in]  Specifies the y-coordinate of the starting point of the text.
		Orientation - [in]  Specifies the angle, in degrees, between the escapement vector and the x-axis of the device. 
		Str			- [in]  String to be drawn.
	Returns: 
		None.
}
        procedure TextShow (X : Double; Y : Double; Orientation : Double; Str : AnsiString);overload;
{
	Description:
		Writes a character string at the specified location using the currently selected font.
	Parameters:
		X           - [in]  Specifies the x-coordinate of the starting point of the text.
		Y           - [in]  Specifies the y-coordinate of the starting point of the text.
		Orientation - [in]  Specifies the angle, in degrees, between the escapement vector and the x-axis of the device. 
		Str			- [in]  String to be drawn.
	Returns: 
		None.
}
        procedure TextShow (X : Double; Y : Double; Orientation : Double; Str : WideString);overload;
{ 
	Description
		This function sets the mode that determines how the outline
		character is used. By default, the outline character is used
		for filling operations by which inside the outline path is
		painted solidly with the current fill color. This may be
		changed by calling this function.
	Parameters
		Mode :      [in] Rendering mode
	Returns
		None.
}
        procedure SetTextRenderingMode (Mode : TPDFRenderingMode);
{ 
	Description
		This function sets the horizontal scaling factor in a
		percentage. This essentially expands or compresses the
		horizontal dimension of the string. The default value for
		this parameter is 100 (%).
	Parameters
		Scale :     [in] horizontal scaling factor in a percentage.
	Returns
		None.                                                       
}
        procedure SetHorizontalScaling (Scaling : Double);
{ 
	Description
		Function paints context of the metafile on the page.
	Parameters
		Metafile :  [in] Handle to the enhanced metafile
		X        :  [in] The X-coordinate of the upper-left corner
						of the drawing rectangle.
		Y        :  [in] The Y-coordinate of the upper-left corner
						of the drawing rectangle.
		XScale :    [in] X-scaling factor to play metafile.
		YScale :    [in] Y-scaling factor to play metafile.
	Returns
		None.
}
        procedure PlayMetaFile (Metafile : TMetafile; X : Double; Y : Double; XScale : Double; YScale : Double);
{
	Description:
		Returns the width of a text string as it would be displayed in the current font.
	Parameters:
		Str        - [in]  Specifies the character string for which the text width is determined.
	Returns: 
		Width of the text.
}
        function GetTextWidth (Str : AnsiString) : Double;overload;
{
	Description:
		Returns the width of a text string as it would be displayed in the current font.
	Parameters:
		Str        - [in]  Specifies the character string for which the text width is determined.
	Returns: 
		Width of the text.
}
        function GetTextWidth (Str : WideString) : Double;overload;
{
	Description:
		This procedure sets the additional space (in points) that should be 
		inserted between words, i.e., for every space character found in the text string.
	Parameters:
		Spacing     - [in]  Additional space (in points) that should be inserted between words
	Returns: 
		None.
}
        procedure SetWordSpacing (Spacing : Double);
{ 
	Description
		This function places the image data of size ( Width , Height)
		with one corner at (x,y), and angle, specified in degrees
		(ImageIndex is returned by one of the function appending
		image to PDF document) into the current content stream for
		the page
	Parameters
		Index    :  [in] Index of the image appended to PDF document early.
		X        :  [in] The x-coordinate of the lower\-left corner of
						the image.
		Y        :  [in] The y-coordinate of the lower\-left corner of
						the image.
		Width    :  [in] The width of the image.
		Height   :  [in] The height of the image.
		Angle    :  [in] Specifies the angle, in degrees, between the
						escapement vector and the x\-axis.
	Returns
		None.                                                                
}
        procedure ShowImage (Index : Cardinal; X : Double; Y : Double; Width : Double; Height : Double; Angle : Double);
{ 
	Description
		This function sets extended graphic state which can be
		created with function PDFExtGraphicStateNew.
	Parameters
		ExtState :     [in] Extended graphic state objgect which parameters need apply
	Returns
		None.                                                        
}
        procedure SetGState (ExtState : TPDFExtGraphicState);
{ 
	Description
		The SetMiterLimit method sets the miter limit of this object.
	Parameters
		MiterLimit :  [in] Real number that specifies the miter limit of this object.
	Returns
		None.                                                            
}
        procedure SetMiterLimit (MiterLimit : Double);
{
	Description:
		This function sets the additional space that should be inserted between characters.
	Parameters:
		Spacing     - [in]  Size between character in points
	Returns: 
		None.
}
        procedure SetCharacterSpacing (Spacing : Double);
{
	Description:
		This function sets the active font for text operations.
	Parameters:
		Index       - [in]  Index of the font appended to PDF document early.
		FontSize    - [in]  Size of output text, in units
		Charset     - [in]  Charset of the output text
		Underline   - [in]  Specifies an underlined font if set to true.
		StrikeOut   - [in]  Specifies an strikeout font if set to true. 
	Returns: 
		None.
}
        procedure SetActiveFont (Index : Cardinal; FontSize : Double; UnderLine : Boolean; StrikeOut : Boolean; Charset :TFontCharset  = 0 );overload;
{ 
	Description
		Use Pie to append a pie-shaped wedge on the path. The wedge
		is defined by the ellipse bounded by the rectangle determined
		by the points ( x1, y1 ) and ( x2, y2). The drawn section is
		determined by two lines radiating from the center of the
		ellipse through the points ( x3, y3 ) and ( x4, y4 )
	Parameters
		X1 :        [in] Specifies the x\-coordinate of the upper\-left
						corner of the bounding rectangle.
		Y1 :        [in] Specifies the y\-coordinate of the upper\-left
						corner of the bounding rectangle.
		X2 :        [in] Specifies the x\-coordinate of the lower\-right
						corner of the bounding rectangle.
		Y2 :        [in] Specifies the y\-coordinate of the lower\-right
						corner of the bounding rectangle.
		X3 :        [in] Specifies the x\-coordinate of the point that
						defines the arc's starting point.
		Y3 :        [in] Specifies the y\-coordinate of the point that
						defines the arc's starting point .
		X4 :        [in] Specifies the x\-coordinate of the point that
						defines the arc's endpoint.
		Y4 :        [in] Specifies the y\-coordinate of the point that
						defines the arc's endpoint.
	Note
		Current point is center of the wedge.
	Returns
		None.                                                            
}
        procedure Pie (X1 : Double; Y1 : Double; X2 : Double; Y2 : Double; X3 : Double; Y3 : Double; X4 : Double; Y4 : Double);overload;
{ 
	Description
		This closes a path by connecting the first and the last point
		in the path currently constructed. Calling of this procedure
		is often needed to avoid a notch in a stroked path, and to
		make "line join" work correctly in joining the first and the
		last points.
	Returns
		None.                                                         
}
        procedure ClosePath;
{
	Description: 
		Clears the current path in the PaintBox. Current point becomes undefined.
	Returns: 
		None.
}
        procedure NewPath;
{
	Description: 
		This function uses the current path as the boundary for color filling 
		and uses the "non-zero winding number" rule.
	Returns: 
		None.
}
        procedure Fill;
{ 
	Description
		This function installs the current paths as the boundary for
		clipping subsequent drawing and uses the "even-odd" rule for
		defining the "inside" that shows through the clipping window.
		The use of the clip operator may require some care, because
		clip and eoclip operators do not consume the current path.
	Note
		There is not practical way of removing a clipping path,
		except by saving and restoring a graphical state before
		clipping is imposed.
	Returns
		None.                                                         
}
        procedure EoClip;
{ 
	Description
		This function installs the current paths as the boundary for
		clipping of subsequent drawing. The use of the clip operator
		may require some care, because clip and eoclip operators do
		not consume the current path.
	Note
		There is no practical way of removing a clipping path, except
		by save and restore a graphical state before clipping is
		imposed.
	Returns
		None.                                                         
}
        procedure Clip;
{ 
	Description
		This procedure creates an ellipse path specified by top left
		point at pixel coordinates ( x1, y1 ) and the bottom right
		point at ( x2, y2 ) in the counter-clock-wise direction.
	Parameters
		X1 :        [in] The x\-coordinate of the upper\-left corner of the
						bounding rectangle.
		Y1 :        [in] The y\-coordinate of the upper\-left corner of the
						bounding rectangle.
		X2 :        [in] The x\-coordinate of the lower\-right corner of the
						bounding rectangle.
		Y2 :        [in] The y\-coordinate of the lower\-right corner of the
						bounding rectangle.
	Note
		If you need an ellipse drawn in the clock-wise direction,
		please use Arc. This function performs a move to angle 0
		(right edge) of the ellipse. Current point also will be at
		the same location after the call.
	Returns
		None.                                                                
}
        procedure Ellipse (X1 : Double; Y1 : Double; X2 : Double; Y2 : Double);
{ 
	Description
		This procedure creates a circular path centered at (X, Y)
		with radius \R in the counter-clock-wise direction.
	Parameters
		X        :  [in] The X-coordinate of the center of the circle.
		Y        :  [in] The Y-coordinate of the center of the circle.
		R :         [in] The radius of the circle.
	Note
		If you need a circle drawn in the clock-wise direction,
		please use Arc. This function performs a move to angle 0
		(right edge) of the circle. Current point also will be at the
		same location after the call.
	Returns
		None.
}
        procedure Circle (X : Double; Y : Double; R : Double);
{
	Description:
		This function draws a rectangle with one corner at ( x1, y1 ) and second at ( x2, y2 ).
	Parameters:
		X1          - [in]  The x-coordinate of the upper-left corner of the rectangle.
		Y1          - [in]  The y-coordinate of the upper-left corner of the rectangle.
		X2          - [in]  The x-coordinate of the lower-right corner of the rectangle.
		Y2          - [in]  The y-coordinate of the lower-right corner of the rectangle.
	Returns:
		None.
}
        procedure Rectangle (X1 : Double; Y1 : Double; X2 : Double; Y2 : Double);
{
	Description
		Use Pie to append a pie-shaped wedge on the path. The wedge
		is defined by the ellipse bounded by the rectangle determined
		by the points ( x1, y1 ) and ( x2, y2). The drawn section is
		determined by two lines ( BegAngle and EndAngle, specified in
		degrees).
	Parameters
		X1 :          [in] The x\-coordinate of the upper\-left corner of
						the rectangle.
		Y1 :          [in] The y\-coordinate of the upper\-left corner of
						the rectangle.
		X2 :          [in] The x\-coordinate of the lower\-right corner of
						the rectangle.
		Y2 :          [in] The y\-coordinate of the lower\-right corner of
						the rectangle.
		BeginAngle :  [in] Specifies the starting angle in degrees relative
						to the x\-axis.
		EndAngle :    [in] Specifies the ending angle in degrees relative to
						the x\-axis.
	Note
		Current point is center of the wedge.
	Returns
		None.
}
        procedure Pie (X1 : Double; Y1 : Double; X2 : Double; Y2 : Double; BeginAngle : Double; EndAngle : Double);overload;
{
	Description
		Use Arc to create an elliptically curved path. The arc
		traverses the perimeter of an PBXEllipse which is bounded by
		the points ( x1, y1 ) and ( x2, y2 ). The drawn arc is
		following the perimeter of the ellipse , counterclockwise,
		from the starting point to the ending point. The starting
		point is defined by the intersection of the ellipse and a
		line is defined by the center of the ellipse and ( x3, y3).
		The ending point is defined by the intersection of the
		ellipse and a line is defined by the center of the ellipse
		and ( x4, y4 ).
	Parameters
		X1 :        [in] Specifies the x\-coordinate of the upper\-left
						corner of the bounding rectangle.
		Y1 :        [in] Specifies the y\-coordinate of the upper\-left
						corner of the bounding rectangle.
		X2 :        [in] Specifies the x\-coordinate of the lower\-right
						corner of the bounding rectangle.
		Y2 :        [in] Specifies the y\-coordinate of the lower\-right
						corner of the bounding rectangle.
		X3 :        [in] Specifies the x\-coordinate of the point that
						defines the arc's starting point.
		Y3 :        [in] Specifies the y\-coordinate of the point that
						defines the arc's starting point .
		X4 :        [in] Specifies the x\-coordinate of the point that
						defines the arc's endpoint.
		Y4 :        [in] Specifies the y\-coordinate of the point that
						defines the arc's endpoint.
	Returns
		None.
}
        procedure Arc (X1 : Double; Y1 : Double; X2 : Double; Y2 : Double; X3 : Double; Y3 : Double; X4 : Double; Y4 : Double);overload;
{ 
	Description
		Use Arc to create an elliptically curved path. The arc
		traverses the perimeter of an ellipse which is bounded by the
		points ( x1, y1 ) and ( x2, y2 ). The drawn arc is following
		the perimeter of the ellipse, counterclockwise, from the
		starting point to the ending point. The starting point is
		defined by the intersection of the ellipse and a line is
		defined by BegAngle and EndAngle, specified in degrees.
	Parameters
		X1 :          [in] Specifies the x\-coordinate of the upper\-left
						corner of the bounding rectangle.
		Y1 :          [in] Specifies the y\-coordinate of the upper\-left
						corner of the bounding rectangle.
		X2 :          [in] Specifies the x\-coordinate of the lower\-right
						corner of the bounding rectangle.
		Y2 :          [in] Specifies the y\-coordinate of the lower\-right
						corner of the bounding rectangle.
		BeginAngle :  [in] Specifies the starting angle in degrees relative
						to the x\-axis.
		EndAngle :    [in] Specifies the ending angle in degrees relative to
						the x\-axis.
	Returns
		None.                                                                
}
        procedure Arc (X1 : Double; Y1 : Double; X2 : Double; Y2 : Double; BeginAngle : Double; EndAngle : Double);overload;
        procedure Close;
{
	Description:
		This procedure adds a Bezier cubic curve segment to the path starting at 
		the current point as ( x0, y0 ), using two points ( x1, y1 ) and ( x2, y2 ) 
		as control points, and terminating at point ( x3, y3 ). The new current 
		point will be ( x3, y3 ). If there is no current point, an error will result.
	Parameters:
		X1          - [in]  The logical x-coordinate of the first control point position. 
		Y1          - [in]  The logical y-coordinate of the first control point position. 
		X2          - [in]  The logical x-coordinate of the second control point position. 
		Y2          - [in]  The logical y-coordinate of the second control point position. 
		X3          - [in]  The logical x-coordinate of the new position. 
		Y3          - [in]  The logical y-coordinate of the new position.
	Returns: 
		None.
}
        procedure CurveTo (X1 : Double; Y1 : Double; X2 : Double; Y2 : Double; X3 : Double; Y3 : Double);
{
	Description
		This function is used for the first filling the inside with
		the current fill color ( uses the "non-zero winding number"
		rule ), and then stroking the path with the current stroke
		color. PDF's graphics state maintains separate colors to fill
		and stroke operations, thus these combined operators are
		available.
	Returns
		None.                                                         
}
        procedure EoFillAndStroke;
{ 
	Description
		This function is used for the first filling the inside with
		the current fill color, ( uses the "evenodd" rule for
		defining an "inside" that is painted ) and then stroking the
		path with the current stroke color. PDF's graphics state
		maintains separate colors to fill and stroke operations, thus
		these combined operators are available.
	Returns
		None.                                                         
}
        procedure FillAndStroke;
{
	Description: 
		This function uses the current path as the boundary for color filling 
		and uses the "evenodd" rule for defining an "inside" that is painted.
	Returns: 
		None.
}
        procedure EoFill;
{ 
	Description
		This procedure adds a line segment to the path, starting at
		the current point and ending at point ( x, y ).
		Current point sets to ( x, y ).
	Parameters
		X        :  [in] The logical x-coordinate of the endpoint for the line.
		Y        :  [in] The logical y-coordinate of the endpoint for the line.
	Returns
		None.                                                               
}
        procedure LineTo (X : Double; Y : Double);
{
	Description:
		This procedure moves the current point to the location specified by ( x, y ).
	Parameters:
		X           - [in]  The logical x-coordinate of the new position. 
		Y           - [in]  The logical y-coordinate of the new position.
	Returns: 
		None.
}
        procedure MoveTo (X : Double; Y : Double);
{
	Description:
		This function strokes the current paths by the current stroke color 
		and current line width.
	Returns: 
		None.
}
        procedure Stroke;
{ 
	Description
		\Returns width of the PaintBox.
	Returns
		Width of the PaintBox in logical units.      
}
        property Width : Double read GetWidth;
{ 
	Description
		\Returns height of the PaintBox.
	Returns
		Height of the PaintBox in logical units.      
}
        property Height : Double read GetHeight;
        property Handle: PBXHandle read PB;
    end;



{
	TPDFPage is a high-level class representing PDF page object.
}
    TPDFPage = class
    private
        PIndex : Cardinal;
        Doc : TPDFDocument;
        FPBs: array of TPDFPaintBox;
        FCount: Integer;
        function GetParent: PDFDocHandle;
        property Idx : Cardinal read PIndex;
        property Parent : PDFDocHandle read GetParent;
        function GetCount: Cardinal;
        procedure CloseContents;
{
    Description :
        Create Paint Box for Last Page Content with defined resolution.
    Parameters :
        Resolution - [in] Points per inch (dpi).
    Returns :
        PDFPaintBox object.
}
        function CreatePaintBox: TPDFPaintBox; overload;
{
    Description
        Create Paint Box for Page Content according to index with defined resolution.
    Parameters
        Index :       [in] Content's Index in Page Content Streams.
        Resolution :  [in] Points per inch (dpi).
    Returns
        PDFPaintBox object.
}
        function CreatePaintBox(Index : Integer): TPDFPaintBox;
            overload;
        function GetAngle : TPDFPageRotateAngle;
        function GetPaintBoxes(Index:Cardinal): TPDFPaintBox;
        function GetPaintBox: TPDFPaintBox;
        procedure SetAngle (Angle : TPDFPageRotateAngle);
    public
{
    Create TPDFPage object for specified
}
        constructor Create(Document : TPDFDocument; AIndex : Cardinal);
        destructor Destroy; override;
{
	Description
		Remove Content from Page Content Streams according to index.
	Parameters
		Index :  [in] Content Index for deleting.
	Returns
		None.
}
        procedure RemoveContent (Index : Integer);
{
	Description :
		Add void Content to Page Content Streams.
	Returns :
		Page Content Streams Count.
}
        function AddContent : Integer;
        function CreateWrappedPaintBox: TPDFPaintBox;
{
	Description :
		Insert void Content to Page Content Streams in Indexed Site.
	Parameters :
		Index   - [in] Content Index for inserting.
	Returns :
		Index of void Page Content in Content Streams.
}
        function InsertContent (Index : Integer) : Integer;
{
	Description
		Get Typed Rectangle PageBox.
	Parameters
		index :  [in] PageBox Type for request.
	Returns
		PageBox Typed Rectangle.
}
        function GetBox (index : TPDFPageBoxType) : TPDFRect;
{
	Description
		Set Typed Rectangle PageBox.
	Parameters
		index :  [in] PageBox Type for setting.
		Box :  [in] PageBox Rectangle.
	Returns
		None.
}
        procedure SetBox (index : TPDFPageBoxType; Box : TPDFRect);
{
	Description
		Get Rotation Angle of Page.
	Returns
		Page Rotation Angle by which the page displayed or printed
		should be rotated clockwise.
}
        property Angle : TPDFPageRotateAngle read GetAngle write SetAngle;
{
	Description :
		Get Page Content Streams Count.
	Returns :
		Page Content Streams Count.
}
        property Count: Cardinal read GetCount;
        property PaintBoxes[Index:Cardinal]: TPDFPaintBox read GetPaintBoxes;
        property PaintBox: TPDFPaintBox read GetPaintBox;
    end;


{
	TPDFAcroInformation is class for manage acroform object values.
}
    TPDFAcroInformation = class
    private
        FDocument : TPDFDocument;
        constructor Create ( ADocument: TPDFDocument );
        function GetCount : Cardinal;
    public

{
	Description
		Gets Full name of Acro Form Object in Document according to Index.
	Parameters
		Index :  [in] Index as from 0. Object number in AcroForm.
	Returns
		Full name of Acro Form Object ( where partial names are separated by point ).
}
    function GetNameByIndex (Index: Cardinal): PDFString;

{
	Description
		Gets Value of Acro Form Object in Document according to Index.
	Parameters
		Index :  [in] Index as from 0. Object number in AcroForm.
	Returns
		Text string
}
    function GetValueByIndex (Index: Cardinal): PDFString;


{
	Description
		Gets Type of Acro Form Object in Document according to Index.
	Parameters
		Index :  [in] Index as from 0. Object number in AcroForm.
	Returns
		Typed value, for example PushButton, ComboBox, etc.
	See Also
		PDFAcroType
}
  function GetTypeByIndex( Index: Cardinal ): TPDFAcroType ;

{
	Description
		Gets flag of Acro Form Object in Document according to Index.
	Parameters
		Index :  [in] Index as from 0. Object number in AcroForm.
	Returns
		Flag of the acroform object. More information about this value you can find in PDF Reference.
	See Also
		PDFAcroType
}
  function GetFlagsByIndex( Index: Cardinal ): Integer;

{
	Description
		Gets index of Acro Form Object in Document according to name.
	Parameters
		FieldName	:  [in] Name of the acroform object.
		CheckUnicodeNames : [in] if true function will check acrofields with unicode names
	Returns
		Index of the acroform object.
}
  function GetIndexByName( FieldName:AnsiString; CheckUnicodeNames:Boolean): Cardinal;

{
	Description
		Sets value of Acro Form Object in Document according to Index.
	Parameters
		Index :  [in] Index as from 0. Object number in AcroForm.
		NewValue : [in] New value of the acroform object.
	Returns
		None.
	Note
		Value "Off" turn off checkbox and radiobutton group fields.
		Any other value turn on checkbox field.
		For radiobutton group you must use only values received with GetOptionValueByIndex function
}
  procedure SetValueByIndex ( Index: Cardinal; NewValue: AnsiString);


{
	Description
		Gets count of acroform object's options. Can be used only for combobox, listbox and radiobutton group.
	Parameters
		Index :  [in] Index as from 0. Object number in AcroForm.
	Returns
		Count of acroform object's options.
}
  function GetOptionCountByIndex ( Index :Cardinal): Cardinal;


{
	Description
		Gets option value of acroform object.
		Can be used only for combobox, listbox and radiobutton group.
	Parameters
		Index :  [in] Index as from 0. Object number in AcroForm.
		OptionIndex : [in]  Index of the option in acroform field.
	Returns
		Option value of acroform object.
}
   function GetOptionValueByIndex ( Index, OptionIndex: Cardinal): PDFString;


{
	Description
		Gets Cos Object of Acro Form Object in Document according to Index.
	Parameters
		Doc :  [in] Source Document.
		Index :  [in] Index as from 0. Object number in AcroForm.
	Returns
		Cos Object Handle.
}
  function GetObjByIndex( Index:Cardinal ):TPDFCosObject ;

{
	Description
		Gets number of Acro Objects in Document.
	Returns
		Number of Acro Objects in Document.
}
        property Count: Cardinal read GetCount;
    end;


{
	PDFAcroObject ia an abstract base class for all acroobjects
}
    TPDFAcroObject = class
    private
        constructor Create ( Page: TPDFPage );
        procedure SetCaption (Value : AnsiString);
        procedure SetFlag (Value : TPDFAcroObjectFlags);
    procedure SetAnnotationFlag(const Value: Word);
    protected
        FDocument : PDFDocHandle;
        AcroIndex : Cardinal;
    public
{
Description
		Adds Action OnEvent in Acro Form Object (Control).
	Parameters
		Action :           [in] Action for include to trigger.
		EventType :             [in] Type of event on Control for Action
	Returns
		None.
}
        procedure AddAction (Action : TPDFAction; EventType : TPDFAcroEventType);
{
	Description
		Sets Acroobject border
	Parameters
		BorderColor :       [in] Color of the border line.
		FillColor :			[in] Color of the background.
		BorderWidth:		[in] Width of the border line.
	Returns
		None.
}
        procedure SetBorder (BorderColor : TPDFColor; FillColor : TPDFColor; BorderWidth : Double);
{
	Description
		Sets Acroobject drawing font
	Parameters
		FontIndex :			[in] Index of the font stored in PDF document.
		FontSize :			[in] Size of the used font.
		FontColor :			[in] Color of the used font.
	Returns
		None.
}
        procedure SetFont (FontIndex : Cardinal; FontSize : Double; FontColor : TPDFColor);


{
	Description
		Sets caption of the acroform object.
	Parameters
		Value :			[in] Caption which need set .
	Returns
		None.
}
        property Caption : AnsiString write SetCaption;
{
	Description
		Sets flags specifying various characteristics of the acroform object.
	Parameters
		Value :				[in] Characteristics of the acroform object .
	Returns
		None.
}
        property Flag : TPDFAcroObjectFlags write SetFlag;
{
	Description
		Sets flags specifying various characteristics of the acroform object.
	Parameters
		Value :				[in] Characteristics of the acroform object .
	Returns
		None.
}
        property AnnotationFlag : Word write SetAnnotationFlag;


    end;
{
    TPDFAcroEdit is similar to TEdit and creates a control which accepts input in a PDF document.
}
    TPDFAcroEditBox = class (TPDFAcroObject)
    private
        procedure SetAlign (Value : TPDFAcroQuadding);
        procedure SetMaxLen (Value : Cardinal);
    public
{
	Description:
		Create new acro edit box.
	Parameters:
		Page					- [ in ] Page where will located created edit box.
		Box						- [ in ] The annotation rectangle, defining the location of the acro field on the page
		Name					- [ in ] Name of Acroform object, Name of Acroform field is used to export when the PDF document is
										submitted	
}
        constructor Create(Page : TPDFPage; Box : TPDFRect; Name : AnsiString);
{	
	Description
		Sets maximum length of the edit box text.
	Parameters
		Value :			[in] Maximum length of the edit box text.
	Returns
		None.                                                            
}
        property MaxLen : Cardinal write SetMaxLen;
{	
	Description
		Sets text justification of the edit box.
	Parameters
		Value :				[in] Text justification which need set.
	Returns
		None.                                                            
}
        property Align : TPDFAcroQuadding write SetAlign;
    end;

{
    A TPDFAcroSignatureBox  create empty signature field represents an electronic “signature” for authenticating
        the identity of a user.
}
    TPDFAcroSignatureBox = class (TPDFAcroObject)
    public
{
	Description:
		Create new acro empty signature box.
	Parameters:
		Page					- [ in ] Page where will located created signature box.
		Box						- [ in ] The annotation rectangle, defining the location of the acro field on the page
		Name					- [ in ] Name of Acroform object, Name of Acroform field is used to export when the PDF document is
										submitted	
}
        constructor Create(Page : TPDFPage; Box : TPDFRect; Name : AnsiString);
    end;
{
    TPDFAcroButton creates a button in a PDF document.
    A button represents an interactive control on the screen that the user can manipulate with the mouse.
}
    TPDFAcroPushButton = class (TPDFAcroObject)
    private
        procedure SetMiter (Value : Double);
    public
{
	Description:
		Create new acro push button.
	Parameters:
		Page					- [ in ] Page where will located created push button.
		Box						- [ in ] The annotation rectangle, defining the location of the acro field on the page
		Name					- [ in ] Name of Acroform object, Name of Acroform field is used to export when the PDF document is
									submitted	
}
        constructor Create(Page : TPDFPage; Box : TPDFRect; Name : AnsiString);
{	
	Description
		Sets miter of the pushbutton.
	Parameters
		Miter :				[in] Miter of pushbutton, bevel size.
	Returns
		None.                                                            
}
        property Miter : Double write SetMiter;
    end;


{
    TPDFAcroRadioButton is similar to TRadioButton and creates a radiobutton in a PDF document.
}
    TPDFAcroRadioButton = class (TPDFAcroObject)
    public
{
	Description:
		Create new acro radio button.
	Parameters:
		Page					- [ in ] Page where will located created radio button.
		Box						- [ in ] The annotation rectangle, defining the location of the acro field on the page
		Name					- [ in ] Name of Acroform object, Name of Acroform field is used to export when the PDF document is
										submitted	
		GroupName				- [ in ] Name of Radio Buttons Group to which RadioButton will be linked.
		InitialState			- [ in ] Value of the check box.
}
        constructor Create(Page : TPDFPage; Box : TPDFRect; Name : AnsiString; GroupName : AnsiString; InitaialState : Boolean);
{	
	Description
		Sets display style for radiobutton and checkbox.
	Parameters
		Sign :				[in] Code of Mark character in CheckBox or RadioButton.
		Style :				[in] Style of CheckBox or RadioButton - rectangle or circle.
	Returns
		None.
}
        procedure SetStyle (Sign : TPDFCheckBoxSign; Style : TPDFCheckBoxStyle);
    end;

{
    TPDFAcroCheckBox is similar to TCheckBox (checkbox toggles between two states, on and off) and
    creates a checkbox in a PDF Document.
}
    TPDFAcroCheckBox = class (TPDFAcroObject)
    public
{
	Description:
		Create new acro check box.
	Parameters:
		Page					- [ in ] Page where will located created check box.
		Box						- [ in ] The annotation rectangle, defining the location of the acro field on the page
		Name					- [ in ] Name of Acroform object, Name of Acroform field is used to export when the PDF document is
										submitted
		InitialState		- [ in ] Value of the check box.
}
        constructor Create(Page : TPDFPage; Box : TPDFRect; Name : AnsiString; InitaialState : Boolean);
{	
	Description
		Sets display style for radiobutton and checkbox.
	Parameters
		Sign :				[in] Code of Mark character in CheckBox or RadioButton.
		Style :				[in] Style of CheckBox or RadioButton - rectangle or circle.
	Returns
		None.
}
        procedure SetStyle (Sign : TPDFCheckBoxSign; Style : TPDFCheckBoxStyle);
    end;


{
    TPDFAcroListBox is similar to TListBox and creates a listbox in a PDF Document.
}
    TPDFAcroListBox = class (TPDFAcroObject)
    public
{
	Description:
		Create new acro list box.
	Parameters:
		Page					- [ in ] Page where will located created list box.
		Box						- [ in ] The annotation rectangle, defining the location of the acro field on the page
		Name					- [ in ] Name of Acroform object, Name of Acroform field is used to export when the PDF document is
									submitted
}
        constructor Create(Page : TPDFPage; Box : TPDFRect; Name : AnsiString);
{
	Description
		Appends items to ListBox or to ComboBox.
	Parameters
		Item :				[in] Item which need append to list.
	Returns
		None.
}
        procedure AppendItem (Item : AnsiString);
    end;

{
    TPDFAcroComboBox is similar to TComboBox and creates a ComboBox in a PDF Document.
    A combo box consisting of a dropdown list optionally accompanied by an editable text box in which the user can type a value other than the predefined choices.
}
    TPDFAcroComboBox = class (TPDFAcroObject)
    public
{
	Description:
		Create new acro combo box.
	Parameters:
		Page					- [ in ] Page where will located created combo box.
		Box						- [ in ] The annotation rectangle, defining the location of the acro field on the page
		Name					- [ in ] Name of Acroform object, Name of Acroform field is used to export when the PDF document is
									submitted
}
        constructor Create(Page : TPDFPage; Box : TPDFRect; Name : AnsiString);
{
	Description
		Appends items to ListBox or to ComboBox.
	Parameters
		Item :				[in] Item which need append to list.
	Returns
		None.
}
        procedure AppendItem (Item : AnsiString);
    end;


{
    Description:
        Checks PDF string is unicode or ansi string.
    Parameters:
        A: [in] PDF string which need check
    Returns:
        True for unicode string, false for ansi string.
}
    function PDFStringIsUnicode( A: PDFString ): Boolean;
{
    Description:
        Converts PDF string to wide string.
    Parameters:
        A: [in] PDF string which need convert.
    Returns:
        Unicode value of the PDF string.
    Note.
        Before call this function need check PDF string is Unicode or not with PDFStringIsUnicode function.
}
    function PDFStringToUnicode ( A: PDFString ): WideString;
{
    Description:
        Converts PDF string to ansi string.
    Parameters:
        A: [in] PDF string which need convert.
    Returns:
        A ansi value of the PDF string.
    Note.
        Before call this function need check PDF string is Unicode or not with PDFStringIsUnicode function.
}
    function PDFStringToString ( A: PDFString ): AnsiString;
{
    Description:
        Creates a TPDFColor structure which will invisible for acroform objects.
    Returns:
        The return value is the invisible color.
}

    function NoColor : TPDFColor;
{
	Description:
		Creates a PDFRGB structure from triple of the values.
	Parameters:
		R   - [in] Specifies the intensity of the red color.
		G   - [in] Specifies the intensity of the green color.
		B   - [in] Specifies the intensity of the blue color.
	Returns:
		The return value is the resultant RGB color.
	Remarks
		The intensity for each argument is in the range 0 through 1.
		If all three intensities are zero, the result is black.
		If all three intensities are 1, the result is white.
}
    function ToRGB (R : Double; G : Double; B : Double) : TPDFRGB;
{
	Description:
		Creates a PDFColor structure from intensity of the CMYK.
	Parameters:
		C   - [in] Specifies the intensity of the cyan color.
		M   - [in] Specifies the intensity of the magenta color.
		Y   - [in] Specifies the intensity of the yellow color.
		K   - [in] Specifies the intensity of the black color.
	Returns:
		The return value is the resultant CMYK color.
}
    function CMYKToColor (C : Double; M : Double; Y : Double; K : Double) : TPDFColor;
{
	Description:
		Creates a PDFColor structure from intensity of the gray.
	Parameters:
		G   - [in] Specifies the intensity of the gray color.
	Returns:
		The return value is the resultant gray color.
	Remarks
		The intensity for argument is in the range 0 through 1.
		If intensity are zero, the result is black.
		If intensity are 1, the result is white.
}
    function GrayToColor (G : Double) : TPDFColor;
{
	Description:
		Creates a PDFColor structure from triple of the values.
	Parameters:
		R   - [in] Specifies the intensity of the red color.
		G   - [in] Specifies the intensity of the green color.
		B   - [in] Specifies the intensity of the blue color.
	Returns: 
		The return value is the resultant RGB color. 
	Remarks
		The intensity for each argument is in the range 0 through 1. 
		If all three intensities are zero, the result is black. 
		If all three intensities are 1, the result is white.
}
    function RGBToColor (R : Double; G : Double; B : Double) : TPDFColor;

function FitBToDest (Page : Integer) : TPDFExplicitDestination;
function FitRToDest (Page : Integer; Left : Double; Bottom : Double; Right : Double; Top : Double) : TPDFExplicitDestination;
function FitBVToDest (Page : Integer; Left : Double) : TPDFExplicitDestination;
function FitBHToDest (Page : Integer; Top : Double) : TPDFExplicitDestination;
function FitVToDest (Page : Integer; Left : Double) : TPDFExplicitDestination;
function XYZToDest (Page : Integer; Left : Double; Top : Double; Zoom : Double) : TPDFExplicitDestination;
function FitHToDest (Page : Integer; Top : Double) : TPDFExplicitDestination;
function FitToDest (Page : Integer) : TPDFExplicitDestination;







implementation

uses VersyJPEG, VersyTiff;


function SetPosition (Lib:PDFLibHandle; AStream:TStream; Position: Cardinal): Cardinal; cdecl;
var
    Err:Boolean;
begin
    Err := False;
    Result := 0;
    try
        AStream.Position := Position;
        Result := AStream.Position;
    except
        on Exception do
        begin
          Err := True;
        end;
    end;
    if Err then
        PPCallException ( Lib, 22 );
end;

function GetPosition (Lib:PDFLibHandle; AStream:TStream ):Cardinal; cdecl;
var
    Err:Boolean;
begin
    Err := False;
    Result := 0;
    try
        Result := AStream.Position;
    except
        on Exception do
        begin
          Err := True;
        end;
    end;
    if Err then
        PPCallException ( Lib, 22 );
end;

function GetSize ( Lib:PDFLibHandle; AStream: TStream ):Cardinal;cdecl;
var
    Err:Boolean;
begin
    Err := False;
    Result := 0;
    try
        Result := AStream.Size;
    except
        on Exception do
        begin
          Err := True;
        end;
    end;
    if Err then
        PPCallException ( Lib, 22 );
end;

function ReadBuffer ( Lib:PDFLibHandle; AStream:TStream; Buffer:Pointer;Size: Cardinal ):Cardinal;cdecl;
var
    Err:Boolean;
begin
    Err := False;
    Result := 0;
    try
        Result := AStream.read( Buffer^, Size );
    except
        on Exception do
        begin
          Err := True;
        end;
    end;
    if Err then
        PPCallException ( Lib, 22 );
end;

function WriteBuffer ( Lib:PDFLibHandle; AStream: TStream;  Buffer:Pointer;Size: Cardinal ):Cardinal;cdecl;
var
    Err:Boolean;
begin
    Err := False;
    Result := 0;
    try
        Result := AStream.Write( Buffer^, Size );
    except
        on Exception do
        begin
          Err := True;
        end;
    end;
    if Err then
        PPCallException ( Lib, 22 );
end;

function GetChar ( Lib:PDFLibHandle; AStream:TStream):Integer;cdecl;
var
    Ch:Byte;
    I:Integer;
    Err:Boolean;
begin
    Err := False;
    Result := 0;
    try
        I := AStream.read( Ch, 1 );
        if I = 0 then
            Result := -1
        else
            Result := Ch;
    except
        on Exception do
        begin
          Err := True;
        end;
    end;
    if Err then
        PPCallException ( Lib, 22 );
end;

function LookChar ( Lib:PDFLibHandle; AStream:TStream):Integer;cdecl;
var
    Ch:Byte;
    I:Integer;
    Position: Cardinal;
    Err:Boolean;
begin
    Err := False;
    Result := 0;
    try
        Position := AStream.Position;
        I := AStream.read( Ch, 1 );
        AStream.Position := Position;
        if I = 0 then
            Result := -1
        else
            Result := Ch;
    except
        on Exception do
        begin
          Err := True;
        end;
    end;
    if Err then
        PPCallException ( Lib, 22 );
end;



function FitBHToDest(Page: Integer;
  Top: Double): TPDFExplicitDestination;
begin
    Result.ExType := vetFitBH;
    Result.Page := Page;
    Result.a := Top;
end;

function FitBToDest(
  Page: Integer): TPDFExplicitDestination;
begin
    Result.ExType := vetFitB;
    Result.Page := Page;
end;

function FitBVToDest(Page: Integer;
  Left: Double): TPDFExplicitDestination;
begin
    Result.ExType := vetFitBV;
    Result.Page := Page;
    Result.a := Left;
end;

function FitHToDest(Page: Integer;
  Top: Double): TPDFExplicitDestination;
begin
    Result.ExType := vetFitH;
    Result.Page := Page;
    Result.a := Top;
end;

function FitRToDest(Page: Integer; Left, Bottom,
  Right, Top: Double): TPDFExplicitDestination;
begin
    Result.ExType := vetFitR;
    Result.Page := Page;
    Result.a := Left;
    Result.b := Bottom;
    Result.c := Right;
    Result.d := Top;
end;

function FitToDest(
  Page: Integer): TPDFExplicitDestination;
begin
    Result.ExType := vetFit;
    Result.Page := Page;
end;

function FitVToDest(Page: Integer;
  Left: Double): TPDFExplicitDestination;
begin
    Result.ExType := vetFitV;
    Result.Page := Page;
    Result.a := Left;
end;

function XYZToDest(Page: Integer; Left, Top,
  Zoom: Double): TPDFExplicitDestination;
begin
    Result.ExType := vetXYZ;
    Result.Page := Page;
    Result.a := Left;
    Result.b := Top;
    Result.c := Zoom;
end;

function CosHandleToCosObject ( Handle: PDFCosHandle ):TPDFCosObject;
var
 CT: Integer;
begin
    CT := CosGetType( Handle );
    case CT of
        2: Result := TPDFCosNull.Create( Handle );
        4: Result := TPDFCosInteger.Create( Handle );
        8: Result := TPDFCosReal.Create( Handle );
        16: Result := TPDFCosBoolean.Create( Handle );
        32: Result := TPDFCosName.Create( Handle );
        64: Result := TPDFCosString.Create ( Handle );
        128: Result := TPDFCosDictionary.Create( Handle );
        256: Result := TPDFCosArray.Create( Handle );
        512: Result := TPDFCosStream.Create( Handle );
    else
        Result := TPDFCosNull.Create( Handle );
    end;
end;

function NoColor : TPDFColor;
begin
    Result.Space := vcsGray;
    Result.CompOne := $DAB;
end;

function ToRGB (R : Double; G : Double; B : Double) : TPDFRGB;
begin
    Result.R := R;
    Result.G := G;
    Result.B := B;
end;

function CMYKToColor (C : Double; M : Double; Y : Double; K : Double) : TPDFColor;
begin
    Result.Space:= vcsCMYK;
    Result.CompOne := C;
    Result.CompTwo := M;
    Result.CompThree := Y;
    Result.CompFour := K;
end;
function GrayToColor (G : Double) : TPDFColor;
begin
    Result.Space := vcsGray;
    Result.CompOne := G;
end;
function RGBToColor (R : Double; G : Double; B : Double) : TPDFColor;
begin
    Result.Space:= vcsRGB;
    Result.CompOne := R;
    Result.CompTwo := G;
    Result.CompThree :=B;
end;

function PCharToPDFString( C:PAnsiChar; Len: Integer ):PDFString;
var
    ps:PDFString;
begin
    if (Len = 0) or (C = nil)  then
    begin
        ps := nil;
        Result := ps;
        Exit;
    end;
    SetLength ( ps, Len );
    Move ( C^,ps[0], Len );
    Result:= ps;
end;

function PDFStringIsUnicode( A: PDFString ): Boolean;
begin
  if Length ( A ) < 2 then
  begin
    Result := False;
    Exit;
  end;
  if ( A[0] = $FE ) and ( A [1] = $FF ) then
    Result := True
  else
    Result := False;
end;

function PDFStringToUnicode ( A: PDFString ): WideString;
var
   Tmp:WideString;
   i :Integer;
begin
    i := Length ( A );
    if i < 2 then
        raise EPDFException.Create('Is not Unicode string');
    if ( A[0] <>  $FE ) or ( A [1] <>  $FF ) then
        raise EPDFException.Create('Is not Unicode string');
    if i = 2 then
    begin
        Tmp := '';
        Exit;
    end;
    SetLength ( Tmp, ( i shr 1 ) - 1 );
    Move( A[2], Tmp[1], i - 2 );
    Result := Tmp;
end;

function PDFStringToString ( A: PDFString ): AnsiString;
var
    Str:AnsiString;
    I:Integer;
begin
    I:= Length ( A );
    if I = 0 then
    begin
        Result :='';
        Exit;;
    end;
    if ( (I >= 2 ) and ( A[0] = $FE ) and ( A[1] = $FF ) ) then
    begin
        raise EPDFException.Create('Is Unicode string');
    end;
    SetLength ( Str, I );
    Move( A[0], Str[1], I );
    Result := Str;
end;

function CharSetToCodePage ( Charset: Byte ): Integer;
begin
  if Charset = Default_Charset then
    Charset := GetDefFontCharSet;
  case Charset of
    ANSI_CHARSET: Result := 1252;
    RUSSIAN_CHARSET: Result := 1251;
    TURKISH_CHARSET: Result := 1254;
    SHIFTJIS_CHARSET: Result := 932;
    HANGEUL_CHARSET: Result := 949;
    CHINESEBIG5_CHARSET: Result := 950;
    GB2312_CHARSET: Result := 936;
    JOHAB_CHARSET: Result := 1361;
    HEBREW_CHARSET: Result := 1255;
    ARABIC_CHARSET: Result := 1256;
    GREEK_CHARSET: Result := 1253;
    THAI_CHARSET: Result := 874;
    EASTEUROPE_CHARSET: Result := 1250;
    MAC_CHARSET: Result := 10000;
    BALTIC_CHARSET: Result := 1257;
    VIETNAMESE_CHARSET: Result := 1258;
  else
    Result := 1252;
  end;
end;

function StringToPDFString( Str: AnsiString; Charset: TFontCharset): PDFString;
var
  A: PDFString;
  CodePage : Integer;
  I: Integer;
  Len: Integer;
  W: PWideChar;
  B: Array of word;
begin
    CodePage := CharSetToCodePage( Charset );
    Len := Length ( Str );
    i := MultiByteToWideChar ( CodePage, 0, PAnsiChar ( Str ), Len , nil, 0 );
    if i = 0 then
        raise EPDFException.Create('Cannot convert to unicode');
    SetLength ( A, (i + 1)  shl 1 );
    SetLength ( B, I );
    W := @B[0];
    if MultiByteToWideChar ( CodePage, 0, PAnsiChar ( Str ), Len , W, i ) = 0 then
        raise EPDFException.Create('Cannot convert to unicode');
    move ( B[0], A[2], I shl 1 );
    A[0] := $FF;
    A[1] := $FE;
    Result := A;
end;

function WideStringToPDFString ( Str: WideString ): PDFString;
var
    A: PDFString;
    I: Integer;
begin
    I :=  Length ( Str )  shl 1;
    SetLength ( A, I + 2 );
    A[0] := $FF;
    A[1] := $FE;
    Move ( Str[1], A[2], I );
    Result := A;
end;

procedure ErrorCall( Lib: PDFLibHandle; ErrCode: ppInt32; Opaque: Pointer ); cdecl;
var
  S:AnsiString;
  i: Integer;
begin
  SetLength ( S, 200);
  i:= VSGetErrorStr( ErrCode, PAnsiChar ( S ) );
  SetLength ( S, i );
  raise EPDFException.Create( String(S) );
end;


function malloc ( Count: Cardinal ):Pointer; cdecl;
begin
  Result := AllocMem(Count);
end;

function realloc ( Dest:Pointer; Count: Cardinal ):Pointer;cdecl;
begin
  ReallocMem(Dest,Count);
  Result := Dest;
end;
procedure freem ( Dest: Pointer );cdecl;
begin
  FreeMem(Dest);
end;




{ TPDFLibrary }

function TPDFLibrary.AtomToString(Atom: ppAtom): AnsiString;
var
 S:PAnsiChar;
begin
    S := ULAtomToString( FLib, Atom );
    Result := S;
end;

constructor TPDFLibrary.Create(LicenseName, LicenseKey: AnsiString);
begin
  FLib := InitPDFLibraryWithParams ( PAnsiChar ( LicenseName ), PAnsiChar ( LicenseKey ),
    nil, nil, nil, ErrorCall, nil );
  if FLib = nil then
    raise EPDFException.Create( 'Cannot start PDF Engine.' );
end;

destructor TPDFLibrary.Destroy;
begin
    DonePDFLibrary( @FLib );
    inherited;
end;

function TPDFLibrary.ExistsAtomForString(Str: AnsiString): Boolean;
begin
  Result := ULExistsAtomForString( FLib, PAnsiChar ( Str ) );
end;

function TPDFLibrary.StringToAtom(Str: AnsiString): ppAtom;
begin
    Result := ULStringToAtom ( FLib, PAnsiChar ( Str ) );
end;

{ TPDFDocument }


procedure TPDFDocument.LoadFromBuffer(Buffer: Pointer; Size: Integer);
begin
    ClearAll;
    FDoc := PDFDocLoadFromBuffer( FLibrary.Handle, Buffer, Size );
    FAcroInfo := nil;
    FOutlineRoot := nil;
    FPageCount := -1;
    FPackContent := True;
    FResolution := 72;
    FThreadCount := -1;
end;

procedure TPDFDocument.LoadFromFile( FileName: AnsiString);
begin
    ClearAll;
    FDoc := PDFDocLoadFromFile( FLibrary.Handle, PAnsiChar( FileName ) );
    FAcroInfo := nil;
    FOutlineRoot := nil;
    FPageCount := -1;
    FPackContent := True;
    FResolution := 72;
    FThreadCount := -1;
end;



procedure TPDFDocument.AppendNamedDestination(Destination: TPDFExplicitDestination; Name: AnsiString; Charset: TFontCharset);
var
    ps: PDFString;
    Dest: TPDFExplicitDest;
begin
    ps:=nil;
    Move( Destination, Dest, SizeOf ( Dest ) );
    Dest.ExType := Integer ( Destination.ExType );
    if (Name = '') or (Charset = 0 ) or ( ( Charset = DEFAULT_CHARSET ) and ( GetDefFontCharSet = 0 ) ) then
    begin
      PDFDestinationNamedNewM ( FDoc, PAnsiChar ( Name), Length ( Name ), @Dest );
    end else
    begin
        ps := StringToPDFString( Name, Charset );
        PDFDestinationNamedNewM ( FDoc, PAnsiChar ( @ps[0]), Length ( ps ), @Dest );
    end;
end;

function TPDFDocument.AppendPage(Size: TPDFPageSize;
  Orientation: TPDFPageOrientation): Cardinal;
begin
    if FPageCount = -1 then
        GetPageCount;
    Inc ( FPageCount );
    Result := PDFDocAppendPage2( FDoc, Ord ( Size ) , Ord ( Orientation ) );
end;

function TPDFDocument.AppendPage(Width, Height: Double): Cardinal;
begin
    if FPageCount = -1 then
        GetPageCount;
    Inc ( FPageCount );
    Result := PDFDocAppendPage( FDoc, Width, Height );
end;


procedure TPDFDocument.AppendSignature(FileName, Name, Reason: AnsiString;
  PKCS7: Boolean; Password: AnsiString);
begin
    PDFDocAppendSignatureFromFile ( FDoc, PAnsiChar ( FileName ), PAnsiChar ( Name ), PAnsiChar ( Reason ), PKCS7, PAnsiChar ( Password ) );
end;

function TPDFDocument.CheckPassword(Password: AnsiString): TKeyValidType;
begin
    Result:= TKeyValidType ( PDFDocCheckPassword( FDoc, PAnsiChar ( Password ) ) );
end;

procedure TPDFDocument.LoadFromStream(AStream: TStream);
var
    S: PDFStreamHandle;
begin
    ClearAll;
    S := ULStreamCustomNew ( FLibrary.Handle, @ReadBuffer, @WriteBuffer, @GetPosition,
            @SetPosition,@GetSize, @GetChar, @LookChar, AStream );
    try
        FDoc := PDFDocLoadFromStream( FLibrary.Handle, S );
    except
        on  Exception do
        begin
            ULStreamClose( S );
            raise;
        end;
    end;
    FLoadedFromStream := True;
    FStream := S;
    FAcroInfo := nil;
    FOutlineRoot := nil;
    FPageCount := -1;
    FPackContent := True;
    FResolution := 72;
    FThreadCount := -1;
end;

function TPDFDocument.GetIsCrypted: Boolean;
begin
    Result := PDFDocIsCrypted( FDoc );
end;

function TPDFDocument.GetPermission: TPDFDocumentRestrictions;
var
    I: Integer;
    P: TPDFDocumentRestrictions;
begin
    P := [];
    I := PDFDocGetPermission( FDoc );
    if I and drPrint <>0 then P := P + [ vdrPrint ];
    if I and drModifyContent <>0 then P := P + [ vdrModifyContent ];
    if I and drCopyInformation <>0 then P := P + [ vdrCopyInformation ];
    if I and drModifyAnnotation <>0 then P := P + [ vdrModifyAnnotation ];
    if I and drFillAcroForm <>0 then P := P + [ vdrFillAcroForm ];
    if I and drExtractTextAndImage <>0 then P := P + [ vdrExtractTextAndImage ];
    if I and drAssembleDocument <>0 then P := P + [ vdrAssembleDocument ] ;
    if I and drPrintHiResolution <>0 then P := P + [ vdrPrintHiResolution ];
    Result := P;
end;


constructor TPDFDocument.Create(ALibrary: TPDFLibrary);
begin
    FDoc := PDFDocCreate ( ALibrary.Handle );
    FLibrary := ALibrary;
    FAcroInfo := nil;
    FOutlineRoot := nil;
    FPageCount := -1;
    FPackContent := True;
    FResolution := 72;
    FThreadCount := -1;
    FLoadedFromStream := False;
end;



destructor TPDFDocument.Destroy;
begin
    ClearAll;
  inherited;
end;

function TPDFDocument.FontAppendStandard(Font: TPDFStdandardFont;
  Encode: TPDFEncodingType): Cardinal;
begin
    Result := PDFFontStandardAppend( FDoc, Integer( Font ), Integer( Encode ) );
end;

function TPDFDocument.FontAppendTrueType(FontName: AnsiString; Bold,
  Italic: Boolean): Cardinal;
begin
    Result := PDFFontTrueTypeAppend ( FDoc, PAnsiChar ( FontName ), Bold, Italic );
end;

function TPDFDocument.FontAppendTrueType(FontFileName: AnsiString): Cardinal;
begin
    Result := PDFFontTrueTypeAppendFromFile( FDoc, PAnsiChar ( FontFileName ) );
end;

function TPDFDocument.FontAppendTrueType(AStream: TStream): Cardinal;
var
    S: PDFStreamHandle;
begin
    S := ULStreamCustomNew ( FLibrary.Handle, @ReadBuffer, @WriteBuffer, @GetPosition,
            @SetPosition,@GetSize, @GetChar, @LookChar, AStream );
    try
            Result := PDFFontTrueTypeAppendFromStream( FDoc, S );
    finally
        ULStreamClose ( S );
    end;
end;


function TPDFDocument.GetAcroInformation: TPDFAcroInformation;
begin
    if FAcroInfo = nil then
    begin
        FAcroInfo := TPDFAcroInformation.Create( Self );
    end;
    Result := FAcroInfo;
end;


function TPDFDocument.GetDocumentInformation(
  Info: TPDFInformation): PDFString;
var
    C:PAnsiChar;
    Len: Cardinal;
begin
    C := PDFDocGetInfo( FDoc, Integer(Info), @Len );
    Result := PCharToPDFString( C, Len );
end;



function TPDFDocument.GetOutlineRoot: TPDFOutline;
    procedure FillChilds( Outline: TPDFOutline );
        var
            I, C:Integer;
            H: PDFOutlineHandle;
    begin
        C := PDFOutlineGetCount( Outline.Handle );
        if C = 0 then
            Exit;
        Outline.FCount := C;
        SetLength ( Outline.FList, Outline.FCount );
        H := PDFOutlineGetFirstChild( Outline.Handle );
        for I := 0 to C - 1 do
        begin
            Outline.FList[I] := TPDFOutline.Create( H );
            H := PDFOutlineGetNext( H );
            Outline.FList[I].FParent := Outline;
        end;
        for I := 1 to C -1 do
        begin
            Outline.FList[I].FPrev := Outline.FList[I-1];
        end;
        for I := 0 to C - 2 do
        begin
            Outline.FList[I].FNext := Outline.FList[I+1];
        end;
        for I := 0 to C - 1 do
        begin
            FillChilds ( Outline.FList[I] );
        end;
    end;
    function FillOutline:TPDFOutline;
        var
            AH:PDFOutlineHandle;
            T: TPDFOutline;
    begin
        AH := PDFDocGetOutlineRoot( FDoc );
        T:= TPDFOutline.Create( AH );
        if  CosGetType( AH ) <> 2 then
            FillChilds ( T );
        Result := T;
    end;
begin
    if FOutlineRoot = nil then
    begin
        FOutlineRoot := FillOutline ;
    end;
    Result := FOutlineRoot;
end;

function TPDFDocument.GetPageCount: Cardinal;
begin
    if FPageCount = -1 then
        FPageCount := PDFDocGetPageCount( FDoc );
    Result := FPageCount;
end;



function TPDFDocument.GetThreadCount: Cardinal;
begin
    if FThreadCount = -1 then
    begin
       FThreadCount:= PDFDocGetThreadCount( FDoc );
       SetLength ( FThreads, FThreadCount );
    end;
    Result := FThreadCount;
end;

function TPDFDocument.GetThreads(Index:Cardinal): TPDFThread;
begin
    if Index >= ThreadCount then
        raise EPDFException.Create( 'Out of bound.');
    if FThreads[Index] = nil then
    begin
        FThreads[Index] := TPDFThread.Create( PDFDocGetThread ( FDoc, index ) );
    end;
    Result := FThreads[Index];
end;

function TPDFDocument.ImageAppend(Buffer: Pointer; Size, Index: Cardinal;
  Compression: TPDFImageCompressionType): Cardinal;
begin
    Result := PDFImageAppendToDocFromBuffer( FDoc, Buffer, Size, Index, Integer ( Compression ) );
end;

function TPDFDocument.ImageAppend(AStream: TStream; Index: Cardinal;
  Compression: TPDFImageCompressionType): Cardinal;
var
    S: PDFStreamHandle;
begin
    S := ULStreamCustomNew ( FLibrary.Handle, @ReadBuffer, @WriteBuffer, @GetPosition,
            @SetPosition,@GetSize, @GetChar, @LookChar, AStream );
    try
        Result := PDFImageAppendToDocFromStream( FDoc, AStream, Index, Integer ( Compression ) );
    finally
        ULStreamClose ( S );
    end;


end;

function TPDFDocument.ImageAppend(FileName: AnsiString; Index: Cardinal;
  Compression: TPDFImageCompressionType): Cardinal;
begin
    Result := PDFImageAppendToDocFromFile( FDoc, PAnsiChar ( FileName ), Index, Integer ( Compression ) );
end;

function TPDFDocument.ImageAppend(Image: TPDFImage;
  Compression: TPDFImageCompressionType): Cardinal;
begin
        Result := PDFImageAppendToDoc( Image.Handle, FDoc, Integer ( Compression ) );
end;

function TPDFDocument.ImageAppendAsMask(Image: TPDFImage;
  Compression: TPDFImageCompressionType): Cardinal;
begin
    Result := PDFImageAppendToDocAsMask( Image.Handle, FDoc, Integer ( Compression ) );
end;

function TPDFDocument.ImageAppendWithMask(Image: TPDFImage;
  Compression: TPDFImageCompressionType; Mask: Integer): Cardinal;
begin
    Result := PDFImageAppendToDocWithMask( Image.Handle, FDoc, Integer ( Compression ), Mask );
end;

procedure TPDFDocument.SaveToStream(AStream: TStream);
var
    S: PDFStreamHandle;
begin
    BeforeSave;
    S := ULStreamCustomNew ( FLibrary.Handle, @ReadBuffer, @WriteBuffer, @GetPosition,
            @SetPosition,@GetSize, @GetChar, @LookChar, AStream );
    try
         PDFDocSaveToStream( FDoc, S );
    finally
        ULStreamClose ( S );
    end;
end;

procedure TPDFDocument.SaveToFile(FileName: AnsiString);
begin
    BeforeSave;
    PDFDocSaveToFile( FDoc, PAnsiChar ( FileName ) );
end;

procedure TPDFDocument.SetAutoLaunch(AAutoLaunch: Boolean);
begin
    PDFDocSetAutoLaunch( FDoc, AAutoLaunch );
end;

procedure TPDFDocument.SetEMFBWImagesAsJBIG2(AsJBIG2: Boolean);
begin
    PDFDocSetEMFBWImagesAsJBIG2( FDoc, AsJBIG2 );
end;

procedure TPDFDocument.SetEMFColorImagesAsJpeg(AsJpeg: Boolean);
begin
    PDFDocSetEMFColorImagesAsJpeg( FDoc, AsJpeg );
end;

procedure TPDFDocument.SetJpegImageQuality(Quality: Integer);
begin
    PDFDocSetJpegImageQuality( FDoc, Quality );
end;

procedure TPDFDocument.SetLinearized(ALinearized: Boolean);
begin
    PDFDocSetLinearized( FDoc, ALinearized);
end;

procedure TPDFDocument.SetPacked(IsPacked: Boolean);
begin
    PDFDocSetPacked( FDoc, IsPacked );
end;

procedure TPDFDocument.SetRemoveUnUsed(ARemoveUnUsed: Boolean);
begin
    PDFDocSetRemoveUnUsed( FDoc, ARemoveUnUsed );
end;

procedure TPDFDocument.SetSecurity( Permission: TPDFDocumentRestrictions; KeyLength: TPDFProtectionType;
  UserPassword, OwnerPassword: AnsiString);
var
    I: Integer;
begin
    I := drAllDisabled;
    if vdrPrint in Permission then I := I or drPrint;
    if vdrModifyContent in Permission then I := I or drModifyContent;
    if vdrCopyInformation in Permission then I := I or drCopyInformation;
    if vdrModifyAnnotation in Permission then I := I or drModifyAnnotation;
    if vdrFillAcroForm in Permission then I := I or drFillAcroForm;
    if vdrExtractTextAndImage in Permission then I := I or drExtractTextAndImage;
    if vdrAssembleDocument in Permission then I := I or drAssembleDocument;
    if vdrPrintHiResolution in Permission then I := I or drPrintHiResolution;
    PDFDocSetSecurity( FDoc, I, Integer ( KeyLength ), PAnsiChar ( UserPassword ), PAnsiChar ( OwnerPassword ) );
end;

procedure TPDFDocument.RemoveSecurity;
begin
  PDFDocRemoveSecurity( FDoc);
end;
procedure TPDFDocument.SetEncryptMetaData ( IsEncrypt: Boolean );
begin
  PDFDocSetEncryptMetaData( FDoc, IsEncrypt );
end;


procedure TPDFDocument.SetUsedDC(DC: HDC);
begin
    PDFDocSetUsedDC( FDoc, DC );
end;

procedure TPDFDocument.SetDocumentInformation(Info: TPDFInformation;
  Value: AnsiString; CharSet: TFontCharset);
var
    ps: PDFString;
begin
    ps :=nil;
    if (Value = '') or (Charset = 0 ) or ( ( Charset = DEFAULT_CHARSET ) and ( GetDefFontCharSet = 0 ) ) then
    begin
      PDFDocSetInfo( FDoc, Integer ( Info ), PAnsiChar ( Value ), Length ( Value ) );
    end else
    begin
        ps := StringToPDFString( Value, Charset );
        PDFDocSetInfo( FDoc, Integer ( Info ), PAnsiChar ( @ps[0] ), Length ( ps ) );
    end;
end;

procedure TPDFDocument.SetDocumentInformation(Info: TPDFInformation; Value: WideString);
var
    ps: PDFString;
begin
    ps := WideStringToPDFString( Value );
    PDFDocSetInfo( FDoc, Integer ( Info ), PAnsiChar ( @ps[0] ), Length ( ps ) );
end;

procedure TPDFDocument.AppendSignature(Buffer: Pointer; Size: Integer;
  Name, Reason: AnsiString; PKCS7: Boolean; Password: AnsiString);
begin
    PDFDocAppendSignatureFromBuffer ( FDoc, Buffer, Size, PAnsiChar ( Name ), PAnsiChar ( Reason ), PKCS7, PAnsiChar ( Password ) );
end;

procedure TPDFDocument.AppendSignature(AStream: TStream; Name,
  Reason: AnsiString; PKCS7: Boolean; Password: AnsiString);
var
    S: PDFStreamHandle;
begin
    S := ULStreamCustomNew ( FLibrary.Handle, @ReadBuffer, @WriteBuffer, @GetPosition,
            @SetPosition,@GetSize, @GetChar, @LookChar, AStream );
    try
        PDFDocAppendSignatureFromStream ( FDoc, S, PAnsiChar ( Name ), PAnsiChar ( Reason ), PKCS7, PAnsiChar ( Password ) );
    finally
        ULStreamClose ( S );
    end;

end;



procedure TPDFDocument.AppendNamedDestination(
  Destination: TPDFExplicitDestination; Name: WideString);
var
    ps: PDFString;
    Dest: TPDFExplicitDest;
begin
    Move( Destination, Dest, SizeOf ( Dest ) );
    Dest.ExType := Integer ( Destination.ExType );
    ps := WideStringToPDFString( Name );
    PDFDestinationNamedNewM ( FDoc, PAnsiChar ( @ps[0]), Length ( ps ), @Dest );
end;

function TPDFDocument.FontAppendType1(AStream: TStream): Cardinal;
var
    S: PDFStreamHandle;
begin
    S := ULStreamCustomNew ( FLibrary.Handle, @ReadBuffer, @WriteBuffer, @GetPosition,
            @SetPosition,@GetSize, @GetChar, @LookChar, AStream );
    try
        Result := PDFFontType1AppendFromStream( FDoc, S );
    finally
        ULStreamClose ( S );
    end;
end;

function TPDFDocument.FontAppendType1(FontFileName: AnsiString): Cardinal;
begin
    Result := PDFFontType1AppendFromFile( FDoc, PAnsiChar ( FontFileName ) );
end;

function TPDFDocument.GetPages(Index: Cardinal): TPDFPage;
begin
    if Index >= PageCount then
        raise EPDFException.Create('Out of bound.');
    if Index >= Cardinal ( Length ( FPages ) ) then
        SetLength ( FPages, Index + 1 );
    if FPages[Index] = nil then
    begin
        FPages[Index] := TPDFPage.Create ( Self, Index );
    end;
    Result := FPages [ Index ];
end;

procedure TPDFDocument.BeforeSave;
var
    I: Integer;
begin
    for I:= 0 to Length ( FPages ) - 1  do
        if FPages[ I ] <> nil then
            FPages[I].CloseContents;
end;

procedure TPDFDocument.ClearAll;
    procedure DestroyOutlines ( Outline: TPDFOutline );
    var
        I: Integer;
    begin
        for I:= 0 to Outline.FCount-1 do
            DestroyOutlines( Outline.Childs[I]);
        Outline.Free;
    end;
var
    I: Integer;
begin
  PDFDocClose( FDoc );
  if FLoadedFromStream  then
    ULStreamClose ( FStream );
  FLoadedFromStream := False;

  if FAcroInfo <> nil then
  begin
        FAcroInfo.Free;
        FAcroInfo := nil;
  end;

  if FOutlineRoot <> nil then
  begin
        DestroyOutlines( FOutlineRoot);
        FOutlineRoot := nil;
  end;
  for I := 0 to Length ( FPages) - 1 do
    if FPages[I] <> nil then
        FPages[I].Free;
  FPages := nil;
  for I := 0 to Length ( FThreads) -1 do
    if FThreads[I] <> nil then
        FThreads[I].Free;
  FThreads := nil;
end;

function TPDFDocument.ThreadAppend: TPDFThread;
var
    H: PDFThreadHandle;
begin
    ThreadCount;
    Inc ( FThreadCount );
    SetLength ( FThreads, FThreadCount );
    H := PDFThreadNew( FDoc );
    FThreads[FThreadCount - 1 ] := TPDFThread.Create( H );
    Result := FThreads[FThreadCount - 1 ];
end;

function TPDFDocument.CreateDestination(IsIndirect: Boolean; Str: AnsiString;
  Charset: TFontCharset): TPDFDestination;
var
    ps: PDFString;
begin
    ps :=nil;
    if ( Str = '') or (Charset = 0 ) or ( ( Charset = DEFAULT_CHARSET ) and ( GetDefFontCharSet = 0 ) ) then
    begin
        Result :=  TPDFDestination ( PDFDestinationFromString(  FDoc, PAnsiChar ( Str ), Length ( Str ), IsIndirect ) );
    end else
    begin
        ps := StringToPDFString( Str, Charset );
        Result := TPDFDestination ( PDFDestinationFromString(  FDoc, PAnsiChar ( @ps[0] ), Length ( ps ), IsIndirect ) );
    end;
end;

function TPDFDocument.CreateDestination(IsIndirect: Boolean;
  Str: WideString): TPDFDestination;
var
    ps: PDFString;
begin
    ps := WideStringToPDFString( Str );
    Result := TPDFDestination ( PDFDestinationFromString(  FDoc, PAnsiChar ( @ps[0] ), Length ( ps ), IsIndirect ) );
end;

function TPDFDocument.CreateDestination(IsIndirect: Boolean; Destination:
    TPDFExplicitDestination; IsOtherDocument: Boolean): TPDFDestination;
var
    Dest: TPDFExplicitDest;
begin
    Move( Destination, Dest, SizeOf ( Dest ) );
    Dest.ExType := Integer ( Destination.ExType );
    Result := TPDFDestination ( PDFDestinationFromExplitM( FDoc, @Dest, IsOtherDocument, IsIndirect ) );
end;


procedure TPDFDocument.Clear;
begin
    ClearAll;
    FDoc := PDFDocCreate( FLibrary.Handle );

    FAcroInfo := nil;

    FOutlineRoot := nil;
    FPageCount := -1;
    FPackContent := True;
    FResolution := 72;
    FThreadCount := -1;
    FLoadedFromStream := False;
end;

function TPDFDocument.GetVersion: TPDFVersion;
begin
     Result := TPDFVersion ( PDFDocGetVersion( FDoc ) );
end;

procedure TPDFDocument.SetVersion(const Value: TPDFVersion);
begin
    PDFDocSetVersion( FDoc, Ord ( Value )  );
end;

function TPDFDocument.PlaceSignature(Page: TPDFPage; Box: TPDFRect;
  Resolution: Cardinal): TPDFPaintBox;
var
  PBX:PBXHandle;
begin
  PBX := PDFPagePlaceSignatureM( Page.Parent, Page.PIndex, @Box, Resolution);
  Result := TPDFPaintBox.Create( nil, PBX);
end;

{ TPDFStream }

constructor TPDFStream.Create(AStream: PDFStreamHandle);
begin
    FStream := AStream;
end;


function TPDFStream.Read(var Buffer; Count: Integer): Longint;
begin
    Result := ULStreamReadBuffer( FStream, Buffer, Count );
end;

function TPDFStream.Seek(const Offset: Int64;
  Origin: TSeekOrigin): Int64;
var
  Pos:Integer;
begin
  if ( Offset = 0 ) and ( Origin = soCurrent ) then
  begin
    Result := ULStreamGetPosition( FStream );
    exit;
  End;
  case Origin of
        soBeginning:
            Pos:= Offset;
        soCurrent:
            Pos:= Offset + ULStreamGetPosition( FStream );
        soEnd:
            Pos := Offset + ULStreamGetSize( FStream );
        else
            Pos := Offset;
  end;
  Result:=ULStreamSetPosition( FStream, Pos );
end;

procedure TPDFStream.SetSize(NewSize: Integer);
begin
    if FIsReadStream then
        raise EPDFException.Create('Cannot change size of readonly stream');
    if FIsReadStream then
        raise EPDFException.Create('Cannot change size of readonly stream');
    ULStreamSetSize(FStream, NewSize );
end;

procedure TPDFStream.SetSize(const NewSize: Int64);
begin
if FIsReadStream then
        raise EPDFException.Create('Cannot change size of readonly stream');
    if FIsReadStream then
        raise EPDFException.Create('Cannot change size of readonly stream');
    ULStreamSetSize(FStream, NewSize );
end;

function TPDFStream.Write(const Buffer; Count: Integer): Longint;
begin
    Result := ULStreamWriteBuffer( FStream, Buffer, Count );
end;


{ TPDFImage }


function TPDFImage.GetColorSpace: TPDFColorSpace;
var
    I:integer;
begin
    I := PDFImageGetColorDevice( FImage );
    if i = 1 then
        Result := vcsGray
    else if i = 3 then
        Result := vcsRGB
    else
        Result := vcsCMYK;
end;

function TPDFImage.GetDepth: TPDFComponentDepth;
var
    I: Integer;
begin
    I := PDFImageGetDepth( FImage );
    case I of
        1: Result := vcd1bit;
        2: Result := vcd2bit;
        4: Result := vcd4bit;
        8: Result := vcd8bit;
    else
        Result := vcd16bit;
    end;
end;

function TPDFImage.GetHeight: Cardinal;
begin
    Result := PDFImageGetHeight( FImage );
end;

function TPDFImage.GetScanLine(Row: Cardinal): Pointer;
begin
    Result := PDFImageGetScanLine( FImage, Row );
end;

class function TPDFImage.GetTIFFImagesCount(ALibrary: TPDFLibrary;
  Buffer: Pointer; Size: Cardinal): Cardinal;
begin
    Result := PDFImageGetTIFFCountFromBuffer( ALibrary.Handle, Buffer, Size );
end;

class function TPDFImage.GetTIFFImagesCount(ALibrary: TPDFLibrary;
  FileName: AnsiString): Cardinal;
begin
    Result := PDFImageGetTIFFCountFromFile( ALibrary.Handle, PAnsiChar ( FileName ) );
end;

class function TPDFImage.GetTIFFImagesCount(ALibrary: TPDFLibrary;
  AStream: TStream): Cardinal;
var
    S: PDFStreamHandle;
begin
    S := ULStreamCustomNew ( ALibrary.Handle, @ReadBuffer, @WriteBuffer, @GetPosition,
            @SetPosition,@GetSize, @GetChar, @LookChar, AStream );
    try
        Result := PDFImageGetTIFFCountFromStream( ALibrary.Handle, S );
    finally
        ULStreamClose ( S );
    end;
end;

function TPDFImage.GetWidth: Cardinal;
begin
    Result := PDFImageGetWidth ( FImage );
end;


constructor TPDFImage.CreateFromHandle(ALibrary: TPDFLibrary; BitmapHandle: HBITMAP);
begin
    FImage := PDFImageLoadFromHandle( ALibrary.Handle, BitmapHandle );
end;

constructor TPDFImage.CreateFromBuffer(ALibrary: TPDFLibrary; Buffer: Pointer;
  Size, Index: Cardinal);
begin
    FImage := PDFImageLoadFromBuffer( ALibrary.Handle, Buffer, Size, Index );
end;

constructor TPDFImage.CreateFromStream(ALibrary: TPDFLibrary; AStream: TStream;
  Index: Cardinal);
var
    S: PDFStreamHandle;
begin
    S := ULStreamCustomNew ( ALibrary.Handle, @ReadBuffer, @WriteBuffer, @GetPosition,
            @SetPosition,@GetSize, @GetChar, @LookChar, AStream );
    try
            FImage := PDFImageLoadFromStream( ALibrary.Handle, S, Index );
    finally
        ULStreamClose ( S );
    end;

end;

constructor TPDFImage.Create(ALibrary: TPDFLibrary; Width,
  Height: Cardinal; Depth: TPDFComponentDepth; Space: TPDFColorSpace);
var
    cd, cs: Integer;
begin
    case Space of
        vcsGray: cs := 1;
        vcsRGB: cs := 3;
        else
            cs:= 4;
    end;
    cd := 1 shl Ord ( Depth );
    FImage := PDFImageCreate ( ALibrary.Handle, Width, Height, cd,cs );
end;

constructor TPDFImage.CreateFromFile(ALibrary: TPDFLibrary; FileName: AnsiString; Index: Cardinal);
begin
    FImage := PDFImageLoadFromFile( Alibrary.Handle, PAnsiChar ( FileName ), Index );
end;

destructor TPDFImage.Destroy;
begin
    PDFImageFree( FImage );
    inherited;
end;

{ TPDFOutline }

function TPDFOutline.AddNewChild: TPDFOutline;
var
    T,N:TPDFOutline;
begin
    T := GetLastChild;
    Inc ( FCount );
    SetLength ( FList, FCount );
    N := TPDFOutline.Create( PDFOutlineAddNewChild( FOutline ) );
    N.FParent := Self;
    if T <> nil then
    begin
        N.FPrev := T;
        T.FNext := N;
    end;
    N.FIndex := FCount - 1;
    FList [ FCount -1 ] := N;
    Result:= N;
end;

function TPDFOutline.AddNewNext: TPDFOutline;
var
    I, C :Integer;
    T,N:TPDFOutline;
begin
    if FParent = nil then
        raise EPDFException.Create('Cannot create sibling for root outline');
    N := TPDFOutline.Create( PDFOutlineAddNewNext( FOutline ) );
    N.FParent := FParent;
    T := GetNext;
    FNext := N;
    N.FPrev := Self;
    if T <> nil then
    begin
      N.FNext := T;
      T.FPrev := N;
    end;
    C := -1;
    for I:= 0 to FParent.FCount-1 do
    begin
        if FParent.FList[I] = Self then
        begin
            C := I;
            Break;
        end;
    end;
    if C = -1 then
        raise EPDFException.Create('Cannot calculate index of the outline');
    Inc ( FParent.FCount );
    SetLength ( FParent.FList, FParent.FCount );
    for I:= FParent.FCount - 1 downto C + 2 do
        FParent.FList[I] := FParent.FList[I-1];
    FParent.FList[C+1] := N;
    Result:= N;
end;

function TPDFOutline.AddNewPrev: TPDFOutline;
var
    I, C :Integer;
    T,N:TPDFOutline;
begin
    if FParent = nil then
        raise EPDFException.Create('Cannot create sibling for root outline');
    N := TPDFOutline.Create( PDFOutlineAddNewPrev( FOutline ) );
    N.FParent := FParent;
    T := GetPrev;
    FPrev := N;
    N.FNext := Self;
    if T <> nil then
    begin
      N.FPrev := T;
      T.FNext := N;
    end;
    C := -1;
    for I:= 0 to FParent.FCount-1 do
    begin
        if FParent.FList[I] = Self then
        begin
            C := I;
            Break;
        end;
    end;
    if C = -1 then
        raise EPDFException.Create('Cannot calculate index of the outline');
    Inc ( FParent.FCount );
    SetLength ( FParent.FList, FParent.FCount );
    for I:= FParent.FCount - 1 downto C + 1 do
        FParent.FList[I] := FParent.FList[I-1];
    FParent.FList[C] := N;
    Result:= N;
end;

function TPDFOutline.AddNewSibling: TPDFOutline;
begin
    if FParent = nil then
        raise EPDFException.Create('Cannot create sibling for root outline');
    Result:= FParent.AddNewChild;
end;

constructor TPDFOutline.Create(AHandle: PDFOutlineHandle);
begin
   FOutline := AHandle;
   FParent := nil;
   FCount := 0;
   FList := nil;
   FNext := nil;
   FPrev := nil;
   FIndex := 0;
end;

function TPDFOutline.GetChild(Index: Cardinal): TPDFOutline;
begin
  if (FList<>nil) and ( FCount >Index ) then
        Result := FList[Index]
    else
        Result := nil;
end;

function TPDFOutline.GetCount: Integer;
begin
    Result := FCount;
end;

function TPDFOutline.GetFirstChild: TPDFOutline;
begin
    if (FList<>nil) and ( FCount <>0 ) then
        Result := FList[0]
    else
        Result := nil;
end;

function TPDFOutline.GetHasChildren: Boolean;
begin
    Result := (FList<>nil) and ( FCount <>0 );
end;

function TPDFOutline.GetLastChild: TPDFOutline;
begin
    if (FList<>nil) and ( FCount <>0 ) then
        Result := FList[FCount - 1]
    else
        Result := nil;
end;

function TPDFOutline.GetNext: TPDFOutline;
begin
    Result:= FNext;
end;

function TPDFOutline.GetParent: TPDFOutline;
begin
    Result:= FParent;
end;

function TPDFOutline.GetPrev: TPDFOutline;
begin
    Result:= FPrev;
end;

procedure TPDFOutline.SetAction(Value: TPDFAction);
begin
    PDFOutlineSetAction( FOutline, Value.Handle );
end;

procedure TPDFOutline.SetColor(Value: TPDFRGB);
var
    C: PDFColor;
begin
    C.Device := 3;
    c.Comp1 := Value.R;
    c.Comp2 := Value.G;
    c.Comp3 := Value.B;
    PDFOutlineSetColor( FOutline, @C);
end;

procedure TPDFOutline.SetDestination(Value: TPDFDestination);
begin
    PDFOutlineSetDestination( FOutline, Pointer ( Value ) );
end;

procedure TPDFOutline.SetExpanded(Value: Boolean);
begin
    PDFOutlineSetExpanded( FOutline, Value );
end;

procedure TPDFOutline.SetFlags(Value: Integer);
begin
    PDFOutlineSetFlags( FOutline, Value);
end;

procedure TPDFOutline.SetTitle(Title: AnsiString;Charset:TFontCharset = 0 );
var
    ps: PDFString;
begin
    ps :=nil;
    if (Title = '') or (Charset = 0 ) or ( ( Charset = DEFAULT_CHARSET ) and ( GetDefFontCharSet = 0 ) ) then
    begin
      PDFOutlineSetTitle( FOutline, PAnsiChar ( Title ), Length ( Title ) );
    end else
    begin
        ps := StringToPDFString( Title, Charset );
        PDFOutlineSetTitle( FOutline, PAnsiChar ( @ps[0] ), Length ( ps ) );
    end;
end;

procedure TPDFOutline.SetTitle(Title: WideString);
var
    ps: PDFString;
begin
    ps := WideStringToPDFString( Title );
    PDFOutlineSetTitle( FOutline, PAnsiChar ( @ps[0] ), Length ( ps ) );
end;

{ TPDFAction }

procedure TPDFAction.AppendNextAction(AAction: TPDFAction);
begin
    PDFActionSetNext( FAction, AAction.Handle );
end;

constructor TPDFAction.Create(AHandle: PDFActionHandle);
begin
  FAction := AHandle;
end;



{ TPDFAcroInformation }

constructor TPDFAcroInformation.Create(ADocument: TPDFDocument);
begin
    FDocument := ADocument;
end;

function TPDFAcroInformation.GetCount: Cardinal;
begin
    Result := PDFAcroGetCount( FDocument.Handle );
end;

function TPDFAcroInformation.GetNameByIndex (Index: Cardinal): PDFString;
var
    C:Pointer;
    Len:Cardinal;
    IsUnicode: Boolean;
    ps : PDFString;
begin
  C := PDFAcroGetNameByIndex( FDocument.Handle, Index, @IsUnicode, @Len);
  if Len = 0 then
    begin
        ps := nil;
        Result := ps;
        Exit;
    end;
  if IsUnicode then
  begin
    SetLength(ps, (Len+1) shl 1);
    ps[0] := $FE;
    ps[1] := $FF;
    Move ( C^,ps[2], Len shl 1);
  end else
  begin
    SetLength(ps, Len);
    Move ( C^,ps[0], Len );
  end;
  Result:= ps;
end;

function TPDFAcroInformation.GetValueByIndex (Index: Cardinal): PDFString;
var
    C:PAnsiChar;
    Len:Cardinal;
begin
    C := PDFAcroGetValueByIndex( FDocument.Handle, Index, @Len );
    Result := PCharToPDFString( C, Len );
end;

function TPDFAcroInformation.GetTypeByIndex( Index: Cardinal ): TPDFAcroType ;
begin
  Result := PDFAcroGetTypeByIndex( FDocument.Handle, Index);
end;

function TPDFAcroInformation.GetFlagsByIndex( Index: Cardinal ): Integer;
begin
  Result := PDFAcroGetFlagsByIndex( FDocument.Handle, Index);
end;

function TPDFAcroInformation.GetIndexByName( FieldName:AnsiString; CheckUnicodeNames:Boolean): Cardinal;
begin
  Result := PDFAcroGetIndexByName(FDocument.Handle, PAnsiChar(FieldName), CheckUnicodeNames);
end;

procedure TPDFAcroInformation.SetValueByIndex ( Index: Cardinal; NewValue: AnsiString);
begin
  PDFAcroSetValueByIndex(FDocument.Handle, Index, PAnsiChar(NewValue));
end;

function TPDFAcroInformation.GetOptionCountByIndex ( Index :Cardinal): Cardinal;
begin
  Result := PDFAcroGetOptionCountByIndex( FDocument.Handle, Index);
end;

function TPDFAcroInformation.GetOptionValueByIndex ( Index, OptionIndex: Cardinal): PDFString;
var
    C:PAnsiChar;
    Len:Cardinal;
begin
    C := PDFAcroGetOptionValueByIndex( FDocument.Handle, Index, OptionIndex, @Len );
    Result := PCharToPDFString( C, Len );
end;




function TPDFAcroInformation.GetObjByIndex(Index: Cardinal): TPDFCosObject;
begin
  Result := CosHandleToCosObject(  PDFAcroGetObjByIndex( FDocument.Handle, Index));
end;

{ TPDFBead }

function TPDFBead.GetPage: Integer;
begin
    Result := PDFBeadGetPage( FBead );
end;

function TPDFBead.GetRectangle: TPDFRect;
var
    R: TPDFRect;
begin
    PDFBeadGetRect (FBead, @R );
    Result := R;
end;


procedure TPDFBead.SetRectangle(Rect: TPDFRect);
begin
    PDFBeadSetRectM  ( FBead, @Rect );
end;


constructor TPDFBead.Create(Parent: TPDFThread; AHandle: PDFBeadHandle);
begin
    FBead := AHandle;
    FParent := Parent;
end;

function TPDFBead.GetNext: TPDFBead;
var
    I, k: Integer;
begin
    k := -1;
    for I := 0 to FParent.GetCount - 1 do
        if FParent.FBeads[I] = Self then
            k := I;
    if k < 0 then
        raise EPDFException.Create('Cannot find next bead.');
    if k = FParent.FBeadCount - 1 then
        Result := FParent.FBeads[0]
    else
        Result := FParent.FBeads[k+1];
end;

function TPDFBead.GetPrev: TPDFBead;
var
    I, k: Integer;
begin
    k := -1;
    for I := 0 to FParent.GetCount - 1 do
        if FParent.FBeads[I] = Self then
            k := I;
    if k < 0 then
        raise EPDFException.Create('Cannot find next bead.');
    if k = 0 then
        Result := FParent.FBeads[FParent.FBeadCount - 1]
    else
        Result := FParent.FBeads[k-1];
end;



{ TPDFThread }

function TPDFThread.AppendBead(Page: TPDFPage; Rect: TPDFRect): TPDFBead;
var
tmp : TPDFbead;
begin
    Count;
    tmp := TPDFBead.Create( Self, PDFThreadAppendBeadM( FThread, Page.Idx, @Rect ) );
    Inc ( FBeadCount );
    SetLength ( FBeads, FBeadCount );
    FBeads[FBeadCount -1 ] := tmp;
    Result := tmp;
end;

function  TPDFThread.CalcBeads: Integer;
var
    F, H:PDFBeadHandle;
begin
    FBeadCount := 0;
    H := PDFThreadGetFirstBead( FThread );
    F := H;
    while CosGetType( H ) <> 2 do
    begin
        Inc ( FBeadCount );
        SetLength ( FBeads, FBeadCount );
        FBeads[FBeadCount - 1] := TPDFBead.Create( Self, H );
        H := PDFBeadGetNext( H );
        if H = F then
            Break;
    end;
    Result := FBeadCount;
end;

constructor TPDFThread.Create(AThread: PDFThreadHandle);
begin
    FThread := AThread;
    FBeadCount := 0;
    FBeads := nil;
end;

destructor TPDFThread.Destroy;
var
    I: Integer;
begin
    inherited;
    for I := 0 to FBeadCount - 1 do
        FBeads[I].Free;
end;

function TPDFThread.GetBeads(Index: Cardinal): TPDFBead;
begin
    if Index >= Count then
        raise EPDFException.Create('Out of bound.');
    Result := FBeads[Index];
end;

function TPDFThread.GetCount: Cardinal;
begin
    if FBeadCount = 0 then
        CalcBeads;
    Result := FBeadCount;
end;


function TPDFThread.GetInfo(Info: TPDFInformation): PDFString;
var
 C:PAnsiChar;
 Len:ppUns32;
begin
    C := PDFThreadGetInfo( FThread, Integer ( Info ), @Len );
    Result := PCharToPDFString( C, Len );
end;


procedure TPDFThread.SetInfo(Info: TPDFInformation; Value: AnsiString;
  Charset: TFontCharset);
var
    ps: PDFString;
begin
    ps :=nil;
    if ( Value = '') or (Charset = 0 ) or ( ( Charset = DEFAULT_CHARSET ) and ( GetDefFontCharSet = 0 ) ) then
    begin
      PDFThreadSetInfo ( FThread, Integer ( Info ), PAnsiChar ( Value ), Length ( Value ) );
    end else
    begin
        ps := StringToPDFString( Value, Charset );
        PDFThreadSetInfo( FThread, Integer ( Info ), PAnsiChar ( @ps[0] ), Length ( ps ) );
    end;
end;

procedure TPDFThread.SetInfo(Info: TPDFInformation; Value: WideString);
var
    ps: PDFString;
begin
    ps := WideStringToPDFString( Value );
    PDFThreadSetInfo( FThread, Integer ( Info ), PAnsiChar ( @ps[0] ), Length ( ps ) );
end;



{ TPDFPagesConnector }

constructor TPDFPagesConnector.Create(ASource, ATarget: TPDFDocument);
begin
  Connection := PDFCreateDocumentConnection ( ASource.Handle, ATarget.Handle);
end;


destructor TPDFPagesConnector.Destroy;
begin
  PDFFreeDocumentConnection ( @Connection );
  inherited;
end;

procedure TPDFPagesConnector.AppendPage(PageIndex: Cardinal);
begin
    PDFSelectPageFromSourceDocument ( @Connection, PageIndex );
end;

procedure TPDFPagesConnector.Execute;
begin
    PDFCopyPagesToDestinationDocument ( @Connection );
end;



{ TPDFPaintBox }

procedure TPDFPaintBox.Arc(X1, Y1, X2, Y2, BeginAngle, EndAngle: Double);
begin
    PBXArc2( PB, X1, Y1, X2, Y2, BeginAngle, EndAngle );
end;

procedure TPDFPaintBox.Arc(X1, Y1, X2, Y2, X3, Y3, X4, Y4: Double);
begin
    PBXArc ( PB, X1, Y1, X2, Y2, X3, Y3, X4, Y4 );
end;

procedure TPDFPaintBox.Circle(X, Y, R: Double);
begin
    PBXCircle ( PB, X, Y, R );
end;

procedure TPDFPaintBox.Clip;
begin
    PBXClip ( PB );
end;


procedure TPDFPaintBox.ClosePath;
begin
    PBXClosePath( PB );
end;


constructor TPDFPaintBox.Create(Parent:TPDFPage; AHandle: Pointer);
begin
    PB := AHandle;
    FPage := Parent;
end;

procedure TPDFPaintBox.CurveTo(X1, Y1, X2, Y2, X3, Y3: Double);
begin
    PBXCurveTo( PB, X1, Y1, X2, Y2, X3, Y3 );
end;

destructor TPDFPaintBox.Destroy;
begin
  if FPage <> nil then
    PBXClose ( PB, FPage.Doc.FPackContent )
  else
    PBXClose ( PB, True);
  inherited;
end;

procedure TPDFPaintBox.Close;
var
    I: Integer;
begin
  if FPage <> nil then
  begin
    for I:= 0 to FPage.Count -1 do
    if FPage.FPBs[I] = Self then
        FPage.FPBs[I] := nil;
  end;
  Free;
end;

procedure TPDFPaintBox.Ellipse(X1, Y1, X2, Y2: Double);
begin
    PBXEllipse ( PB, X1, Y1, X2, Y2 );
end;

procedure TPDFPaintBox.EoClip;
begin
    PBXEoClip( PB );
end;

procedure TPDFPaintBox.EoFill;
begin
    PBXEoFill( PB );
end;

procedure TPDFPaintBox.EoFillAndStroke;
begin
    PBXEoFillAndStroke ( PB );
end;

procedure TPDFPaintBox.Fill;
begin
    PBXFill ( PB );
end;

procedure TPDFPaintBox.FillAndStroke;
begin
    PBXFillAndStroke( PB );
end;

function TPDFPaintBox.GetHeight: Double;
begin
    Result := PBXGetHeight( PB );
end;

function TPDFPaintBox.GetTextWidth(Str: AnsiString): Double;
begin
    Result := PBXGetTextWidth ( PB, PAnsiChar ( Str ) );
end;

function TPDFPaintBox.GetTextWidth(Str: WideString): Double;
var
 L: Integer;
begin
    L := Length ( Str );
    Result := PBXGetUnicodeWidth( PB, PWORD ( @Str[1] ), L );
end;

function TPDFPaintBox.GetWidth: Double;
begin
    Result := PBXGetWidth( PB );
end;

procedure TPDFPaintBox.LineTo(X, Y: Double);
begin
    PBXLineTo (PB, X, Y );
end;

procedure TPDFPaintBox.MoveTo(X, Y: Double);
begin
    PBXMoveTo ( PB, X, Y );
end;

procedure TPDFPaintBox.NewPath;
begin
    PBXNewPath ( PB );
end;

procedure TPDFPaintBox.NoDash;
begin
    PBXNoDash( PB );
end;

procedure TPDFPaintBox.Pie(X1, Y1, X2, Y2, X3, Y3, X4, Y4: Double);
begin
    PBXPie( PB, X1, Y1, X2, Y2, X3, Y3, X4, Y4 );
end;

procedure TPDFPaintBox.Pie(X1, Y1, X2, Y2, BeginAngle, EndAngle: Double);
begin
    PBXPie2 ( PB, X1, Y1, X2, Y2, BeginAngle, EndAngle );
end;

procedure TPDFPaintBox.PlayMetaFile(Metafile: TMetafile; X, Y, XScale,
  YScale: Double);
begin
    PBXPlayMetaFile( PB, Metafile.Handle, X, Y, XScale, YScale );
end;

procedure TPDFPaintBox.Rectangle(X1, Y1, X2, Y2: Double);
begin
    PBXRectangle( PB, X1, Y1, X2, Y2 );
end;

procedure TPDFPaintBox.RectRotated(X, Y, W, H, Angle: Double);
begin
    PBXRectRotated( PB, X,Y, W, H, Angle );
end;

procedure TPDFPaintBox.RoundRect(X1, Y1, X2, Y2, X3, Y3: Double);
begin
    PBXRoundRect ( PB, X1, Y1, X2, Y2, X3, Y3 );
end;


procedure TPDFPaintBox.SetActiveFont(Index: Cardinal; FontSize: Double;
  UnderLine, StrikeOut: Boolean; Charset: TFontCharset);
begin
    PBXSetActiveFontWithCharset( PB, Index, FontSize, Charset, UnderLine, StrikeOut );
end;

procedure TPDFPaintBox.SetCharacterSpacing(Spacing: Double);
begin
    PBXSetCharacterSpacing( PB, Spacing );
end;

procedure TPDFPaintBox.SetColor(Color: TPDFColor);
var
  C:PDFColor;
begin
    Move ( Color, C, SizeOf ( Color ) );
    case Color.Space of
        vcsGray : C.Device:= 1;
        vcsRGB: C.Device :=3;
        vcsCMYK: C.Device :=4;
    end;
    PBXSetColorM( PB, @C );
end;

procedure TPDFPaintBox.SetDash(Dash: AnsiString);
begin
    PBXSetDash( PB, PAnsiChar ( Dash ) );
end;

procedure TPDFPaintBox.SetFillColor(Color: TPDFColor);
var
  C:PDFColor;
begin
    Move ( Color, C, SizeOf ( Color ) );
    case Color.Space of
        vcsGray : C.Device:= 1;
        vcsRGB: C.Device :=3;
        vcsCMYK: C.Device :=4;
    end;
    PBXSetFillColorM( PB, @C );
end;

procedure TPDFPaintBox.SetFlatness(Flatness: Integer);
begin
    PBXSetFlatness( PB, Flatness );
end;

procedure TPDFPaintBox.SetGState(ExtState: TPDFExtGraphicState);
begin
    PBXSetGState( PB, ExtState.Idx);
end;

procedure TPDFPaintBox.SetHorizontalScaling(Scaling: Double);
begin
    PBXSetHorizontalScaling( PB, Scaling );
end;

procedure TPDFPaintBox.SetLineCap(LineCap: TPDFLineCap);
begin
    PBXSetLineCap( PB, Integer ( LineCap ) );
end;

procedure TPDFPaintBox.SetLineJoin(LineJoin: TPDFLineJoin);
begin
    PBXSetLineJoin( PB, Integer ( LineJoin ) );
end;

procedure TPDFPaintBox.SetLineWidth(LineWidth: Double);
begin
    PBXSetLineWidth( PB, LineWidth );
end;

procedure TPDFPaintBox.SetMiterLimit(MiterLimit: Double);
begin
    PBXSetMiterLimit( PB, MiterLimit );
end;

procedure TPDFPaintBox.SetStrokeColor(Color: TPDFColor);
var
  C:PDFColor;
begin
    Move ( Color, C, SizeOf ( Color ) );
    case Color.Space of
        vcsGray : C.Device:= 1;
        vcsRGB: C.Device :=3;
        vcsCMYK: C.Device :=4;
    end;
    PBXSetStrokeColorM( PB, @C );
end;

procedure TPDFPaintBox.SetTextRenderingMode(Mode: TPDFRenderingMode);
begin
    PBXSetTextRenderingMode( PB, Integer ( Mode ) );
end;

procedure TPDFPaintBox.SetWordSpacing(Spacing: Double);
begin
    PBXSetWordSpacing( PB, Spacing );
end;

procedure TPDFPaintBox.ShowImage(Index: Cardinal; X, Y, Width, Height,
  Angle: Double);
begin
    PBXShowImage( PB, Index, X, Y, Width, Height, Angle );
end;

procedure TPDFPaintBox.StateRestore;
begin
    PBXStateRestore( PB );
end;

procedure TPDFPaintBox.StateStore;
begin
    PBXStateStore( PB );
end;

procedure TPDFPaintBox.Stroke;
begin
    PBXStroke ( PB );
end;

procedure TPDFPaintBox.TextShow(X, Y, Orientation: Double; Str: AnsiString);
begin
    PBXTextOut( PB, X, Y, Orientation, PAnsiChar ( Str ) );
end;

procedure TPDFPaintBox.TextShow(X, Y, Orientation: Double;
  Str: WideString);
begin
    PBXUnicodeTextOut ( PB, X, Y, Orientation, PWord ( @Str[1] ), Length ( Str ) );
end;

procedure TPDFPaintBox.AppendLine(LineCode: AnsiString);
begin
    PBXAppendLine( PB, PAnsiChar ( LineCode ) );
end;

{ TPDFPage }

function TPDFPage.AddContent: Integer;
begin
    Count;
    Result := PDFPageAddContent( Doc.Handle, PIndex );
    Inc ( FCount );
    SetLength ( FPBs, FCount );
end;

function TPDFPage.CreateWrappedPaintBox: TPDFPaintBox;
begin
  CloseContents;
  Result := TPDFPaintBox.Create( Self, PDFPageCreatePaintBoxAndRepackContents(Doc.Handle, PIndex, Doc.Resolution));
  FCount := 1;
  SetLength ( FPBs, FCount );
  FPBs[0] := Result;
end;

constructor TPDFPage.Create(Document: TPDFDocument; AIndex: Cardinal);
begin
    Doc := Document;
    PIndex := AIndex;
    FCount := -1;
    FPBs := nil;
end;

destructor TPDFPage.Destroy;
var
    I: Integer;
begin
    inherited;
    for I := 0 to Length ( FPBs ) -1 do
        if FPBs[I] <> nil then
            FPBs[I].Free;
end;

function TPDFPage.CreatePaintBox: TPDFPaintBox;
begin
    Result := TPDFPaintBox.Create( Self, PDFPageCreatePaintBox( Doc.Handle, PIndex, Doc.Resolution  ) );
end;

function TPDFPage.CreatePaintBox(Index: Integer): TPDFPaintBox;
begin
    Result := TPDFPaintBox.Create( Self, PDFPageCreatePaintBoxFromContent( Doc.Handle, PIndex, Index, Doc.Resolution ));
end;

function TPDFPage.GetAngle: TPDFPageRotateAngle;
begin
    Result := TPDFPageRotateAngle( PDFPageGetRotateAngle( Doc.Handle, PIndex ) );
end;

function TPDFPage.GetBox(index: TPDFPageBoxType): TPDFRect;
begin
    Result := PDFPageGetBox( Doc.Handle, PIndex, Integer ( index ) );
end;

function TPDFPage.GetCount: Cardinal;
begin
    if FCount = -1 then
    begin
        FCount := PDFPageGetContentCount ( Doc.Handle, PIndex );
        SetLength ( FPBs, FCount );
    end;
    Result := FCount;
end;

function TPDFPage.GetPaintBoxes(Index:Cardinal): TPDFPaintBox;
begin
    if ( Index = 0) and ( Count = 0 ) then
    begin
        FCount := 1;
        SetLength ( FPBs, 1 );
        FPBs[0] := CreatePaintBox ;
        Result := FPBs[0];
        Exit;
    end;
    if Index >= Cardinal ( FCount ) then
    begin
        raise EPDFException.Create( 'Out of bound.');
    end;
    if FPBs[Index] = nil then
    begin
        FPBs[Index] := CreatePaintBox( Index );
    end;
    Result := FPBs[Index];
end;

function TPDFPage.GetPaintBox: TPDFPaintBox;
var
    i: Integer;
begin
    i:= Count;
    if i = 0 then
    begin
        Result := PaintBoxes[0];
    end else
    begin
        Result := PaintBoxes[Count -1 ];
    end;
end;

function TPDFPage.InsertContent(Index: Integer): Integer;
var
    i: Integer;
begin
    i := Count;
    if i < Index then
        raise EPDFException.Create('Out of bound.');
    Result := PDFPageInsertContent ( Doc.Handle, PIndex, Index );
    Inc ( FCount );
    SetLength ( FPBS, FCount );
    for i:= FCount - 1 downto Index + 1 do
        FPBs[i] := FPbs[i-1];
    FPBs[Index]:= nil;
end;

procedure TPDFPage.RemoveContent(Index: Integer);
var
    i: Integer;
begin
    i := Count;
    if i <= Index then
        raise EPDFException.Create('Out of bound.');
    if FPBs[Index] <> nil then
        FPBs[Index].Free;
    PDFPageRemoveContent( Doc.Handle, PIndex, Index );
    if i = 0 then
    begin
        FCount := 0;
        FPBs := nil;
        Exit;
    end;
    for i := Index + 1 to FCount-1 do
        FPBs[ i -1 ] := FPBs[ i ];
    Dec ( FCount );
    SetLength ( FPBs, FCount );
end;

procedure TPDFPage.SetAngle(Angle: TPDFPageRotateAngle);
begin
    PDFPageSetRotateAngle ( Doc.Handle, PIndex, Integer ( Angle ) );
end;

procedure TPDFPage.SetBox(index: TPDFPageBoxType; Box: TPDFRect);
begin
    PDFPageSetBoxM ( Doc.Handle, PIndex, Integer ( index ), @Box );
end;

function TPDFPage.GetParent: PDFDocHandle;
begin
    Result := Doc.Handle;
end;

procedure TPDFPage.CloseContents;
var
    I: Integer;
begin
    for I := 0 to Count -1 do
    begin
      if FPBs[I] <> nil then
      begin
            FPBs[I].Free;
            FPBs[I] := nil;
      end;
    end;
end;

{ TPDFAcroObject }

procedure TPDFAcroObject.AddAction(Action: TPDFAction;
  EventType: TPDFAcroEventType);
begin
    PDFAcroObjectAddAction( FDocument, AcroIndex, Action.Handle, Integer ( EventType ) );
end;

constructor TPDFAcroObject.Create(Page: TPDFPage);
begin
    FDocument := Page.Parent;
    AcroIndex := Page.Idx;
end;

procedure TPDFAcroObject.SetAnnotationFlag(const Value: Word);
begin
  PDFAcroObjectSetAnnotationFlag( FDocument, AcroIndex, Value);
end;

procedure TPDFAcroObject.SetBorder(BorderColor, FillColor: TPDFColor;
  BorderWidth: Double);
var
  C1, C2:PDFColor;
begin
    Move ( BorderColor, C1, SizeOf ( TPDFColor ) );
    case BorderColor.Space of
        vcsGray : C1.Device:= 1;
        vcsRGB: C1.Device :=3;
        vcsCMYK: C1.Device :=4;
    end;
    Move ( FillColor, C2, SizeOf ( TPDFColor ) );
    case FillColor.Space of
        vcsGray : C2.Device:= 1;
        vcsRGB: C2.Device :=3;
        vcsCMYK: C2.Device :=4;
    end;
    PDFAcroObjectSetBorderM( FDocument, AcroIndex, @C1, @C2, BorderWidth );
end;

procedure TPDFAcroObject.SetCaption(Value: AnsiString);
begin
    PDFAcroObjectSetCaption( FDocument, AcroIndex, PAnsiChar ( Value ) );
end;

procedure TPDFAcroObject.SetFlag(Value: TPDFAcroObjectFlags);
var
    I: Cardinal;
begin
    I := aofDefault;
    if vaofRequired in Value then I := I or aofRequired;
    if vaofReadOnly in Value then I := I or aofReadOnly;
    if vaofNoExport in Value then I := I or aofNoExport;
    if vaofMultiLine in Value then I := I or aofMultiLine;
    if vaofPassword in Value then I := I or aofPassword;
    if vaofEdit in Value then I := I or aofEdit;
    if vaofMultiSelect in Value then I := I or aofMultiSelect;
    if vaofDoNotSpellCheck in Value then I := I or aofDoNotSpellCheck;
    if vaofDoNotScroll in Value then I := I or aofDoNotScroll;
    PDFAcroObjectSetFlag( FDocument, AcroIndex, I );
end;

procedure TPDFAcroObject.SetFont(FontIndex: Cardinal; FontSize: Double;
  FontColor: TPDFColor);
var
  C:PDFColor;
begin
    Move ( FontColor, C, SizeOf ( TPDFColor ) );
    case FontColor.Space of
        vcsGray : C.Device:= 1;
        vcsRGB: C.Device :=3;
        vcsCMYK: C.Device :=4;
    end;
    PDFAcroObjectSetFontM ( FDocument, AcroIndex, FontIndex, FontSize, @C );
end;

{ TPDFAcroEditBox }

constructor TPDFAcroEditBox.Create(Page: TPDFPage; Box: TPDFRect;
  Name: AnsiString);
begin
    FDocument := Page.Parent;
    AcroIndex := PDFPageAppendEditBoxM ( FDocument, Page.Idx, @Box, PAnsiChar ( Name ) );
end;

procedure TPDFAcroEditBox.SetAlign(Value: TPDFAcroQuadding);
begin
    PDFAcroEditBoxSetAlign( FDocument, AcroIndex, Integer ( Value ) );
end;

procedure TPDFAcroEditBox.SetMaxLen(Value: Cardinal);
begin
    PDFAcroEditBoxSetMaxLen( FDocument, AcroIndex, Value );
end;

{ TPDFAcroSignatureBox }

constructor TPDFAcroSignatureBox.Create(Page: TPDFPage; Box: TPDFRect;
  Name: AnsiString);
begin
    FDocument := Page.Parent;
    AcroIndex := PDFPageAppendSignatureBoxM ( FDocument, Page.Idx, @Box, PAnsiChar ( Name ) );
end;

{ TPDFAcroPushButton }

constructor TPDFAcroPushButton.Create(Page: TPDFPage; Box: TPDFRect;
  Name: AnsiString);
begin
    FDocument := Page.Parent;
    AcroIndex := PDFPageAppendPushButtonM ( FDocument, Page.Idx, @Box, PAnsiChar ( Name ) );
end;

procedure TPDFAcroPushButton.SetMiter(Value: Double);
begin
    PDFAcroPushButtonSetMiter( FDocument, AcroIndex, Value );
end;



{ TPDFAcroRadioButton }

constructor TPDFAcroRadioButton.Create(Page: TPDFPage; Box: TPDFRect; Name,
  GroupName: AnsiString; InitaialState: Boolean);
begin
    FDocument := Page.Parent;
    AcroIndex := PDFPageAppendRadioButtonM( FDocument, Page.Idx, @Box, PAnsiChar ( Name ), PAnsiChar ( GroupName ), InitaialState );
end;

procedure TPDFAcroRadioButton.SetStyle(Sign: TPDFCheckBoxSign; Style: TPDFCheckBoxStyle);
var
  i: Integer;
begin
    case Sign of
        vcbsFinger     : i := cbsFinger;
        vcbsPen        : i := cbsPen;
        vcbsVmark      : i := cbsVmark;
        vcbsNike       : i := cbsNike;
        vcbsCross      : i := cbsCross;
        vcbsX          : i := cbsX;
        vcbsCheck      : i := cbsCheck;
        vcbsPlus       : i := cbsPlus;
        vcbsDiamond    : i := cbsDiamond;
        vcbsStar       : i := cbsStar;
        vcbsFlower     : i := cbsFlower;
        vcbsSnow       : i := cbsSnow;
        vcbsCircle     : i := cbsCircle;
        else
            i := cbsRectangle;
    end;
    PDFAcroObjectSetStyle ( FDocument, AcroIndex, i, Integer ( Style ) );
end;

{ TPDFAcroCheckBox }

constructor TPDFAcroCheckBox.Create(Page: TPDFPage; Box: TPDFRect;
  Name: AnsiString; InitaialState: Boolean);
begin
    FDocument := Page.Parent;
    AcroIndex := PDFPageAppendCheckBoxM ( FDocument, Page.Idx, @Box, PAnsiChar ( Name ), InitaialState );
end;

procedure TPDFAcroCheckBox.SetStyle(Sign: TPDFCheckBoxSign; Style: TPDFCheckBoxStyle);
var
  i: Integer;
begin
    case Sign of
        vcbsFinger     : i := cbsFinger;
        vcbsPen        : i := cbsPen;
        vcbsVmark      : i := cbsVmark;
        vcbsNike       : i := cbsNike;
        vcbsCross      : i := cbsCross;
        vcbsX          : i := cbsX;
        vcbsCheck      : i := cbsCheck;
        vcbsPlus       : i := cbsPlus;
        vcbsDiamond    : i := cbsDiamond;
        vcbsStar       : i := cbsStar;
        vcbsFlower     : i := cbsFlower;
        vcbsSnow       : i := cbsSnow;
        vcbsCircle     : i := cbsCircle;
        else
            i := cbsRectangle;
    end;
    PDFAcroObjectSetStyle ( FDocument, AcroIndex, i, Integer ( Style ) );
end;

{ TPDFAcroListBox }

constructor TPDFAcroListBox.Create(Page: TPDFPage; Box: TPDFRect;
  Name: AnsiString);
begin
    FDocument := Page.Parent;
    AcroIndex := PDFPageAppendListBoxM ( FDocument, Page.Idx, @Box, PAnsiChar ( Name ));
end;

procedure TPDFAcroListBox.AppendItem(Item: AnsiString);
begin
    PDFAcroObjectAppendItem( FDocument, AcroIndex, PAnsiChar ( Item ) );
end;


{ TPDFAcroComboBox }

constructor TPDFAcroComboBox.Create(Page: TPDFPage; Box: TPDFRect;
  Name: AnsiString);
begin
    FDocument := Page.Parent;
    AcroIndex := PDFPageAppendComboBoxM ( FDocument, Page.Idx, @Box, PAnsiChar ( Name ) );
end;

procedure TPDFAcroComboBox.AppendItem(Item: AnsiString);
begin
    PDFAcroObjectAppendItem( FDocument, AcroIndex, PAnsiChar ( Item ) );
end;


{ TPDFCosObject }

constructor TPDFCosObject.Create(AHandle: PDFCosHandle);
begin
    Obj := AHandle;
end;

function TPDFCosObject.CreateCopy(Document: TPDFDocument): TPDFCosObject;
begin
    Result := TPDFCosObject.Create( CosCopy ( Document.Handle, Obj ) );
end;

procedure TPDFCosObject.FreeObj;
begin
    CosFree ( Obj );
end;


function TPDFCosObject.GetGeneration: Word;
begin
    Result := CosGetGeneration( Obj );
end;

function TPDFCosObject.GetID: Cardinal;
begin
    Result := CosGetID ( Obj );
end;

function TPDFCosObject.GetIsIndirect: Boolean;
begin
    Result := CosIsIndirect ( Obj );
end;

{ TPDFCosNull }

constructor TPDFCosNull.Create(Document: TPDFDocument);
begin
    Obj  := CosNullNew( Document.Handle )
end;

constructor TPDFCosNull.Create(AHandle: PDFCosHandle);
begin
    Obj := AHandle;
end;

{ TPDFCosBoolean }

constructor TPDFCosBoolean.Create(Document: TPDFDocument; IsIndirect,
  Value: Boolean);
begin
    Obj := CosBoolNew( Document.Handle, IsIndirect, Value );
end;

constructor TPDFCosBoolean.Create(AHandle: PDFCosHandle);
begin
    Obj := AHandle;
end;

function TPDFCosBoolean.GetValue: Boolean;
begin
    Result := CosBoolGetValue( Obj );
end;

procedure TPDFCosBoolean.SetValue(Value: Boolean);
begin
    CosBoolSetValue( Obj, Value );
end;

{ TPDFCosReal }

constructor TPDFCosReal.Create(Document: TPDFDocument; IsIndirect: Boolean;
  Value: Double);
begin
    Obj := CosRealNew ( Document.Handle, IsIndirect, Value );
end;

constructor TPDFCosReal.Create(AHandle: PDFCosHandle);
begin
    Obj := AHandle;
end;

function TPDFCosReal.GetValue: Double;
begin
    Result := CosRealGetValue ( Obj );
end;

procedure TPDFCosReal.SetValue(Value: Double);
begin
    CosRealSetValue( obj, Value );
end;

{ TPDFCosInteger }

constructor TPDFCosInteger.Create(Document: TPDFDocument;
  IsIndirect: Boolean; Value: Integer);
begin
    Obj := CosIntNew( Document.Handle, IsIndirect, Value )
end;

constructor TPDFCosInteger.Create(AHandle: PDFCosHandle);
begin
    Obj := AHandle;
end;

function TPDFCosInteger.GetValue: Integer;
begin
    Result := CosIntGetValue( Obj );
end;

procedure TPDFCosInteger.SetValue(Value: Integer);
begin
    CosIntSetValue( Obj, Value );
end;

{ TPDFCosName }

constructor TPDFCosName.Create(Document: TPDFDocument; IsIndirect: Boolean;
  Value: Cardinal);
begin
    Obj := CosNameNew( Document.Handle, IsIndirect, Value );
end;

constructor TPDFCosName.Create(Document: TPDFDocument; IsIndirect: Boolean;
  Value: AnsiString);
begin
    Obj := CosNameNew ( Document.Handle, IsIndirect, Document.Owner.StringToAtom( Value ) );
end;

constructor TPDFCosName.Create(AHandle: PDFCosHandle);
begin
    Obj := AHandle;
end;

function TPDFCosName.GetValue: Cardinal;
begin
    Result := CosNameGetValue ( Obj );
end;

procedure TPDFCosName.SetValue(Value: Cardinal);
begin
    CosNameSetValue( Obj, Value );
end;


{ TPDFCosString }

constructor TPDFCosString.Create(Document: TPDFDocument;
  IsIndirect: Boolean; Value: AnsiString; Charset: TFontCharset);
var
    ps: PDFString;
begin
    ps :=nil;
    if ( Value = '') or (Charset = 0 ) or ( ( Charset = DEFAULT_CHARSET ) and ( GetDefFontCharSet = 0 ) ) then
    begin
      Obj := CosStringNew ( Document.Handle, IsIndirect, PAnsiChar ( Value ), Length ( Value ) );
    end else
    begin
        ps := StringToPDFString( Value, Charset );
        Obj := CosStringNew ( Document.Handle, IsIndirect, PAnsiChar ( @ps[0] ), Length ( ps ) );
    end;
end;

constructor TPDFCosString.Create(Document: TPDFDocument;
  IsIndirect: Boolean; Value: WideString);
var
    ps: PDFString;
begin
    ps := WideStringToPDFString( Value );
    Obj := CosStringNew ( Document.Handle, IsIndirect, PAnsiChar ( @ps[0] ), Length ( ps ) );
end;

constructor TPDFCosString.Create(Document: TPDFDocument;
  IsIndirect: Boolean; Buffer: Pointer; Len: Cardinal);
begin
    Obj := CosStringNew ( Document.Handle, IsIndirect, PAnsiChar ( Buffer ), Len );
end;

constructor TPDFCosString.Create(AHandle: PDFCosHandle);
begin
    Obj := AHandle;
end;

function TPDFCosString.GetValueString: PDFString;
var
    C:PAnsiChar;
    Len:Cardinal;
begin
    C := CosStringGetValue( Obj, @Len );
    Result := PCharToPDFString( C, Len );
end;

procedure TPDFCosString.SetValue(Value: Pointer; Len: Cardinal);
begin
    CosStringSetValue ( Obj, PAnsiChar ( Value ), Len );
end;

procedure TPDFCosString.SetValueString(Value: AnsiString;
  Charset: TFontCharset);
var
    ps: PDFString;
begin
    ps :=nil;
    if ( Value = '') or (Charset = 0 ) or ( ( Charset = DEFAULT_CHARSET ) and ( GetDefFontCharSet = 0 ) ) then
    begin
        CosStringSetValue ( Obj, PAnsiChar ( Value ), Length ( Value ) );
    end else
    begin
        ps := StringToPDFString( Value, Charset );
        CosStringSetValue ( Obj, PAnsiChar ( @ps[0] ), Length ( ps ) );
    end;
end;

procedure TPDFCosString.SetValueString(Value: WideString);
var
    ps: PDFString;
begin
    ps := WideStringToPDFString( Value );
    CosStringSetValue ( Obj, PAnsiChar ( @ps[0] ), Length ( ps ) );
end;

{ TPDFCosArray }

function TPDFCosArray.Append(CosObj: TPDFCosObject): Integer;
begin
    Result := CosArrayAppend ( Obj, CosObj.Handle );
end;

procedure TPDFCosArray.Clear;
begin
    CosArrayClear ( Obj );
end;

constructor TPDFCosArray.Create(Document: TPDFDocument;
  IsIndirect: Boolean; Entries: Integer);
begin
    Obj := CosArrayNew ( Document.Handle, IsIndirect, Entries );
end;

constructor TPDFCosArray.Create(AHandle: PDFCosHandle);
begin
    Obj := AHandle;
end;

function TPDFCosArray.GetCount: Integer;
begin
    Result := CosArrayCount ( Obj );
end;

function TPDFCosArray.GetItem(index: Integer): TPDFCosObject;
begin
   Result := CosHandleToCosObject( CosArrayItem( Obj, index ) );
end;

function TPDFCosArray.Insert(CosObj: TPDFCosObject;
  Index: Integer): Integer;
begin
    Result := CosArrayInsert (Obj, CosObj.Handle, Index );
end;

procedure TPDFCosArray.Remove(Index: Integer);
begin
    CosArrayRemove( Obj, Index );
end;

{ TPDFCosDictionary }

procedure TPDFCosDictionary.Clear;
begin
    CosDictClear( Obj );
end;

constructor TPDFCosDictionary.Create(Document: TPDFDocument;
  IsIndirect: Boolean; Entries: Integer);
begin
    Obj := CosDictNew( Document.Handle, IsIndirect, Entries )
end;

constructor TPDFCosDictionary.Create(AHandle: PDFCosHandle);
begin
    Obj := AHandle;
end;


function TPDFCosDictionary.GetValue(index: Cardinal): TPDFCosObject;
begin
    Result := CosHandleToCosObject( CosDictValueByName( Obj, index ) );
end;

procedure TPDFCosDictionary.RemoveKey(Key: Cardinal);
begin
    CosDictRemoveKey( Obj, Key );
end;


procedure TPDFCosDictionary.SetValue(index: Cardinal;
  CosObject: TPDFCosObject);
begin
    CosDictAppend( Obj, Index, CosObject.Handle );
end;

{ TPDFCosStream }

constructor TPDFCosStream.Create(Document: TPDFDocument; Entries: Integer);
begin
    Obj := CosStreamNew( Document.Handle, True, Entries );
end;

constructor TPDFCosStream.Create(AHandle: PDFCosHandle);
begin
    Obj := AHandle;
end;

function TPDFCosStream.GetStream: TPDFStream;
begin
    Result := TPDFStream.Create( CosStreamGetValue( Obj ) );
end;



{ TPDFExtGraphicState }

constructor TPDFExtGraphicState.Create(Document: TPDFDocument);
begin
    Index := PDFExtGraphicStateNew( Document.Handle );
    FDoc := Document.Handle;
end;

procedure TPDFExtGraphicState.SetAlphaFill(AAlphaFill: Double);
begin
    PDFExtGraphicStateSetAlphaFill( FDoc, Index, AAlphaFill);
end;

procedure TPDFExtGraphicState.SetAlphaIsShape(AAlphaIsShape: Boolean);
begin
    PDFExtGraphicStateSetAlphaIsShape( FDoc, Index, AAlphaIsShape);
end;

procedure TPDFExtGraphicState.SetAlphaStroke(AAlphaStroke: Double);
begin
    PDFExtGraphicStateSetAlphaStroke( FDoc, Index, AAlphaStroke );
end;

procedure TPDFExtGraphicState.SetBlendMode(ABlendMode: TPDFBlendMode);
begin
    PDFExtGraphicStateSetBlendMode( FDoc, Index, Integer ( ABlendMode ) );
end;

procedure TPDFExtGraphicState.SetCTM(a, b, c, d, e, f: Double);
begin
  PDFExtGraphicStateSetCTM( FDoc, Index, a,b,c,d,e,f );
end;

procedure TPDFExtGraphicState.SetDashPattern(DashPattern: TPDFCosObject);
begin
    PDFExtGraphicStateSetDashPattern( FDoc, Index, DashPattern.Handle );
end;

procedure TPDFExtGraphicState.SetFlatness(AFlatness: Double);
begin
    PDFExtGraphicStateSetFlatness( FDoc, Index, AFlatness );
end;

procedure TPDFExtGraphicState.SetLineCap(ALineCap: TPDFLineCap);
begin
    PDFExtGraphicStateSetLineCap( FDoc, Index, Integer ( ALineCap ) );
end;

procedure TPDFExtGraphicState.SetLineJoin(ALineJoin: TPDFLineJoin);
begin
    PDFExtGraphicStateSetLineJoin( FDoc, Index, Integer ( ALineJoin ) );
end;

procedure TPDFExtGraphicState.SetLineWidth(ALineWidth: Double);
begin
    PDFExtGraphicStateSetLineWidth( FDoc, Index, ALineWidth );
end;

procedure TPDFExtGraphicState.SetMiterLimit(AMiterLimit: Double);
begin
    PDFExtGraphicStateSetMitterLimit( FDoc, Index, AMiterLimit );
end;

procedure TPDFExtGraphicState.SetOverprintFill(AOverprintFill: Boolean);
begin
    PDFExtGraphicStateSetOverprintFill( FDoc, Index, AOverprintFill );
end;

procedure TPDFExtGraphicState.SetOverprintMode(AOverprintMode: Integer);
begin
    PDFExtGraphicStateSetOverprintMode( FDoc, Index, AOverprintMode );
end;

procedure TPDFExtGraphicState.SetOverprintStroke(AOverprintStroke: Boolean);
begin
    PDFExtGraphicStateSetOverprintStroke( FDoc, Index, AOverprintStroke );
end;

procedure TPDFExtGraphicState.SetRenderingIntent(
  ARenderingIntent: TPDFRenderingIntents);
begin
    PDFExtGraphicStateSetRenderingIntent( FDoc, Index, Integer ( ARenderingIntent ) );
end;

procedure TPDFExtGraphicState.SetSmoothness(ASmoothness: Double);
begin
    PDFExtGraphicStateSetSmoothness( FDoc, Index, ASmoothness );
end;

procedure TPDFExtGraphicState.SetStrokeAdjustment(
  AStrokeAdjustment: Boolean);
begin
    PDFExtGraphicStateSetStrokeAdjustment( FDoc, Index, AStrokeAdjustment );
end;

procedure TPDFExtGraphicState.SetTextKnockout(AKnockout: Boolean);
begin
    PDFExtGraphicStateSetTextKnockout( FDoc, Index, AKnockout );
end;


{ TPDFAnnotation }

constructor TPDFAnnotation.Create;
begin
//Empty
end;

procedure TPDFAnnotation.SetAction(Value: TPDFAction);
begin
    PDFAnnotationSetAction( FAnnot, Value.Handle);
end;

procedure TPDFAnnotation.SetAlphaBlending(Value: Double);
begin
    PDFAnnotationSetAlphaBlending( FAnnot, Value );
end;

procedure TPDFAnnotation.SetBorderStyle(Width: Double;
  Style: TPDFBorderStyle; Dash: TPDFCosObject);
begin
    PDFAnnotationSetBorderStyle( FAnnot, Width, Integer ( Style ), Dash.Handle );
end;

procedure TPDFAnnotation.SetColor(Value: TPDFRGB);
var
    C: PDFColor;
begin
    C.Device := 3;
    C.Comp1 := Value.R;
    C.Comp2 := Value.G;
    C.Comp3 := Value.B;
    PDFAnnotationSetColorM( FAnnot, @C );
end;

procedure TPDFAnnotation.SetFlags(Value: TPDFAnnotationFlags);
var
    FL:Word;
begin
    FL := 0;
    if vafInvisible in Value then FL := FL or afInvisible;
    if vafHidden in Value then FL := FL or afHidden;
    if vafPrint in Value then FL := FL or afPrint;
    if vafNoZoom in Value then FL := FL or afNoZoom;
    if vafNoRotate in Value then FL := FL or afNoRotate;
    if vafNoView in Value then FL := FL or afNoView;
    if vafReadOnly in Value then FL := FL or afReadOnly;
    if vafListBox in Value then FL := FL or afListBox;
    if vafToggleNoView in Value then FL := FL or afToggleNoView;
    PDFAnnotationSetFlag( FAnnot, FL );
end;

procedure TPDFAnnotation.SetName(Value: AnsiString);
begin
    PDFAnnotationSetName( FAnnot, PAnsiChar ( Value ), Length ( Value ) );
end;

procedure TPDFAnnotation.SetTitle(Value: AnsiString; Charset: TFontCharset);
var
    ps: PDFString;
begin
    ps :=nil;
    if ( Value = '') or (Charset = 0 ) or ( ( Charset = DEFAULT_CHARSET ) and ( GetDefFontCharSet = 0 ) ) then
    begin
        PDFAnnotationSetTitle ( FAnnot, PAnsiChar ( Value ), Length ( Value ) );
    end else
    begin
        ps := StringToPDFString( Value, Charset );
        PDFAnnotationSetTitle ( FAnnot, PAnsiChar ( @ps[0] ), Length ( ps ) );
    end;
end;

procedure TPDFAnnotation.SetTitle(Value: WideString);
var
    ps: PDFString;
begin
    ps := WideStringToPDFString( Value );
    PDFAnnotationSetTitle ( FAnnot, PAnsiChar ( @ps[0] ), Length ( ps ) );
end;

{ TPDFAnnotationText }

constructor TPDFAnnotationText.Create(Page: TPDFPage; Box: TPDFRect;
  IsOpened: Boolean; Name: TPDFAnnotationTextName; Content: AnsiString; Charset: TFontCharset);
var
    ps: PDFString;
begin
    ps :=nil;
    if ( Content = '') or (Charset = 0 ) or ( ( Charset = DEFAULT_CHARSET ) and ( GetDefFontCharSet = 0 ) ) then
    begin
        FAnnot := PDFPageAppendAnnotationTextM( Page.Parent, Page.Idx, @Box, PAnsiChar ( Content), Length ( Content ),
            IsOpened, Integer ( Name ) );
    end else
    begin
        ps := StringToPDFString( Content, Charset );
        FAnnot := PDFPageAppendAnnotationTextM( Page.Parent, Page.Idx, @Box, PAnsiChar ( @ps[0] ), Length ( ps ),
            IsOpened, Integer ( Name ) );
    end;
end;

constructor TPDFAnnotationText.Create(Page: TPDFPage; Box: TPDFRect;
  IsOpened: Boolean; Name: TPDFAnnotationTextName; Content: WideString);
var
    ps: PDFString;
begin
    ps := WideStringToPDFString( Content );
    FAnnot := PDFPageAppendAnnotationTextM( Page.Parent, Page.Idx, @Box, PAnsiChar ( @ps[0] ), Length ( ps ),
            IsOpened, Integer ( Name ) );
end;

{ TPDFAnnotationLink }

constructor TPDFAnnotationLink.Create(Page: TPDFPage; Box: TPDFRect;
  Destination: TPDFDestination; Mode: TPDFHighLightMode; Visible: Boolean);
begin
    FAnnot := PDFPageAppendAnnotationLinkWithDestM( Page.Parent, Page.Idx, @Box, Pointer ( Destination ), Integer ( Mode), Visible );
end;

constructor TPDFAnnotationLink.Create(Page: TPDFPage; Box: TPDFRect;
  Action: TPDFAction; Mode: TPDFHighLightMode; Visible: Boolean);
begin
    FAnnot := PDFPageAppendAnnotationLinkWithActionM( Page.Parent, Page.Idx, @Box, Action.Handle, Integer ( Mode ), Visible );
end;

{ TPDFAnnotationStamp }


constructor TPDFAnnotationStamp.Create(Page: TPDFPage; Box: TPDFRect;
  Name: TPDFAnnotationStampName; Content: AnsiString; Charset: TFontCharset);
var
    ps: PDFString;
begin
    ps :=nil;
    if ( Content = '') or (Charset = 0 ) or ( ( Charset = DEFAULT_CHARSET ) and ( GetDefFontCharSet = 0 ) ) then
    begin
        FAnnot := PDFPageAppendAnnotationStampM( Page.Parent, Page.Idx, @Box, PAnsiChar ( Content), Length ( Content ), Integer ( Name ) );
    end else
    begin
        ps := StringToPDFString( Content, Charset );
        FAnnot := PDFPageAppendAnnotationStampM ( Page.Parent, Page.Idx, @Box, PAnsiChar ( @ps[0] ), Length ( ps ), Integer ( Name ) );
    end;
end;

constructor TPDFAnnotationStamp.Create(Page: TPDFPage; Box: TPDFRect;
  Name: TPDFAnnotationStampName; Content: WideString);
var
    ps: PDFString;
begin
    ps := WideStringToPDFString( Content );
    FAnnot := PDFPageAppendAnnotationStampM( Page.Parent, Page.Idx, @Box, PAnsiChar ( @ps[0] ), Length ( ps ), Integer ( Name ) );

end;

constructor TPDFAnnotationStamp.Create(Page : TPDFPage; Box : TPDFRect; Name : TPDFAnnotationStampName; Resolution:Cardinal; var PaintBox: TPDFPaintBox; Content : AnsiString; Charset: TFontCharset = 0);
var
    ps: PDFString;
    PB:Pointer;
begin
    ps :=nil;
    if ( Content = '') or (Charset = 0 ) or ( ( Charset = DEFAULT_CHARSET ) and ( GetDefFontCharSet = 0 ) ) then
    begin
        FAnnot := PDFPageAppendAnnotationStampWithDrawBoxM( Page.Parent, Page.Idx, @Box, PAnsiChar ( Content), Length ( Content ), Integer ( Name ), Resolution, @PB );
    end else
    begin
        ps := StringToPDFString( Content, Charset );
        FAnnot := PDFPageAppendAnnotationStampWithDrawBoxM ( Page.Parent, Page.Idx, @Box, PAnsiChar ( @ps[0] ), Length ( ps ), Integer ( Name ), Resolution, @PB );
    end;
   PaintBox := TPDFPaintBox.Create(nil, PB);
end;
constructor TPDFAnnotationStamp.Create(Page : TPDFPage; Box : TPDFRect; Name : TPDFAnnotationStampName; Resolution:Cardinal; var PaintBox: TPDFPaintBox;Content : WideString);
var
  ps: PDFString;
  PB:Pointer;
begin
    ps := WideStringToPDFString( Content );
    FAnnot := PDFPageAppendAnnotationStampWithDrawBoxM( Page.Parent, Page.Idx, @Box, PAnsiChar ( @ps[0] ), Length ( ps ), Integer ( Name ), Resolution, @PB );
    PaintBox := TPDFPaintBox.Create(nil, PB);
end;


{ TPDFActionGoTo }

constructor TPDFActionGoTo.Create(Document: TPDFDocument;
  Destination: TPDFDestination);
begin
    FAction := PDFActionNewGoToDestination( Document.Handle,Pointer ( Destination ) );
end;

{ TPDFActionGoToRemote }

constructor TPDFActionGoToRemote.Create(Document: TPDFDocument;
  AFileName: AnsiString; Destination: TPDFDestination; AInNewWindow: Boolean);
begin
    FAction := PDFActionNewGoToRemote( Document.Handle, PAnsiChar (AFileName), Length ( AFileName ),
        Pointer ( Destination ), AInNewWindow)
end;

{ TPDFActionLaunch }

constructor TPDFActionLaunch.Create(Document: TPDFDocument; FileName,
  DefaultDir, Operation, Params: AnsiString; InNewWindow: Boolean);
begin
    FAction := PDFActionNewLaunch( Document.Handle, PAnsiChar ( FileName ), PAnsiChar ( DefaultDir ), PAnsiChar ( Operation ),
        PAnsiChar ( Params ), InNewWindow );
end;

{ TPDFActionURL }

constructor TPDFActionURL.Create(Document: TPDFDocument; AURL: AnsiString;
  AIsMap: Boolean);
begin
    FAction := PDFActionNewURI( Document.Handle, PAnsiChar ( AURL ), Length (AURL ), AIsMap );
end;

{ TPDFActionHide }

procedure TPDFActionHide.Append(AnnotationName: AnsiString);
begin
    PDFActionHideAddAnnotationName( FAction, PAnsiChar ( AnnotationName), Length ( AnnotationName ) );
end;

procedure TPDFActionHide.Append(Annotation: TPDFAnnotation);
begin
    PDFActionHideAddAnnotation( FAction, Annotation.Handle );
end;

constructor TPDFActionHide.Create(Document: TPDFDocument;
  AIsHide: Boolean);
begin
    FAction := PDFActionNewHide ( DOcument.Handle, AIsHide );
end;

{ TPDFActionNamed }

constructor TPDFActionNamed.Create(Document: TPDFDocument;
  ANamedType: TPDFNamedActionType);
begin
    FAction := PDFActionNewNamed( Document.Handle, Integer ( ANamedType ) );
end;

{ TPDFActionSubmitForm }

procedure TPDFActionSubmitForm.Append(AnnotationName: AnsiString);
begin
    PDFActionSubmitFormAddAnnotationName( FAction, PAnsiChar ( AnnotationName), Length ( AnnotationName ) );
end;

procedure TPDFActionSubmitForm.Append(Annotation: TPDFAnnotation);
begin
    PDFActionSubmitFormAddAnnotation( FAction, Annotation.Handle );
end;

constructor TPDFActionSubmitForm.Create(Document: TPDFDocument;
  AURL: AnsiString; AFlags: TPDFSubmitFormFlags);
var
    I: Integer;
begin
    I := 0;
    if vsffExclude in AFlags then  I := I or sffExclude;
    if vsffIncludeNoValueFields in AFlags then  I := I or sffIncludeNoValueFields;
    if vsffExportFormat in AFlags then  I := I or sffExportFormat;
    if vsffGetMethod in AFlags then  I := I or sffGetMethod;
    if vsffSubmitCoordinates in AFlags then  I := I or sffSubmitCoordinates;
    if vsffXML in AFlags then  I := I or sffXML;
    if vsffIncludeAnnotations in AFlags then  I := I or sffIncludeAnnotations;
    if vsffSubmitPDF in AFlags then  I := I or sffSubmitPDF;
    FAction := PDFActionNewSubmitForm( Document.Handle, PAnsiChar (AURL), Length ( AURL ), I );
end;

{ TPDFActionResetForm }

procedure TPDFActionResetForm.Append(AnnotationName: AnsiString);
begin
    PDFActionResetFormAddAnnotationName( FAction, PAnsiChar ( AnnotationName), Length ( AnnotationName ) );
end;

procedure TPDFActionResetForm.Append(Annotation: TPDFAnnotation);
begin
    PDFActionResetFormAddAnnotation( FAction, Annotation.Handle );
end;

constructor TPDFActionResetForm.Create(Document: TPDFDocument;
  Exclude: Boolean);
begin
    FAction := PDFActionNewResetForm( Document.Handle, Exclude );
end;

{ TPDFActionImportData }

constructor TPDFActionImportData.Create(Document: TPDFDocument;
  AFileName: AnsiString);
begin
    FAction := PDFActionNewImportData( Document.Handle, PAnsiChar ( AFileName ), Length ( AFileName ) );
end;

{ TPDFActionJavaScript }

constructor TPDFActionJavaScript.Create(Document: TPDFDocument;
  AJavaScriptString: AnsiString);
begin
    FAction := PDFActionNewJavaScript( Document.Handle, PAnsiChar ( AJavaScriptString ), Length ( AJavaScriptString ) );
end;

constructor TPDFActionJavaScript.Create(Document: TPDFDocument;
  AJavaScriptStream: TPDFCosStream);
begin
    FAction := PDFActionNewJavaScriptStream( Document.Handle, AJavaScriptStream.Handle );
end;

end.
