#ifndef BYTESCOUTPDFTYPES_H
#define BYTESCOUTPDFTYPES_H
#include "VSTypes.h"

namespace Versy
{
namespace PDF
{
/// <summary>
/// CIE-based colors rendering mode (when rendered into device colors)
/// </summary>
    public __value enum ColorRenderingModeType {
		/// <summary>
		///  Colors are represented solely with respect to the light source; no correction is made for the output medium's white point (such as the color of unprinted paper). 
		/// </summary>
		AbsoluteColors,    
		/// <summary>
		///  Colors are represented with respect to the combination of the light source and the output medium's white point (such as the color of unprinted paper).    
		/// </summary>
		RelativeColors,   
        /// <summary>
		///  Colors are represented in a manner that preserves or emphasizes saturation. 
		/// </summary>
		Saturation,             
        /// <summary>
		///  Colors are represented in a manner that provides a pleasing perceptual appearance. 
		/// </summary>
		Perceptual              
    } ;
    
/// <summary>
/// Blending mode
/// </summary>
    public __value enum BlendingModeType{
        /// <summary>
		///  Selects the source color, ignoring the backdrop 
		/// </summary>
		Normal,                  
        /// <summary>
		///  Multiplies the backdrop and source color values *
		/// </summary>
		Multiply,                
		/// <summary>
		///  Multiplies the complements of the backdrop and source color values, then complements the result 
		/// </summary>
		Screen,                  
        /// <summary>
		///  Multiplies or screens the colors, depending on the backdrop color. 
		/// </summary>
		Overlay,                 
        /// <summary>
		///  Selects the darker of the backdrop and source colors. *
		/// </summary>
		Darken,                  
        /// <summary>
		///  Selects the lighter of the backdrop and source colors. 
		/// </summary>
		Lighten,                 
		/// <summary>
		///  Brightens the backdrop color to reflect the source color. Painting with black produces no change. 
		/// </summary>
		ColorDodge,              
        /// <summary>
		///  Darkens the backdrop color to reflect the source color. Painting with white produces no change. 
		/// </summary>
		ColorBurn,               
        /// <summary>
		///  Multiplies or screens the colors, depending on the source color value. 
		/// </summary>
		HardLight,               
        /// <summary>
		///  Darkens or lightens the colors, depending on the source color value. 
		/// </summary>
		SoftLight,               
        /// <summary>
		///  Subtracts the darker of the two constituent colors from the lighter. 
		/// </summary>
		Difference,              
		/// <summary>
		///  Produces an effect similar to that of the Difference mode, but lower in contrast. 
		/// </summary>
		Exclusion                
    } ;

/// <summary>
/// Password type of encrypted PDF Document
/// </summary>
    public __value enum PasswordValidationType  {
        /// <summary>
		///  Invalid Password
		/// </summary>
		Invalid                      = 0,
        /// <summary>
		///  User Password Type
		/// </summary>
		UserPassword                         = 1, 
        /// <summary>
		///  Owner Password TYpe
		/// </summary>
		OwnerPassword                        = 2  
    };

/// <summary>
/// Protection Key-Length Type of Crypted PDF Document
/// </summary>
    public __value enum SecurityKeyLength {
        /// <summary>
		///   40-Bit protection key length 
		/// </summary>
		_40Bit                           = 0, 
        /// <summary>
		///  128-Bit protection key length 
		/// </summary>
		_128Bit                          = 1, 
		/// <summary>
		///  128-Bit protection key length (AES protection)
		/// </summary>
		_128BitAES                       = 2,
		/// <summary>
		///  256-Bit protection key length (AES protection)
		/// </summary>
		_256BitAES                       = 3  
    } ;

/// <summary>
/// Supported PDF format versions
/// </summary>
    public __value enum Version {
        /// <summary>
		///  PDF Document format version 1.0
		/// </summary>
		v10                    = 0,
        /// <summary>
		///  PDF Document format version 1.1 
		/// </summary>
		v11                    = 1,        
        /// <summary>
		///  PDF Document format version 1.2 
		/// </summary>
		v12                    = 2,        
        /// <summary>
		///  PDF Document format version 1.3 
		/// </summary>
		v13                    = 3,        
        /// <summary>
		///  PDF Document format version 1.4 
		/// </summary>
		v14                    = 4,        
        /// <summary>
		///  PDF Document format version 1.5 
		/// </summary>
		v15                    = 5,		
        /// <summary>
		///  PDF Document format version 1.6 
		/// </summary>
		v16                    = 6 		
    } ;

/// <summary>
/// Page sizes used in PDF documents
/// </summary>
    public __value enum PageSizeType {
        /// <summary>
		///  Letter page size - 792 x 612 
		/// </summary>
		Letter = 0,   
        /// <summary>
		///  A4 page size - 842 x 595 
		/// </summary>
		A4,           
        /// <summary>
		///  A3 page size 1190 x 842 
		/// </summary>
		A3,           
        /// <summary>
		///  Legal page size 1008 x 612 
		/// </summary>
		Legal,        
        /// <summary>
		///  B5 page size 728 x 516 
		/// </summary>
		B5,           
        /// <summary>
		///  C5 page size 649 x 459 
		/// </summary>
		C5,           
        /// <summary>
		///  8x11 page size 792 x 595 
		/// </summary>
		_8x11_,         
        /// <summary>
		///  B4 page size 1031 x 728 
		/// </summary>
		B4,           
        /// <summary>
		///  A5 page size 595 x 419 
		/// </summary>
		A5,           
        /// <summary>
		///  Foilo page size 936 x 612 
		/// </summary>
		Folio,        
        /// <summary>
		///  Executive page size 756 x 522 
		/// </summary>
		Executive,    
        /// <summary>
		///  B4 Envelope page size 1031 x 728 
		/// </summary>
		EnvB4,        
        /// <summary>
		///  B5 Envelope page size 708 x 499 
		/// </summary>
		EnvB5,        
        /// <summary>
		///  C6 Envelope page size 459 x 323 
		/// </summary>
		EnvC6,        
        /// <summary>
		///  DL Envelope page size 623 x 312 
		/// </summary>
		EnvDL,        
        /// <summary>
		///  Monarch Envelope page size  540 x 279 
		/// </summary>
		EnvMonarch,   
        /// <summary>
		///  Envelope 9 page size  639 x 279 
		/// </summary>
		Env9,         
        /// <summary>
		///  Envelope 10 page size  684 x 297 
		/// </summary>
		Env10,        
        /// <summary>
		///  Envelope 11 page size  747 x 324 
		/// </summary>
		Env11         
    };

/// <summary>
/// Page orientation type
/// </summary>
    public __value enum PageOrientationType {
        /// <summary>
		///  Portrait 
		/// </summary>
		Portrait   = 0, 
        /// <summary>
		///  Landscape 
		/// </summary>
		LandScape       
    } ;

/// <summary>
/// Document information property type
/// </summary>
    public __value enum DocumentInfoPropertyType {  
        /// <summary>
		///  Information about creator 
		/// </summary>
		Creator = 0,      
        /// <summary>
		///  Information about author 
		/// </summary>
		Author,           
		/// <summary>
		///  Information about date
		/// </summary>
		CreationDate,            
        /// <summary>
		///  Information about producer 
		/// </summary>
		Producer,         
        /// <summary>
		///  Information about title 
		/// </summary>
		Title,            
        /// <summary>
		///  Information about subject 
		/// </summary>
		Subject,          
        /// <summary>
		///  Information about keywords 
		/// </summary>
		KeyWords,         
        /// <summary>
		///  Information about modification data
		/// </summary>
		ModificationDate
    };

