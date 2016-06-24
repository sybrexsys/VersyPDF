// This is the main DLL file.

#include "stdafx.h"

#include "VersyPDFNET.h"
#include "VSCosAI.h"

namespace Versy
{
namespace PDF
{
    CosObjectBase * PDFCosHandleToClass( PDFCosHandle Handle ){
        CosObjectBase * Obj ;
        switch( _CosGetType ( Handle ) ) {
            case CosName:
                Obj = new CosNameObject( Handle );
        	    break;
            case CosBoolean:
                Obj = new CosBooleanObject ( Handle );
                break;
            case CosReal:
                Obj = new CosRealObject ( Handle );
                break;
            case CosInteger:
                Obj = new CosIntegerObject ( Handle );
                break;
            case CosString:
                Obj = new CosStringObject ( Handle );
                break;
            case CosArray:
                Obj = new CosObjectArray ( Handle );
                break;
            case CosDict:
                Obj = new CosDictionaryObject ( Handle );
                break;
            case CosStream:
                Obj = new CosStreamObject ( Handle );
                break;
            case CosNull:
            default:
                Obj = new CosNullObject ( Handle );
                break;
        }
        return Obj;
    }
    Thread * Bead::get_thread(){
        PDFThreadHandle h;
        h = PDFBeadGetThread( bead );
        return new Thread( h );
    }
    Thread * Document::GetThreads( ppInt32 index){
        return new Thread( PDFDocGetThread ( FDoc, index) ) ;
    }

	void cpyw (char *s, const __wchar_t * Codes, ppUns32 Length)
	{
		ppUns32 i;
		for(i=0;i< Length;i++){
			if ( ( Codes[i] < 128 ) || ( ( Codes[i] < 255 ) && ( Codes[i] > 159 ) ) ){
				s[i]= (char)Codes[i];;
				continue;
			}
			switch (Codes[i]){
			case 8364: 
				s[i] = (unsigned char)128;
				continue;		
			case 129 : 
				s[i] = (unsigned char)129;
				continue;
			case 8218: 
				s[i] = (unsigned char)130;
				continue;
			case 402 : 
				s[i] = (unsigned char)131;
				continue;
			case 8222: 
				s[i] = (unsigned char)132;
				continue;
			case 8230: 
				s[i] = (unsigned char)133;
				continue;
			case 8224: 
				s[i] = (unsigned char)134;
				continue;
			case 8225: 
				s[i] = (unsigned char)135;
				continue;
			case 710 : 
				s[i] = (unsigned char)136;
				continue;
			case 8240: 
				s[i] = (unsigned char)137;
				continue;
			case 352 : 
				s[i] = (unsigned char)138;
				continue;
			case 8249: 
				s[i] = (unsigned char)139;
				continue;
			case 338 : 
				s[i] = (unsigned char)140;
				continue;
			case 141 : 
				s[i] = (unsigned char)141;
				continue;
			case 381 : 
				s[i] = (unsigned char)142;
				continue;
			case 143 : 
				s[i] = (unsigned char)143;
				continue;
			case 144 : 
				s[i] = (unsigned char)144;
				continue;
			case 8216: 
				s[i] = (unsigned char)145;
				continue;
			case 8217: 
				s[i] = (unsigned char)146;
				continue;
			case 8220: 
				s[i] = (unsigned char)147;
				continue;
			case 8221: 
				s[i] = (unsigned char)148;
				continue;
			case 8226: 
				s[i] = (unsigned char)149;
				continue;
			case 8211: 
				s[i] = (unsigned char)150;
				continue;
			case 8212: 
				s[i] = (unsigned char)151;
				continue;
			case 732 : 
				s[i] = (unsigned char)152;
				continue;
			case 8482: 
				s[i] = (unsigned char)153;
				continue;
			case 353 : 
				s[i] = (unsigned char)154;
				continue;
			case 8250: 
				s[i] = (unsigned char)155;
				continue;
			case 339 : 
				s[i] = (unsigned char)156;
				continue;
			case 157 : 
				s[i] = (unsigned char)157;
				continue;
			case 382 : 
				s[i] = (unsigned char)158;
				continue;
			case 376 : 
				s[i] = (unsigned char)159;
				continue;
			}
			;
			s[i] = (char)32;
		}
	}

