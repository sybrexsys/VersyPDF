#pragma once
#include "VSLibA.h"
#include "VSDocA.h"
#include "VSDocAI.h"
#include "VSExcept.h"	
#include "VSError.h"
#include "VSPageA.h"
#include "VSCanvasA.h"
#include "VSBaseA.h"
#include "VSFontA.h"
#include "VSGStateA.h"
#include "VSImageA.h"
#include "VSCosA.h"
#include "VSCosAI.h"
#include "VSThreadA.h"
#include "VSPagesA.h"
#include "VSActionA.h"
#include "VSOutlineA.h"
#include "VSAcroFormA.h"
#include "VSAnnotA.h"
#include "VSAcroInfoA.h"
#include <vcclr.h>
#include "VersyPDFTypes.h"

using namespace System;
using namespace System::Runtime::InteropServices;

#define PDFStrToPCharB( str, ch ) \
    char* ch; \
    ppBool IsAnsi;\
    ppInt32 StrLen; \
    const __wchar_t __pin * wch = PtrToStringChars(str); \
    IsAnsi = IsANSICodesOnly ( wch, str->Length ); \
    if ( IsAnsi ){ \
        ch = (char*)(void*)Marshal::StringToHGlobalAnsi(str);\
        StrLen = str->Length; \
    } else {\
        StrLen = ( str->Length << 1 )+ 2; \
        ch = new char [ StrLen ];\
        ch[0] = (unsigned char)(0xFE);\
        ch[1] = (unsigned char)(0xFF);\
        memcpy ( &(ch[2]), wch, str->Length << 1 ); \
    }
    


#define PDFStrToPCharE( ch ) \
    if ( IsAnsi ) \
        Marshal::FreeHGlobal(ch); \
    else \
        delete [] ch ; \
/*


#define PDFStrToPCharCheckOnlyB( str, ch ) \
    char* ch; \
    ppBool IsAnsi;\
    const __wchar_t __pin * wch = PtrToStringChars(str); \
    IsAnsi = IsANSICodesOnly ( wch, str->Length ); \
    if ( IsAnsi ){ \
        ch = (char*)(void*)Marshal::StringToHGlobalAnsi(str);\
    };



#define PDFStrToPCharCheckOnlyE( ch ) \
    if ( IsAnsi ) \
        Marshal::FreeHGlobal(ch); \
*/

#define PDFStrToPCharCheckOnlyB( str, ch ) \
	char* ch = NULL; \
	ppBool IsAnsi;\
	const __wchar_t __pin * wch = PtrToStringChars(str); \
	IsAnsi = IsANSICodesOnly ( wch, str->Length ); \
	if ( IsAnsi ){ \
	ch = new char [ str->Length +1 ];\
	ch[str->Length] = 0;\
	cpyw(ch,wch,str->Length);\
	};\



#define PDFStrToPCharCheckOnlyE( ch ) \
	if ( IsAnsi ) \
	delete [] ch; \


#define OSStrToPCharB( str, ch )   char* ch = (char*)(void*)Marshal::StringToHGlobalAnsi(str)
#define OSStrToPCharE( ch ) Marshal::FreeHGlobal(ch)

#define StrToWChar( str, wch ) const __wchar_t __pin * wch = PtrToStringChars(str)

#define BOOL(x)(x?True:)

#define RGB_TO_INTENSITY_FACTOR 0.003921568627450980392156862745098; // 1/255 value to convert 0..255 values to 0..1 values

/// <summary>
/// Bytescout.PDF is a namespace with Bytescout.PDF library classes: write/read/modify/sign PDF documents using this .NET library
/// </summary>
namespace Versy
{
namespace PDF
{
	public __gc class CosObjectBase;
    public __gc class Bead;
    public __gc class Thread;
    public __gc class Action;
	public __gc class Page;
    public __gc class Annotation;
    public __gc class Outline;
	public __gc class Destination;
	public __gc class ControlList;
	public __gc class Document;
	public __gc class Image;
	public __gc class PDFDataStream;