    [System::Serializable, System::Flags]
/// <summary>
/// Document restriction flags
/// </summary>
    public __value enum DocumentPermissionType{
	/// <summary>
		///  Disable all
		/// </summary>
		DisableAll			  = 0,
        /// <summary>
		///  Allow printing of the document 
		/// </summary>
		Print                 = 1,   
        /// <summary>
		///  Allowe modify the content of the document 
		/// </summary>
		ModifyContent         = 2,   
        /// <summary>
		///  Allowe copy text and graphics from the document 
		/// </summary>
		CopyInformation       = 4,   
        /// <summary>
		///  Allowe add or modify annotations 
		/// </summary>
		ModifyAnnotation      = 8,   
        /// <summary>
		///  Allowe fill in interactive form fields 
		/// </summary>
		FillAcroForm          = 16,  
        /// <summary>
		///  Allowe extract text and graphics from the document 
		/// </summary>
		ExtractTextAndImages   = 32,  
        /// <summary>
		///  Allowe assemble the document 
		/// </summary>
		AssembleDocument      = 64,  
        /// <summary>
		///  Allowe print the document with high resolution 
		/// </summary>
		PrintHiResolution     = 128, 
		/// <summary>
		///  Allow all
		/// </summary>
		EnableAll			  = 255  	
    };

	/// <summary>
	/// Horizontal justification type
	/// </summary>
    public __value enum HorizontalJustificationType {
		/// <summary>
		/// Left
		/// </summary>
        Left,             
		/// <summary>
		/// Center
		/// </summary>
        Center,           
		/// <summary>
		/// Right
		/// </summary>
        Right
    } ;
    