    ppBool IsANSICodesOnly (const __wchar_t * Codes, ppUns32 Length)
    {
        ppUns32 i;
        __wchar_t Code;
        for ( i = 0 ; i < Length; i++ ){
            Code = Codes[i];
            if ( Code < 128 )
                continue;
            if ( ( Code < 255 ) && ( Code > 159 ) )
                continue;
            if ( ( Code == 8364 ) ||
                ( Code == 129 )  ||
                ( Code == 8218 ) ||
                ( Code == 402 )  ||
                ( Code == 8222 ) ||
                ( Code == 8230 ) ||
                ( Code == 8224 ) ||
                ( Code == 8225 ) ||
                ( Code == 710 )  ||
                ( Code == 8240 ) ||
                ( Code == 352 )  ||
                ( Code == 8249 ) ||
                ( Code == 338 )  ||
                ( Code == 141 )  ||
                ( Code == 381 )  ||
                ( Code == 143 )  ||
                ( Code == 144 )  ||
                ( Code == 8216 ) ||
                ( Code == 8217 ) ||
                ( Code == 8220 ) ||
                ( Code == 8221 ) ||
                ( Code == 8226 ) ||
				( Code == 8211 ) ||
				( Code == 8212 ) ||
                ( Code == 732 )  ||
                ( Code == 8482 ) ||
                ( Code == 353 )  ||
                ( Code == 8250 ) ||
                ( Code == 339 )  ||
                ( Code == 157 )  ||
                ( Code == 382 )  ||
                ( Code == 376 ) )
                continue;
            return false;
        }
        return true;      
    }

    String * PDFStringToString( char * Str, ppInt32 Length){
        if ( !Length || !Str )
            return new String("");
        if ( Length == 1 ){
            return new String( Str, 0, 1 );
        }
        if ( Str[0] == 0xFE && Str[1] == 0xFF ){
            return new String ( (__wchar_t *) Str, 1, (Length >> 1) - 1 );
        }
        return new String ( Str, 0, Length );
    }

    void Outline::set_action ( Action * Value){
        PDFOutlineSetAction( outline, Value->Handle );
    }
	void Outline::set_destination ( Destination * Value){
		PDFOutlineSetDestination( outline, Value->Handle );
	}
	void Annotation::set_action ( Action * Value ){
		PDFAnnotationSetAction( Annot, Value->Handle );
	}

	ControlList * Document::get_Controls(){
		if ( FAcro ) return FAcro;
		return FAcro = new ControlList ( this );
	}


	Bead * Thread::AddBead ( Page * page, Rectangle Rect ){
		TPDFRect R;
		R.xl = Rect.Left;
		R.xr = Rect.Right;
		R.yl = Rect.Bottom;
		R.yr = Rect.Top;
		return new Bead( PDFThreadAppendBead ( thread, page->Index, R ) );
	} 
	AnnotationLink::AnnotationLink ( Page * page, Rectangle ARectangle, Action * action, HighlightingModeType Mode, ppBool Visible ){
		TPDFRect R;
		R.xl = ARectangle.Left;
		R.xr = ARectangle.Right;
		R.yl = ARectangle.Bottom;
		R.yr = ARectangle.Top;
		Annot = PDFPageAppendAnnotationLinkWithAction ( page->Parent->Handle, page->Index, R, action->Handle, (THighLightMode) Mode, Visible );
	}

	Document* PDFDocEngine::AddDocument()
	{
		PDFDocEngine* Library = this;
		Document* doc = __gc new Document( Library );
		return doc;
	}

	Document* PDFDocEngine::AddDocument(PDFDataStream * APDFDataStream)
	{	
		PDFDocEngine* Library = this;
		Document* doc = __gc new Document( Library, APDFDataStream);
		return doc;
	}
	Document* PDFDocEngine::AddDocument(String * FileName)
	{
		PDFDocEngine* Library = this;
		Document* doc = __gc new Document( Library, FileName);
		return doc;
	}
	Document* PDFDocEngine::AddDocument(unsigned char Buffer __gc[], int BufferSize)
	{
		PDFDocEngine* Library = this;
		Document* doc = __gc new Document( Library, Buffer,  BufferSize);
		return doc;
	}