    ppBool IsANSICodesOnly(const __wchar_t * Codes, ppUns32 Length);
	void cpyw (char *s, const __wchar_t * Codes, ppUns32 Length);
    CosObjectBase * PDFCosHandleToClass( PDFCosHandle Handle );
    Action * PDFActionHandleToClass ( PDFActionHandle Handle );
    Annotation * PDFAnnotationHandleToClass ( PDFAnnotationHandle Handle );
    String * PDFStringToString( char * Str, ppInt32 Length);

/// <summary>
/// PDFDocEngine is the main static class with the engine working with PDF documents. This engine must be initialized first before working with Document and other classes
/// </summary>
    public __gc class PDFDocEngine: public IDisposable {
        PDFLibHandle FLib;
private:
		bool disposed;

protected:
		virtual void Dispose(bool /*disposeManagedResources*/)
		{
			// process only if mananged and unmanaged resources have
			// not been disposed of.
			if (!this->disposed)
			{
		  
			// DISPOSING OBJECTS

			PDFLibHandle LB = FLib;
			if ( FLib )
				DonePDFLibrary( &LB );


		    disposed=true;
			}
		}

	private public: 
		__property PDFLibHandle get_Handle () { return FLib; };
    public:

   void Dispose()
   {
      Dispose(true);
      // tell the GC that the Finalize process no longer needs
      // to be run for this object
      System::GC::SuppressFinalize(this);
   }


/// <summary>
/// PDFDocEngine class constructor initialization (main static class containing engine to write/read/modify/sign PDF documents). In Demo version of the library provide empty license name and license key
/// </summary>
/// <param name="LicenseName">License Name for check license information.</param>
/// <param name="LicenseKey">License key for check license information.</param>
        PDFDocEngine  ( String * Name, String * LicenseKey)
		{
			Name;
			LicenseKey;

			disposed = false;
			/*
			OSStrToPCharB ( Name, ln );
            OSStrToPCharB ( Serial, lk );
            try {
			*/
                FLib = InitPDFLibrary ("", "");
			/*
            } __finally {
                OSStrToPCharE ( ln );
                OSStrToPCharE ( lk );
            }
			*/
			if (! FLib )
				throw new PDFException ("Cannot startup PDF engine" );

        };
        ~PDFDocEngine  (){
			// call Dispose with false.  Since we're in the
			// destructor call, the managed resources will be
			// disposed of anyways.
			Dispose(false);
        };
/// <summary>
/// Internally used function for converting cos object string into atom 
/// </summary>
/// <param name="Str">String object Name</param>
/// <returns>Atom Key</returns>
        ppAtom StringToAtom ( String * Str ){
            OSStrToPCharB ( Str, S );
            ppAtom R;
            try{
                R = ULStringToAtom( FLib, S );
            } __finally {
                OSStrToPCharE ( S );
            }
            return R;
        }
/// <summary>
/// Internally used function for converting cos object atom into string
/// </summary>
/// <param name="Atom">Atom Key</param>
/// <returns>String object name</returns>
        String * AtomToString ( ppAtom Atom ){
            char * S;
            String * Str;
            S = ULAtomToString( FLib, Atom );
            Str = new String ( S );
            return Str;
        }
/// <summary>
/// Checks if atom exists in the list of available atoms for string object
/// </summary>
/// <param name="Str">String object name</param>
/// <returns>True if exists, false if not found</returns>
		ppBool AtomExistsForString ( String * Str ){
            OSStrToPCharB ( Str, S );
            ppBool R;
            try{
                R = ULExistsAtomForString( FLib, S );
            } __finally {
                OSStrToPCharE ( S );
            }
            return R;
        }

/// <summary>
/// Creates new empty document (if you need to create new document)
/// </summary>
/// <returns>Document object to operate with created document</returns>
		Document* AddDocument();
/// <summary>
/// Reads existing document from PDF stream object and creates new Document object
///	IMPORTANT: In the Full version of the product use of this method/property requires an additional "Existing PDF Support License" license
/// </summary>
/// <param name="AStream">PDF stream containing existing PDF document</param>
/// <returns>Document object to operate with the document</returns>
		Document* AddDocument(PDFDataStream * AStream);
/// <summary>
/// Reads existing document from the  and creates new new document object already containing existing document pages and data. IMPORTANT: In the Full version of the product use of this method/property requires an additional "Existing PDF Support License" license
/// <param name="FileName">Filename of existing PDF document to read</param>
/// </summary>
/// <returns>Document object to operate with the document</returns>
		Document* AddDocument(String * FileName);
/// <summary>
/// Reads existing document from the buffer and creates new new document object already containing existing document pages and data
///	IMPORTANT: In the Full version of the product use of this method/property requires an additional "Existing PDF Support License" license
/// <param name="Buffer">Byte buffer containing existing PDF document</param>
/// <param name="BufferSize">Buffer size</param>
/// </summary>
/// <returns>Document object to operate with the document</returns>
		Document* AddDocument(unsigned char Buffer __gc[], int BufferSize);

		Image* AddImage(ppUns32 Width, ppUns32 Height, ColorBitDepth Depth, ColorSpaceType Space);
		Image* AddImage(String * FileName, ppUns32 Index);
		Image* AddImage(PDFDataStream * AStream, ppUns32 Index);
		Image* AddImage(unsigned char Buffer __gc[], ppUns32 BufferSize, ppUns32 Index);
		Image* AddImage(HBITMAP BitmapHandle);

		PDFDataStream * AddPDFDataStream ();
		/// <summary>
		/// IMPORTANT: In the Full version of the product use of this method/property requires an additional "Existing PDF Support License" license
		/// <summary>
		PDFDataStream * AddPDFDataStream (String * FileName, FileOpenMode Mode );
		/// <summary>
		/// IMPORTANT: In the Full version of the product use of this method/property requires an additional "Existing PDF Support License" license
		/// <summary>
		PDFDataStream * AddPDFDataStream (unsigned char Source __gc[], ppInt32 Length );
	};


/// <summary>
/// PDF stream object class
/// </summary>
    public __gc class PDFDataStream : public IDisposable{
        PDFStreamHandle stream;
        PDFDocEngine  * Lib;
        ppUns8 *Buf;

private:
		bool disposed;

protected:
		virtual void Dispose(bool /*disposeManagedResources*/)
		{
			// process only if mananged and unmanaged resources have
			// not been disposed of.
			if (!this->disposed)
			{
		  
			// DISPOSING OBJECTS

			Lib = NULL;
            
            if ( Buf )
                delete [] Buf;

		    disposed=true;
			}
		}

	private public: 
		__property PDFStreamHandle get_Handle(){
			return stream;
		}
		PDFDataStream ( PDFStreamHandle AStream ){
			disposed = false;
			Lib = NULL;
			Buf = NULL;
			stream = AStream;
		}
    public:

   void Dispose()
   {
      Dispose(true);
      // tell the GC that the Finalize process no longer needs
      // to be run for this object
      System::GC::SuppressFinalize(this);
   }


/// <summary>
/// Creates new in-memory PDF stream object
/// </summary>
/// <param name="Library">existing PDFDocEngine object</param>
        PDFDataStream( PDFDocEngine  * Library){
			disposed = false;
            Lib = Library;
            stream = ULStreamMemNew( Library->Handle, 0 );
            Buf = NULL;
        }
/// <summary>
/// Creates new PDF stream object based on existing PDF document file 
/// </summary>
/// <param name="Library">existing PDFDoc Engine object</param>
/// <param name="FileName">Filename of existing PDF document file</param>
/// <param name="Mode">File open mode (reading or writing)</param>
        PDFDataStream ( PDFDocEngine  * Library, String * FileName, FileOpenMode Mode ){
			disposed = false;
            Lib = Library;
            Buf = NULL;
            OSStrToPCharB( FileName, fn );
            try {
                stream = ULStreamFileNew(Library->Handle, fn, (ppFileOpenMode)Mode );
            } __finally {
                OSStrToPCharE ( fn );
            }
        }
/// <summary>
/// Converts memory buffer into PDF stream object
/// </summary>
/// <param name="Library">existing PDFDocEngine class object</param>
/// <param name="Source">Source byte buffer to read from</param>
/// <param name="Length">Buffer size to read (in bytes)</param>
		PDFDataStream ( PDFDocEngine  * Library, unsigned char Source __gc[], ppInt32 Length ){
			disposed = false;
            Lib = Library;
            Buf = new ppUns8 [ Length ];
            Marshal::Copy( Source, 0, Buf, Length);
            stream = ULStreamMemReadOnlyNew( Library->Handle, Buf, Length );
        }
        ~PDFDataStream(){
			// call Dispose with false.  Since we're in the
			// destructor call, the managed resources will be
			// disposed of anyways.
			Dispose(false);
        }  
/// <summary>
/// Reads current position in stream ( offset from the beginning)
/// </summary>
        __property ppInt32 get_Position (){
            return ULStreamGetPosition( stream );
        }
/// <summary>
/// Set current position (offset) in the stream 
/// </summary>
/// <param name="Pos">New position ( offset from the beginning )</param>
		__property void set_Position (ppInt32 Pos){
            ULStreamSetPosition(stream, Pos);
        }
/// <summary>
/// Gets stream size in bytes.
/// </summary>
        __property ppInt32 get_Size ( ){
            return ULStreamGetSize( stream );
        }
/// <summary>
/// Set new stream size (in bytes)
/// </summary>
/// <param name="Size">New size</param>
        __property void set_Size ( ppInt32 Size ){
            ULStreamSetSize( stream, Size );
        }
/// <summary>
/// Clear PDF stream content and set size to 0
/// </summary>
        void Clear (){
            ULStreamClear( stream, 0 );
        }
/// <summary>
/// Closes PDF stream.
/// </summary>
/// <returns>None.</returns>
		void Close (){
			ULStreamClose( stream );
		}
/// <summary>
/// Copy data from current stream object into another existing stream object
/// </summary>
/// <param name="TargetStream">Target PDF stream object</param>
/// <returns>None</returns>
        void CopyTo ( PDFDataStream * TargetStream ){
            ULStreamCopyToStream( stream, TargetStream->Handle );
        }
/// <summary>
/// Read given number of bytes from buffer into stream 
/// </summary>
/// <param name="Buffer">Byte buffer</param>
/// <param name="Count">Number of bytes to read</param>
/// <returns>Number of bytes successfully read</returns>
        ppInt32 ReadBuffer ( unsigned char Buffer __gc[], ppInt32 Count ){
            ppUns8 * buf = new ppUns8 [ Count ];
            ppInt32 Res;
            try{
				Res = ULStreamReadBuffer( stream, buf, Count );
                Marshal::Copy( buf, Buffer, 0, Count);
            } __finally {
                delete [] buf;
            }
            return Res;
        }
/// <summary>
/// Writes from memory buffer to PDF stream some count of the bytes.
/// </summary>
/// <param name="Buffer">Memory buffer with data.</param>
/// <param name="Count">Count of bytes which we want to write instream.</param>
/// <returns>Count of written bytes in stream.</returns>
        ppInt32 WriteBuffer ( unsigned char Buffer __gc[], ppInt32 Count ){
            ppUns8 * buf = new ppUns8 [ Count ];
            ppInt32 Res;
            try{
                Marshal::Copy( Buffer, 0, Buf, Count);
                Res = ULStreamWriteBuffer( stream, Buf, Count );
            } __finally {
                delete [] buf;
            }
            return Res;
        }
/// <summary>
/// Read one single byte from the stream
/// </summary>
/// <returns>Single byte (char) from stream. Returns -1 if the end of the stream is reached already</returns>
        ppInt32 ReadChar(){
            return ULStreamReadChar( stream );
        }
/// <summary>
/// Reads one character from stream. Same as ULStreamReadChar, onlystream position stays on that place.
/// </summary>
/// <returns>One character form stream. If it returns -1 than it is EOF ( end of stream ).</returns>
        ppInt32 ReadChar2(){
            return ULStreamLookChar( stream );
        }
/// <summary>
/// Writes one single byte into stream
/// </summary>
/// <param name="Character">Byte (character) to write into stream</param>
/// <returns>Number of bytes written to the stream</returns>
        ppInt32 WriteChar( char Character ){
            return ULStreamWriteChar( stream, Character );
        }
    }; 
/// <summary>
/// Image object class desigined to handle images for PDF documents
/// </summary>
    public __gc class Image: public IDisposable
{
        PDFImageHandle image;
        PDFDocEngine  * Lib;
protected:
		virtual void Dispose(bool /*disposeManagedResources*/)
		{
			// process only if mananged and unmanaged resources have
			// not been disposed of.
			if (!this->disposed)
			{
		  
			// DISPOSING OBJECTS

			if ( image )
				PDFImageFree ( image );
            Lib = NULL;


		    disposed=true;
			}
		}
private:
		bool disposed;

	private public:
		__property PDFImageHandle get_Handle(){
			return image;
		}
    public:

   void Dispose()
   {
      Dispose(true);
      // tell the GC that the Finalize process no longer needs
      // to be run for this object
      System::GC::SuppressFinalize(this);
   }

/// <summary>
/// Creates new Image object
/// </summary>
/// <param name="Library">Exsiting PDFDocEngine object</param>
/// <param name="Width">Width of the image (pixels)</param>
/// <param name="Height">Height of the image (pixels)</param>
/// <param name="Depth">Color bit depth of the image</param>
/// <param name="Space">Color space type for use in new image object</param>
/// <returns>new Image object</returns>
        Image( PDFDocEngine  * Library, ppUns32 Width, ppUns32 Height, ColorBitDepth Depth, ColorSpaceType Space){
            disposed = false;
			Lib = Library;
            image = PDFImageCreate( Lib->Handle, Width, Height, (ppComponentDepth)Depth, (TPDFColorDevice) Space );
        };
/// <summary>
/// Creates new Image object from existing image file 
/// </summary>
/// <param name="Library">Existing PDFDocEngine class object</param>
/// <param name="FileName">Filename of the existing image file (BMP, JPG, TIFF) to read</param>
/// <param name="Index">(TIFF image files only) Index of frame to read from TIFF image file</param>
        Image( PDFDocEngine  * Library, String * FileName, ppUns32 Index){
			disposed = false;           
			OSStrToPCharB ( FileName, fn );
            Lib = Library;
            try {
                image = PDFImageLoadFromFile( Lib->Handle, fn, Index );
            } __finally {
                OSStrToPCharE ( fn );
            }          
        };
/// <summary>
/// Creates new Image object from the PDF stream
/// </summary>
/// <param name="Library">Existing PDFDocEngine object</param>
/// <param name="AStream">Existing PDFDataStream object to read image from</param>
/// <param name="Index">(TIFF images only) Index of the frame to read from multi-page TIFF image</param>
        Image( PDFDocEngine  * Library, PDFDataStream * AStream, ppUns32 Index){
            disposed = false;
			Lib = Library;
            image = PDFImageLoadFromStream( Lib->Handle, AStream->Handle, Index );
        };
/// <summary>
/// Creates new Image object from existing buffer with image data
/// </summary>
/// <param name="Library">Existing PDFDocEngine class object</param>
/// <param name="Buffer">Buffer with image data (byte buffer)</param>
/// <param name="BufferSize">Size of buffer (in bytes)</param>
/// <param name="Index">(TIFF only): index of frame in TIFF image for muli-page TIFF files</param>
        Image( PDFDocEngine  * Library, unsigned char Buffer __gc[], ppUns32 BufferSize, ppUns32 Index){
            disposed = false;
			Lib = Library;
            ppUns8 *buf = new ppUns8 [BufferSize];
            try{
                Marshal::Copy ( Buffer, 0, buf, BufferSize );
                image = PDFImageLoadFromBuffer( Lib->Handle, buf, BufferSize, Index );
            } __finally {
                delete [] buf;
            }
            
        };
/// <summary>
/// Creates new Image object from existing Bitmap handle (Windows Bitmap handle) 
/// </summary>
/// <param name="Library">Existing PDFDocEngine object</param>
/// <param name="Handle">Windows Bitmap handle</param>
        Image( PDFDocEngine  * Library, HBITMAP BitmapHandle){
            disposed = false;
			Lib = Library;
            image = PDFImageLoadFromHandle( Lib->Handle, BitmapHandle );
        };
        ~Image(){
			// call Dispose with false.  Since we're in the
			// destructor call, the managed resources will be
			// disposed of anyways.
			Dispose(false);
        };
/// <summary>
/// Reads number of frames in existing TIFF image file 
/// </summary>
/// <param name="Library">Existing PDFDocEngine object</param>
/// <param name="FileName">Filename of existing image</param>
/// <returns>Number of frames in TIFF image (1 or more)</returns>
        static ppUns32 GetTIFFFramesCount ( PDFDocEngine  * Library, String * FileName){
            OSStrToPCharB ( FileName, fn );
            ppUns32 R;
            try {
                R = PDFImageGetTIFFCountFromFile( Library->Handle, fn );
            } __finally {
                OSStrToPCharE ( fn );
            }    
            return R;
        };
/// <summary>
/// Reads number of frames in existing TIFF image contained in PDF stream
/// </summary>
/// <param name="Library">Existing PDFDocEngine object</param>
/// <param name="AStream">Existing PDF stream object with TIFF image data</param>
/// <returns>Number of frames in TIFF image (1 or more)</returns>
        static ppUns32 GetTIFFFramesCount( PDFDocEngine  * Library, PDFDataStream * AStream){
            return  PDFImageGetTIFFCountFromStream( Library->Handle, AStream->Handle );
        };
/// <summary>
/// Reads number of frames in existing TIFF image contained in PDF stream
/// </summary>
/// <param name="Library">Existing PDFDocEngine object</param>
/// <param name="Buffer">Buffer (bytes) with TIFF image data</param>
/// <param name="BufferSize">Size (in bytes) of the buffer with TIFF image data</param>
/// <returns>Number of frames in TIFF image (1 or more)</returns>
        static ppUns32 GetTIFFFramesCount( PDFDocEngine  * Library,unsigned char Buffer __gc[], ppUns32 BufferSize){
            ppUns32 Res;
            ppUns8 *buf = new ppUns8 [BufferSize];
            try{
                Marshal::Copy( Buffer, 0, buf, BufferSize );
                Res = PDFImageGetTIFFCountFromBuffer( Library->Handle, buf, BufferSize );
            } __finally {
                delete [] buf;
            }
            return Res;
        };
/// <summary>
/// Returns color space type used in the current image
/// </summary>
        __property ColorSpaceType get_ColorSpace (){
            return (ColorSpaceType) PDFImageGetColorDevice( image );
        }
/// <summary>
/// Returns color bit depth used in the current image
/// </summary>
        __property ColorBitDepth get_Depth (){
            return (ColorBitDepth) PDFImageGetDepth( image );
        }
/// <summary>
/// Returns width of the image
/// </summary>
        __property ppUns32 get_Width (){
            return  PDFImageGetWidth( image );
        }
/// <summary>
/// Returns height of the image
/// </summary>
        __property ppUns32 get_Height (){
            return  PDFImageGetHeight( image );
        }
/// <summary>
/// Provides direct access to the lines (scanlines) of the image
/// </summary>
/// <param name="Index">Index of the scan line to read</param>
/// <returns>Pointer to the buffer with scan line data</returns>
        void * GetScanLine(ppUns32 index){
            return PDFImageGetScanLine( image, index );
        }
        
    };
/// <summary>
/// Outline object class. Outlines are used in PDF bookmarks tree to provide fast access to the topics in PDF document (outlines are displayed on the left panel in PDF reader applications)
/// </summary>
    public __gc class Outline{
	private:    
		PDFOutlineHandle outline;
	private public:
		__property PDFOutlineHandle get_Handle(){
			return outline;
		}
		Outline( PDFOutlineHandle Handle ){
			outline = Handle;
		}
    public:

/// <summary>
/// Creates new outline and adds this outline to the parent of current outline so new outline will become a sibling to the current
/// </summary>
/// <returns>New Outline object</returns>
        Outline * AddSiblingToParentOutline (){
            return new Outline ( PDFOutlineAddNewSibling( outline ) );
        }
/// <summary>
/// Creates and adds new outline right after the current outline so new outline becomes next outline
/// </summary>
/// <returns>New Outline object</returns>
        Outline * AddNextToParentOutline (){
            return new Outline ( PDFOutlineAddNewNext( outline ) );
        }
/// <summary>
/// Creates new outline and inserts new outline right before the current outline so new outline becomes previous outline for the current
/// </summary>
/// <returns>New Outline object</returns>
        Outline * AddPreviousToParentOutline (){
            return new Outline ( PDFOutlineAddNewPrev( outline ) );
        }
/// <summary>
/// Creates and adds new outline as a child for the current outline
/// </summary>
/// <returns>New Outline object</returns>
        Outline * AddChild (){
            return new Outline ( PDFOutlineAddNewChild(outline ) );
        }

/// <summary>
/// Deletes the outline from the list of outlines
/// </summary>
        void Delete (){			
			PDFOutlineDelete(outline);
        }

/// <summary>
/// Returns the first child outline (if outline has children)
/// </summary>
        __property Outline * get_First(){
            return new Outline ( PDFOutlineGetFirstChild(outline ) );
        }
/// <summary>
/// Returns the last child outline (if outline has children)
/// </summary>
        __property Outline * get_Last(){
            return new Outline ( PDFOutlineGetLastChild(outline ) );
        }
/// <summary>
/// Returns previous outline
/// </summary>
        __property Outline * get_Previous(){
            return new Outline ( PDFOutlineGetPrev(outline ) );
        }
/// <summary>
/// Returns next outline 
/// </summary>
        __property Outline * get_Next(){
            return new Outline ( PDFOutlineGetNext(outline ) );
        }

/// <summary>
/// Returns parent outline
/// </summary>
        __property Outline * get_Parent(){
            return new Outline ( PDFOutlineGetParent(outline ) );
        }
/// <summary>
/// Indicates if current outline has children or not
/// </summary>
        __property ppBool get_HasChildren(){
            return PDFOutlineHasChildren( outline ) ;
        }
/// <summary>
/// Returns number of children of current outline
/// </summary>
        __property ppInt32 get_Count(){
            return PDFOutlineGetCount( outline ) ;
        }
/// <summary>
/// Sets default expanded state to True or False for the current outline
/// </summary>
/// <param name="Value">True to expand outline by default or False to collaps by default</param>
        __property void set_Expanded (ppBool Value) {
            PDFOutlineSetExpanded( outline, Value );
        }      
/// <summary>
/// Sets outline settings flags for the current outline
/// </summary>
/// <param name="Flags">Outline settings flags</param>
        __property void set_Flags (ppInt32 Value) {
            PDFOutlineSetFlags( outline, Value );
        }
/// <summary>
/// Sets title of the outline
/// </summary>
/// <param name="Value">New outline title</param>
        __property void set_Title( String * Value ){
            PDFStrToPCharB( Value, v );
            try {
                PDFOutlineSetTitle( outline, v, StrLen );
            } __finally {
                PDFStrToPCharE ( v );
            }
        }       
/// <summary>
/// Sets color of outline. Use CreateRGBColor() to create new RGB color
/// </summary>
/// <param name="Value">New outline color</param>
        __property void set_Color( RGBColor Value ){
            TPDFColor C;
            C.Device = cgRGB;
            C.Color.RGB.R = Value.R;
            C.Color.RGB.G = Value.G;
            C.Color.RGB.B = Value.B;
            PDFOutlineSetColor( outline, &C );
        }
/// <summary>
/// Sets action to execute on outline click (for example you can set to open URL on outline (bookmark) click)
/// </summary>
/// <param name="Value">Action object to set to outline</param>
        __property void set_action ( Action * Value);
/// <summary>
/// Sets destination object for the outline (destination defines how to display target page or part of the page)
/// </summary>
/// <param name="Value">Destination object linked to the outline</param>
        __property void set_destination ( Destination * Value);
    };

/// <summary>
/// Document class objects to work with PDF document. Class provides methods to add pages, sign, linearize, protect document
/// To create new empty document use PDFDocEngine.AddDocument 
/// To create Document object from existing PDF document use PDFDocEngine.AddDocument
/// </summary>
public __gc class Document: public IDisposable
	{
        PDFDocHandle FDoc;
        PDFDocEngine  * FLibrary;
        ppUns8 *Buf;
		ControlList *FAcro;
		bool disposed;

	private:
		void SetInitialProperties(); // set initial properties of the document
	private public:
		__property PDFDocHandle get_Handle () { 
			return FDoc; 
		};
		__property PDFDocEngine  * get_Owner (){
			return FLibrary;
		}
	protected:
	virtual void Dispose(bool /*disposeManagedResources*/)
    {
	 // process only if mananged and unmanaged resources have
     // not been disposed of.

      if (!this->disposed)
	  {
		    if ( FDoc )
				PDFDocClose ( FDoc );
            if ( Buf )
                delete [] Buf;
            FLibrary = NULL;
			FAcro = NULL;

		    disposed=true;
	  }
	}
    public:

   void Dispose()
   {
      Dispose(true);
      // tell the GC that the Finalize process no longer needs
      // to be run for this object.
	  System::GC::SuppressFinalize(this);
   }



/// <summary>
/// Create New PDF Document.
/// </summary>
/// <param name="Library">PDF Library object.</param>

        Document ( PDFDocEngine * Library){
			disposed=false;
            FLibrary = Library;
            Buf = NULL;
            FDoc = PDFDocCreate ( Library->Handle );
			SetInitialProperties(); // set initial properties
        };
/// <summary>
/// Load PDF Document from file. IMPORTANT: In the Full version of the product use of this method/property requires an additional "Existing PDF Support License" license
/// </summary>
/// <param name="Library">PDF Library object.</param>
/// <param name="FileName">Name of input file.</param>

        Document ( PDFDocEngine  * Library, String * FileName ){
			disposed=false;
			FLibrary = Library;
            Buf = NULL;
            OSStrToPCharB ( FileName, fn);
            try {
                FDoc = PDFDocLoadFromFile ( Library->Handle, fn );
				SetInitialProperties(); // set initial properties
            } __finally {
                OSStrToPCharE ( fn );
            }
        };

/// <summary>
/// Load PDF Document from memory stream or file stream. IMPORTANT: In the Full version of the product use of this method/property requires an additional "Existing PDF Support License" license
/// </summary>
/// <param name="Library">PDF Library object.</param>
/// <param name="AStream">PDF stream object.</param>

        Document ( PDFDocEngine  * Library, PDFDataStream * AStream ){
			disposed=false;
            FLibrary = Library;
            Buf = NULL;
            FDoc = PDFDocLoadFromStream ( Library->Handle, AStream->Handle );
			SetInitialProperties(); // set initial properties
        };
/// <summary>
/// Load PDF Document from Memory buffer. IMPORTANT: In the Full version of the product use of this method/property requires an additional "Existing PDF Support License" license
/// </summary>
/// <param name="Library">PDF Library object.</param>
/// <param name="Buffer">Pointer on PDF document beginning.</param>
/// <param name="BufferSize">Length of buffer in bytes.</param>

        Document ( PDFDocEngine  * Library, unsigned char Buffer __gc[], int BufferSize ){
			disposed=false;
			FLibrary = Library;
            Buf = new ppUns8 [ BufferSize ];
            try{
                Marshal::Copy( Buffer,0, Buf, BufferSize );
                FDoc = PDFDocLoadFromBuffer ( Library->Handle, Buf, BufferSize );
				SetInitialProperties(); // set initial properties
            } catch (...) {
                delete [] Buf;
                throw;
            }
        };
        ~Document (){

			// call Dispose with false.  Since we're in the
			// destructor call, the managed resources will be
			// disposed of anyways.
			Dispose(false);
        };       

/// <summary>
/// Returns Root COS object of the document
/// </summary>
		CosObjectBase * Document::GetRootCosObject() {
			PDFCosHandle R = PDFDocGetRoot(FDoc);
			return PDFCosHandleToClass(R);
		} 

/// <summary>
/// Remove COS objects dictionary using using dictionary key string
/// </summary>
		void Document::RemoveCOSDictionaryObjectByKey(String * Key);

/// <summary>
/// Removes all outlines from PDF document
/// </summary>
	void Document::RemoveAllOutlines();

/// <summary>
/// Removes all permissions from digital signature from the document
/// </summary>
	void Document::RemoveAllPermissions();	

/// <summary>
/// Add page in PDF Document.
/// </summary>
/// <param name="Width">Width of page.</param>
/// <param name="Height">Height of page.</param>
/// <returns>Index of New page in Document.</returns>
        //ppUns32 AddPage ( ppReal Width, ppReal Height ){
        //    return PDFDocAppendPage( FDoc, Width, Height );
        //};
/// <summary>
/// Add page in PDF Document.
/// </summary>
/// <param name="Size">Type of page Size.</param>
/// <param name="Orientation">Orientation of page.</param>
/// <returns>Index of New page in Document.</returns>
        //ppUns32 AddPage ( PageSizeType Size, PageOrientationType Orientation ){
        //    return PDFDocAppendPage2( FDoc, ( TPDFPageSize ) Size, ( TPDFPageOrientation ) Orientation );
        //};

		Page* AddPage( ppReal Width, ppReal Height );
		Page* AddPage( PageSizeType Size, PageOrientationType Orientation );
		//Page* AddPage();
		//Page* AddPage(int index);

		/// <summary>
/// Save PDF Document in file.
/// </summary>
/// <param name="FileName">Name of output file, text string.</param>
/// <returns>None.</returns>
        void Save ( String * FileName ){
	OSStrToPCharB ( FileName, fn );
            try {
                PDFDocSaveToFile( FDoc, fn );
            } __finally {
                OSStrToPCharE ( fn );
            }
        };
/// <summary>
/// Save PDF Document in memory stream or file stream.
/// IMPORTANT: In the Full version of the product use of this method/property requires having an additional "Web License" license
/// </summary>
/// <param name="AStream">[out] PDF stream Handle.</param>
/// <returns>None.</returns>
        void Save ( PDFDataStream * AStream ){
		PDFDocSaveToStream( FDoc, AStream->Handle );
        };


/// <summary>
/// Save PDF Document in memory stream or file stream.
/// IMPORTANT: In the Full version of the product use of this method/property requires having an additional "Web License" license
/// </summary>
/// <param name="stream">[out] System.IO.Stream stream</param>
/// <returns>None.</returns>
		void Save ( System::IO::Stream* stream ){
			PDFDataStream * AStream =
				__gc new PDFDataStream(FLibrary);

			PDFDocSaveToStream( FDoc, AStream->Handle );

			unsigned char buff __gc[] =
				new unsigned char __gc[AStream->Size]; 

			AStream->Position = 0;
			AStream->ReadBuffer(buff, AStream->Size);

			stream->Write(buff, 0, AStream->Size);
        };


/// <summary>
/// Save PDF Document in memory.
/// </summary>
/// <returns>Byte array of stored PDF document.</returns>
        unsigned char BinaryImage () __gc[]{
			PDFDataStream * AStream =
				__gc new PDFDataStream(FLibrary);

			PDFDocSaveToStream( FDoc, AStream->Handle );

			unsigned char buff __gc[] =
				new unsigned char __gc[AStream->Size]; 

			AStream->Position = 0;
			AStream->ReadBuffer(buff, AStream->Size);

			return buff;
		};
/// <summary>
/// Checking the PDF Document on Validity of Password. IMPORTANT: In the Full version of the product use of this method/property requires an additional "Existing PDF Support License" license
/// </summary>
/// <param name="Password">Password text string.</param>
/// <returns>Password Validity Type. If Invalid then Password is invalid.</returns>
        PasswordValidationType ValidatePassword ( String * Password ){
       
			TKeyValidType r;
            OSStrToPCharB ( Password, pas );
            try {
                r = PDFDocCheckPassword( FDoc, pas );
            } __finally {
                OSStrToPCharE ( pas );
            }         
            return (PasswordValidationType) r;
        };

		void RedrawAcroForm (){

			PDFDocSetAppendRedrawAcroForm(FDoc);

		};


/// <summary>
/// Set PDF format version for PDF document
/// </summary>
/// <param name="Version">Version number</param>
        __property void set_Version ( Versy::PDF::Version version ){

            PDFDocSetVersion (FDoc, (TPDFVersion) version);

        };

/// <summary>
/// Returns PDF format version used in PDF document 
/// </summary>
		__property Versy::PDF::Version get_Version (){
			
			return (Versy::PDF::Version)PDFDocGetVersion(FDoc);

		}


/// <summary>
/// Checking the PDF Document on Security. IMPORTANT: In the Full version of the product use of this method/property requires an additional "Existing PDF Support License" license
/// </summary>
        __property ppBool get_Protected (){
            return PDFDocIsCrypted ( FDoc );
        };
/// <summary>
/// Set PDF Document Security. IMPORTANT: In the Full version of the product use of this method/property requires an additional "Sign And Advanced Security License" license
/// </summary>
/// <param name="Permissions">Document Permissions's Flags.</param>
/// <param name="KeyLength">40\-bits or 128\-bits Security Key Length.</param>
/// <param name="UserPassword">User Password text string.</param>
/// <param name="OwnerPassword">Owner Password text string.</param>
/// <returns>None.</returns>
        void AddProtection (DocumentPermissionType Permissions, SecurityKeyLength KeyLength, String * UserPassword, String * OwnerPassword ){
            
			OSStrToPCharB ( UserPassword, up );
            OSStrToPCharB ( OwnerPassword, op );
            try {
                PDFDocSetSecurity( FDoc, Permissions, ( TPDFProtectionType ) KeyLength, up, op);
            } __finally {
                OSStrToPCharE ( up );
                OSStrToPCharE ( op );
            }           
        };
/// <summary>
/// Remove PDF Document Security. IMPORTANT: In the Full version of the product use of this method/property requires an additional "Existing PDF Support License" license
/// </summary>
/// <param name="Doc">PDF Document Handle.</param>
/// <returns>None.</returns>
		void RemoveProtection ()
		{

			PDFDocRemoveSecurity ( FDoc );
		}
		
/// <summary>
/// IMPORTANT: In the Full version of the product use of this method/property requires an additional "Sign And Advanced Security License" license
/// </summary>
		void SetMetaDataEncryption( ppBool CryptMetadata){
			PDFDocSetEncryptMetaData( FDoc, CryptMetadata);
		}
/// <summary>
/// Add one of 14 standard fonts to document.
/// </summary>
/// <param name="Font">One of standard font.</param>
/// <param name="Encode">Font encoding type.</param>
/// <returns>Font index in PDF document.</returns>
		ppUns32 AddFontStandard ( StandardFontType Font, FontEncodingType Encode ){
            return PDFFontStandardAppend( FDoc, (TPDFStdandardFont)Font, (TPDFEncodingType) Encode );
        }
/// <summary>
/// Add true type font to document by name.
/// </summary>
/// <param name="FontName">Font name.</param>
/// <param name="Bold">Is bold style.</param>
/// <param name="Italic">Is italic style.</param>
/// <returns>Font index in PDF document.</returns>
        ppUns32 AddFontTrueType( String * FontName, ppBool Bold, ppBool Italic ){
            OSStrToPCharB ( FontName, fn );
            ppInt32 R;
            try {
                R = PDFFontTrueTypeAppend( FDoc, fn, Bold, Italic );
            } __finally {
                OSStrToPCharE ( fn );
            }
            return R;
        }
/// <summary>
/// Add true type font to document from file.
/// </summary>
/// <param name="FontFileName">Font filename.</param>
/// <returns>Font index in PDF document.</returns>
		ppUns32 AddFontTrueType ( String * FontFileName ){
            OSStrToPCharB ( FontFileName, fn );
            ppInt32 R;
            try {
                R = PDFFontTrueTypeAppendFromFile( FDoc, fn );
            } __finally {
                OSStrToPCharE ( fn );
            }
            return R;
        }
/// <summary>
/// Add true type font to document from stream.
/// </summary>
/// <param name="AStream">Font stream.</param>
/// <returns>Font index in PDF document.</returns>
		ppUns32 AddFontTrueType ( PDFDataStream * AStream ){
            return PDFFontTrueTypeAppendFromStream( FDoc, AStream->Handle );
        }
/// <summary>
/// Add Type1 font to document from file.
/// </summary>
/// <param name="FontFileName">Font filename.</param>
/// <returns>Font index in PDF document.</returns>
		ppUns32 AddFontType1 ( String * FontFileName ){
            OSStrToPCharB ( FontFileName, fn );
            ppInt32 R;
            try {
                R = PDFFontType1AppendFromFile( FDoc, fn );
            } __finally {
                OSStrToPCharE ( fn );
            }
            return R;
        }

/// <summary>
/// Add Type1 font to document from stream.
/// </summary>
/// <param name="AStream">Font stream.</param>
/// <returns>Font index in PDF document.</returns>
		ppUns32 AddFontType1 ( PDFDataStream * AStream ){
            return PDFFontType1AppendFromStream( FDoc, AStream->Handle );
        }

/// <summary>
/// Appends image to document with specified compression from file.
/// </summary>
/// <param name="FileName">Filename of file where stored image</param>
/// <param name="Index">Index of the image if file ( Used only for Tiff files )</param>
/// <param name="Compression">Image compression type.</param>
/// <returns>Index of the image in PDF document.</returns>
        ppUns32 AddImage( String * FileName, ppUns32 Index, ImageCompressionType Compression ){
            OSStrToPCharB ( FileName, fn );
            ppInt32 R;
            try{
                R = PDFImageAppendToDocFromFile( FDoc, fn, Index, (TImageCompressionType)Compression );
            } __finally {
                OSStrToPCharE ( fn );
            }
            return R;
        }
/// <summary>
/// Appends image to document with specified compression from stream.
/// </summary>
/// <param name="AStream">stream where stored image</param>
/// <param name="Index">Index of the image if file ( Used only for Tiff files )</param>
/// <param name="Compression">Image compression type.</param>
/// <returns>Index of the image in PDF document.</returns>
        ppUns32 AddImage( PDFDataStream * AStream, ppUns32 Index, ImageCompressionType Compression ){
            return PDFImageAppendToDocFromStream( FDoc, AStream->Handle, Index, (TImageCompressionType)Compression );
        }

/// <summary>
/// Appends image to document with specified compression from memory buffer.
/// </summary>
/// <param name="Buffer">Storage location for data.</param>
/// <param name="BufferSize">Size of the storage.</param>
/// <param name="Index">Index of the image if file ( Used only for Tiff files )</param>
/// <param name="Compression">Image compression type.</param>
/// <returns>Index of the image in PDF document.</returns>
        ppUns32 AddImage( void * Buffer, ppUns32 BufferSize, ppUns32 Index, ImageCompressionType Compression ){
            return PDFImageAppendToDocFromBuffer( FDoc, Buffer, BufferSize, Index, (TImageCompressionType)Compression );
        }

/// <summary>
/// Appends image to document with specified compression.
/// </summary>
/// <param name="Image">PDF Image handle.</param>
/// <param name="Compression">Image compression type.</param>
/// <returns>Index of the image in PDF document.</returns>
        ppUns32 AddImage( Image * Image, ImageCompressionType Compression ){
            return PDFImageAppendToDoc( Image->Handle, FDoc, (TImageCompressionType) Compression );
        }

/// <summary>
/// Appends b/w image to document with specified compression as transparent mask.
/// </summary>
/// <param name="Image">PDF Image handle.</param>
/// <param name="Compression">Image compression type.</param>
/// <returns>Index of the image in PDF document.</returns>
		ppUns32 AddImageAsMask( Image * Image, ImageCompressionType Compression ){
            return PDFImageAppendToDocAsMask( Image->Handle, FDoc, (TImageCompressionType) Compression );
        }
/// <summary>
/// Appends image to document with specified compression with transparent mask.
/// </summary>
/// <param name="Image">PDF Image handle.</param>
/// <param name="Compression">Image compression type.</param>
/// <returns>Index of the image in PDF document.</returns>
        ppUns32 AddImageWithMask( Image * Image, ImageCompressionType Compression, ppInt32 Mask ){
            return PDFImageAppendToDocWithMask( Image->Handle, FDoc, (TImageCompressionType) Compression, Mask );
        }
/// <summary>
/// Creates new destination name in name table and assigns to it explicit destination
/// </summary>
/// <param name="Name">Specifies the name which will be appended toname table.</param>
/// <param name="Destination">Explicit destination which will be respectedto this name</param>
/// <returns>None.</returns>
        void AddNamedDestination ( String * Name, DestinationInfo Destination ){
			PDFExplicitDest Dest;
            Dest.a = Destination.a;
            Dest.b = Destination.b;
            Dest.c = Destination.c;
            Dest.d = Destination.d;
            Dest.Page = Destination.PageIndex;
            Dest.Type = (PDFExplicitDestType)Destination.Type;
            OSStrToPCharB ( Name, N );
            try {
                PDFDestinationNamedNew ( FDoc, N,Name->Length, Dest );
            } __finally {
                OSStrToPCharE ( N );
            }
        }  

/// <summary>
/// Returns document security permisssions settings. IMPORTANT: In the Full version of the product use of this method/property requires an additional "Existing PDF Support License" license
/// </summary>
        __property DocumentPermissionType get_Permissions (){           
			return (DocumentPermissionType)PDFDocGetPermission ( FDoc );
        };
/// <summary>
/// Returns number of pages in PDF document
/// </summary>
        __property ppInt32 get_PageCount () {
            return PDFDocGetPageCount( FDoc );
        };
/// <summary>
/// Set to True to compress document or to False not to use compression. IMPORTANT: In the Full version of the product use of this method/property requires an additional "Linearization and Compression License" license
/// </summary>
/// <param name="Compressed">True to compress document, False to skip compression</param>
        __property void set_Compressed ( ppBool Compressed ){
            PDFDocSetPacked ( FDoc, Compressed );
        };
/// <summary>
/// Set to True to linearize document or False to skip linearization
/// Linearization is used for PDF document intended to publish online. Linearized documents are capable of displaying first pages while whole document is not loaded yet
///	IMPORTANT: In the Full version of the product use of this method/property requires an additional "Linearization and Compression License" license
/// </summary>
/// <param name="Linearized">True to linearize document, False to skip linearization</param>
        __property void set_Linearized ( ppBool Linearized ){
            PDFDocSetLinearized ( FDoc, Linearized );
        };
/// <summary>
/// Set to true to optimize document by removing all unsued and unlinked objects from it
/// IMPORTANT: In the Full version of the product use of this method/property requires an additional Linearization and Compression License license
/// </summary>
/// <param name="Optimized">True to turn optimization on, False to turn off optimization</param>
        __property void set_Optimized ( ppBool Optimized ){
			PDFDocSetRemoveUnUsed ( FDoc, Optimized );
        };
/// <summary>
/// JPEG images compression quality factor (from 0 to 100). 0 is the best compression, 100 is the best quality but less compression. 75 is recommended for most images
/// </summary>
/// <param name="Quality">JPEG compression factor from 0 to 100 (75 is recommended)</param>
        __property void set_JPEGQuality ( ppInt32 Quality ){
            PDFDocSetJpegImageQuality( FDoc, Quality );
        };
/// <summary>
/// AutoLaunch property defines if library will automatically open PDF document in default PDF viewer application after saving PDF to the file
/// </summary>
/// <param name="AutoLaunch">True to open generated PDF document file in default PDF viewer, False otherwise</param>
        __property void set_AutoLaunch ( ppBool AutoLaunch ){
            PDFDocSetAutoLaunch( FDoc, AutoLaunch );
        };
/// <summary>
/// Set referenced HDC (device context handle) to be used for EMF2PDF conversion
/// </summary>
/// <param name="DC">Referenced device context handle</param>
        __property void set_HDC_ReferencedDC ( void * DC ){
            PDFDocSetUsedDC ( FDoc, ( HDC ) DC );
        };
/// <summary>
/// Set to True to use JPEG compression for raster bitmaps produced while doing EMF2PDF conversion. Set to False to use Flate compression for EMF2PDF generated bitmaps instead
/// </summary>
/// <param name="AsJpeg">True to use Jpeg compression for EMF2PDF generated bitmaps, False to use Flate compression instead </param>
        __property void set_HDC_UseJPEGForImages ( ppBool AsJpeg ){
            PDFDocSetEMFColorImagesAsJpeg( FDoc, AsJpeg);
        };
/// <summary>
/// Set to True to use JBIG2 compression for raster bitmaps produced while doing EMF2PDF conversion. Set to False to use Flate compression for EMF2PDF generated bitmaps instead
/// </summary>
/// <param name="AsJBIG2">True to use JBIG2 compression for EMF2PDF generated bitmaps, False to use Flate compression instead </param>
        __property void set_HDC_UseJBIG2ForImages ( ppBool AsJBIG2 ){
            PDFDocSetEMFBWImagesAsJBIG2( FDoc, AsJBIG2 );
        };

/// <summary>
/// Returns number of Threads in document.
/// </summary>
        __property ppInt32 get_ThreadCount (){
            return PDFDocGetThreadCount( FDoc );
        }

/// <summary>
/// Returns Thread object referenced by its index from PDF document.
/// </summary>
/// <param name="index">Index of Thread object in PDF Document</param>
/// <returns>Returns Thread object referenced by its index from PDF document </returns>
        Thread * GetThreads( ppInt32 index);  
/// <summary>
/// Returns root parent outline object for outlines tree in PDF document
/// </summary>
        __property Outline * get_Outlines(){
            PDFOutlineHandle R;
            R = PDFDocGetOutlineRoot( FDoc );
            return new Outline ( R );
        }
/// <summary>
/// Adds digital signature from digital signature file (*.pfx) to the document. IMPORTANT: In the Full version of the product use of this method/property requires an additional "Sign And Advanced Security License" license
/// </summary>
/// <param name="FileName">Filename of PFX Personal Signature (*.pfx) file</param>
/// <param name="Name">Name to set for signature object that will be added to the PDF document</param>
/// <param name="Reason">Reason text for siginging. For example: "I Accept"</param>
/// <param name="PKCS7">True to use Adobe.PPKMS and adbe.pkcs7.sha1 encryption system sub-filter, False to use Adobe.PPKLite and adbe.x509.rsa_sha1 encryption system sub-filter</param>
/// <param name="Password">Owner password for digital signature used</param>
/// <returns>None</returns>
		void AddSignature( String * FileName, String * Name, String * Reason, ppBool PKCS7, String * Password){
			OSStrToPCharB ( FileName, fn );
			OSStrToPCharB ( Name, nm);
			OSStrToPCharB ( Reason, rsn );
			OSStrToPCharB ( Password, pwd );
			try{
				PDFDocAppendSignatureFromFile( FDoc, fn, nm, rsn, PKCS7, pwd );
			} __finally {
				OSStrToPCharE ( fn );
				OSStrToPCharE ( nm);
				OSStrToPCharE ( rsn );
				OSStrToPCharE ( pwd );
			}
		}
/// <summary>
/// Adds digital signature from digital signature file (*.pfx) to the document. IMPORTANT: In the Full version of the product use of this method/property requires an additional "Sign And Advanced Security License" license
/// </summary>
/// <param name="Buffer">Memory buffer with personal signature stored</param>
/// <param name="BufferSize">Buffer size</param>
/// <param name="Name">Name to set for signature object that will be added to the PDF document</param>
/// <param name="Reason">Reason text for siginging. For example: "I Accept"</param>
/// <param name="PKCS7">True to use Adobe.PPKMS and adbe.pkcs7.sha1 encryption system sub-filter, False to use Adobe.PPKLite and adbe.x509.rsa_sha1 encryption system sub-filter</param>
/// <param name="Password">Owner password for digital signature used</param>
/// <returns>None</returns>
		void AddSignature( unsigned char Buffer __gc[], int BufferSize, String * Name, String * Reason, ppBool PKCS7, String * Password){		
			OSStrToPCharB ( Name, nm);
			OSStrToPCharB ( Reason, rsn );
			OSStrToPCharB ( Password, pwd );
			try{
				Buf = new ppUns8 [ BufferSize ];
				try{
					Marshal::Copy( Buffer,0, Buf, BufferSize );
					PDFDocAppendSignatureFromBuffer( FDoc, Buf, BufferSize, nm, rsn, PKCS7, pwd );
				} __finally {
					delete [] Buf;
				}
			} __finally {
				OSStrToPCharE ( nm);
				OSStrToPCharE ( rsn );
				OSStrToPCharE ( pwd );
			}
		}
/// <summary>
/// Returns controls list object containing list of controls in the document
/// </summary>
        __property ControlList * get_Controls ();
/// <summary>
/// Returns document information property referenced by property type (author, subject, keywords ..)
/// </summary>
/// <param name="index">Document information property type (author, subject, keywords ..)</param>
/// <returns>Returns document information property referenced by property type (author, subject, keywords ..)</returns>
		String * GetDocumentInfo ( DocumentInfoPropertyType index ){
			char * Str;
			ppUns32 Len;
			Str = PDFDocGetInfo( FDoc, (TPDFInformation) index, &Len );
			return PDFStringToString( Str, Len );
		}
/// <summary>
/// Sets document property information referenced by property type
/// </summary>
/// <param name="index">Document information property type (author, subject, keywords ..)</param>
/// <param name="Value">New string value for the document property</param>
/// <returns>None.</returns>
		void SetDocumentInfo ( DocumentInfoPropertyType index, String * Value){
			PDFStrToPCharB( Value, v );
			try {
				PDFDocSetInfo( FDoc, (TPDFInformation)index, v, StrLen );
			} __finally {
				PDFStrToPCharE ( v );
			}
		}

/// <summary>
/// Get Page from document
/// </summary>
		Page* GetPage(ppUns32 PageIndex);

		/// <summary>
		/// Adds attached file into the PDF document from a file
		/// </summary>
		/// <param name="FileName">Filename of existing file to attach to the PDF document</param>
		/// <param name="FileNameToShowInDocument">Filename of file attachment to show in the PDF document when user viewing the document</param>
		/// <param name="Title">Title of file attachment to show in PDF document</param>
		/// <param name="Compress">Set to True if you want to compress file attachment </param>
		void AddAttachedFile(String * FileName, String * FileNameToShowInDocument, String * Title, ppBool Compress);

		/// <summary>
		/// Adds attached file into the PDF document from a buffer
		/// </summary>
		/// <param name="Buffer">Buffer containing file data to add as attachment file into PDF document</param>
		/// <param name="BufferSize">Size of buffer with file data</param>
		void AddAttachedFileFromBuffer(Byte Buffer __gc[], ppUns32 BufferSize, String * FileNameToShowInDocument, String * Title, ppBool Compress);

		/// <summary>
		/// Adds attached file into the PDF document from a PDFDataStream
		/// </summary>
		/// <param name="PDFStream">PDFStream object containing file data to add as attachment file into PDF document</param>
		void AddAttachedFileFromPDFStream(PDFDataStream * PDFStream, String * FileNameToShowInDocument, String * Title, ppBool Compress);

		/// <summary>
		/// Adds attached file into the PDF document from a Stream
		/// </summary>
		/// <param name="Stream">Stream object containing file data to add as attachment file into PDF document</param>
		void AddAttachedFileFromPDFStream(System::IO::Stream * Stream, String * FileNameToShowInDocument, String * Title, ppBool Compress);

/// <summary>
/// Checks if the document contains PDF package or not
/// </summary>
/// <returns>Returns True if the document contains PDF package, False otherwise</returns>
		bool IsPDFPackage();

/// <summary>
/// Adds javascript code to execute when PDF document is opened in PDF reader(viewer) application
/// </summary>
/// <param name="JSName">Name to set for new javascript object</param>
/// <param name="JS">javascript source code to execute on PDF document startup</param>
		void AddStartupJavaScript(String * JSName, String * JS);
};
/// <summary>
/// Bead class implementing a way to work with Beads (see Thread class for working with threads in document)
/// </summary>
    public __gc class Bead: public IDisposable{
        PDFBeadHandle bead;
private:
		bool disposed;
protected:
		virtual void Dispose(bool /*disposeManagedResources*/)
		{
			// process only if mananged and unmanaged resources have
			// not been disposed of.
			if (!this->disposed)
			{
		  
			// DISPOSING OBJECTS

			// no resources to dispose

		    disposed=true;
			}
		}
	private public:
		__property PDFBeadHandle get_Handle(){
			return bead;
		}
		/// <summary>
		/// 
		/// </summary>
		Bead ( PDFBeadHandle AHandle ){			
			disposed = false;
			bead = AHandle;
		}
    public:
        ~Bead (){
			// call Dispose with false.  Since we're in the
			// destructor call, the managed resources will be
			// disposed of anyways.
			Dispose(false);
        }

   void Dispose()
   {
      Dispose(true);
      // tell the GC that the Finalize process no longer needs
      // to be run for this object
      System::GC::SuppressFinalize(this);
   }


/// <summary>
/// Returns previous bead
/// </summary>
        __property Bead * get_Previous(){
            PDFBeadHandle B;
            B = PDFBeadGetPrev( bead );
            return new Bead ( B );
        }
/// <summary>
/// Returns next bead
/// </summary>
        __property Bead * get_Next(){
			PDFBeadHandle B;
			B = PDFBeadGetNext( bead );
			return new Bead ( B );
		}
     
/// <summary>
/// Returns a rectangle specifying area on the page used for the bead
/// </summary>
        __property Rectangle get_rectangle(){
            TPDFRect R;
            Rectangle RO;
            PDFBeadGetRect( bead, &R );
            RO.Left = R.xl;
            RO.Right = R.xr;
            RO.Top = R.yr;
            RO.Bottom = R.yl;
            return RO;
        }
/// <summary>
/// Sets a rectangle specifying area on the page used for the bead
/// </summary>
/// <param name="Rect">rectangle specifying area on the page used for the bead</param>
        __property void set_rectangle(Rectangle Rect){
            TPDFRect R;
            R.xl = Rect.Left;
            R.xr = Rect.Right;
            R.yl = Rect.Bottom;
            R.yr = Rect.Top;
            PDFBeadSetRect(  bead, R);
        }
/// <summary>
/// Returns index of the page used with the current bead 
/// </summary>
        __property ppInt32 get_Page(){
            return PDFBeadGetPage( bead);
        }
/// <summary>
/// Returns Thread for the current bead
/// </summary>
        __property Thread * get_thread();
	};
/// <summary>
/// Thread class is used for articles on the pages. Also see Bead class, beads are used with threads
/// </summary>
    public __gc class Thread: public IDisposable{
        Document *FDoc;
        PDFThreadHandle thread;

private:
		bool disposed;

protected:
		virtual void Dispose(bool /*disposeManagedResources*/)
		{
			// process only if mananged and unmanaged resources have
			// not been disposed of.
			if (!this->disposed)
			{
		  
			// DISPOSING OBJECTS

			FDoc = NULL;

		    disposed=true;
			}
		}

	private public:
		__property PDFThreadHandle get_Handle (){
			return thread;
		}
		/// <summary>
		/// 
		/// </summary>
		Thread ( PDFThreadHandle AHandle ){
			disposed = false;
			FDoc = NULL;
			thread = AHandle;
		}
    public:


   void Dispose()
   {
      Dispose(true);
      // tell the GC that the Finalize process no longer needs
      // to be run for this object
      System::GC::SuppressFinalize(this);
   }
/// <summary>
/// Creates new thread object attached to the current document
/// </summary>
/// <param name="Document">Creates new thread object attached to the current document</param>

        Thread ( Document * ADocument ){
			disposed = false;
            FDoc = ADocument;
            thread = PDFThreadNew( FDoc->Handle );
        }
        ~Thread (){
			// call Dispose with false.  Since we're in the
			// destructor call, the managed resources will be
			// disposed of anyways.
			Dispose(false);
        }
/// <summary>
/// Adds new bead to the thread on the given page on the given rectangle area
/// </summary>
/// <param name="page">Page where new bead is located</param>
/// <param name="Rect">Rectangle area specifying rectangle of the bead on the page</param>
/// <returns>Bead object</returns>
		Bead * AddBead ( Page * page, Rectangle Rect );

/// <summary>
/// Returns first bead in the thread
/// </summary>
        __property Bead * get_FirstBead(){
            return new Bead ( PDFThreadGetFirstBead( thread));
        }

/// <summary>
/// Returns thread property string (referenced by thread property type) from the thread. Property types are used are the same as document property types
/// </summary>
/// <param name="Info">Thread property type</param>
/// <returns>String value of the property</returns>
        String * GetInfo( DocumentInfoPropertyType Info){
            String *st;
            ppUns32 LN;
            char *a;
            a = PDFThreadGetInfo(  thread, (TPDFInformation)Info, &LN );
            st = PDFStringToString( a, LN );
            return st;
        }
/// <summary>
/// Sets thread property string (referenced by thread property type) from the thread. Property types are used are the same as document property types
/// </summary>
/// <param name="Info">Thread property type</param>
/// <param name="Value">New String value of the property</param>
/// <returns>None</returns>
        void SetInfo( DocumentInfoPropertyType Info, String * Value){
            PDFStrToPCharB(Value, v);
            try{
                PDFThreadSetInfo(  thread, (TPDFInformation)Info, v, StrLen );
            } __finally {
                PDFStrToPCharE ( v );
            }
        }
/// <summary>
/// Removes thread from the Document
/// </summary>
/// <returns>None</returns>
        void Delete (){
            PDFThreadDelete(  thread );
            Finalize ();
        }     
    };
/// <summary>
/// PagesManager is class used to extract copy pages from one document and add these pages into another document
/// </summary>
    public __gc class PagesManager: public IDisposable{
        Document * Source;
        Document * Target;
        PPDFDocumentConnection Connection;
		System::String __gc * targetFileName;
private:
		bool disposed;
protected:
		virtual void Dispose(bool /*disposeManagedResources*/)
		{
			// process only if mananged and unmanaged resources have
			// not been disposed of.
			if (!this->disposed)
			{
		  
			// DISPOSING OBJECTS

/*
if ( Connection )
				PDFFreeDocumentConnection ( Connection );
            delete [] Connection;
            Source = NULL;
            Target = NULL;
*/

//               PDFFreeDocumentConnection ( Connection );

//            Source = NULL;
//            Target = NULL;

		    disposed=true;
			}
		}


    public:

   void Dispose()
   {
      Dispose(true);
      // tell the GC that the Finalize process no longer needs
      // to be run for this object
      System::GC::SuppressFinalize(this);
   }


/// <summary>
/// Creates PagesManager object instance and connects PagesManager to source and destination document objects. IMPORTANT: In the Full version of the product use of this method/property requires an additional "Split And Merge License" license
/// </summary>
/// <param name="ASource">Source document filename</param>
/// <param name="ATarget">Destination document filename</param>
		PagesManager(PDFDocEngine  * DocEngine, String __gc * source, String __gc * target)
		{
			disposed = false;
			targetFileName = target;
			Source = __gc new Document(DocEngine, source);
			if(System::IO::File::Exists(targetFileName)){
				// create using existing file
				Target = __gc new Document(DocEngine, target);
		} else {
				// create new document
				Target = __gc new Document(DocEngine);
			}
            Connection = PDFCreateDocumentConnection ( Source->Handle, Target->Handle );
		}
/// <summary>
/// Creates PagesManager object instance and connects PagesManager to source and destination document objects. IMPORTANT: In the Full version of the product use of this method/property requires an additional "Split And Merge License" license
/// If you do not have Additional Existing PDF License in full version of the library please use PagesManager(filename, filename) instead.
/// </summary>
/// <param name="ASource">Source document object</param>
/// <param name="ATarget">Destination document object</param>
        PagesManager( Document * ASource, Document * ATarget){
			disposed = false;

/*
            Source = ASource;
            Target = ATarget;
            Connection = new TPDFDocumentConnection [1];
            memset ( Connection, 0, sizeof ( TPDFDocumentConnection ) );
            PDFCreateDocumentConnection (Source->Handle, Target->Handle, Connection );
*/
			Source = ASource;
            Target = ATarget;
            Connection = PDFCreateDocumentConnection ( Source->Handle, Target->Handle );

        }

/// <summary>
/// Returns number of pages in the source document
/// </summary>
/// <returns>number of pages in the source document</returns>
		__property ppInt32 get_SourcePageCount() 
		{
			if(Source){
				return Source->PageCount;
			}
			else return -1;
		}



        ~PagesManager(){

			// call Dispose with false.  Since we're in the
			// destructor call, the managed resources will be
			// disposed of anyways.
			Dispose(false);

        }
/// <summary>
/// Adds index of the page to copy from the source document and add to the destination document
/// </summary>
/// <param name="PageIndex">integer index of page from Source Document, as from 0</param>
/// <returns>None.</returns>
        void AddPage( ppUns32 PageIndex ){
            PDFSelectPageFromSourceDocument ( Connection, PageIndex );
        }
/// <summary>
/// Extracts pages (previously selected using .AddPage method) from the source document and appends these pages to the destination document. This method also resets stored pages so to copy other pages you should use AddPage method again
/// </summary>
/// <returns>None</returns>
        void Execute (){
            PDFCopyPagesToDestinationDocument ( Connection );
			
             PDFFreeDocumentConnection ( Connection );

			// if manager was intitialized without Existing PDF license then we should save to the file
			// and delete internal document instances
			if(targetFileName != NULL)
			{
			
				System::IO::MemoryStream * stream = __gc new System::IO::MemoryStream();
			
				Target->Save(stream);
				

				delete Target;
				delete Source;

				if(System::IO::File::Exists(targetFileName))
					System::IO::File::Delete(targetFileName);
				
				stream->Seek(0, System::IO::SeekOrigin::Begin);
				System::IO::FileStream * fs =
					__gc new System::IO::FileStream(targetFileName, System::IO::FileMode::CreateNew);
				stream->WriteTo(fs);
				fs->Close();
			}

            Source = NULL;
            Target = NULL;
        }
	};
/// <summary>
/// CosObjectBase class is a base class for Cos objects (low-level objects of PDF documents)
/// </summary>
    public __gc __abstract class CosObjectBase: public IDisposable{
    protected:
        PDFCosHandle Obj;
        Document * FDoc;

		virtual void Dispose(bool /*disposeManagedResources*/)
		{
			// process only if mananged and unmanaged resources have
			// not been disposed of.
			if (!this->disposed)
			{
		  
			// DISPOSING OBJECTS

			FDoc = NULL;

		    disposed=true;
			}
		}

private:
		bool disposed;
	private public:
		CosObjectBase ( PDFCosHandle Handle ){
			Obj = Handle;
			FDoc = NULL;
		}
		__property PDFCosHandle get_Handle (){
			return Obj;
		}
		CosObjectBase ( ){			
			disposed = false;
			Obj = NULL;
			FDoc= NULL;
		}
    public:       
		~CosObjectBase ( ){
			// call Dispose with false.  Since we're in the
			// destructor call, the managed resources will be
			// disposed of anyways.
			Dispose(false);
		}


   void Dispose()
   {
      Dispose(true);
      // tell the GC that the Finalize process no longer needs
      // to be run for this object
      System::GC::SuppressFinalize(this);
   }
/// <summary>
/// Returns True if Cos object is indirect or False otherwise
/// </summary>
        __property ppBool get_IsIndirect (){
            return CosIsIndirect( Obj );
        }
/// <summary>
/// Returns ID of indirect Cos object
/// </summary>
        __property ppUns32 get_ID (){
            return CosGetID(Obj);
        }
/// <summary>
/// Returns generation index of indirect Cos object
/// </summary>
        __property ppUns16 get_Generation (){
            return CosGetGeneration(Obj);
        }
/// <summary>
/// Creates a copy of current Cos object and appends this new Cos object to the given document
/// </summary>
/// <param name="Document">Document to add Cos object into</param>
/// <returns>New Cos object</returns>  
        CosObjectBase * CreateCopy ( Document * Document){
            PDFCosHandle nw = CosCopy ( Document->Handle, Obj );
            return PDFCosHandleToClass( nw );
        }
/// <summary>
/// Destroys Cos object. If Cos object is an array, dictionary or stream then all direct child Cos object are destroyed as well. Indirect child Cos objects are not destroyed
/// </summary>
/// <returns>None</returns>
		void Free (){
			CosFree( Obj );
		}
/// <summary>
/// Returns Cos object type
/// </summary>
        __property CosObjectType get_CosType (){
			return (CosObjectType) CosGetType( Obj );
		}
    };
/// <summary>
/// CosNullObject class implements Null Cos object
/// </summary>
    public __gc class CosNullObject: public CosObjectBase{
	private public:
		CosNullObject ( PDFCosHandle Handle ):CosObjectBase ( Handle ){}
    public:
/// <summary>
/// Creates new Null Cos object. IMPORTANT: In the Full version of the product use of this method/property requires having a "Pro License" license
/// </summary>
/// <param name="Document">Parent document for new cos object</param>

        CosNullObject ( Document *Document ){
			FDoc = Document;
            Obj = CosNullNew ( FDoc->Handle );
        }
    };
/// <summary>
/// CosBooleanObject class implements Boolean Cos object
/// </summary>
    public __gc class CosBooleanObject: public CosObjectBase {
	private public:

        CosBooleanObject ( PDFCosHandle Handle ):CosObjectBase ( Handle ){}
	public:
/// <summary>
/// Creates new Boolean Cos object. IMPORTANT: In the Full version of the product use of this method/property requires having a "Pro License" license
/// </summary>
/// <param name="Document">Parent document</param>
/// <param name="IsIndirect">True to create indirect object or False to create direct object</param>
/// <param name="Value">Default boolean value of the object</param>

        CosBooleanObject ( Document * Document, ppBool IsIndirect, ppBool Value ){
		FDoc = Document;
        	Obj = CosBoolNew( FDoc->Handle, IsIndirect, Value );
        }
/// <summary>
/// Returns boolean value of the object
/// </summary>
        __property ppBool get_Value ( ){
            return CosBoolGetValue( Obj );
        }
/// <summary>
/// Sets boolean value of the Cos object
/// </summary>
/// <param name="Value">New boolean value of the object</param>
        __property void set_Value ( ppBool Value){
            CosBoolSetValue( Obj, Value );
        }
    };
/// <summary>
/// CosRealObject class implements Real Cos object 
/// </summary>
    public __gc class CosRealObject: public CosObjectBase {
	private public:
		CosRealObject ( PDFCosHandle Handle ):CosObjectBase ( Handle ){}
    public:       
/// <summary>
/// Creates new Real Cos object. IMPORTANT: In the Full version of the product use of this method/property requires having a "Pro License" license
/// </summary>
/// <param name="Document">Parent document</param>
/// <param name="IsIndirect">True to create indirect object or False to create direct object</param>
/// <param name="Value">Default real value of the object</param>

        CosRealObject ( Document * Document, ppBool IsIndirect, ppReal Value ){
			FDoc = Document;
            Obj = CosRealNew( FDoc->Handle, IsIndirect, Value );
        }
/// <summary>
/// Returns value of Cos object
/// </summary>
        __property ppReal get_Value ( ){
            return CosRealGetValue( Obj );
        }
/// <summary>
/// Sets value of the object
/// </summary>
/// <param name="Value">New value</param>
        __property void set_Value ( ppReal Value){
            CosRealSetValue( Obj, Value );
        }
    };
/// <summary>
/// CosIntegerObject class implements Integer Cos object
/// </summary>
	public __gc class CosIntegerObject: public CosObjectBase {
	private public:
		CosIntegerObject ( PDFCosHandle Handle ):CosObjectBase ( Handle ){}
    public:
/// <summary>
/// Creates new Integer Cos object. IMPORTANT: In the Full version of the product use of this method/property requires having a "Pro License" license
/// </summary>
/// <param name="Document">Parent document</param>
/// <param name="IsIndirect">True to create indirect object or False to create direct object</param>
/// <param name="Value">Default integer value of the object</param>
        CosIntegerObject ( Document * Document, ppBool IsIndirect, ppInt32 Value ){
			FDoc = Document;
            Obj = CosIntNew( FDoc->Handle, IsIndirect, Value );
        }
/// <summary>
/// Returns value of the object
/// </summary>
        __property ppInt32 get_Value ( ){
            return CosIntGetValue( Obj );
        }
/// <summary>
/// Sets value of the object
/// </summary>
/// <param name="Value">Sets value of the object</param>
        __property void set_Value ( ppInt32 Value){
            CosIntSetValue( Obj, Value );
        }
    };
/// <summary>
/// CosNameObject class implements Name Cos object
/// </summary>
	public __gc class CosNameObject: public CosObjectBase {
	private public:
		CosNameObject ( PDFCosHandle Handle ):CosObjectBase ( Handle ){}
    public:
/// <summary>
/// Creates new Name Cos object. IMPORTANT: In the Full version of the product use of this method/property requires having a "Pro License" license
/// </summary>
/// <param name="Document">Parent document</param>
/// <param name="IsIndirect">True to create indirect object or False to create direct object</param>
/// <param name="Value">Default name value of the object</param>

        CosNameObject ( Document * Document, ppBool IsIndirect, ppAtom Value ){
			FDoc = Document;
            Obj = CosNameNew( FDoc->Handle, IsIndirect, Value );
        }
/// <summary>
/// Creates new Name Cos object. IMPORTANT: In the Full version of the product use of this method/property requires having a "Pro License" license
/// </summary>
/// <param name="Document">Parent document</param>
/// <param name="IsIndirect">True to create indirect object or False to create direct object</param>
/// <param name="Value">Default name value of the object</param>

        CosNameObject ( Document * Document, ppBool IsIndirect, String * Value ){
			OSStrToPCharB ( Value, V );
            try {
                FDoc = Document;
                Obj = CosNameNew( FDoc->Handle, IsIndirect, ULStringToAtom(CD(FDoc->Handle)->Lib, V ) );
            } __finally {
                OSStrToPCharE ( V );
            }
        }
/// <summary>
/// Returns value of the object
/// </summary>
        __property ppAtom get_Value ( ){
            return CosNameGetValue( Obj );
        }
/// <summary>
/// Sets value of the object
/// </summary>
/// <param name="Value">Sets value of the object</param>
        __property void set_Value ( ppAtom Value){
            CosNameSetValue( Obj, Value );
        }
/// <summary>
/// Returns string value of the object
/// </summary>
        __property String * get_ValueString ( ){
            return new String ( ULAtomToString(_CosLib( Obj ), CosNameGetValue( Obj ) )  );
        }
/// <summary>
/// Sets string value of the object
/// </summary>
/// <param name="Value">New string value</param>
        __property void set_ValueString ( String * Value){
            OSStrToPCharB ( Value, V );
            try {
                CosNameSetValue( Obj, ULStringToAtom(_CosLib( Obj), V ) );
            } __finally {
                OSStrToPCharE ( V );
            }          
        }

    };
/// <summary>
/// CosStringObject is class for work with cos string object.
/// </summary>
	public __gc class CosStringObject: public CosObjectBase {
	private public:
		CosStringObject ( PDFCosHandle Handle ):CosObjectBase ( Handle ){}
    public:

/// <summary>
/// Creates new String Cos object. IMPORTANT: In the Full version of the product use of this method/property requires having a "Pro License" license
/// </summary>
/// <param name="Document">Parent document</param>
/// <param name="IsIndirect">True to create indirect object or False to create direct object</param>
/// <param name="Value">Default value of the object</param>
        CosStringObject ( Document * Document, ppBool IsIndirect, String * Value ){
			FDoc = Document;
            PDFStrToPCharB ( Value, V );
            try {
                Obj = CosStringNew( FDoc->Handle, IsIndirect, V, StrLen  );
            } __finally {
                PDFStrToPCharE ( V );
            }
        }

/// <summary>
/// Creates new String Cos object. IMPORTANT: In the Full version of the product use of this method/property requires having a "Pro License" license
/// </summary>
/// <param name="Document">Parent document</param>
/// <param name="IsIndirect">True to create indirect object or False to create direct object</param>
/// <param name="Buffer">Buffer with the string</param>
/// <param name="Length">Buffer size (string length in bytes)</param>

        CosStringObject ( Document * Document,  ppBool IsIndirect, unsigned char Buffer __gc[], ppUns32 Length){
			FDoc = Document;
            char * buf = new char [ Length ];
            try{
                Marshal::Copy( Buffer, 0, buf, Length );
                Obj = CosStringNew( FDoc->Handle, IsIndirect, buf, Length );
            } __finally {
                delete [] buf;
            }
        }
/// <summary>
/// Sets new value
/// </summary>
/// <param name="Value">New value</param>
        __property void set_Value( unsigned char Value __gc[]){
            char * ar;
            ar = new char [ Value->Length ];
            try {
                Marshal::Copy(Value, 0, ar, Value->Length );
                CosStringSetValue( Obj, ar, Value->Length );
            } __finally {
                delete [] ar;
            }
        }
/// <summary>
/// Returns value of the object
/// </summary>
        __property unsigned char get_Value()__gc []{
            unsigned char V __gc[];
            ppUns32 Length;
            char * val;
            val = CosStringGetValue( Obj, &Length);
            V = new  unsigned char __gc [ Length ];
            Marshal::Copy( val, V,0, Length );
            return V;
        }
/// <summary>
/// Sets new string value of the object
/// </summary>
/// <param name="Value">New string value of the object</param>
        __property void set_ValueString ( String * Value){
            PDFStrToPCharB ( Value, v );
            try {
                CosStringSetValue( Obj, v, StrLen );
            } __finally {
                PDFStrToPCharE ( v );
            }
        }
/// <summary>
/// Returns string value of the object
/// </summary>
        __property String * get_ValueString ( ){
            ppUns32 Length;
            char * val;
            val = CosStringGetValue( Obj, &Length);
            return PDFStringToString( val, Length );
        }

    };
/// <summary>
/// CosObjectArray class implements Array Cos object
/// </summary>
    public __gc class CosObjectArray: public CosObjectBase {
	private public:
		CosObjectArray ( PDFCosHandle Handle ):CosObjectBase ( Handle ){}
    public:       

/// <summary>
/// Creates new Array Cos object. IMPORTANT: In the Full version of the product use of this method/property requires having a "Pro License" license
/// </summary>
/// <param name="Document">Parent document</param>
/// <param name="IsIndirect">True to create indirect object or False to create direct object</param>
/// <param name="Entries">Size of array</param>
        CosObjectArray ( Document * Document, ppBool IsIndirect, ppInt32 Entries ){
			FDoc = Document;
            Obj = CosArrayNew( FDoc->Handle, IsIndirect, Entries );
        }
/// <summary>
/// Returns number of elements in array
/// </summary>
        __property ppInt32 get_Count(){
            return CosArrayCount( Obj );
        }
/// <summary>
/// Returns specified Cos Object using its index from Array Cos object
/// </summary>
/// <param name="Index">Index of Cos object to extract from Array. Index is zero-based</param>
/// <returns>Returns Cos Object from array by its index. If Index is outside array bounds then function returns Null Cos object</returns>
        CosObjectBase * GetItem( ppInt32 index){
            return PDFCosHandleToClass( CosArrayItem( Obj, index ) );
        }
/// <summary>
/// Removes and destroys all objects in array
/// </summary>
/// <returns>None</returns>
        void Clear(){
            CosArrayClear( Obj );
        }
/// <summary>
/// Removes cos object referenced by its index from array. Cos objects next to removed object are moved to the new position and array is rebounded automatically
/// </summary>
/// <param name="Index">Index of Cos object to remove</param>
/// <returns>None</returns>
        void Remove ( ppInt32 Index ){
            CosArrayRemove( Obj, Index );
        }
/// <summary>
/// Adds Cos object to array
/// </summary>
/// <param name="CosObj">Existing Cos object to add into arrays</param>
/// <returns>Index of object in array</returns>
        ppInt32 Add ( CosObjectBase * CosObj ){
            return CosArrayAppend( Obj, CosObj->Handle );
        }
/// <summary>
/// Inserts Cos object into array at given position
/// </summary>
/// <param name="CosObj">Existing Cos object to insert into array</param>
/// <param name="Index">Position to insert Cos object into array</param>
/// <returns>Index of Cos object inserted</returns>
        ppInt32 Insert ( CosObjectBase * CosObj, ppInt32 Index ){
            return CosArrayInsert( Obj, CosObj->Handle, Index );
        }
    };

/// <summary>
/// Structure contains dictionary pair 
/// </summary>
    public __value struct DictionaryPair {
    public: 
        ppAtom Key;			/* Key part of key-value pair */
        CosObjectBase * Value; /* Value part of key-value pair */
    };

/// <summary>
/// CosDictionaryObject class implements Dictionary Cos object
/// </summary>
	public __gc class CosDictionaryObject: public CosObjectBase {
	private public:
		CosDictionaryObject ( PDFCosHandle Handle ):CosObjectBase ( Handle ){}
		CosDictionaryObject ():CosObjectBase (){}
    public:
/// <summary>
/// Creates new Dictionary Cos object. IMPORTANT: In the Full version of the product use of this method/property requires having a "Pro License" license
/// </summary>
/// <param name="Document">Parent document</param>
/// <param name="IsIndirect">True to create indirect object or False to create direct object</param>
/// <param name="Entries">Size of dictionary (dictionary is resized automatically when required)</param>

        CosDictionaryObject ( Document * Document, ppBool IsIndirect, ppInt32 Entries ){
			FDoc = Document;
            Obj = CosDictNew( FDoc->Handle, IsIndirect, Entries );
        }
/// <summary>
/// Returns number of key-value pairs in the dictionary
/// </summary>
        __property ppInt32 get_Count(){
            return CosDictCount( Obj );
        }
/// <summary>
/// Returns key-value pair from dictionary using its index
/// </summary>
/// <param name="Index">Index of the pair to return</param>
/// <returns>Key-valye pair</returns>
        DictionaryPair GetPair ( ppInt32 index){
            DictionaryPair Pair;
            ppAtom Key;
            PDFCosHandle CH;
            CosDictGetPair( Obj, index, &Key, &CH );
            Pair.Key = Key;
            Pair.Value = PDFCosHandleToClass( CH );
            return Pair;
        }
/// <summary>
/// Removes and destroys all pairs from dictionary and clears the dictinoary
/// </summary>
/// <returns>None</returns>
        void Clear(){
            CosDictClear( Obj );
        }
/// <summary>
/// Removes and destroys pair referenced by its key from dictionary
/// </summary>
/// <param name="Key">Key of the pair to remove</param>
/// <returns>None</returns>
        void RemoveKey ( ppAtom Key ){
            CosDictRemoveKey( Obj, Key );
        }
/// <summary>
/// Returns value referenced by its atom index
/// </summary>
/// <param name="index">Index of the value to return</param>
/// <returns>Returns Cos object found. If object was not found then returns Null Cos object</returns>
        CosObjectBase * GetValues ( ppAtom index ){
            return PDFCosHandleToClass( CosDictValueByName(Obj, index) );
        }
/// <summary>
/// Sets value of the pair referenced by key
/// </summary>
/// <param name="index">Index of the key to set</param>
/// <param name="CosObject">The value to set.</param>
/// <returns>None</returns>
        void SetValue ( ppAtom index, CosObjectBase * CosObject ){
            CosDictAppend( Obj, index, CosObject->Handle );
        }
/// <summary>
/// Returns the value of the key referenced by string value
/// </summary>
/// <param name="index">String value of the key to find</param>
/// <returns>Returns Cos object or Null Cos object if index is out of bounds</returns>
        CosObjectBase * GetValues ( String * index ){
            CosObjectBase * R;
            OSStrToPCharB ( index, ch);
            try {
                R = PDFCosHandleToClass( CosDictValueByName(Obj, ULStringToAtom( _CosLib ( Obj ), ch ) ) );
            } __finally {
                OSStrToPCharE ( ch );
            }
            return R;
        }
/// <summary>
/// Sets value of the key
/// </summary>
/// <param name="index">String value of the key to set value</param>
/// <param name="CosObject">New Cos Object value</param>
/// <returns>None</returns>
        void SetValue ( String * index, CosObjectBase * CosObject ){
            OSStrToPCharB ( index, ch);
            try {
                CosDictAppend( Obj, ULStringToAtom( _CosLib ( Obj ), ch ), CosObject->Handle );
            } __finally {
                OSStrToPCharE ( ch );
            }
        }
    };
/// <summary>
/// CosStreamObject class implements PDFDataStream Cos object
/// </summary>
	public __gc class CosStreamObject: public CosDictionaryObject {
	private public:
		CosStreamObject( PDFCosHandle Handle ):CosDictionaryObject ( Handle ){}
    public:
/// <summary>
/// Creates new PDFDataStream Cos object. IMPORTANT: In the Full version of the product use of this method/property requires having a "Pro License" license
/// </summary>
/// <param name="Document">Parent document</param>
/// <param name="Entries">PDFDataStream Size</param>
        CosStreamObject ( Document * Document, ppInt32 Entries ){
			FDoc = Document;
            Obj = CosStreamNew( FDoc->Handle, true, Entries );
        }
/// <summary>
/// Returns value of the object
/// </summary>
/// <returns>Returns value of the object</returns>
        PDFDataStream * get_stream (){
            PDFStreamHandle S;
            S = CosStreamGetValue( Obj );
            return new PDFDataStream( S );
        }
    };

/// <summary>
/// DrawingState class implements drawings state controller  in PDF documents
/// </summary>
    public __gc class DrawingState: public IDisposable{
        ppInt32 Index;
        Document * FDoc;
private:
		bool disposed;

protected:
		virtual void Dispose(bool /*disposeManagedResources*/)
		{
			// process only if mananged and unmanaged resources have
			// not been disposed of.
			if (!this->disposed)
			{
		  
			// DISPOSING OBJECTS

			FDoc = NULL;

		    disposed=true;
			}
		}


	private public:
		__property ppInt32 get_Idx (){
			return Index;
		}
    public:
/// <summary>
/// Creates new drawing state
/// </summary>
/// <param name="Document">Parent PDF document</param>

        DrawingState( Document * Document ){
			disposed = false;
            FDoc = Document;
            Index = PDFExtGraphicStateNew( FDoc->Handle );
        }
        ~DrawingState (){
			// call Dispose with false.  Since we're in the
			// destructor call, the managed resources will be
			// disposed of anyways.
			Dispose(false);            
        }

   void Dispose()
   {
      Dispose(true);
      // tell the GC that the Finalize process no longer needs
      // to be run for this object
      System::GC::SuppressFinalize(this);
   }


        
/// <summary>
/// Sets line width for current drawing state
/// </summary>
/// <param name="ALineWidth">Line width</param>
        __property void set_LineWidth ( ppReal ALineWidth ){
            PDFExtGraphicStateSetLineWidth( FDoc->Handle, Index, ALineWidth );
        }
/// <summary>
/// Sets line cap style type for use in the current drawing state
/// </summary>
/// <param name="ALineCap">Line cap type</param>
        __property void set_LineCap ( LineCapType ALineCap ){
            PDFExtGraphicStateSetLineCap( FDoc->Handle, Index, (TPDFLineCap) ALineCap);
        }
/// <summary>
/// Sets line join type for use in the current drawing state
/// </summary>
/// <param name="ALineJoin">Line Join Type</param>
        __property void set_LineJoin ( LineJoinType ALineJoin ){
            PDFExtGraphicStateSetLineJoin( FDoc->Handle, Index, (TPDFLineJoin) ALineJoin);
        }
/// <summary>
/// Sets mitter limit value for use in the current graphic state
/// </summary>
/// <param name="AMitterLimit">Mitter limit value</param>
        __property void set_MiterLimit ( ppReal AMiterLimit ){
            PDFExtGraphicStateSetMitterLimit( FDoc->Handle, Index, AMiterLimit );
        }
/// <summary>
/// Sets "alpha is shape" mode for current graphic state
/// </summary>
/// <param name="AAlphaIsShape">Alpha is shape mode (True or False)</param>
        __property void set_AlphaIsShape ( ppBool AAlphaIsShape ){
            PDFExtGraphicStateSetAlphaIsShape( FDoc->Handle, Index, AAlphaIsShape );
        }
/// <summary>
/// Sets alpha stroke value for use in the current graphic state
/// </summary>
/// <param name="AAlphaStroke">Alpha stroke value</param>
        __property void set_AlphaStroke ( ppReal AAlphaStroke ){
            PDFExtGraphicStateSetAlphaStroke( FDoc->Handle, Index, AAlphaStroke );
        }
/// <summary>
/// Sets alpha fill value for current graphic state
/// </summary>
/// <param name="AAlphaFill">Alpha fill value</param>
        __property void set_AlphaFill ( ppReal AAlphaFill ){
            PDFExtGraphicStateSetAlphaFill( FDoc->Handle, Index, AAlphaFill );
        }
/// <summary>
/// Setting text knockout for current graphic state.
/// </summary>
/// <param name="ATextKnockout">Text knockout.</param>
        __property void set_TextKnockout ( ppBool AKnockout ){
            PDFExtGraphicStateSetTextKnockout( FDoc->Handle, Index, AKnockout );
        }
/// <summary>
/// Setting overprint stroke for current graphic state.
/// </summary>
/// <param name="AOverprintStroke">Overprint stroke.</param>
        __property void set_OverprintStroke ( ppBool AOverprintStroke ){
            PDFExtGraphicStateSetOverprintStroke( FDoc->Handle, Index, AOverprintStroke );
        }
/// <summary>
/// Turns overprint fill mode on or off for current graphic state.
/// </summary>
/// <param name="AOverprintFill">Overprint fill mode (True or False)</param>
        __property void set_OverprintFill ( ppBool AOverprintFill ){
            PDFExtGraphicStateSetOverprintFill( FDoc->Handle, Index, AOverprintFill );
        }
/// <summary>
/// Sets overprint mode value for current graphic state
/// </summary>
/// <param name="AOverprintMode">Overprint mode value</param>
        __property void set_OverprintMode ( ppInt32 AOverprintMode ){
            PDFExtGraphicStateSetOverprintMode ( FDoc->Handle, Index, AOverprintMode );
        }
/// <summary>
/// Sets flatness value for current graphic state
/// </summary>
/// <param name="AFlatness">Flatness value</param>
        __property void set_Flatness ( ppReal AFlatness ){
            PDFExtGraphicStateSetFlatness ( FDoc->Handle, Index, AFlatness );
        }
/// <summary>
/// Sets graphic state color rendering mode type
/// </summary>
/// <param name="ARenderingIntent">Сolor rendering mode type</param>
        __property void set_ColorRenderingMode ( ColorRenderingModeType ARenderingIntent ){
            PDFExtGraphicStateSetRenderingIntent ( FDoc->Handle, Index, (TPDFRenderingIntents)ARenderingIntent );
        }
/// <summary>
/// Sets blending mode type for the current graphic state
/// </summary>
/// <param name="ABlendMode">Blending mode type</param>
        __property void set_BlendingMode ( BlendingModeType ABlendMode ){
            PDFExtGraphicStateSetBlendMode ( FDoc->Handle, Index, (TPDFBlendMode) ABlendMode );
        }
/// <summary>
/// Sets smoothness valye for the current graphic state
/// </summary>
/// <param name="ASmoothness">Smoothness value</param>
        __property void set_Smoothness ( ppReal ASmoothness ){
            PDFExtGraphicStateSetSmoothness ( FDoc->Handle, Index, ASmoothness );
        }
/// <summary>
/// Turns stroke adjustment on or off for the current graphic state
/// </summary>
/// <param name="AStrokeAdjustment">Stroke adjustment mode (True or False)</param>
        __property void set_StrokeAdjustment ( ppBool AStrokeAdjustment ){
            PDFExtGraphicStateSetStrokeAdjustment( FDoc->Handle, Index, AStrokeAdjustment );
        }
/// <summary>
/// Sets dash pattern object for use in the current graphic state
/// </summary>
/// <param name="DashPattern">Dash pattern object (CosObjectBase)</param>
        __property void set_DashPattern( CosObjectBase * DashPattern){
            PDFExtGraphicStateSetDashPattern( FDoc->Handle, Index, DashPattern->Handle );
        }
    };
/// <summary>
/// Drawing class implements drawings for use with pages in PDF document. See .AddDrawing for drawing API commands draw on the page
/// </summary>
    public __gc class Drawing: public IDisposable{
        PBXHandle PB;
        Object *Parent;
private:
		bool disposed;

protected:
		virtual void Dispose(bool /*disposeManagedResources*/)
		{
			// process only if mananged and unmanaged resources have
			// not been disposed of.
			if (!this->disposed)
			{
		  
			// DISPOSING OBJECTS

			Parent = NULL;

		    disposed=true;
			}
		}

	public private:
		Drawing ( Object * AParent, PBXHandle Handle ){
			disposed = false;
			Parent = AParent;
			PB = Handle;
		}
    public:
        ~Drawing (){
			// call Dispose with false.  Since we're in the
			// destructor call, the managed resources will be
			// disposed of anyways.
			Dispose(false);            
        }

   void Dispose()
   {
      Dispose(true);
      // tell the GC that the Finalize process no longer needs
      // to be run for this object
      System::GC::SuppressFinalize(this);
   }

/// <summary>
/// Width of the Drawing
/// </summary>
        __property ppReal get_Width (){
            return PBXGetWidth( PB );
        }
/// <summary>
/// Height of the Drawing
/// </summary>
        __property ppReal get_Height (){
            return PBXGetHeight( PB );
        }
/// <summary>
/// Disposes the instance of the Drawing and writes its compressed content to the page 
/// </summary>
/// <returns>None</returns>
		void Close ( ){
			PBXClose ( PB, true );
		}
/// <summary>
/// Disposes the instance of the Drawing and writes its uncompressed content to the page 
/// </summary>
/// <returns>None</returns>
		void CloseUnPacked ( ){
			PBXClose ( PB, false );
		}


/// <summary>
/// Creates and returns Color structure from Red, Green, Blue (RGB) values (0..255)
/// </summary>
/// <param name="R">Intensity of the red component (0..255)</param>
/// <param name="G">Intensity of the green component (0..255)</param>
/// <param name="B">Intensity of the blue component (0..255)</param>
/// <returns>Color structure</returns>
        static Color RGBToColor ( unsigned char R, unsigned char G, unsigned char B ){
            Color Color;
            Color.Space = ColorSpaceType::RGB;
            Color.Color1 = R * RGB_TO_INTENSITY_FACTOR;
            Color.Color2 = G * RGB_TO_INTENSITY_FACTOR;
            Color.Color3 = B * RGB_TO_INTENSITY_FACTOR;
            return Color;
        };

/// <summary>
/// Creates and returns Color structure from Red, Green, Blue (RGB) intensity (0..1) values
/// </summary>
/// <param name="R">Intensity of the red component (0..1)</param>
/// <param name="G">Intensity of the green component (0..1)</param>
/// <param name="B">Intensity of the blue component (0..1)</param>
/// <returns>Color structure</returns>
        static Color RGBToColorUsingIntensityFactors ( ppReal R, ppReal G, ppReal B ){
            Color Color;
            Color.Space = ColorSpaceType::RGB;
            Color.Color1 = R;
            Color.Color2 = G;
            Color.Color3 = B;
            return Color;
        };
/// <summary>
/// Creates and returns Color structure from Gray component
/// </summary>
/// <param name="G">Intensity of the gray component(0..1)</param>
/// <returns>Color structure</returns>
        static Color GrayToColor ( ppReal G){
            Color Color;
            Color.Space = ColorSpaceType::Gray;
            Color.Color1 = G;
            return Color;
        }
/// <summary>
/// Creates and returns Color structure CMYK (cyan, magenta, yellow, black) components
/// </summary>
/// <param name="C">Intensity of the cyan component(0..1)</param>
/// <param name="M">Intensity of the magenta color(0..1)</param>
/// <param name="Y">Intensity of the yellow color(0..1)</param>
/// <param name="K">Intensity of the black color(0..1)</param>
/// <returns>Color structure</returns>
        static Color CMYKToColor ( ppReal C, ppReal M, ppReal Y, ppReal K ){
            Color Color;
            Color.Space = ColorSpaceType::CMYK;
            Color.Color1 = C;
            Color.Color2 = M;
            Color.Color3 = Y;
            Color.Color4 = K;
            return Color;
        };
/// <summary>
/// Creates and returns RGBColor structure from RGB (red, green, blue) components (0..255)
/// </summary>
/// <param name="R">Intensity of the red component(0..255)</param>
/// <param name="G">Intensity of the green component(0..255)</param>
/// <param name="B">Intensity of the blue component(0..255)</param>
/// <returns>Returns RGBColor structure</returns>
		static RGBColor CreateRGBColor ( unsigned char R, unsigned char G, unsigned char B ){ 
			//		static RGBColor ToRGB ( ppReal R, ppReal G, ppReal B ){
			RGBColor Color;
			Color.R = R * RGB_TO_INTENSITY_FACTOR;
			Color.G = G * RGB_TO_INTENSITY_FACTOR;
			Color.B = B * RGB_TO_INTENSITY_FACTOR;
			return Color;
		};

/// <summary>
/// Creates and returns RGBColor structure from RGB (red, green, blue) components based on intensity (0..1)
/// </summary>
/// <param name="R">Intensity of the red component(0..1)</param>
/// <param name="G">Intensity of the green component(0..1)</param>
/// <param name="B">Intensity of the blue component(0..1)</param>
/// <returns>Returns RGBColor structure</returns>
		static RGBColor CreateRGBColorFromIntensityFactors ( ppReal R, ppReal G, ppReal B ){ 
			//		static RGBColor ToRGB ( ppReal R, ppReal G, ppReal B ){
			RGBColor Color;
			Color.R = R;
			Color.G = G;
			Color.B = B;
			return Color;
		};

/// <summary>
/// Clears current paint path and starts new path. Current point becomes undefined.
/// </summary>
/// <returns>None</returns>
        void NewPath (){
            PBXNewPath( PB );
        }
/// <summary>
/// Closes path and connects first and last points in the current path 
/// </summary>
/// <returns>None</returns>
        void ClosePath (){
            PBXClosePath(PB);
        }
/// <summary>
/// Sets current path as a clipping boundaries for subsequent drawings
/// </summary>
/// <returns>None</returns>
        void Clip (){
            PBXClip(PB);
        }
/// <summary>
/// Sets current path as a clipping boundaries for subsequent drawings and uses "even-odd" rule for defining the "inside" that shows through the clipping window
/// </summary>
/// <returns>None</returns>
        void EoClip (){
            PBXEoClip(PB);
        }
/// <summary>
/// Sets current path as a clipping boundaries for color filling and uses the "non-zero winding number" rule
/// </summary>
/// <returns>None</returns>
        void Fill (){
            PBXFill(PB);
        }
/// <summary>
/// Sets current path as a clipping boundaries for color filling and uses the "evenodd" rule for defining an "inside" that is painted.
/// </summary>
/// <returns>None</returns>
        void EoFill (){
            PBXEoFill(PB);
        }
/// <summary>
/// Sets current path as a clipping boundaries for the first color filling inside with the current fill color. Uses the "evenodd" rule for defining an "inside" that is painted. Then strokes the path with the current stroke color
/// </summary>
/// <returns>None.</returns>
        void FillAndStroke (){
            PBXFillAndStroke( PB );
        }
/// <summary>
/// Sets current path as a clipping boundaries for the first filling the inside with the current fill color (uses the "non-zero winding number"rule ), and then stroking the path with the current stroke color
/// </summary>
/// <returns>None</returns>
        void EoFillAndStroke (){
            PBXEoFillAndStroke ( PB );
        }
/// <summary>
/// Strokes the current paths with the current stroke color and current line width
/// </summary>
/// <returns>None</returns>
        void Stroke (){
            PBXStroke ( PB );
        }
/// <summary>
/// Moves the current point to new location (x,y)
/// </summary>
/// <param name="X">New X position</param>
/// <param name="Y">New Y position</param>
/// <returns>None</returns>
        void MoveTo ( ppReal X, ppReal Y){
            PBXMoveTo( PB, X, Y);
        }
/// <summary>
/// Draws a line from current position to the new position. New position becomes the current position
/// </summary>
/// <param name="X">X position</param>
/// <param name="Y">X position</param>
/// <returns>None</returns>
        void LineTo ( ppReal X, ppReal Y){
            PBXLineTo ( PB, X, Y );
        }
/// <summary>
/// Draws a Bezier cubic curve from the current point ( x0, y0 ) using two points ( x1, y1 ) and ( x2, y2 ) as a control points and terminated at ( x3, y3 ) point. Requires current point to be defined (call .MoveTo before). New current position is X3,Y3
/// </summary>
/// <param name="X1">X1</param>
/// <param name="Y1">Y1</param>
/// <param name="X2">X2</param>
/// <param name="Y2">Y2</param>
/// <param name="X3">X3</param>
/// <param name="Y3">Y3</param>
/// <returns>None.</returns>
        void CurveTo ( ppReal X1, ppReal Y1, ppReal X2, ppReal Y2, ppReal X3, ppReal Y3 ){
            PBXCurveTo( PB, X1, Y1, X2, Y2, X3, Y3);
        }
/// <summary>
/// Draws a rectangle from top left point ( x1, y1 ) to right bottom point ( x2, y2 )
/// </summary>
/// <param name="X1">X1</param>
/// <param name="Y1">Y1</param>
/// <param name="X2">X2</param>
/// <param name="Y2">Y2</param>
/// <returns>None</returns>
        void rectangle ( ppReal X1, ppReal Y1, ppReal X2, ppReal Y2 ){
            PBXRectangle( PB, X1, Y1, X2, Y2 );
        }
/// <summary>
/// Draws a circle centered at (X, Y) with radius R (counter-clockwise). Also see .Arc()
/// </summary>
/// <param name="X">X of the center</param>
/// <param name="Y">Y of the center</param>
/// <param name="R">Radius</param>
/// <returns>None</returns>
        void Circle ( ppReal X, ppReal Y, ppReal R){
            PBXCircle ( PB, X, Y, R );
        }
/// <summary>
/// Draws an ellipse from top left (x1,y1) and bottom right (x2,y2). Also see .Arc()
/// </summary>
/// <param name="X1">X1 left</param>
/// <param name="Y1">Y1 top</param>
/// <param name="X2">X2 right</param>
/// <param name="Y2">Y2 bottom</param>
/// <returns>None</returns>
        void Ellipse ( ppReal X1, ppReal Y1, ppReal X2, ppReal Y2){
            PBXEllipse (PB, X1, Y1, X2, Y2 );
        }
/// <summary>
/// Draws an arc
/// </summary>
/// <param name="X1">X1 left</param>
/// <param name="Y1">Y1 top</param>
/// <param name="X2">X2 right</param>
/// <param name="Y2">Y2 bottom</param>
/// <param name="X3">X coordinate of the point to start arc</param>
/// <param name="Y3">Y coordinate of the point to start arc</param>
/// <param name="X4">X coordinate of the point to end arc</param>
/// <param name="Y4">Y coordinate of the point to end arc</param>
/// <returns>None</returns>
        void Arc ( ppReal X1, ppReal Y1, ppReal X2, ppReal Y2, ppReal X3, ppReal Y3, ppReal X4, ppReal Y4   ){
            PBXArc( PB, X1,Y1,X2,Y2,X3,Y3,X4,Y4);
        }
/// <summary>
/// Draws an arc
/// </summary>
/// <param name="X1">X1 left</param>
/// <param name="Y1">Y1 top</param>
/// <param name="X2">X2 right</param>
/// <param name="Y2">Y2 bottom</param>
/// <param name="BeginAngle">Starting angle</param>
/// <param name="EndAngle">End angle</param>
/// <returns>None</returns>
        void Arc ( ppReal X1, ppReal Y1, ppReal X2, ppReal Y2, ppReal BeginAngle, ppReal EndAngle ){
            PBXArc2( PB, X1,Y1,X2,Y2, BeginAngle, EndAngle );
        }
/// <summary>
/// Draws a Pie
/// </summary>
/// <param name="X1">X1 left</param>
/// <param name="Y1">Y1 top</param>
/// <param name="X2">X2 right</param>
/// <param name="Y2">Y2 bottom</param>
/// <param name="X3">X coordinate of the point to start arc</param>
/// <param name="Y3">Y coordinate of the point to start arc</param>
/// <param name="X4">X coordinate of the point to end arc</param>
/// <param name="Y4">Y coordinate of the point to end arc</param>
/// <returns>None</returns>
        void Pie ( ppReal X1, ppReal Y1, ppReal X2, ppReal Y2, ppReal X3, ppReal Y3, ppReal X4, ppReal Y4   ){
            PBXPie( PB, X1,Y1,X2,Y2,X3,Y3,X4,Y4);
        }
/// <summary>
/// Draws a Pie
/// </summary>
/// <param name="X1">X1 left</param>
/// <param name="Y1">Y1 top</param>
/// <param name="X2">X2 right</param>
/// <param name="Y2">Y2 bottom</param>
/// <param name="BeginAngle">Starting angle</param>
/// <param name="EndAngle">End angle</param>
/// <returns>None</returns>
        void Pie ( ppReal X1, ppReal Y1, ppReal X2, ppReal Y2, ppReal BeginAngle, ppReal EndAngle ){
            PBXPie2( PB, X1,Y1,X2,Y2, BeginAngle, EndAngle );
        }
/// <summary>
/// Draws a rotated rectangle
/// </summary>
/// <param name="X">X left</param>
/// <param name="Y">Y top</param>
/// <param name="W">Width of the rectangle</param>
/// <param name="H">Height of the rectangle.</param>
/// <param name="Angle">Rotation angle</param>
/// <returns>None</returns>
        void RectRotated ( ppReal X, ppReal Y, ppReal W, ppReal H, ppReal Angle){
            PBXRectRotated( PB, X, Y, W, H, Angle );
        }
/// <summary>
/// Draws a rounded rectangle
/// </summary>
/// <param name="X1">X1 left</param>
/// <param name="Y1">Y1 top</param>
/// <param name="X2">X2 right</param>
/// <param name="Y2">Y2 bottom</param>
/// <param name="X3">Width of the ellipse to use to round corners of the rectangle</param>
/// <param name="Y3">Height of the ellipse to use to round corners of the rectangle</param>
/// <returns>None</returns>
        void RoundRect ( ppReal X1, ppReal Y1, ppReal X2, ppReal Y2, ppReal X3, ppReal Y3 ){
            PBXRoundRect( PB, X1, Y1, X2, Y2, X3, Y3);
        }
/// <summary>
/// Pushes a copy of current drawing state into the stack
/// </summary>
/// <returns>None.</returns>
        void StateStore (){
            PBXStateStore( PB);
        }
/// <summary>
/// Restores drawing state from the stack 
/// </summary>
/// <returns>None.</returns>
        void StateRestore (){
            PBXStateRestore( PB);
        }
/// <summary>
/// Resets dash line pattern to solid line
/// </summary>
/// <returns>None</returns>
        void NoDash (){
            PBXNoDash(PB);
        }
/// <summary>
/// Sets dash pattern style
/// </summary>
/// <param name="Dash">New dash pattern style</param>
/// <returns>None</returns>
        void SetDash(String * Dash){
            OSStrToPCharB( Dash, d );
            try{
                PBXSetDash(PB, d);
            } __finally {
                OSStrToPCharE ( d );
            }
        }
/// <summary>
/// Sets flatness value
/// </summary>
/// <param name="Flatness">New flatness tolerance value</param>
/// <returns>None</returns>
        void SetFlatness ( ppInt32 Flatness ){
            PBXSetFlatness(PB, Flatness );
        }
/// <summary>
/// Sets fill/stroke color 
/// </summary>
/// <param name="Color">Color for stroke and fill commands</param>
/// <returns>None</returns>
        void SetColor ( Color Color ){
            TPDFColor C;
            switch ( Color.Space ){
                case ColorSpaceType::Gray:
                    C.Device = cgGray;
                    C.Color.Gray = Color.Color1;
                    break;
                case ColorSpaceType::RGB:
                    C.Device = cgRGB;
                    C.Color.RGB.R = Color.Color1;
                    C.Color.RGB.G = Color.Color2;
                    C.Color.RGB.B = Color.Color3;
                    break;
                case ColorSpaceType::CMYK:
                    C.Device = cgCMYK;
                    C.Color.CMYK.C = Color.Color1;
                    C.Color.CMYK.M = Color.Color2;
                    C.Color.CMYK.Y = Color.Color3;
                    C.Color.CMYK.K = Color.Color4;
                    break;
				default:
					// should not happen
					assert(false);
					return;
            }
            PBXSetColor( PB, C );
        }
/// <summary>
/// Sets filling color
/// </summary>
/// <param name="Color">New color value for filling</param>
/// <returns>None</returns>
        void SetFillColor ( Color Color){
            TPDFColor C;
            switch ( Color.Space ){
                case ColorSpaceType::Gray:
                    C.Device = cgGray;
                    C.Color.Gray = Color.Color1;
                    break;
                case ColorSpaceType::RGB:
                    C.Device = cgRGB;
                    C.Color.RGB.R = Color.Color1;
                    C.Color.RGB.G = Color.Color2;
                    C.Color.RGB.B = Color.Color3;
                    break;
                case ColorSpaceType::CMYK:
                    C.Device = cgCMYK;
                    C.Color.CMYK.C = Color.Color1;
                    C.Color.CMYK.M = Color.Color2;
                    C.Color.CMYK.Y = Color.Color3;
                    C.Color.CMYK.K = Color.Color4;
                    break;
				default:
					// should not happen
					assert(false);
					return;
            }
            PBXSetFillColor( PB, C );
        }
/// <summary>
/// Sets stroking color
/// </summary>
/// <param name="Color">New stroking color</param>
/// <returns>None</returns>
        void SetStrokeColor ( Color Color){
            TPDFColor C;
            switch ( Color.Space ){
                case ColorSpaceType::Gray:
                    C.Device = cgGray;
                    C.Color.Gray = Color.Color1;
                    break;
                case ColorSpaceType::RGB:
                    C.Device = cgRGB;
                    C.Color.RGB.R = Color.Color1;
                    C.Color.RGB.G = Color.Color2;
                    C.Color.RGB.B = Color.Color3;
                    break;
                case ColorSpaceType::CMYK:
                    C.Device = cgCMYK;
                    C.Color.CMYK.C = Color.Color1;
                    C.Color.CMYK.M = Color.Color2;
                    C.Color.CMYK.Y = Color.Color3;
                    C.Color.CMYK.K = Color.Color4;
                    break;
				default:
					// should not happen
					assert(false);
					return;
            }
            PBXSetStrokeColor( PB, C );
        }
/// <summary>
/// Sets line width
/// </summary>
/// <param name="LineWidth">New line width value</param>
/// <returns>None</returns>
        void SetLineWidth ( ppReal LineWidth ){
            PBXSetLineWidth ( PB, LineWidth );
        }
/// <summary>
/// Sets line cap style
/// </summary>
/// <param name="LineCap">New line cap style type</param>
/// <returns>None.</returns>
        void SetLineCap ( LineCapType LineCap ){
            PBXSetLineCap ( PB, (TPDFLineCap)LineCap);
        }
/// <summary>
/// Sets line joing type
/// </summary>
/// <param name="LineJoin">New line join type</param>
/// <returns>None</returns>
        void SetLineJoin ( LineJoinType LineJoin ){
            PBXSetLineJoin ( PB, (TPDFLineJoin)LineJoin);
        }
/// <summary>
/// Sets meter limit value
/// </summary>
/// <param name="MiterLimit">New mitter limit value</param>
/// <returns>None.</returns>
        void SetMiterLimit ( ppReal MiterLimit){
            PBXSetMiterLimit ( PB, MiterLimit );
        }
/// <summary>
/// Sets new graphic state using existing DrawingState object
/// </summary>
/// <param name="ExtState">Existing DrawingState object</param>
/// <returns>None</returns>
        void SetGState ( DrawingState * ExtState ){
            PBXSetGState( PB, ExtState->Idx );
        }
/// <summary>
/// Places the image referenced by the image to the paint box (see .Drawing.AddDrawing
/// </summary>
/// <param name="Index">Index of existing image previously added to the document</param>
/// <param name="X">X left for image</param>
/// <param name="Y">Y top for image</param>
/// <param name="Width">Width of the image</param>
/// <param name="Height">Height of the image</param>
/// <param name="Angle">Rotation angle</param>
/// <returns>None</returns>
        void PlaceImage ( ppUns32 Index, ppReal X, ppReal Y, ppReal Width, ppReal Height, ppReal Angle ){
            PBXShowImage ( PB, Index, X, Y, Width, Height, Angle );
        }
/// <summary>
/// Sets active font for text
/// </summary>
/// <param name="Index">Index of existing font</param>
/// <param name="FontSize">Font size to use for text</param>
/// <param name="Charset">Charset to use with text</param>
/// <param name="Underline">True to use underline</param>
/// <param name="StrikeOut">True to strike out text</param>
/// <returns>None</returns>
        void SetActiveFont ( ppUns32 Index, ppReal FontSize, ppUns8 Charset, ppBool UnderLine, ppBool StrikeOut){
            PBXSetActiveFontWithCharset(PB, Index, FontSize, Charset, UnderLine, StrikeOut );
        }
/// <summary>
/// Sets active font for the text
/// </summary>
/// <param name="Index">Index of existing font</param>
/// <param name="FontSize">Font size to use for text</param>
/// <param name="Underline">True to use underline</param>
/// <param name="StrikeOut">True to strike out text</param>
/// <returns>None</returns>
        void SetActiveFont ( ppUns32 Index, ppReal FontSize, ppBool UnderLine, ppBool StrikeOut){
            PBXSetActiveFont(PB, Index, FontSize, UnderLine, StrikeOut );
        }
/// <summary>
/// Sets character spacing value
/// </summary>
/// <param name="Spacing">Character spacing value</param>
/// <returns>None</returns>
        void SetCharacterSpacing ( ppReal Spacing ){
            PBXSetCharacterSpacing( PB, Spacing );
        }
/// <summary>
/// This procedure sets the additional space (in points) that should be inserted between words, i.e., for every space character found in the text string.
/// </summary>
/// <param name="Spacing">Additional space (in points) that should be inserted between words</param>
/// <returns>None.</returns>
        void SetWordSpacing ( ppReal Spacing ){
            PBXSetWordSpacing( PB, Spacing );
        }
/// <summary>
/// Sets horizontal scaling (percents)
/// </summary>
/// <param name="Scale">Horizontal scaling factor (percents)</param>
/// <returns>None</returns>
        void SetHorizontalScaling ( ppReal Scaling ){
            PBXSetHorizontalScaling( PB, Scaling );
        }
/// <summary>
/// Sets text rendering mode type
/// </summary>
/// <param name="Mode">New text rendering mode type</param>
/// <returns>None</returns>
        void SetTextRenderingMode( TextRenderingModeType Mode ){
            PBXSetTextRenderingMode( PB, (ppInt32) Mode );
        }
/// <summary>
/// Draws a text at the specified location using currently active font ( see .SetActiveFont)
/// </summary>
/// <param name="X">X left</param>
/// <param name="Y">Y top</param>
/// <param name="Orientation">Rotation angle</param>
/// <param name="Str">Text to draw</param>
/// <returns>None</returns>
        void PlaceText ( ppReal X, ppReal Y, ppReal Orientation, String * Str ){

			if (Str->Length == 0)
				Str = " ";

            PDFStrToPCharCheckOnlyB ( Str, TextStr );
            try {
                if ( IsAnsi)
                    PBXTextOut( PB, X, Y, Orientation, TextStr );
                else 
                    PBXUnicodeTextOut(PB, X, Y, Orientation, (PppUns16)wch, Str->Length );

            } __finally {
                PDFStrToPCharCheckOnlyE ( TextStr );
            }
        }


/// <summary>
/// Draws a text box at the specified location with speicified width, height and interval using currently active font ( see .SetActiveFont)
/// </summary>
/// <param name="X">X left</param>
/// <param name="Y">Y top</param>
/// <param name="Width">Box width</param>
/// <param name="Height">Box height</param>
/// <param name="Interval">Interval between text</param>
/// <param name="Str">Text to draw</param>
/// <returns>None</returns>
        ppUns32 PlaceTextBox ( ppReal X, ppReal Y,  ppReal Width, ppReal Height, ppReal Interval, String * Str ){
			
			if (Str->Length == 0)
				Str = " ";

			ppUns32 Result;
			PDFStrToPCharCheckOnlyB ( Str, TextStr );
			try {
				if ( IsAnsi)
					Result = PBXTextOutBox( PB, X, Y, Width,Height, Interval, TextStr );
				else 
					Result = PBXUnicodeOutBox(PB, X, Y, Width,Height, Interval, (PppUns16)wch, Str->Length );

			} __finally {
				PDFStrToPCharCheckOnlyE ( TextStr );
			}
			return Result;
        }

/// <summary>
/// Returns width of the text string if it was rendered with the currently active font
/// </summary>
/// <param name="Str">Text string</param>
/// <returns>Width of the text string if it was rendered with the currently active font</returns>
        ppReal GetTextWidth( String * Str ){
            ppReal W;
            PDFStrToPCharCheckOnlyB ( Str, TextStr );
            try {
                if ( IsAnsi )
                    W = PBXGetTextWidth( PB, TextStr );
                else 
                    W = PBXGetUnicodeWidth(PB, (PppUns16)wch, Str->Length );
            } __finally {
                PDFStrToPCharCheckOnlyE ( TextStr );
            }
            return W;
        }


/// <summary>
/// Draws EMF metafile using its handle
///	IMPORTANT: In the Full version of the product use of this method/property requires having a "Pro License" license
/// </summary>
/// <param name="Metafile">Metafile handle</param>
/// <param name="X">X left</param>
/// <param name="Y">Y top</param>
/// <param name="XScale">X scaling factor</param>
/// <param name="YScale">Y scaling factor</param>
/// <returns>None</returns>
        void PlayMetaFile ( IntPtr Metafile, ppReal X, ppReal Y, ppReal XScale, ppReal YScale ){
		PBXPlayMetaFile(PB, (HGDIOBJ)Metafile, X, Y, XScale, YScale );
        }
    };

/// <summary>
/// Page class represents a page in PDF document. Use .AddDrawing to add drawings to the page
/// </summary>
    public __gc class Page: public IDisposable
    {
    private:
        Document * Doc;
        ppUns32 PageIndex;
		//void Init();
		bool disposed;

protected:
		virtual void Dispose(bool /*disposeManagedResources*/)
		{
			// process only if mananged and unmanaged resources have
			// not been disposed of.
			if (!this->disposed)
			{
		  
			// DISPOSING OBJECTS

			Doc = NULL;

		    disposed=true;
			}
		}

	private public:
		__property Document * get_Parent(){
			return Doc;
		}
		__property ppUns32 get_Index(){
			return PageIndex;
		}
    public:
		Page ( Document * Document, ppUns32 AIndex, bool bExist) : bExist(bExist)
		{
			disposed = false;
			Doc = Document;
            PageIndex = AIndex;

			// this.bExist = bExist;
			//Init();
        };
        ~Page (){
			// call Dispose with false.  Since we're in the
			// destructor call, the managed resources will be
			// disposed of anyways.
			Dispose(false);            
        };

   void Dispose()
   {
      Dispose(true);
      // tell the GC that the Finalize process no longer needs
      // to be run for this object
      System::GC::SuppressFinalize(this);
   }

/// <summary>
/// Returns number of content streams used in the page
/// </summary>
        __property ppInt32 get_ContentCount () { 
            return PDFPageGetContentCount(Doc->Handle, PageIndex); 
        };
/// <summary>
/// Returns rectangle on the page for the given page box type
/// </summary>
/// <param name="index">PageBox Type</param>
/// <returns>Rectangle area</returns>
        Rectangle GetBoxType( PageBoxType index){
            TPDFRect Rect;
            Rectangle R;
            Rect = PDFPageGetBox( Doc->Handle, PageIndex, (TPDFPageBoxType) index );
            R.Bottom = Rect.yl;
            R.Left = Rect.xl;
            R.Right = Rect.xr;
            R.Top = Rect.yr;
 
            return R;
        };

		void SetThumbnail( ppUns32 ImageIndex  ){
			PDFPageSetTumbnail(Doc->Handle, PageIndex, ImageIndex);
		}

		
/// <summary>
/// Sets rectangle for the given page box type for the page
/// </summary>
/// <param name="index">PageBox Type</param>
/// <param name="BoxType">Rectangle</param>
/// <returns>None</returns>
        void SetBoxType ( PageBoxType index, Rectangle BoxType){
            TPDFRect Rect;
            Rect.xl = BoxType.Left;
            Rect.xr = BoxType.Right;
            Rect.yl = BoxType.Bottom;
            Rect.yr = BoxType.Top;
            PDFPageSetBox ( Doc->Handle, PageIndex, (TPDFPageBoxType) index, Rect );
        };

/// <summary>
/// Returns rotation angle of the page
/// </summary>
        __property PageRotationAngleType get_Angle ( ){
            return (PageRotationAngleType)PDFPageGetRotateAngle( Doc->Handle, PageIndex );
        }
/// <summary>
/// Sets rotation angle for the page
/// </summary>
/// <param name="Angle">Rotation angle for the page</param>
        __property void set_Angle ( PageRotationAngleType Angle ){
            PDFPageSetRotateAngle( Doc->Handle, PageIndex,(TPDFPageRotateAngle) Angle );
        };
/// <summary>
/// Adds content stream to the page
/// </summary>
/// <returns>Number of content streams currently exists for the page</returns>
        ppInt32 AddContent (){
            return PDFPageAddContent( Doc->Handle, PageIndex );
        };
/// <summary>
/// Inserts empty content to the page
/// </summary>
/// <param name="Index">Index of the content to insert into</param>
/// <returns>Index of content stream inserted</returns>
        ppInt32 InsertContent ( ppInt32 Index ){
            return PDFPageInsertContent( Doc->Handle, PageIndex, Index );
        };
/// <summary>
/// Removes content stream referenced by the index from the page 
/// </summary>
/// <param name="Index">Index of the content stream to remove</param>
/// <returns>None</returns>
        void RemoveContent ( ppInt32 Index ){
            return PDFPageRemoveContent ( Doc->Handle, PageIndex, Index );
        };
/// <summary>
/// Adds Drawing with the given DPI resolution into the current page
/// </summary>
/// <param name="Resolution">New Drawing resolution (dpi). 72 dpi is a recommended value.</param>
/// <returns>New Drawing object</returns>
        Drawing * AddDrawing ( ppInt32 Resolution ){
            // PBXHandle PDFPageCreateDrawingAndRepackContents
			Drawing * R;
            PBXHandle H;
            if(bExist)
				H = PDFPageCreatePaintBoxAndRepackContents( Doc->Handle, PageIndex, Resolution );
			else
				H = PDFPageCreatePaintBox( Doc->Handle, PageIndex, Resolution );

            R = new Drawing( this, H );
            return R;
        };
/// <summary>
/// Adds Drawing into the current page with the 72 DPI resolution
/// </summary>
/// <returns>New Drawing object</returns>
		Drawing * AddDrawing (){
            Drawing * R;
            PBXHandle H;
            if(bExist)
				H = PDFPageCreatePaintBoxAndRepackContents( Doc->Handle, PageIndex, 72 );
			else
				H = PDFPageCreatePaintBox( Doc->Handle, PageIndex, 72 );
            R = new Drawing( this, H );
            return R;
        };

/// <summary>
/// Adds Drawing with the given DPI resolution into the current page at the given content stream position
/// </summary>
/// <param name="Index">Content's Index in page Content Streams to insert new Drawing</param>
/// <param name="Resolution">New Drawing resolution (dpi). 72 dpi is a recommended value.</param>
/// <returns>New Drawing object</returns>

        Drawing * AddDrawing ( ppInt32 Index, ppInt32 Resolution ){
            Drawing * R;
            PBXHandle H;
            if(bExist)
				H = PDFPageCreatePaintBoxAndRepackContents( Doc->Handle, PageIndex, Resolution );
			else
				H = PDFPageCreatePaintBoxFromContent( Doc->Handle, PageIndex, Index, Resolution );
            R = new Drawing( this, H );
            return R;
        };


        // added by mironichev - begin
		Drawing * AddVisibleSignature ( Rectangle Rect, ppInt32 Resolution ){            
			Drawing * R;
			TPDFRect pdfR;
			pdfR.xl = Rect.Left;
            pdfR.xr = Rect.Right;
            pdfR.yr = Rect.Top;
            pdfR.yl = Rect.Bottom;
			
            PBXHandle H;
/*            if(bExist)
				H = PDFPageCreateDrawingAndRepackContents( Doc->Handle, PageIndex, Resolution );
			else
				H = PDFPageCreateDrawingFromContent( Doc->Handle, PageIndex, Index, Resolution );
*/
			H = PDFPagePlaceSignature(Doc->Handle, PageIndex, pdfR, Resolution);
            R = new Drawing( this, H );
            return R;
        };

		Drawing * AddVisibleSignature ( Rectangle Rect){            
			Drawing * R;
			TPDFRect pdfR;
			pdfR.xl = Rect.Left;
            pdfR.xr = Rect.Right;
            pdfR.yr = Rect.Top;
            pdfR.yl = Rect.Bottom;
			
            PBXHandle H;
/*            if(bExist)
				H = PDFPageCreateDrawingAndRepackContents( Doc->Handle, PageIndex, Resolution );
			else
				H = PDFPageCreateDrawingFromContent( Doc->Handle, PageIndex, Index, Resolution );
*/
			H = PDFPagePlaceSignature(Doc->Handle, PageIndex, pdfR, 72);
            R = new Drawing( this, H );
            return R;
        };
		// added by mironichev - end

	protected:
		bool bExist;
};


/// <summary>
/// Destionation class implements destinations used with Outlines to specify how to display page or area, or page in external document
/// </summary>
	public __gc class Destination {
	private:
		PDFDestinationHandle Dest;
	public:

		Destination ( Document * Doc, String * Str, ppBool IsIndirect ){
			OSStrToPCharB( Str, s );
			try {
				Dest = PDFDestinationFromString ( Doc->Handle, s, Str->Length, IsIndirect ) ;
			} __finally {
				OSStrToPCharE ( s );
			}
		}
		Destination ( Document * Doc, DestinationInfo Destination, ppBool IsOtherDocument, ppBool IsIndirect){
			PDFExplicitDest D;
			D.a = Destination.a;
			D.b = Destination.b;
			D.c = Destination.c;
			D.d = Destination.d;
			D.Page = Destination.PageIndex;
			D.Type = (PDFExplicitDestType)Destination.Type;
			Dest = PDFDestinationFromExplit ( Doc->Handle, D, IsOtherDocument, IsIndirect );
		}
		__property PDFDestinationHandle get_Handle (){
			return Dest;
		}
		static DestinationInfo Zoom( ppInt32 page, ppReal Left, ppReal Top, ppReal Zoom ){
			DestinationInfo TMP;
			TMP.Type = DestinationType::Zoom;
			TMP.PageIndex = page;
			TMP.a = Left;
			TMP.b = Top;
			TMP.c = Zoom;
			return TMP;
		}
		static DestinationInfo FitPage( ppInt32 page){
			DestinationInfo TMP;
			TMP.Type = DestinationType::FitPage;
			TMP.PageIndex = page;
			return TMP;
		}
		static DestinationInfo FitWidth( ppInt32 page, ppReal Top){
			DestinationInfo TMP;
			TMP.Type = DestinationType::FitWidth;
			TMP.PageIndex = page;
			TMP.a = Top;
			return TMP;
		}
		static DestinationInfo FitHeight( ppInt32 page, ppReal Left){
			DestinationInfo TMP;
			TMP.Type = DestinationType::FitHeight;
			TMP.PageIndex = page;
			TMP.a = Left;
			return TMP;
		}
		static DestinationInfo FitRectangle( ppInt32 page, ppReal Left, ppReal Bottom, ppReal Right, ppReal Top ){
			DestinationInfo TMP;
			TMP.Type = DestinationType::FitRectangle;
			TMP.PageIndex = page;
			TMP.a = Left;
			TMP.b = Bottom;
			TMP.c = Right;
			TMP.d = Top;
			return TMP;
		}
		static DestinationInfo FitBounds( ppInt32 page){
			DestinationInfo TMP;
			TMP.Type = DestinationType::FitBounds;
			TMP.PageIndex = page;
			return TMP;
		}
		static DestinationInfo FitBoundsByWidth( ppInt32 page, ppReal Top){
			DestinationInfo TMP;
			TMP.Type = DestinationType::FitBoundsByWidth;
			TMP.PageIndex = page;
			TMP.a = Top;
			return TMP;
		}
		static DestinationInfo FitBoundsByHeight( ppInt32 page, ppReal Left){
			DestinationInfo TMP;
			TMP.Type = DestinationType::FitBoundsByHeight;
			TMP.PageIndex = page;
			TMP.a = Left;
			return TMP;
		}
	};


/// <summary>
/// Annotation class implements base annotation object in PDF (do not used directly, see AnnotationText and other classes)
/// </summary>
	public __gc __abstract class Annotation: public IDisposable{
    protected:
        PDFAnnotationHandle Annot;
        Document * FDoc;
protected:
		virtual void Dispose(bool /*disposeManagedResources*/)
		{
			// process only if mananged and unmanaged resources have
			// not been disposed of.
			if (!this->disposed)
			{
		  
			// DISPOSING OBJECTS

			FDoc = NULL;

		    disposed=true;
			}
		}
	private:
		bool disposed;
	private public:
		Annotation (){
			disposed = false;
			Annot = NULL;
			FDoc = NULL;
		}
		__property PDFAnnotationHandle get_Handle (){
			return Annot;
		}

    public:
        ~Annotation(){
			// call Dispose with false.  Since we're in the
			// destructor call, the managed resources will be
			// disposed of anyways.
			Dispose(false);
        }

   void Dispose()
   {
      Dispose(true);
      // tell the GC that the Finalize process no longer needs
      // to be run for this object
      System::GC::SuppressFinalize(this);
   }
/// <summary>
/// Sets annotations settings flags
/// </summary>
/// <param name="Value">Annotation settings flags</param>
        __property void set_Flags( AnnotationFlag Value ){
			PDFAnnotationSetFlag( Annot, (ppUns16)Value);
		}		
/// <summary>
/// Sets alpha blending value for the annotation
/// </summary>
/// <param name="Value">Alpha blending value</param>
        __property void set_AlphaBlendingValue ( ppReal Value ){
			PDFAnnotationSetAlphaBlending( Annot, Value );
		}
/// <summary>
/// Sets annotation title
/// </summary>
/// <param name="Value">New annotation title</param>
        __property void set_Title ( String * Value ){
			PDFStrToPCharB( Value, T);
			try {
				PDFAnnotationSetTitle( Annot, T, StrLen );
			} __finally {
				PDFStrToPCharE ( T );
			}
		}
/// <summary>
/// Sets annotation name
/// </summary>
/// <param name="Value">Specifies annotation's name.</param>
        __property void set_Name ( String * Value ){
			PDFStrToPCharB( Value, N);
			try {
				PDFAnnotationSetName( Annot, N, StrLen );
			} __finally {
				PDFStrToPCharE ( N );
			}
		}
/// <summary>
/// Sets annotation border style
/// </summary>
/// <param name="Width">Border width</param>
/// <param name="Style">Border style type</param>
/// <param name="Dash">Cos Object with dash pattern</param>
/// <returns>None</returns>
		void SetBorderStyle ( ppReal Width, AnnotationBorderStyleType Style, CosObjectBase * Dash ){
			PDFAnnotationSetBorderStyle( Annot, Width, (TBorderStyle)Style, Dash->Handle );
		}
/// <summary>
/// Sets annotation color (RGB). Use CreateRGBColor() to create new RGB color
/// </summary>
/// <param name="Value">New RGB color value</param>
        __property void set_Color ( RGBColor Value){
			TPDFColor C;
			C.Device = cgRGB;
			C.Color.RGB.R = Value.R;
			C.Color.RGB.G = Value.G;
			C.Color.RGB.B = Value.B;
			PDFAnnotationSetColor( Annot, C );
		}
/// <summary>
/// Sets action to execute on annotation activation
/// </summary>
/// <param name="Value">Existing action to execute</param>
        __property void set_action ( Action * Value );
    };	
/// <summary>
/// AnnotationText class implements text annotation object
/// </summary>
	public __gc class AnnotationText: public Annotation {
	public:
/// <summary>
/// Creates new text annotation object.
/// </summary>
/// <param name="page">Page to place annotation into</param>
/// <param name="ARectangle">Rectangle of the annotation</param>
/// <param name="Content">Text content of the annotation</param>
/// <param name="IsOpened">True to display annotation in open mode or False to display annotation closed</param>
/// <param name="AType">Icon type displayed for the annotation</param>
/// <returns>AnnotationText object</returns>
		AnnotationText ( Page * page, Rectangle ARectangle, String * Content, ppBool IsOpened, AnnotationTextType AType  ){
			TPDFRect R;
			R.xl = ARectangle.Left;
			R.xr = ARectangle.Right;
			R.yl = ARectangle.Bottom;
			R.yr = ARectangle.Top;
			PDFStrToPCharB ( Content, c );
			try {
				Annot = PDFPageAppendAnnotationText ( page->Parent->Handle, page->Index, R, c, StrLen, IsOpened, (TAnnotationTextName) AType );
			} __finally {
				PDFStrToPCharE ( c );
			}		
		}
	};
/// <summary>
/// AnnotationLink class implements link annotation object in PDF
/// </summary>
	public __gc class AnnotationLink: public Annotation {
	public:
/// <summary>
/// Creates new link annotation object.
/// </summary>
/// <param name="page">Page to place annotation into</param>
/// <param name="ARectangle">Rectangle of the annotation</param>
/// <param name="Content">Text content of the annotation</param>
/// <param name="Mode">Default highlighting mode type</param>
/// <param name="Visible">True to draw border or False to make border invisible</param>
/// <returns>AnnotationLink object</returns>
		AnnotationLink ( Page * page, Rectangle ARectangle, Destination * Destination, HighlightingModeType Mode, ppBool Visible ){
			TPDFRect R;
			R.xl = ARectangle.Left;
			R.xr = ARectangle.Right;
			R.yl = ARectangle.Bottom;
			R.yr = ARectangle.Top;
			Annot = PDFPageAppendAnnotationLinkWithDest( page->Parent->Handle, page->Index, R, Destination->Handle, (THighLightMode) Mode, Visible );
		}
/// <summary>
/// Create new link annotation.
/// </summary>
/// <param name="page">Page to place annotation into</param>
/// <param name="ARectangle">Rectangle of the annotation</param>
/// <param name="Action">Existing action to execute when annotation is activated</param>
/// <param name="Mode">Default annotation highlighting mode</param>
/// <param name="Visible">True to draw border or False to make border invisible</param>
/// <returns>AnnotationLink object</returns>
		AnnotationLink ( Page * page, Rectangle ARectangle, Action * action, HighlightingModeType Mode, ppBool Visible );
	};
/// <summary>
/// AnnotationStamp class implement stamp annotation object
/// </summary>
	public __gc class AnnotationStamp: public Annotation {
	public:
/// <summary>
/// Creates new stamp annotaion object.
/// </summary>
/// <param name="page">Page to place annotation into</param>
/// <param name="ARectangle">Rectangle of the annotation</param>
/// <param name="Content">Text content of the annotation</param>
/// <param name="Name">Default stamp icon type for use in the stamp annotation</param>
/// <returns>AnnotationStamp object</returns>
		AnnotationStamp ( Page * page, Rectangle ARectangle, String * Content, AnnotationStampIconType Name  ){
			TPDFRect R;
			R.xl = ARectangle.Left;
			R.xr = ARectangle.Right;
			R.yl = ARectangle.Bottom;
			R.yr = ARectangle.Top;
			PDFStrToPCharB ( Content, c );
			try {
				Annot = PDFPageAppendAnnotationStamp ( page->Parent->Handle, page->Index, R, c, StrLen, (TAnnotationStampName) Name );
			} __finally {
				PDFStrToPCharE ( c );
			}		
		}
	};

/// <summary>
/// Action is the base class for all action objects. Do not use this class directly
/// </summary>
	public __gc __abstract class Action: public IDisposable
{
    protected:
        Document * FDocument;
        PDFActionHandle action;

		virtual void Dispose(bool /*disposeManagedResources*/)
		{
			// process only if mananged and unmanaged resources have
			// not been disposed of.
			if (!this->disposed)
			{
		  
			// DISPOSING OBJECTS
			FDocument = NULL;
		    disposed=true;
			}
		}
	
	private:
		bool disposed;
	private public:
		__property PDFActionHandle get_Handle (){
			return action;
		}
		Action (){
			disposed = false;
			action = NULL;
			FDocument = NULL;
		}
    public:
   void Dispose()
   {
      Dispose(true);
      // tell the GC that the Finalize process no longer needs
      // to be run for this object
      System::GC::SuppressFinalize(this);
   }
		~Action (){
			// call Dispose with false.  Since we're in the
			// destructor call, the managed resources will be
			// disposed of anyways.
			Dispose(false);
		}
/// <summary>
/// Sets action to execute after current action
/// </summary>
/// <param name="AAction">Existing action object</param>
/// <returns>None</returns>
        void SetNextActionToExecute ( Action * AAction){
            PDFActionSetNext(  action , AAction->action);
        }
    };
/// <summary>
/// ActionGoTo class implements GoTo action object. Use this action for actions to change page in the current page
/// </summary>
    public __gc class ActionGoTo: public Action {
    public:
/// <summary>
/// Creates new GoTo action object
/// </summary>
/// <param name="Document">Parent document</param>
/// <param name="Destination">Destionation info object</param>
/// <returns>ActionGoTo object</returns>
        ActionGoTo ( Document * Document, Destination *Destination ){
            
            action = PDFActionNewGoToDestination( Document->Handle, Destination->Handle );
        }
    };

/// <summary>
/// ActionGoToRemote class implement GoToRemote action object in PDF. This action is used to open external documents
/// </summary>
	public __gc class ActionGoToRemote: public Action {
    public:
/// <summary>
/// Creates new GoToRemote action object
/// </summary>
/// <param name="Document">Parent document</param>
/// <param name="AFileName">Filename of external PDF document to open</param>
/// <param name="Destination">Destination info</param>
/// <param name="InNewWindow">True to open in new window, False to open in the current document window</param>
/// <returns>ActionGoToRemote object</returns>
        ActionGoToRemote ( Document * Document, String * AFileName, Destination *Destination, ppBool AInNewWindow ){
			AFileName = AFileName->Replace('\\', '//');

            FDocument = Document;
            OSStrToPCharB ( AFileName , FN );
            try {
                action = PDFActionNewGoToRemote( Document->Handle, FN, AFileName->Length, Destination->Handle, AInNewWindow );
            } __finally {
                OSStrToPCharE ( FN );
            }
        }
    };
/// <summary>
/// ActionLaunch class implements Launch application action. This action is used to launch external applications or non-PDF documents from PDF document
/// </summary>
	public __gc class ActionLaunch: public Action {
    public:
/// <summary>
/// Creates new Launch action object
/// </summary>
/// <param name="Document">Parent document</param>
/// <param name="FileName">Filename (application or document) to launch</param>
/// <param name="DefaultDir">Default folder for the application</param>
/// <param name="Operation">For documents only: command string specifying default Explorer action to execute. This can be &qt;print&qt; to print document or &qt;open&qt; to open document</param>
/// <param name="Params">For applications only: command line parameters string to pass to application</param>
/// <param name="InNewWindow">True to open in new window, False to open in the current document window</param>
/// <returns>ActionLaunch object</returns>

		ActionLaunch( Document * Document, String * FileName, String * DefaultDir, String * Operation, String * Params, ppBool InNewWindow){
            OSStrToPCharB ( FileName, FN );
            OSStrToPCharB ( DefaultDir, DF );
            OSStrToPCharB ( Operation, O );
            OSStrToPCharB ( Params, P );
            try {
                action = PDFActionNewLaunch( Document->Handle, FN, DF, O, P , InNewWindow );
            } __finally {
                OSStrToPCharE ( DF );
                OSStrToPCharE ( FN );
                OSStrToPCharE ( O );
                OSStrToPCharE ( P );
            }
        }
    };


/// <summary>
/// Structure used in ActionThread constructor to define information about thread used in action
/// </summary>
	public __value struct ThreadActionSettings {
	/// <summary>
	/// Destination thread type
	/// </summary>
        ThreadActionType DestThreadType;
	/// <summary>
	/// Destination thread object
	/// </summary>
		Thread       *thread;
	/// <summary>
	/// Index of destination thread in document
	/// </summary>
        ppInt32         ThreadIndex;		
	/// <summary>
	/// Destination thread title
	/// </summary>
        String          *ThreadTitle;
	/// <summary>
	/// Destination bead type
	/// </summary>
		BeadActionType   DestBeadType;
	/// <summary>
	/// Destination bead object in the destination thread
	/// </summary>
        Bead         *Bead;
	/// <summary>
	/// Index of destination bead
	/// </summary>
        ppInt32         BeadIndex;
    };


/// <summary>
/// ActionThread class implements Thread action object in PDF. This action is used to begin reading an article thread in PDF document
/// </summary>
	public __gc class ActionThread: public Action {
    public:
/// <summary>
/// Creates new Thread action object
/// </summary>
/// <param name="Document">Parent document</param>
/// <param name="FileName">Filename of PDF document with destination thread in it. If Filename is empty then current document is used for the action</param>
/// <param name="Launch">Destination information for the action</param> 
/// <returns>ActionThread object</returns>
        ActionThread( Document * Document, String * FileName, ThreadActionSettings ThreadInfo){
			PDFThreadActionParam P = {};		
            FDocument = Document;
            if ( ThreadInfo.DestThreadType != ThreadTitle ){
                ThreadInfo.ThreadTitle = "";
            }
            PDFStrToPCharB(ThreadInfo.ThreadTitle, ch );
            OSStrToPCharB ( FileName, FN );
            ppBool UseStr = false;
            switch ( ThreadInfo.DestThreadType) {
                case ThreadClass:
                    P.DestThreadType = taThreadHandle;
                    P.DestThread.ThreadHandle = ThreadInfo.thread->Handle;
            	    break;
                case ThreadIndex:
                    P.DestThreadType = taThreadIndex;
                    P.DestThread.ThreadIndex = ThreadInfo.ThreadIndex;
                    break;
                case ThreadTitle:
                    UseStr = true;
                    P.DestThreadType = taThreadTitle;
                    P.DestThread.ThreadTitle.String = ch;
                    P.DestThread.ThreadTitle.Length = StrLen;
                    break;
            }
            switch ( ThreadInfo.DestBeadType) {
                case BeadClass:
                    P.DestBeadType = taBeadHandle;
                    P.DestBead.BeadHandle = ThreadInfo.Bead->Handle;
                    break;
                case ThreadIndex:
                    P.DestBeadType = taBeadIndex;
                    P.DestBead.BeadIndex = ThreadInfo.BeadIndex;
                    break;
            }
            try {
                action = PDFActionNewThread( FDocument->Handle, FN, FileName->Length, P);
            } __finally {
                PDFStrToPCharE ( ch );
                OSStrToPCharE ( FN );
            }
        }
    };
/// <summary>
/// ActionURL class implements action that opens URL link
/// </summary>
    public __gc class ActionURL : public Action {
    public:
/// <summary>
/// Creates new ActionURL object
/// </summary>
/// <param name="Document">Parent document</param>
/// <param name="AURL">URL link to open</param>
/// <param name="IsMap">Set to True to track mouse position</param>
/// <returns>ActionURL object</returns>
        ActionURL ( Document * Document, String * AURL, ppBool AIsMap){
            FDocument = Document;
            OSStrToPCharB ( AURL, u );
            try {
                action = PDFActionNewURI( FDocument->Handle, u, AURL->Length, AIsMap);
            } __finally {
                OSStrToPCharE ( u );
            }
        }
    };
/// <summary>
/// ActionHide class implements action that will show or hide annotations. Use .AddAnnotationToHide to add annotations to the list of annotations to hide/show. If no annotations were added to the list then all annotations in the document will hide/show
/// </summary>
	public __gc class ActionHide : public Action {
    public:
/// <summary>
/// Creates new ActionHide object.
/// </summary>
/// <param name="Document">Parent document</param>
/// <param name="IsHide">Set to True to hide annotations or False to show annotations</param>
/// <returns>ActionHide object</returns>
        ActionHide ( Document * Document, ppBool AIsHide){
			FDocument = Document;
            action = PDFActionNewHide( FDocument->Handle, AIsHide );
        }
/// <summary>
/// Appends annotation to the list of annotations to hide/show. If no annotations were added to the list then all annotations in the document will hide/show.
/// </summary>
/// <param name="Annotation">Existing Annotation</param>
/// <returns>None</returns>
        void AddAnnotationToHide ( Annotation * annotation ){
            PDFActionHideAddAnnotation( action, annotation->Handle );
        }
/// <summary>
/// Appends annotation to the list of annotations to hide/show. If no annotations were added to the list then all annotations in the document will hide/show.
/// </summary>
/// <param name="AnnotationName">Name of the annotation which is needed to beappended to the list.</param>
/// <returns>None</returns>
        void AddAnnotationToHide ( String * AnnotationName ){
            OSStrToPCharB ( AnnotationName, AN );
            try {
                PDFActionHideAddAnnotationName(  action, AN, AnnotationName->Length );
            } __finally {
                OSStrToPCharE ( AN );
            };
        }
	};
/// <summary>
/// ActionNamed class implements action that executes an action predefined by the viewer application
/// </summary>
	public __gc class ActionNamed : public Action {
    public:
/// <summary>
/// Creates new Named action object
/// </summary>
/// <param name="Document">Parent document</param>
/// <param name="ANamedType">Named action type</param>
/// <returns>ActionNamed object</returns>
        ActionNamed ( Document * Document, NamedActionType ANamedType){
            FDocument = Document;
            action = PDFActionNewNamed( FDocument->Handle,(PDFNamedActionType) ANamedType);
        }
    };

/// <summary>
/// ActionSubmitForm class implements form submission to URL
/// </summary>
	public __gc class ActionSubmitForm : public Action {
    public:
/// <summary>
/// Creates new SubmitForm action object
/// </summary>
/// <param name="Document">Parent document</param>
/// <param name="AURL">URL to submit data</param>
/// <param name="AFlags">Set of form submission settings flags</param>
/// <returns>ActionSubmitForm object</returns>
        ActionSubmitForm ( Document * Document, String * AURL, SubmitFormFlags AFlags){
            FDocument = Document;
            OSStrToPCharB ( AURL, U );
            try {
                action = PDFActionNewSubmitForm( FDocument->Handle, U, AURL->Length, ( ppInt32 ) AFlags );
            } __finally {
                OSStrToPCharE ( U );
            }
        }
/*
/// <summary>
/// Appends control to use its data it form submission
/// </summary>
/// <param name="Annotation">Annotation object which need appendto list.</param>
/// <returns>None.NoteOperation will be performed for all acroform object in thePDF document (flag PDF_SUBMIT_FORM_FLAG_EXCLUDE not used ) ifits not selected any annotation for this action.</returns>
        void Add ( Annotation * annotation ){
            PDFActionSubmitFormAddAnnotation(  action, annotation->Handle );
        }
/// <summary>
/// Appends annotation to list in the submitform action.
/// </summary>
/// <param name="AnnotationName">Name of the annotation which is needed to beappended to the list.</param>
/// <returns>None.NoteOperation will be performed for all acroform object in thePDF document (flag PDF_SUBMIT_FORM_FLAG_EXCLUDE not used ) ifits not selected any annotation for this action.</returns>
        /// <summary>
/// void Add ( String * AnnotationName ){OSStrToPCharB ( AnnotationName, AN );try {PDFActionSubmitFormAddAnnotationName( action, AN, AnnotationName->Length );} __finally {OSStrToPCharE ( AN );};}
/// </summary>
*/
	};

/// <summary>
/// ActionResetForm class implementing form reset action to set form fields to their default values. All fields are reset or add fields to the list of selected fields using .Add method
/// </summary>
	public __gc class ActionResetForm : public Action {
    public:
/// <summary>
/// Creates new ResetForm action object
/// </summary>
/// <param name="Document">Parent document</param>
/// <param name="Exclude">If True then fields added using .Add method are excluded from form reset. False to reset selected fields added using .Add. If not fields were added to the internal list using .Add then all form fields are reseted </param>

        ActionResetForm ( Document * Document, ppBool Exclude){
            FDocument = Document;
            action = PDFActionNewResetForm( FDocument->Handle, Exclude );
        }

/*

/// <summary>
/// Appends annotation to list in the resetform action.
/// NoteReset action will be performed for all annotations in the PDFdocument if its not selected any annotation for this action.
/// </summary>
/// <param name="Annotation">Annotation object which is needed tobe appended to list.</param>
/// <returns>None.</returns>
        void Add ( Annotation * annotation ){
			PDFActionResetFormAddAnnotation( action, annotation->Handle );
        }
/// <summary>
/// Appends annotation to list in the resetform action.
///	Note Reset action will be performed for all annotations in the PDFdocument if its not selected any annotation for this action.
/// </summary>
/// <param name="AnnotationName">Name of the annotation which is needed to beappended to the list.</param>
/// <returns>None.</returns>
        void Add ( String * AnnotationName ){
            OSStrToPCharB ( AnnotationName, AN );
            try {
                PDFActionResetFormAddAnnotationName( action, AN, AnnotationName->Length );
            } __finally {
                OSStrToPCharE ( AN );
            };
        }
*/
};

/// <summary>
/// ActionImportData class implements import data action to import form fields values from external file
/// </summary>
	public __gc class ActionImportData : public Action {
    public:
/// <summary>
/// Creates new Import Data action object
/// </summary>
/// <param name="Document">Parent document</param>
/// <param name="AFileName">Filename of FDF data file to import form fields values from</param>
/// <returns>ActionImportData object</returns>
        ActionImportData ( Document * Document, String * AFileName){
            FDocument = Document;
            OSStrToPCharB ( AFileName, FN );
            try {
                action = PDFActionNewImportData( FDocument->Handle, FN, AFileName->Length );
            } __finally {
                OSStrToPCharE ( FN );
            }
        }
    };
/// <summary>
/// ActionJavaScript class implements javascript execution action
/// </summary>
	public __gc class ActionJavaScript : public Action {
    public:
/// <summary>
/// Creates new JavaScript action object
/// </summary>
/// <param name="Document">Parent document</param>
/// <param name="AJavaScriptString">JavaScript code to execute</param>
/// <returns>ActioJavaScript object</returns>
        ActionJavaScript ( Document * Document, String * AJavaScriptString){
            FDocument = Document;
            OSStrToPCharB ( AJavaScriptString, JS );
            try {
                action = PDFActionNewJavaScript( FDocument->Handle, JS, AJavaScriptString->Length );
            } __finally {
                OSStrToPCharE ( JS );
            }
        }

/// <summary>
/// Creates new JavaScript action object
/// </summary>
/// <param name="Document">Parent document</param>
/// <param name="AJavaScriptString">JavaScript code to execute</param>
/// <param name="AJavaScriptStream">Cos PDFDataStream object with Javascript code stored</param>
/// <returns>ActioJavaScript object</returns>
        ActionJavaScript ( Document * Document, CosStreamObject * AJavaScriptStream){
            FDocument = Document;
            action = PDFActionNewJavaScriptStream( FDocument->Handle, AJavaScriptStream->Handle );
        }
    };
/// <summary>
/// AcroControl is a base abstract class for all control (acrocontrol) objects 
/// IMPORTANT: In the Full version of the product use of this method/property requires an additional "Acroforms Objects License" license
/// </summary>
	public __gc __abstract class AcroControl: public IDisposable {
	protected:
		Document * FDocument;
		ppUns32 AcroIndex;

protected:
		virtual void Dispose(bool /*disposeManagedResources*/)
		{
			// process only if mananged and unmanaged resources have
			// not been disposed of.
			if (!this->disposed)
			{
		  
			// DISPOSING OBJECTS

			FDocument = NULL;


		    disposed=true;
			}
		}


	private:
		bool disposed;
	private public:
		AcroControl(){
			disposed = false;
			FDocument = NULL;
			AcroIndex = ppMAXUns32;
		}
	public:
		~AcroControl(){
			// call Dispose with false.  Since we're in the
			// destructor call, the managed resources will be
			// disposed of anyways.
			Dispose(false);			
		}


   void Dispose()
   {
      Dispose(true);
      // tell the GC that the Finalize process no longer needs
      // to be run for this object
      System::GC::SuppressFinalize(this);
   }

/// <summary>
/// Sets control border style
/// </summary>
/// <param name="BorderColor">Border line color</param>
/// <param name="FillColor">Background color</param>
/// <param name="BorderWidth">Border line width </param>
/// <returns>None</returns>
		void SetBorderStyle ( Color BorderColor, Color FillColor, ppReal BorderWidth ){
			TPDFColor BC, FC;
			switch ( BorderColor.Space ){
				case ColorSpaceType::Gray:
					BC.Device = cgGray;
					BC.Color.Gray = BorderColor.Color1;
					break;
				case ColorSpaceType::RGB:
					BC.Device = cgRGB;
					BC.Color.RGB.R = BorderColor.Color1;
					BC.Color.RGB.G = BorderColor.Color2;
					BC.Color.RGB.B = BorderColor.Color3;
					break;
				case ColorSpaceType::CMYK:
					BC.Device = cgCMYK;
					BC.Color.CMYK.C = BorderColor.Color1;
					BC.Color.CMYK.M = BorderColor.Color2;
					BC.Color.CMYK.Y = BorderColor.Color3;
					BC.Color.CMYK.K = BorderColor.Color4;
					break;
				default:
					// should not happen
					assert(false);
					return;
			}
			switch ( FillColor.Space ){
				case ColorSpaceType::Gray:
					FC.Device = cgGray;
					FC.Color.Gray = FillColor.Color1;
					break;
				case ColorSpaceType::RGB:
					FC.Device = cgRGB;
					FC.Color.RGB.R = FillColor.Color1;
					FC.Color.RGB.G = FillColor.Color2;
					FC.Color.RGB.B = FillColor.Color3;
					break;
				case ColorSpaceType::CMYK:
					FC.Device = cgCMYK;
					FC.Color.CMYK.C = FillColor.Color1;
					FC.Color.CMYK.M = FillColor.Color2;
					FC.Color.CMYK.Y = FillColor.Color3;
					FC.Color.CMYK.K = FillColor.Color4;
					break;
				default:
					// should not happen
					assert(false);
					return;
			}
			PDFAcroObjectSetBorder( FDocument->Handle, AcroIndex,BC, FC, BorderWidth );
		}
/// <summary>
/// Sets font used in the control 
/// </summary>
/// <param name="FontIndex">Index of existing font object stored in the document</param>
/// <param name="FontSize">Text size</param>
/// <param name="FontColor">Text color</param>
/// <returns>None</returns>
		void SetFont ( ppUns32 FontIndex, ppReal FontSize, Color FontColor ){
			TPDFColor  FC;
			switch ( FontColor.Space ){
				case ColorSpaceType::Gray:
					FC.Device = cgGray;
					FC.Color.Gray = FontColor.Color1;
					break;
				case ColorSpaceType::RGB:
					FC.Device = cgRGB;
					FC.Color.RGB.R = FontColor.Color1;
					FC.Color.RGB.G = FontColor.Color2;
					FC.Color.RGB.B = FontColor.Color3;
					break;
				case ColorSpaceType::CMYK:
					FC.Device = cgCMYK;
					FC.Color.CMYK.C = FontColor.Color1;
					FC.Color.CMYK.M = FontColor.Color2;
					FC.Color.CMYK.Y = FontColor.Color3;
					FC.Color.CMYK.K = FontColor.Color4;
					break;
				default:
					// should not happen
					assert(false);
					return;
			}
			PDFAcroObjectSetFont( FDocument->Handle, AcroIndex, FontIndex, FontSize, FC );
		}
/// <summary>
/// Sets caption of the control
/// </summary>
/// <param name="Value">Control caption</param>
        __property void set_Caption ( String * Value){
			OSStrToPCharB ( Value, cap );
			try {
				PDFAcroObjectSetCaption( FDocument->Handle, AcroIndex, cap );
			} __finally {
				OSStrToPCharE ( cap );
			}
		}
/// <summary>
/// Sets control settings flags
/// </summary>
/// <param name="Value">Control settings flags</param>
        __property void set_Flags ( ControlFlags Value){
			PDFAcroObjectSetFlag( FDocument->Handle, AcroIndex, (ppUns32)Value );
		}
/// <summary>
/// Sets Action to execute on the given event type in the control
/// </summary>
/// <param name="Action">Action to execute</param>
/// <param name="EventType">Event type</param>
/// <returns>None</returns>
		void AddAction ( Action * action, ControlEventType EventType ){
			PDFAcroObjectAddAction( FDocument->Handle, AcroIndex, action->Handle, (TPDFAcroEventType) EventType );
		}
		static Color UseGrayColorOnly (){
			Color T;
			T.Space = ColorSpaceType::Gray;
			T.Color1 = NoPDFColor;
			return T;
		}
	};
/// <summary>
/// Class implements edit field control object
/// </summary>
	public __gc class EditBoxControl: public AcroControl{
	public:
/// <summary>
/// Creates new edit field control
/// </summary>
/// <param name="page">Parent page</param>
/// <param name="ARectangle">Control rectangle</param>
/// <param name="Name">Name of the edit field</param>
/// <returns>EditBoxControl object</returns>
		EditBoxControl (Page * page, Rectangle ARectangle, String * Name ){
			TPDFRect R;
			R.xl = ARectangle.Left;
			R.xr = ARectangle.Right;
			R.yl = ARectangle.Bottom;
			R.yr = ARectangle.Top;
			FDocument = page->Parent;
			OSStrToPCharB ( Name, N );
			try {
				AcroIndex = PDFPageAppendEditBox( page->Parent->Handle, page->Index, R, N);
			} __finally {
				OSStrToPCharE ( N );
			}
		}

		/// <summary>
		/// Reads max allowed length of a text in the text field.
		/// </summary>
		/// <returns>The max allowed length of a text in the text field.</returns>
        __property ppUns32 get_MaxLength()
		{
			return PDFAcroGetMaxLengthByIndex(FDocument->Handle, AcroIndex);
        }

		/// <summary>
		/// Set max length of the text can be entered into the text field
		/// </summary>
		/// <param name="Value">Max text length</param>
        __property void set_MaxLength (ppUns32 Value ){
			PDFAcroEditBoxSetMaxLen( FDocument->Handle, AcroIndex, Value );
		}

/// <summary>
/// Sets text alignment type
/// </summary>
/// <param name="Value">Sets text alignment type</param>
        __property void set_Alignment ( ControlTextAlignmentType Value){
			PDFAcroEditBoxSetAlign( FDocument->Handle, AcroIndex, (TPDFAcroQuadding) Value );
		}
	};

/// <summary>
/// Class implements signature box control
/// </summary>
	public __gc class SignatureBoxControl: public AcroControl{
	public:
/// <summary>
/// Creates new signature box control
/// </summary>
/// <param name="page">Parent page</param>
/// <param name="ARectangle">Rectangle area of the control</param>
/// <param name="Name">Name of the control</param>
/// <returns>SignatureBoxControl object</returns>
		SignatureBoxControl (Page * page, Rectangle ARectangle, String * Name ){
			TPDFRect R;
			R.xl = ARectangle.Left;
			R.xr = ARectangle.Right;
			R.yl = ARectangle.Bottom;
			R.yr = ARectangle.Top;
			FDocument = page->Parent;
			OSStrToPCharB ( Name, N );
			try {
				AcroIndex = PDFPageAppendSignatureBox( page->Parent->Handle, page->Index, R, N);
			} __finally {
				OSStrToPCharE ( N );
			}
		}
	};

/// <summary>
/// Class implements button control
/// </summary>
	public __gc class PushButtonControl: public AcroControl{
	public:
/// <summary>
/// Creates new button control
/// </summary>
/// <param name="page">Parent page</param>
/// <param name="ARectangle">Rectangle to define control area</param>
/// <param name="Name">Control name</param>
/// <returns>PushButtonControl object</returns>
		PushButtonControl (Page * page, Rectangle ARectangle, String * Name ){
			TPDFRect R;
			R.xl = ARectangle.Left;
			R.xr = ARectangle.Right;
			R.yl = ARectangle.Bottom;
			R.yr = ARectangle.Top;
			FDocument = page->Parent;
			OSStrToPCharB ( Name, N );
			try {
				AcroIndex = PDFPageAppendPushButton( page->Parent->Handle, page->Index, R, N);
			} __finally {
				OSStrToPCharE ( N );
			}
		}
/// <summary>
/// Sets bevel size
/// </summary>
/// <param name="Value">Bevel size value</param>
        __property void set_SetBevelSize (ppReal Value ){
			PDFAcroPushButtonSetMiter( FDocument->Handle, AcroIndex, Value );
		}
	};

/// <summary>
/// Class implements base class for radio box and checkbox controls
/// </summary>
	public __gc __abstract class AcroRadioAndCheckboxControl: public AcroControl{
	private public:
		AcroRadioAndCheckboxControl (){};
	public:
/// <summary>
/// Sets checkbox style for the control
/// </summary>
/// <param name="Sign">Sign type</param>
/// <param name="Style">Checkbox style (rectangle, circle) </param>
/// <returns>None</returns>
		void SetCheckboxStyle ( CheckBoxMarkStyleType Sign, CheckBoxStyleType Style){
			PDFAcroObjectSetStyle( FDocument->Handle, AcroIndex, (TPDFCheckBoxSign) Sign, (TPDFCheckBoxStyle) Style );
		}
	};

/// <summary>
/// Class implements radio button control
/// </summary>
	public __gc class RadioButtonControl :public AcroRadioAndCheckboxControl{
	public:

/// <summary>
/// Creates new radiobutton control
/// </summary>
/// <param name="page">Parent page</param>
/// <param name="ARectangle">Rectangle to draw control</param>
/// <param name="Name">Control name</param>
/// <param name="GroupName">Name of radio buttons group to link radio button to this group</param>
/// <param name="InitialState">Initial state of the control (checked or not)</param>
/// <returns>RadioButtonControl object</returns>
		RadioButtonControl (Page * page, Rectangle ARectangle, String * Name, String * GroupName, ppBool InitaialState ){
			TPDFRect R;
			R.xl = ARectangle.Left;
			R.xr = ARectangle.Right;
			R.yl = ARectangle.Bottom;
			R.yr = ARectangle.Top;
			FDocument = page->Parent;
			OSStrToPCharB ( Name, N );
			OSStrToPCharB ( GroupName, GN );
			try {
				AcroIndex = PDFPageAppendRadioButton( page->Parent->Handle, page->Index, R, N, GN, InitaialState );
			} __finally {
				OSStrToPCharE ( N );
				OSStrToPCharE ( GN );
			}
		}
	};

/// <summary>
/// Class implements checkbox control
/// </summary>
public __gc class AcroCheckBox :public AcroRadioAndCheckboxControl{
	public:
/// <summary>
/// Creates new checkbox control
/// </summary>
/// <param name="page">Parent page</param>
/// <param name="ARectangle">Rectangle to draw control</param>
/// <param name="Name">Control name</param>
/// <param name="InitialState">Initial control state (checked or not)</param>
/// <returns>AcroCheckBox object</returns>
		AcroCheckBox (Page * page, Rectangle ARectangle, String * Name, ppBool InitaialState ){
			TPDFRect R;
			R.xl = ARectangle.Left;
			R.xr = ARectangle.Right;
			R.yl = ARectangle.Bottom;
			R.yr = ARectangle.Top;
			FDocument = page->Parent;
			OSStrToPCharB ( Name, N );
			try {
				AcroIndex = PDFPageAppendCheckBox( page->Parent->Handle, page->Index, R, N, InitaialState );
			} __finally {
				OSStrToPCharE ( N );
			}
		}
	};

/// <summary>
/// Class implements base class for listbox and combobox controls
/// </summary>
	public __gc __abstract class ListBoxControlBase: public AcroControl{
	private public:
		ListBoxControlBase (){};
	public:
/// <summary>
/// Adds string items into the list
/// </summary>
/// <param name="Item">Item to add into the list</param>
/// <returns>None</returns>
		void AddItem ( String * Item ){
			OSStrToPCharB ( Item, itm );
			try {
				PDFAcroObjectAppendItem( FDocument->Handle, AcroIndex, itm );
			} __finally {
				OSStrToPCharE ( itm );
			}
		}
	};

/// <summary>
/// Class implements listbox control
/// </summary>
	public __gc class ListBoxControl : public ListBoxControlBase {
	public:
/// <summary>
/// Creates new listbox control
/// </summary>
/// <param name="page">Parent page</param>
/// <param name="ARectangle">Rectangle to draw control</param>
/// <param name="Name">Control name</param>
/// <returns>ListBoxControl object</returns>
		ListBoxControl ( Page * page, Rectangle ARectangle, String * Name ){
			TPDFRect R;
			R.xl = ARectangle.Left;
			R.xr = ARectangle.Right;
			R.yl = ARectangle.Bottom;
			R.yr = ARectangle.Top;
			FDocument = page->Parent;
			OSStrToPCharB ( Name, N );
			try {
				AcroIndex = PDFPageAppendListBox( page->Parent->Handle, page->Index, R, N );
			} __finally {
				OSStrToPCharE ( N );
			}
		}
	};

/// <summary>
/// Class implements combobox control
/// </summary>
	public __gc class ComboBoxControl : public ListBoxControlBase {
	public:
/// <summary>
/// Creates new combobox control object
/// </summary>
/// <param name="page">Parent page</param>
/// <param name="ARectangle">Rectangle to draw control</param>
/// <param name="Name">Control name</param>
/// <returns>ComboBoxControl object</returns>
		ComboBoxControl ( Page * page, Rectangle ARectangle, String * Name ){
			TPDFRect R;
			R.xl = ARectangle.Left;
			R.xr = ARectangle.Right;
			R.yl = ARectangle.Bottom;
			R.yr = ARectangle.Top;
			FDocument = page->Parent;
			OSStrToPCharB ( Name, N );
			try {
				AcroIndex = PDFPageAppendComboBox( page->Parent->Handle, page->Index, R, N );
			} __finally {
				OSStrToPCharE ( N );
			}
		}
	};


/// <summary>
/// Class implements list of controls
/// </summary>
	public __gc class ControlList{
	private:
		Document * FDocument;
	public private:
		ControlList (Document * Document){
			FDocument = Document;
		}
	public:
/// <summary>
/// Returns number of controls
/// </summary>
        __property ppUns32 get_Count(){
			return PDFAcroGetCount( FDocument->Handle );
		}
/// <summary>
/// Returns type of the control referenced by its index
/// </summary>
/// <param name="index">Index of control in the list</param>
/// <returns>ControlType value</returns>
		ControlType GetTypeByIndex( ppUns32 index ){
			return ( ControlType ) PDFAcroGetTypeByIndex( FDocument->Handle, index );
		}

/// <summary>
/// Returns Name of the control referenced by the index
/// </summary>
/// <param name="index">Index of control object in the list</param>
/// <returns>Text string containing name of the control or multiple names separated by the . symbol</returns>
		String * GetNameByIndex( ppUns32 index ){
			void * tmp;
			ppBool IsUnicode;
			ppUns32 Len;

			String *Str;
			tmp = PDFAcroGetNameByIndex( FDocument->Handle, index, &IsUnicode, &Len );
			if (!tmp)
				return new String ("");
			if (IsUnicode){
				Str = new String ( (__wchar_t *)tmp,0,Len );
			} else {
				Str = new String ( (char *)tmp, 0, Len );
			}
			return Str;
		}
/// <summary>
/// Returns value of the control referenced by the index
/// </summary>
/// <param name="index">Control index</param>
/// <returns>Text string value of the control</returns>
		String * GetValueByIndex( ppUns32 index ){
			char * tmp;
			ppUns32 Length;
			tmp = (char *)PDFAcroGetValueByIndex( FDocument->Handle, index, &Length );
			return PDFStringToString ( tmp, Length);
		}

/// <summary>
/// Returns value of the control referenced by the index
/// </summary>
/// <param name="OldName">old object name</param>
/// <param name="NewName">new object name</param>
/// <returns>True if object has been renamed, False otherwise</returns>
		bool RenameControl( String* OldName, String* NewName ){
			bool result = false;
			OSStrToPCharB ( OldName, OldN );
			OSStrToPCharB ( NewName, NewN );
			try {
			result = PDFAcroChangeNameByName (FDocument->Handle, OldN, NewN);
			} __finally {
				OSStrToPCharE ( OldN );
				OSStrToPCharE ( NewN );
			}
			return result;

		}

/// <summary>
/// Returns index of the control rerefenced by control name
/// </summary>
/// <param name="Name">Name of the control to find</param>
/// <returns>Index of the control ( zero-based index) or 0xFFFFFFFF if not found</returns>
		ppUns32 GetIndexByName ( String * index ){
			ppInt32 i;
			OSStrToPCharB ( index, idx );
			try {
				i = PDFAcroGetIndexByName( FDocument->Handle, idx, true );
			} __finally {
				OSStrToPCharE ( idx );
			}
			return i;
		}

/// <summary>
/// Sets value of the control referenced by its index
/// </summary>
/// <param name="index">Control index</param>
/// <param name="Value">New control value</param>
/// <returns>None</returns>
		void SetValueByIndex( ppUns32 index, String * Value ){
			OSStrToPCharB ( Value, v );
			try {
				PDFAcroSetValueByIndex( FDocument->Handle, index, v );
			} __finally {
				OSStrToPCharE ( v );
			}
		}
/// <summary>
/// Returns number of available values in the control (for listbox and combobox controls)
/// </summary>
/// <param name="index">Control index</param>
/// <returns>Number of available values</returns>
		ppUns32 GetOptionCountByIndex (ppUns32 index ){
			return PDFAcroGetOptionCountByIndex (FDocument->Handle, index);
		}
/// <summary>
/// Returns selected (by the index) value from the control (listbox or combobox) referenced by the index
/// </summary>
/// <param name="index">Control index</param>
/// <param name="OptionIndex">Index of the value option to return</param>
/// <returns>String value</returns>
		String * OptionValueByIndex( ppUns32 index, ppUns32 OptionIndex){
			char * Str;
			ppUns32 Len;
			Str = PDFAcroGetOptionValueByIndex (FDocument->Handle, index, OptionIndex, &Len);
			return PDFStringToString (Str, Len);
		}
	
	};
}
}