		/// <summary>
		/// Vertical justification type
		/// </summary>
    public __value enum VerticalJustificationType { 
		/// <summary>
		/// Top
		/// </summary>
        Top,   
		/// <summary>
		/// Center
		/// </summary>
        Center, 
		/// <summary>
		/// Bottom
		/// </summary>
        Bottom
    };

/// <summary>
/// The line join style specifies the shape to be used at the corners of paths that are stroked.
/// </summary>
     public __value enum LineJoinType {
        /// <summary>
		// The outer edges of the strokes for the two segments are extended until Miter,/*The outer edges of the strokes for the two segments are extended untilthey meet at an angle, as in a picture frame. If the segments meet at too sharp an angle, a bevel join is used instead.
		/// </summary>
        SetBevelSize,            
        /// <summary>
		///   A circle with a diameter equal to the line width is drawn around the point  where the two segments meet and is filled in, producing a rounded corner. 
		/// </summary>
		Round,            
        /// <summary>
		///   The two segments are finished with butt caps and the resulting notch beyond  the ends of the segments is filled with a triangle 
		/// </summary>
		Bevel             
    } ;



/// <summary>
/// The line cap style specifies the shape to be used at the ends of opened subpaths (and dashes, if any) when they are stroked.
/// </summary>
    public __value enum LineCapType {
        /// <summary>
		///   The stroke is squared off at the endpoint of the path. There is no projection beyond the end of the path. 
		/// </summary>
		ButtEnd,          
        /// <summary>
		///   A semicircular arc with a diameter equal to the line width is drawn around the endpoint and filled in. 
		/// </summary>
		Round,            
        /// <summary>
		///   The stroke continues beyond the endpoint of the path for a distance equal to half the line width and is then squared off. 
		/// </summary>
		ProjectingSquare  
    } ;

/// <summary>
/// Color Space type
/// </summary>
    public __value enum  ColorSpaceType {
        /// <summary>
		///  Gray scale color space type
		/// </summary>
		Gray = 1,  
        /// <summary>
		///  RGB color space type
		/// </summary>
		RGB = 3,  
        /// <summary>
		///  CMYK color space type
		/// </summary>
		CMYK = 4  
    } ; 
	
/// <summary>
/// Color structure to hold color information for use in PDF
/// </summary>
    public __value struct Color {
    public: 
        /// <summary>
		///  Color space type (grayscale, RGB, CMYK)
		/// </summary>
		ColorSpaceType Space; 
		/// <summary>
		///  This variable is used as G for Gray color space type, R for RGB color space type, C for CMYK color space type
		/// </summary>
		ppReal Color1;
		/// <summary>
		///  This variable is used as G for RGB color space type, as M for CMYK color space type		
		/// </summary>
		ppReal Color2; 
        /// <summary>
		///  This variable is used as B component for RGB color space type, as Y for CMYK color space type
		/// </summary>
		ppReal Color3; 
        /// <summary>
		///  This variable is used as K component for CMYK color space type
		/// </summary>
		ppReal Color4; 
    };
	
/// <summary>
/// RGB Color Type
/// </summary>
    public __value struct RGBColor{
        /// <summary>
		///  Red component of Color 
		/// </summary>
		ppReal R; 
        /// <summary>
		///  Green component of Color 
		/// </summary>
		ppReal G; 
        /// <summary>
		///  Blue component of Color 
		/// </summary>
		ppReal B; 
    };
	
/// <summary>
/// Text rendering mode type defining how to fill, stroke glyphs used in the text
/// </summary>
    public __value enum TextRenderingModeType {
        /// <summary>
		///  Fill
		/// </summary>
		Fill    = 0,
		/// <summary>
		///  Stroke
		/// </summary>
		Stroke,			
		/// <summary>
		///  Fill and then stroke
		/// </summary>
		FillAndStroke,			
		/// <summary>
		///  Invisible (neither fill nor stroke text)
		/// </summary>
		Invisible,				
		/// <summary>
		///  Fill text and add to path for the clipping
		/// </summary>
		FillAndAddToPath,		
		/// <summary>
		///  Stroke text and add to path for the clipping
		/// </summary>
		StrokeAndAddToPath,
		/// <summary>
		///  Fill then stroke text and add to path for the clipping
		/// </summary>
		FillAndStrokeAndAddToPath, 
		/// <summary>
		///  Add text to the path for clipping
		/// </summary>
		AddToPath				
    } ;

/// <summary>
/// Specifies file reading/writing mode
/// </summary>
    public __value enum FileOpenMode{
        /// <summary>
		///  Read file
		/// </summary>
		Read = 0, 
		/// <summary>
		///  Write file
		/// </summary>
		Write
    } ;
	
/// <summary>
/// Page rotatione type in degrees
/// </summary>
    public __value enum PageRotationAngleType {
        /// <summary>
		///    No rotation (0 degrees)
		/// </summary>
		_0deg = 0,
		/// <summary>
		///   90 degrees
		/// </summary>
		_90deg,
		/// <summary>
		///  180 degrees
		/// </summary>
		_180deg,
		/// <summary>
		///  270 degrees
		/// </summary>
		_270deg 
    } ;

/// <summary>
/// Standard font types 
/// </summary>
    public __value enum StandardFontType {
        /// <summary>
		///  Helvetica font 
		/// </summary>
		Helvetica,                
        /// <summary>
		///  Helvetica Bold font 
		/// </summary>
		HelveticaBold,            
        /// <summary>
		///  Helvetica Oblique font 
		/// </summary>
		HelveticaOblique,         
        /// <summary>
		///  Helvetica Bold Oblique font 
		/// </summary>
		HelveticaBoldOblique,     
        /// <summary>
		///  Times Roman font 
		/// </summary>
		TimesRoman,               
        /// <summary>
		///  Times Bold font 
		/// </summary>
		TimesBold,                
        /// <summary>
		///  Times Italic font 
		/// </summary>
		TimesItalic,              
        /// <summary>
		///  Times Bold Italicfont 
		/// </summary>
		TimesBoldItalic,          
        /// <summary>
		///  Courier font 
		/// </summary>
		Courier,                  
        /// <summary>
		///  Courier Bold font 
		/// </summary>
		CourierBold,              
        /// <summary>
		///  Courier Oblique font 
		/// </summary>
		CourierOblique,           
        /// <summary>
		///  Courier BoldOblique font 
		/// </summary>
		CourierBoldOblique,       
        /// <summary>
		///  Symbol font 
		/// </summary>
		Symbol,                   
        /// <summary>
		///  Zapf Dingbats font 
		/// </summary>
		ZapfDingbats              
    };

/// <summary>
/// Font encoding type
/// </summary>
    public __value enum FontEncodingType {
        /// <summary>
		///  PDF document encoding
		/// </summary>
		PDFDoc,            
        /// <summary>
		///  Windows ANSI encoding
		/// </summary>
		WinAnsi,           
        /// <summary>
		///  Apple Macintosh encoding
		/// </summary>
		MacRoman,          
        /// <summary>
		///  Standard encoding
		/// </summary>
		Standard
    } ;

/// <summary>
/// Image compression type
/// </summary>
    public __value enum ImageCompressionType {
        /// <summary>
		///  Flate compression 
		/// </summary>
		Flate,            
        /// <summary>
		///  JPEG compression 
		/// </summary>
		JPEG,             
		/// <summary>
		///  JBIG2 compression 
		/// </summary>
		JBIG2,            
        /// <summary>
		///  CCITT 4 compression 
		/// </summary>
		CCITT42D          
    };
/// <summary>
/// Color bit depth 
/// </summary>
    public __value enum ColorBitDepth{
        /// <summary>
		///  1 bit per component  
		/// </summary>
		_1bit  = 1,		
        /// <summary>
		///  2 bit per component  
		/// </summary>
		_2bit  = 2,		
        /// <summary>
		///  4 bit per component  
		/// </summary>
		_4bit  = 4,		
        /// <summary>
		///  8 bit per component
		/// </summary>
		_8bit  = 8,
        /// <summary>
		///  16 bit per component 
		/// </summary>
		_16bit = 16,	
    } ;


/// <summary>
/// Page box type in PDF
/// </summary>
    public __value enum PageBoxType{
        /// <summary>
		///  A rectangle, expressed in default user space units, defining the boundaries of the physical medium on which the page is intended to be displayed or printed
		/// </summary>
		Media, 
        /// <summary>
		///  A rectangle, expressed in default user space units, defining the visible region of default user space. When the page is displayed or printed, its contents are to be clipped (cropped) to this rectangle and then imposed on the output medium in some implementation defined manner
		/// </summary>
		Crop,  
        /// <summary>
		///  A rectangle, expressed in default user space units, defining the region to which the contents of the page should be clipped when output in a production environment
		/// </summary>
		Bleed, 
        /// <summary>
		///  A rectangle, expressed in default user space units, defining the intended dimensions of the finished page after trimming 
		/// </summary>
		Trim,  
        /// <summary>
		///  A rectangle, expressed in default user space units, defining the extent of the pages meaningful content (including potential white space) as intended by the page's creator 
		/// </summary>
		Art    
        };


/// <summary>
/// Rectangle structure to hold information about rectangle
/// </summary>
	public __value struct Rectangle {
     public: 
		 /// <summary>
		 ///  Left 
		 /// </summary>
		 ppReal Left;	
         /// <summary>
		 ///  Top 
		 /// </summary>
		 ppReal Top;		
         /// <summary>
		 ///  Right
		 /// </summary>
		 ppReal Right;	
		 /// <summary>
		 ///  Bottom 
		 /// </summary>
		 ppReal Bottom;	
    };
    
/// <summary>
/// Cos objects type
/// </summary>
 