	Page* Document::AddPage(ppReal Width, ppReal Height)
	{
		ppUns32 PageIndex =
			PDFDocAppendPage( FDoc, Width, Height );

		Page* page =
			__gc new Page(this, PageIndex, false);

		return page; 
	}

	void Document::RemoveCOSDictionaryObjectByKey(String * Key)
	{
		CosDictionaryObject* Root = static_cast<CosDictionaryObject*>(this->GetRootCosObject());
		if (Root!=NULL){
			Root->RemoveKey(FLibrary->StringToAtom(Key));
		}
	}

	void Document::RemoveAllOutlines()
	{
		RemoveCOSDictionaryObjectByKey("Outlines");
	}

	void Document::RemoveAllPermissions()
	{
		RemoveCOSDictionaryObjectByKey("Perms");
	}

	Page* Document::AddPage( PageSizeType Size, PageOrientationType Orientation )
	{
		ppUns32 PageIndex =
			PDFDocAppendPage2( FDoc, ( TPDFPageSize ) Size, ( TPDFPageOrientation ) Orientation );

		Page* page =
			__gc new Page(this, PageIndex, false);

		return page; 
	}

	Page* Document::GetPage(ppUns32 PageIndex)
	{
		if((ppInt32)PageIndex > PageCount - 1)
			throw new Exception(
			String::Format("Page with index {0} not exists", PageIndex.ToString()));

		return __gc new Page(this, PageIndex, true);
	}

	// set initial properties for the document object
	void Document::SetInitialProperties()	
	{
		Linearized = false;
		Optimized = false;	
	}

	
	


	//void Page::Init()
	//{
	//}

	Image * PDFDocEngine::AddImage(ppUns32 Width, ppUns32 Height, ColorBitDepth Depth, ColorSpaceType Space)
	{
		Image* image =
			__gc new Image(this, Width, Height, Depth, Space);
		return image;
	}
	Image * PDFDocEngine::AddImage(String * FileName, ppUns32 Index)
	{
		Image* image =
			__gc new Image(this, FileName, Index);
		return image;
	}
	Image * PDFDocEngine::AddImage(PDFDataStream * AStream, ppUns32 Index)
	{
		Image* image =
			__gc new Image(this, AStream, Index);
		return image;
	}
	Image * PDFDocEngine::AddImage(unsigned char Buffer __gc[], ppUns32 BufferSize, ppUns32 Index)
	{
		Image* image =
			__gc new Image(this, Buffer, BufferSize, Index);
		return image;
	}
	Image * PDFDocEngine::AddImage(HBITMAP BitmapHandle)
	{
		Image* image =
			__gc new Image(this, BitmapHandle);
		return image;
	}

	PDFDataStream * PDFDocEngine::AddPDFDataStream ()
	{
		PDFDataStream * stream =
			__gc new PDFDataStream(this);
		return stream;
	}
	PDFDataStream * PDFDocEngine::AddPDFDataStream (String * FileName, FileOpenMode Mode )
	{
		PDFDataStream * stream =
			__gc new PDFDataStream(this, FileName, Mode);
		return stream;
	}
	PDFDataStream * PDFDocEngine::AddPDFDataStream (unsigned char Source __gc[], ppInt32 Length )
	{
		PDFDataStream * stream =
			__gc new PDFDataStream(this, Source, Length);
		return stream;
	}


	void Versy::PDF::Document::AddAttachedFile(String * FileName, String * FileNameToShowInDocument, String * Title, ppBool Compress)
	{
		// PDFDocAppendFileFromDisk
		// LIB_API void PDFDocAppendFileFromDisk ( 
		// PDFDocHandle Doc, char * FileName, char * EmbedFileName,char * Title, ppUns32 TitleLength, ppBool PackData);

		char* chFileName = (char*)Marshal::StringToHGlobalAnsi(FileName).ToPointer();
		char* chFileNameToShowInDocument = (char*)Marshal::StringToHGlobalAnsi(FileNameToShowInDocument).ToPointer();
		char* chTitle = (char*)Marshal::StringToHGlobalAnsi(Title).ToPointer();

		PDFDocAppendFileFromDisk(FDoc, chFileName, chFileNameToShowInDocument, chTitle, Title->Length, Compress);
	}

	void Versy::PDF::Document::AddAttachedFileFromBuffer(Byte Buffer __gc[], ppUns32 BufferSize, String * FileNameToShowInDocument, String * Title, ppBool Compress)
	{
		// PDFDocAppendFileFromBuffer ( 
		// PDFDocHandle Doc, void * Buffer, ppUns32 BufferSize, char * EmbedFileName, char * Title, 
		// ppUns32 TitleLength, ppBool PackData);
		char* chFileNameToShowInDocument = (char*)Marshal::StringToHGlobalAnsi(FileNameToShowInDocument).ToPointer();
		char* chTitle = (char*)Marshal::StringToHGlobalAnsi(Title).ToPointer();

		IntPtr ptr = Marshal::AllocHGlobal(BufferSize);
		Marshal::Copy(Buffer, 0, ptr, BufferSize);

		PDFDocAppendFileFromBuffer(FDoc, ptr.ToPointer(), BufferSize, chFileNameToShowInDocument, chTitle, Title->Length, Compress);
	}

	void Versy::PDF::Document::AddAttachedFileFromPDFStream(PDFDataStream * PDFStream, String * FileNameToShowInDocument, String * Title, ppBool Compress)
	{
		// PDFDocAppendFileFromStream ( PDFDocHandle Doc, PDFStreamHandle AStream, char * EmbedFileName, char * Title, ppUns32 TitleLength, ppBool PackData);
		char* chFileNameToShowInDocument = (char*)Marshal::StringToHGlobalAnsi(FileNameToShowInDocument).ToPointer();
		char* chTitle = (char*)Marshal::StringToHGlobalAnsi(Title).ToPointer();

		PDFDocAppendFileFromStream(FDoc, PDFStream->get_Handle(), chFileNameToShowInDocument, chTitle, Title->Length, Compress);
	}

	void Versy::PDF::Document::AddAttachedFileFromPDFStream(System::IO::Stream * Stream, String * FileNameToShowInDocument, String * Title, ppBool Compress)
	{
		Byte Buffer __gc[] = __gc new Byte[(int)Stream->Length];
		// Console::WriteLine();
		Stream->Read(Buffer, 0, Buffer->Length);

		 AddAttachedFileFromBuffer(Buffer, Buffer->Length, FileNameToShowInDocument, Title, Compress);
		// PDFStreamHandle handle;
		// PDFDataStream * PDFStream =
		//	new PDFDataStream(FDoc, );
		// AddAttachedFileFromPDFStream(PDFStream, FileNameToShowInDocument, Title, Compress);
	}

	void Versy::PDF::Document::AddStartupJavaScript(String * JSName, String * JS)
	{
		// void PDFDocAppendStartJavaScript ( PDFDocHandle Doc, char * JSName, char * JS);
		char* chJSName = (char*)Marshal::StringToHGlobalAnsi(JSName).ToPointer();
		char* chJS = (char*)Marshal::StringToHGlobalAnsi(JS).ToPointer();

		PDFDocAppendStartJavaScript(FDoc, chJSName, chJS);
	}

/// <summary>
/// Checks if the document contains PDF package or not
/// </summary>
/// <returns>Returns True if the document contains PDF package, False otherwise</returns>
		bool Versy::PDF::Document::IsPDFPackage()
	{
		CosDictionaryObject* Root;
		CosObjectBase* Obj;

Root = dynamic_cast<CosDictionaryObject*>(this->GetRootCosObject());

Obj = Root->GetValues(this->Owner->StringToAtom("Names"));

if(Obj->CosType!=CosObjectType::DictionaryType){	return false;}

Obj = dynamic_cast<CosDictionaryObject*>(Obj)->GetValues(this->Owner->StringToAtom("EmbeddedFiles"));

if(Obj->CosType !=CosObjectType::DictionaryType){return false;}

Obj = Root->GetValues(this->Owner->StringToAtom("Collection"));

if (Obj->CosType != CosObjectType::DictionaryType){	return false;}

return true; 
	}

}
}