    public __value enum CosObjectType {
        /// <summary>
		///  Null Cos object type
		/// </summary>
		NullType                                 = 2, 
//        _Integer                              = 4, /* Integer Cos Object      */
		/// <summary>
		///  Integer Cos Object type 
		/// </summary>
		IntegerType                              = 4,
//        _Real                                 = 8, /* Real Cos Object         */
		/// <summary>
		///  Real Cos Object type
		/// </summary>
		RealType                                 = 8,
//        _Boolean                              = 16, /* Boolean Cos Object      */
		/// <summary>
		///  Boolean Cos Object type
		/// </summary>
		BooleanType                              = 16,
//        _Name                                 = 32, /* Name Cos Object         */
		/// <summary>
		///  Name Cos Object type
		/// </summary>
		NameType                                 = 32,
//        _String                               = 64, /* String Cos Object       */
		/// <summary>
		///  String Cos Object type
		/// </summary>
		StringType                               = 64,
//        _Dict                                 = 128, /* Dictionary Cos Object   */
		/// <summary>
		///  Dictionary Cos Object type   
		/// </summary>
		DictionaryType                                 = 128, 
//        _Array                                = 256, /* Array Cos Object        */
		/// <summary>
		///  Array Cos Object  type       
		/// </summary>
		ArrayType                                = 256, 
//        _PDFDataStream                               = 512  /* stream Cos Object       */
		/// <summary>
		///  PDFDataStream Cos Object type       
		/// </summary>
		PDFDataStreamType                               = 512  
    } ;

/// <summary>
/// Destination type used to define how to display page or given rectangle
/// </summary>
    public __value enum DestinationType {
//        XYZ,                                 /*  Display the page, with the coordinates (<i>left</i>, <i>top</i>) positioned*/
        /// <summary>
		///   Display the page, with the coordinates (<i>left</i>, <i>top</i>) positioned  at the top-left corner of the window and the contents of the page magnified by the factor <i>zoom</i>. A -1 value for any of the parameters <i>left</i>, <i>top</i>, or <i>zoom</i> specifies that the current value of that parameter is to be retained unchanged. 
		/// </summary>
		Zoom,                                 
		//        Fit,                                 /*  Display the page, with its contents magnified just enough*/
		/// <summary>
		///   Display the page, with its contents magnified just enough to fit the entire page within the window both horizontally and vertically. If the required horizontal and vertical magnification factors are different, use the smaller of the two, centering the page within the window in the other dimension. */
		/// </summary>
		FitPage,                                 /*  
		//        FitH,                                /*  Display the page, with the vertical coordinate top positioned*/
		/// <summary>
		///   Display the page, with the vertical coordinate top positioned at the <i>top</i> edge of the window and the contents of the page magnified just enough to fit the entire width of the page within the window.                   
		/// </summary>
		FitWidth,                                
		//        FitV,                                /*  Display the page, with the horizontal coordinate left positioned*/
		/// <summary>
		///   Display the page, with the horizontal coordinate left positioned  at the <i>left</i> edge of the window and the contents of the page magnified just enough to fit the entire height of the page within the window.                  
		/// </summary>
		FitHeight,                                
		//        FitR,                                /*  Display the page, with its contents magnified just enough to fit the rectangle */
		/// <summary>
		///   Display the page, with its contents magnified just enough to fit the rectangle specified by the coordinates <i>left</i>, <i>bottom</i>, <i>right</i>, and <i>top</i> entirely within the window both horizontally and vertically. If the required horizontal and vertical magnification factors are different, use the smaller of the two, centering the rectangle within the window in the other dimension.                              
		/// </summary>
		FitRectangle,                                
		//        FitB,                                /*  Display the page, with its contents magnified just enough to fit its bounding box */
		/// <summary>
		///   Display the page, with its contents magnified just enough to fit its bounding box entirely within the window both horizontally and vertically. If the required horizontal the bounding box within the window in the other dimension. 
		/// </summary>
		FitBounds,                                
		//        FitBH,                               /*  Display the page, with the vertical coordinate top positioned at the <i>top</i>*/
		/// <summary>
		///   Display the page, with the vertical coordinate top positioned at the <i>top</i> edge of the window and the contents of the page magnified just enough to fit the entire width of its bounding box within the window. 
		/// </summary>
		FitBoundsByWidth,                               
		//        FitBV                                /*  Display the page, with the horizontal coordinate left positioned at the <i>left</i> edge of */
		/// <summary>
		///   Display the page, with the horizontal coordinate left positioned at the <i>left</i> edge of the window and the contents of the page magnified just enough to fit the entire height of its bounding box within the window.                                               */
		/// </summary>
		FitBoundsByHeight                                
    };

/// <summary>
/// Structure used to hold page destination information
/// </summary>
    public __value struct DestinationInfo {
        DestinationType     Type;           /*  Explicit destination type                           */
		//        ppInt32             page;           /*  Index of the page in PDF document (Begin from 0 )   */
		/// <summary>
		///   Index of the page in PDF document (starts from 0 (zero) )   
		/// </summary>
		ppInt32             PageIndex;
        /// <summary>
		///   This variable is used as Left for Zoom destination type, as Top for FitWidth destination type and FitBoundsByWidth, as Left for FitHeight, FitBoundsByHeight and FitRectangle types
		/// </summary>
		ppReal              a;              
        /// <summary>
		///   This variable is used as Top for Zoom destination type or as Bottom for FitRectangle destination type                  
		/// </summary>
		ppReal              b;              
        /// <summary>
		///   This variable is used as zoom factor for Zoom destination type or as Right for FitRectangle destination type                  
		/// </summary>
		ppReal              c;              
        /// <summary>
		///   This variable is used as Top for FitRectangle                                     
		/// </summary>
		ppReal              d;              
    };

    
/// <summary>
/// Defines how to store thread information in the Thread Action 
/// </summary>
    public __value enum ThreadActionType{
        /// <summary>
		///  Thread information is stored in PDFThreadHandle 
		/// </summary>
		ThreadClass,                 
        /// <summary>
		///  Thread information is stored in the threads index section in PDF document. First thread is stored with 0 (zero) index
		/// </summary>
		ThreadIndex,                  
        /// <summary>
		///  Information is stored in the thread title
		/// </summary>
		ThreadTitle                   
    } ;

/// <summary>
/// Defines how to store bead information in the thread action
/// </summary>
    public __value enum BeadActionType{
        /// <summary>
		///  Information is stored in the class information section
		/// </summary>
		BeadClass,                   
        /// <summary>
		///  Information is stored in the bead index section. First bead is stored with 0 (zero) index
		/// </summary>
		BeadIndex                     
    } ;

/// <summary>
/// Named action type
/// </summary>
    public __value enum NamedActionType{
        /// <summary>
		///  Go to the next page in the document
		/// </summary>
		NextPage,     
		/// <summary>
		///  Go to the previous page in the document
		/// </summary>
		PreviousPage, 
		/// <summary>
		///  Go to the first page in the document
		/// </summary>
		FirstPage,    
		/// <summary>
		///  Go to the last page of the document
		/// </summary>
		LastPage      
    };

/// <summary>
/// Form submission action flags defining how to submit data
/// </summary>
    [System::Serializable, System::Flags]
    public __value enum SubmitFormFlags{
		/// <summary>
		///  Exclude defined fields from submission
		/// </summary>
		Exclude                 = 1, 
        /// <summary>
		///  Do not include fields with empty values to the submission
		/// </summary>
		IncludeNoValueFields    = 2, 

/// <summary>
/// Export in HTML format if this flag is included and export in FDF format if not included
/// </summary>
		ExportFormat            = 4, 


/// <summary>
/// Use HTTP GET method to submit data. POST method is used if this flag is not included
/// </summary>
        GetMethod               = 8, 

/// <summary>
///  Defines if mouse click coordinates will be submitted or not as a part of form data
/// </summary>
SubmitCoordinates       = 16, 

/// <summary>
/// Set this flag to submit data in XML format. Otherwise FDF or HTML format is used (see ExportFormat flag)
/// </summary>
        XML                     = 32, 

/// <summary>
///  Set this flag to include annotations into the submitted data
/// </summary>
IncludeAnnotations      = 128,

/// <summary>
/// Set this flag to submit data in PDF format. If this flag is enabled then all other flags are ignored except GetMethod flag
/// </summary>
       PDF               = 256 

    };
/// <summary>
/// Annotation types
/// </summary>
	public __value enum AnnotationTextType {
		/// <summary>
		///  Note annotation type
		/// </summary>
		Note ,         
		/// <summary>
		///  Comment annotation type 
		/// </summary>
		Comment ,      
		/// <summary>
		///  Help annotation type 
		/// </summary>
		Help ,         
		/// <summary>
		///  Insert annotation type 
		/// </summary>
		Insert ,       
		/// <summary>
		///  Key annotation type 
		/// </summary>
		Key ,          
		/// <summary>
		///  New paragraph annotation type 
		/// </summary>
		NewParagraph , 
		/// <summary>
		///  Paragraph annotation type
		/// </summary>
		Paragraph      
	};

/// <summary>
/// Annotation highlight mode type: the visual effect used when the mouse button is pressed or hold down
/// </summary>
	public __value enum HighlightingModeType{
		/// <summary>
		///  Do not use higlight effect
		/// </summary>
		None,	
		/// <summary>
		///  Invert content of the annotation
		/// </summary>
		Invert,			
		/// <summary>
		///  Invert border of the annotation
		/// </summary>
		Outline,			
		/// <summary>
		///  Display "down" effect
		/// </summary>
		Push				
	};

/// <summary>
/// Stamp icon types
/// </summary>
	public __value enum AnnotationStampIconType{
		/// <summary>
		///  Approved icon type 
		/// </summary>
		Approved,				
		/// <summary>
		///  AsIs icon type 
		/// </summary>
		AsIs,					
		/// <summary>
		///  Confidential icon type 
		/// </summary>
		Confidential,			
		/// <summary>
		///  Departmental icon type 
		/// </summary>
		Departmental,			
		/// <summary>
		///  Draft icon type 
		/// </summary>
		Draft,				    
		/// <summary>
		///  Experimental icon type 
		/// </summary>
		Experimental,			
		/// <summary>
		///  Expired icon type 
		/// </summary>
		Expired,				    
		/// <summary>
		///  Final icon type 
		/// </summary>
		Final,				    
		/// <summary>
		///  For Comment icon type 
		/// </summary>
		ForComment,			    
		/// <summary>
		///  For public Release icon type 
		/// </summary>
		ForPublicRelease,		
		/// <summary>
		///  Not Approved icon type 
		/// </summary>
		NotApproved,			    
		/// <summary>
		///  Not For Public Release icon type 
		/// </summary>
		NotForPublicRelease,	    
		/// <summary>
		///  Sold icon type 
		/// </summary>
		Sold,					
		/// <summary>
		///  Top Secret icon type 
		/// </summary>
		TopSecret			    
	};

	[System::Serializable, System::Flags]
/// <summary>
/// Annotations options flags controlling annotation options
/// </summary>
	public __value enum AnnotationFlag {
	/// <summary>
/// Do not display annotation if no annotation handler available
/// </summary>
		Invisible		= 1,

	/// <summary>
/// Do not display or print annotation (this flag can be used for annotations used for popup hints)
/// </summary>
		Hidden			= 2,

	/// <summary>
/// Set this flag to print annotation when document is printed. Annotation is not printed otherwise
/// </summary>
		Print			= 4,

	/// <summary>
/// Do not zoom annotation when PDF document is zoomed 
/// </summary>
		NoZoom			= 8,

	/// <summary>
/// Do no rotate annotation to match rotated page in PDF document
/// </summary>
		NoRotate		= 16,

	/// <summary>
/// Do not display annotation when user view the document
/// </summary>
		NoView			= 32,

	/// <summary>
/// Use this flag to set annotation to read-only mode
/// </summary>
		ReadOnly		= 64,

	/// <summary>
/// Use this flag to forbid deletion of the annotation and modification of position, size and other annotation properties
/// </summary>
		ListBox			= 128,

	/// <summary>
/// Invert NoView flag for pre-defined events. For example: turn on NoView when mouse is held down on the annotation
/// </summary>
		ToggleNoView	= 256
	};

/// <summary> 
	/// Annotation border style type
	/// </summary>
	public __value enum AnnotationBorderStyleType{
		/// <summary>
		///  A solid rectangle around the annotation
		/// </summary>
		Solid,		
		/// <summary>
		///  A dashed rectangle around the annotation
		/// </summary>
		Dashed,		
		/// <summary>
		///  A simulated embossed rectangle that appears to be raised above the surface of the page
		/// </summary>
		Beveled,	
		/// <summary>
		///  A simulated engraved rectangle that appears to be recessed below the surface of the page
		/// </summary>
		Inset,		
		/// <summary>
		///  A single line along the bottom of the annotation rectangle
		/// </summary>
		Underline	
	};
	
/// <summary>
	/// Flags specifying control (form control) settings
	/// </summary>
	[System::Serializable, System::Flags]
	public __value enum ControlFlags{
		/// <summary>
		///  Default value 
		/// </summary>
		Default			=0, 
		/// <summary>
		///  Set to turn on requirement to have a value in the control before submission to the URL (submit form action)
		/// </summary>
		Required		=1, 
		/// <summary>
		///  Set to set control into read-only mode
		/// </summary>
		ReadOnly		=2, 
		/// <summary>
		///  Set to skip value in this control while submitting data (with form submit action)
		/// </summary>
		NoExport		=4, 
		/// <summary>
		///  Edit controls only: set to turn on multline mode for the control
		/// </summary>
		MultiLine		=8, 
		/// <summary>
		///  Edit controls only: set to hide entered text with a special character mark (password)
		/// </summary>
		Password		=16,
		/// <summary>
		///  Combobox controls only: set to allow edit value of the combobox
		/// </summary>
		Edit			=32, 
		/// <summary>
		///  Set to allow multiple selection of the values in the control
		/// </summary>
		MultiSelect		=64, 
		/// <summary>
		///  Edit controls only: set to skip spell-checking for the control
		/// </summary>
		DoNotSpellCheck	=128, 
		/// <summary>
		///  Edit controls only: set to turn off scrolling (horizontal for single-line fields, vertical for multiple-line fields
		/// </summary>
		DoNotScroll		=256 
	};

/// <summary>
/// Text alignment style for text used in controls (form controls)
/// </summary>
	public __value enum ControlTextAlignmentType{
		/// <summary>
		///  align text to the left top corner
		/// </summary>
		LeftTop = 0, 
		/// <summary>
		///  align text to the top (centered)
		/// </summary>
		Top,         
		/// <summary>
		///  algin text to the right top corner
		/// </summary>
		RightTop,    
		/// <summary>
		///  align text to left (centered)
		/// </summary>
		Left,        
		/// <summary>
		///  align text to the center
		/// </summary>
		Center,      
		/// <summary>
		///  align text to right (centered)
		/// </summary>
		Right,       
		/// <summary>
		///  align text to left bottom corner
		/// </summary>
		LeftBottom,  
		/// <summary>
		///  align text to the bottom (centered)
		/// </summary>
		Bottom,      
		/// <summary>
		///  align text to the right bottom corner
		/// </summary>
		RightBottom  
	};

/// <summary>
/// Control event types
/// </summary>
	public __value enum ControlEventType{
		/// <summary>
		///  control activation event, main action of the control
		/// </summary>
		Activate = 0,   
		/// <summary>
		///  area entering event
		/// </summary>
		Enter,          
		/// <summary>
		///  area exit event
		/// </summary>
		Exit,           
		/// <summary>
		///  mouse button press event
		/// </summary>
		Press,          
		/// <summary>
		///  mouse button release event
		/// </summary>
		Release,        
		/// <summary>
		///  control focus is on event
		/// </summary>
		FocusOn,        
		/// <summary>
		///  control focus lost event
		/// </summary>
		FocusOff,       
		/// <summary>
		///  control text value changed event
		/// </summary>
		Keystroke,      
		/// <summary>
		///  control value formatted event
		/// </summary>
		Format,         
		/// <summary>
		///  control value validation event
		/// </summary>
		Validate,       
		/// <summary>
		///  value recalculation event
		/// </summary>
		Calculate       
	};

/// <summary>
/// Checkbox control check box style
/// </summary>
	public __value enum CheckBoxStyleType{
//		rectangle = 0,           /* rectangle style */
		/// <summary>
		///  rectangle box
		/// </summary>
		RectangleBox = 0,           
//		Circle                   /* Circle style */
		/// <summary>
		///  Circle box 
		/// </summary>
		CircleBox                   
	} ;

/// <summary>
/// Checkbox check mark style type
/// </summary>
	public __value enum CheckBoxMarkStyleType{
		/// <summary>
		///  'Finger' mark 
		/// </summary>
		Finger     = 0053, 
		/// <summary>
		///  'Pen' mark 
		/// </summary>
		Pen        = 0062, 
		/// <summary>
		///  V-style mark 
		/// </summary>
		Vmark      = 0063, 
		/// <summary>
		///  V-style mark 
		/// </summary>
		Nike       = 0064, 
		/// <summary>
		///  Cross-style mark 
		/// </summary>
		Cross      = 0065, 
		/// <summary>
		///  Cross-style mark 
		/// </summary>
		X          = 0066, 
		/// <summary>
		///  Cross-style mark 
		/// </summary>
		Check      = 0070, 
		/// <summary>
		///  Cross-style mark 
		/// </summary>
		Plus       = 0072, 
		/// <summary>
		///  Rhombus-style mark 
		/// </summary>
		Diamond    = 0106, 
		/// <summary>
		///  'Star' mark 
		/// </summary>
		Star       = 0110, 
		/// <summary>
		///  'Flower' mark 
		/// </summary>
		Flower     = 0137, 
		/// <summary>
		///  'Snowflake' mark 
		/// </summary>
		Snow       = 0144, 
		/// <summary>
		///  'Circle' mark 
		/// </summary>
		Circle     = 0154, 
//		rectangle  = 0156, /* 'rectangle' mark */
		/// <summary>
		///  'Rectangle' mark 
		/// </summary>
		RectangleMark  = 0156, 
		/// <summary>
		///  Rhombus-style mark 
		/// </summary>
		Rhombus    = 0165 
	};

/// <summary>
/// Control object type
/// </summary>
	public __value enum ControlType{
		/// <summary>
		///   Unknown control type used to indicate failure and error
		/// </summary>
		Unknown = 0,     
		/// <summary>
		///   Button control
		/// </summary>
		PushButton,      
		/// <summary>
		///   Checkbox control
		/// </summary>
		CheckBox,        
		/// <summary>
		///   Radio button control
		/// </summary>
		RadioButton,     
		/// <summary>
		///   Editable text field control
		/// </summary>
		EditBox,         
		/// <summary>
		///  Combobox control            
		/// </summary>
		ComboBox,        
		/// <summary>
		///   Listbox control
		/// </summary>
		ListBox,         
		/// <summary>
		///   Signature control (can be used as invisible control)
		/// </summary>
		Signature        
	} ;     
}
}
#endif