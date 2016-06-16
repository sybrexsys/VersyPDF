unit VersyPDFTypes;

{$i vspas.Inc}

interface


type
  ppReal = Double;
  ppInt32 = Integer;
  PppInt32 = ^Integer;
  ppUns32 = Cardinal;
  PppUns32 = ^Cardinal;
  ppAtom = ppUns32;

   PDFCosHandle = Pointer;
   PDFOutlineHandle = Pointer;
   PDFStreamHandle = Pointer;
   PDFDocHandle = Pointer;
   PDFLibHandle = Pointer;
   PDFImageHandle = Pointer;
   PDFActionHandle = Pointer;
   PDFDestinationHandle = Pointer;
   PDFThreadHandle = Pointer;
   PDFBeadHandle = Pointer;
   PBXHandle = Pointer;
   PDFAnnotationHandle = Pointer;

    { A flags specifying various characteristics of the submit form action }
    TPDFSubmitFormFlag = (
		vsffExclude                 , { If clear, the specifies which fields to	include in the submission. }
        vsffIncludeNoValueFields    , { If clear, fields without a value are not submitted.}
		vsffExportFormat            , { If set, field names and values are submitted in HTML Form format.
									 If clear, they are submitted in Forms Data Format }
        vsffGetMethod               , { If set, field names and values are submitted using an HTTP GET
										request; if clear, they are submitted using a POST request.}
        vsffSubmitCoordinates       , { If set, the coordinates of the mouse click that caused the submitform
										action are transmitted as part of the form data.}
        vsffXML                     , { If set, field names and values are submitted in XML format;
										if clear, they are submitted in HTML Form format or Forms Data Format }
        vsffIncludeAnnotations      , { If set, the submitted FDF file includes all
										annotations in the underlying PDF document; if clear, the annotations
										are not included.}
        vsffSubmitPDF                 { If set, the document is submitted in PDF format, using
									  the MIME content type application/pdf. Bibliography). If this flag
									  is set, all other flags are ignored except GetMethod. }
    );
    { A set of flags specifying various characteristics of the submit form action }
    TPDFSubmitFormFlags = set of TPDFSubmitFormFlag; 
    { Flags specifying various characteristics of the annotations. }
    TPDFAnnotationFlag  = (
		{ If set, do not display the annotation if it does not belong
		to one of the standard annotation types and no annotation
		handler is available. If clear, display such an unknown
		annotation using an appearance stream specified by its
		appearance dictionary, if any }
		vafInvisible,

		{ If set, do not display or print the annotation or allow it
		to interact with the user, regardless of its annotation type or
		whether an annotation handler is available. In cases where
		screen space is limited, the ability to hide and show annotations
		selectively can be used in combination with appearance streams
		to display auxiliary pop-up information similar in function to
		online help systems. }
		vafHidden,

		{ If set, print the annotation when the page is printed. If clear,
		never print the annotation, regardless of whether it is displayed
		on the screen. This can be useful, for example, for annotations
		representing interactive pushbuttons, which would serve no
		meaningful purpose on the printed page. }
		vafPrint,

		{ If set, do not scale the annotationís appearance to match the
		magnification of the page. The location of the annotation on the
		page (defined by the upper-left corner of its annotation rectangle)
		remains fixed, regardless of the page magnification. See below for
		further discussion. }
		vafNoZoom,

		{ If set, do not rotate the annotationís appearance to match the
		rotation of the page. The upper-left corner of the annotation
		rectangle remains in a fixed location on the page, regardless of
		the page rotation. See below for further discussion. }
		vafNoRotate,

		{ If set, do not display the annotation on the screen or allow it
		to interact with the user. The annotation may be printed (depending
		on the setting of the Print flag), but should be considered hidden
		for purposes of on-screen display and user interaction. }
		vafNoView,

		{ If set, do not allow the annotation to interact with the user.
		The annotation may be displayed or printed (depending on the
		settings of the NoView and Print flags), but should not respond to
		mouse clicks or change its appearance in response to mouse motions.}
		vafReadOnly	,

		{ If set, do not allow the annotation to be deleted or its
		properties (including position and size) to be modified by the user.
		However, this does not restrict changes to the annotationís contents,
		such as a form field value. }
		vafListBox	,

		{ If set, invert the interpretation of the NoView flag for certain
		events. A typical use is to have an annotation that appears only
		when a mouse cursor is held over it. }
		vafToggleNoView
	);
    { A set of the flags specifying various characteristics of the annotations. }
    TPDFAnnotationFlags = set of TPDFAnnotationFlag;

    { Flags specifying various characteristics of the acroform object.}
  	TPDFAcroObjectFlag = (
		vaofRequired		, { If set, the field must have a value at the time it is exported by a submit-form action }
		vaofReadOnly		, { If set, the user may not change the value of the field. }
		vaofNoExport		, { If set, the field must not be exported by a submit-form action }
		vaofMultiLine		, { If set, the field may contain multiple lines of text; if clear, the fieldís text
							is restricted to a single line. Valid for PDFAcroEditBox only.}
		vaofPassword		, { If set, the field is intended for entering a secure password that should
							not be echoed visibly to the screen. Characters typed from the keyboard
							should instead be echoed in some unreadable form, such as
							asterisks or bullet characters. Valid for PDFAcroEditBox only.}
		vaofEdit			,  { If set, the combo box includes an editable text box as well as a drop list; if
							clear, it includes only a drop list. Valid for PDFAcroComboBox only. }
		vaofMultiSelect		, { If set, more than one of the fieldís option items may be selected
							simultaneously; if clear, no more than one item at a time may be selected.}
		vaofDoNotSpellCheck	, { If set, the text entered in the field will not be spell-checked. Valid for PDFAcroEditBox only.}
		vaofDoNotScroll		  { If set, the field will not scroll (horizontally for single-line
							 fields, vertically for multiple-line fields) to accommodate more text
							 than will fit within its annotation rectangle. Once the field is full, no
							 further text will be accepted.Valid for PDFAcroEditBox only.}
	);
    { A set od the flags specifying various characteristics of the acroform object.}
    TPDFAcroObjectFlags = set of TPDFAcroObjectFlag;



	{ Converting CIE-based colors to device colors  }
  TPDFRenderingIntents = (
		vriAbsoluteColormetric,    { Colors are represented solely with respect to the light source; no
			                      correction is made for the output mediumís white point = (such as
              					  the color of unprinted paper). }
		vriRelativeColorMetrics,   { Colors are represented with respect to the combination of the
								  light source and the output mediumís white point = (such as the
								  color of unprinted paper).    }
        vriSaturation,             { Colors are represented in a manner that preserves or
								  emphasizes saturation. }
        vriPerceptual              { Colors are represented in a manner that provides a pleasing
								  perceptual appearance. }
    ) ;

	{ Blending mode }
    TPDFBlendMode= (
        vbmNormal,                  { Selects the source color, ignoring the backdrop }
        vbmMultiply,                { Multiplies the backdrop and source color values }
		vbmScreen,                  { Multiplies the complements of the backdrop and source color
									 values, then complements the result}
        vbmOverlay,                 { Multiplies or screens the colors, depending on the backdrop color. }
        vbmDarken,                  { Selects the darker of the backdrop and source colors. }
        vbmLighten,                 { Selects the lighter of the backdrop and source colors. }
		vbmColorDodge,              { Brightens the backdrop color to reflect the source color. Painting
									 with black produces no change. }
        vbmColorBurn,               { Darkens the backdrop color to reflect the source color.
									 Painting with white produces no change. }
        vbmHardLight,               { Multiplies or screens the colors, depending on the source
									 color value. }
        vbmSoftLight,               { Darkens or lightens the colors, depending on the source color value. }
        vbmDifference,              { Subtracts the darker of the two constituent colors from the lighter. }
		vbmExclusion                { Produces an effect similar to that of the Difference mode, but lower
									 in contrast. }
    ) ;

    {   Document Restriction's Flags   }
    TPDFDocumentRestriction = (
        vdrPrint,                           {Allowed print the document }
        vdrModifyContent,                   {Allowed modify the content of the document }
        vdrCopyInformation,                 {Allowed copy text and graphics from the document }
        vdrModifyAnnotation,                {Allowed add or modify annotations }
        vdrFillAcroForm,                    {Allowed fill in interactive form fields }
        vdrExtractTextAndImage,             {Allowed extract text and graphics from the document  }
        vdrAssembleDocument,                {Allowed assemble the document }
        vdrPrintHiResolution                {Allowed print the document with high resolution }
    );
    {  A set of the  Document Restriction's Flags   }
    TPDFDocumentRestrictions = set of TPDFDocumentRestriction;


	{  Password Type of Crypted PDF Document. Password Validity. }
     TKeyValidType  = (
        vkvInvalidPassword                      , { Invalid Password }
        vkvUserPassword                         , { User Password }
        vkvOwnerPassword                          { Owner Password }
    );

	{  Protection Key-Length Type of Crypted PDF Document }
    TPDFProtectionType = (
        vptIsNone = 0,
        vptIs40BitProtection                           , {  40 Bit protection key length }
        vptIs128BitProtection                          , { 128 Bit protection key length }
        vptIs128BitAESProtection                       ,  { 128 Bit AES protection key length }
        vptIs256BitAESProtection                         { 256 Bit AES protection key length }
    ) ;

	{   Type of supported PDF Document Versions }
    TPDFVersion = (
        pdfver10,        { PDF Document Version is 1.0 }
        pdfver11,        { PDF Document Version is 1.1 }
        pdfver12,        { PDF Document Version is 1.2 }
        pdfver13,        { PDF Document Version is 1.3 }
        pdfver14,        { PDF Document Version is 1.4 }
        pdfver15,		{ PDF Document Version is 1.5 }
        pdfver16 		{ PDF Document Version is 1.6 }
    ) ;

	{  Type of usual PDF Document's Page Sizes }
    TPDFPageSize = (
        vpsLetter ,      { Document's Page Size is 792 x 612 }
        vpsA4,           { Document's Page Size is 842 x 595 }
        vpsA3,           { Document's Page Size is 1190 x 842 }
        vpsLegal,        { Document's Page Size is 1008 x 612 }
        vpsB5,           { Document's Page Size is 728 x 516 }
        vpsC5,           { Document's Page Size is 649 x 459 }
        vps8x11,         { Document's Page Size is 792 x 595 }
        vpsB4,           { Document's Page Size is 1031 x 728 }
        vpsA5,           { Document's Page Size is 595 x 419 }
        vpsFolio,        { Document's Page Size is 936 x 612 }
        vpsExecutive,    { Document's Page Size is 756 x 522 }
        vpsEnvB4,        { Document's Page Size is 1031 x 728 }
        vpsEnvB5,        { Document's Page Size is 708 x 499 }
        vpsEnvC6,        { Document's Page Size is 459 x 323 }
        vpsEnvDL,        { Document's Page Size is 623 x 312 }
        vpsEnvMonarch,   { Document's Page Size is 540 x 279 }
        vpsEnv9,         { Document's Page Size is 639 x 279 }
        vpsEnv10,        { Document's Page Size is 684 x 297 }
        vpsEnv11         { Document's Page Size is 747 x 324 }
    );

	{  Page Orientation Type  }
    TPDFPageOrientation = (
        vpoPortrait ,      { Orientation of Page is Portrait }
        vpoLandScape       { Orientation of Page is Landscape }
    ) ;
	{  Document Description's Item Type  }
    TPDFInformation = (
        piCreator ,      { Information about creator }
        piAuthor,           { Information about author }
        piDate,             { Information about date }
        piProducer,         { Information about producer }
        piTitle,            { Information about title }
        piSubject,          { Information about subject }
        piKeyWords,         { Information about keywords }
        piModificationData  { Information about modification data }
    );

	{   Document Restriction's Flags   }

	{ }
    TPDFHorJust = (
        vhjLeft,             { }
        vhjCenter,           { }
        vhjRight             { }
    ) ;

	{ }
    TPDFVertJust = (
        vvjTop,              { }
        vvjCenter,           { }
        vvjBottom            { }
    );

    {  The line join style specifies the shape to be used at the corners
    of paths that are stroked. }
    TPDFLineJoin = (
        vljMiter,            {  The outer edges of the strokes for the two segments are extended until
                              they meet at an angle, as in a picture frame. If the segments meet at too
                              sharp an angle, a bevel join is used instead. }
        vljRound,            {  A circle with a diameter equal to the line width is drawn around the point
                                where the two segments meet and is filled in, producing a rounded corner. }
        vljBevel             {  The two segments are finished with butt caps and the resulting notch beyond
                                the ends of the segments is filled with a triangle }
    ) ;



    { The line cap style specifies the shape to be used at the ends
    \of opened subpaths = (and dashes, if any) when they are
    stroked.                                                      }
    TPDFLineCap = (
        vlcButtEnd,          {  The stroke is squared off at the endpoint of the path. There is no projection
                                beyond the end of the path. }
        vlcRound,            {  A semicircular arc with a diameter equal to the line width is drawn around
                                the endpoint and filled in. }
        vlcProjectingSquare  {  The stroke continues beyond the endpoint of the path for a distance equal to
                                half the line width and is then squared off. }
    ) ;

    { Color Device Type }
    TPDFColorSpace = (
        vcsGray ,  { Gray scale Color Device }
        vcsRGB ,  { RGB Color Device }
        vcsCMYK   { CMYK Color Device }
    ) ;

	{ PDF Color Type }
    TPDFColor =  record
        Space: TPDFColorSpace ; { Color Device }
        CompOne: ppReal;	{ G for Gray, R for RGB, C for CMYK }
        CompTwo: ppReal ; { G for RGB, M for CMYK }
        CompThree:ppReal ; { B for RGB, Y for CMYK }
        CompFour:ppReal; { K for CMYK }
    end;

	{ RGB Color Type }
    TPDFRGB =  record
        R:ppReal; { Red component of Color }
        G:ppReal; { Green component of Color }
        B:ppReal; { Blue component of Color }
    end;

	{ Determines whether showing text causes glyph outlines to be stroked, filled	}
    TPDFRenderingMode = (
        vrmFillText    ,		   	    { Fill text.}
        vrmStrokeText,					{ Stroke text. }
        vrmFillAndStrokeText,			{ Fill, then stroke text. }
        vrmInvisibleText,				{ Neither fill nor stroke text = (invisible). }
        vrmFillAndAddToPathText,		{ Fill text and add to path for clipping. }
        vrmStrokeAndAddToPathText,		{ Stroke text and add to path for clipping. }
        vrmFillAndStrokeAndAddToPathText, { Fill, then stroke text and add to path for clipping. }
        vrmAddToPathText				{ Add text to path for clipping. }
    ) ;

	{ Specifies file open mode. }
    TFileOpenMode= (
        vomReadMode , { Read File Mode }
        vomWriteMode     { Write File Mode }
    ) ;

	{ Specifies page rotation, in degrees. }
    TPDFPageRotateAngle = (
        vpra0 ,   {   0 deg. - rotation angle }
        vpra90,      {  90 deg. - rotation angle }
        vpra180,     { 180 deg. - rotation angle }
        vpra270         { 270 deg. - rotation angle }
    ) ;

	{ Standard 14 fonts enum}
    TPDFStdandardFont = (
        vsfHelvetica,                { Helvetica font }
        vsfHelveticaBold,            { Helvetica Bold font }
        vsfHelveticaOblique,         { Helvetica Oblique font }
        vsfHelveticaBoldOblique,     { Helvetica Bold Obliquefont }
        vsfTimesRoman,               { Times Roman font }
        vsfTimesBold,                { Times Bold font }
        vsfTimesItalic,              { Times Italic font }
        vsfTimesBoldItalic,          { Times Bold Italicfont }
        vsfCourier,                  { Courier font }
        vsfCourierBold,              { Courier Bold font }
        vsfCourierOblique,           { Courier Oblique font }
        vsfCourierBoldOblique,       { Courier BoldOblique font }
        vsfSymbol,                   { Symbol font }
        vsfZapfDingbats              { Zapf Dingbats font }
    );

	{ Font encoding}
    TPDFEncodingType = (
        vetPDFDocEncoding,            { PDF Document encoding }
        vetWinAnsiEncoding,           { ANSI windows encoding }
        vetMacRomanEncoding,          { Apple encoding }
        vetStandardEncoding           { Standard encoding }
    ) ;

	{ Available image compression types }
    TPDFImageCompressionType = (
        victFlate,            { FLATE compression }
        victJPEG,             { JPEG compression }
		    victJBIG2,            { JBIG2 compression }
        victCCITT42D          { CCITT 4 compression }
    );
	{ Depth of the each component in color }
    TPDFComponentDepth= (
        vcd1bit  ,		{ 1 bit per component  }
        vcd2bit  ,		{ 2 bit per component  }
        vcd4bit  ,		{ 4 bit per component  }
        vcd8bit  ,		{ 8 bit per component  }
        vcd16bit 	    { 16 bit per component }
    ) ;


    { Page Box Type }
    TPDFPageBoxType= (
            vpbtMediaBox, { A rectangle, expressed in default user space units, defining the boundaries
                         of the physical medium on which the page is intended to be displayed or printed }
            vpbtCropBox,  { A rectangle, expressed in default user space units, defining the visible region
                         of default user space. When the page is displayed or printed, its contents are to
                         be clipped = (cropped) to this rectangle and then imposed on the output medium in
                         some implementation defined manner. }
            vpbtBleedBox, { A rectangle, expressed in default user space units, defining the region to which
                         the contents of the page should be clipped when output in a production environment }
            vpbtTrimBox,  { A rectangle, expressed in default user space units, defining the intended
                         dimensions of the finished page after trimming }
            vpbtArtBox    { A rectangle, expressed in default user space units, defining the extent of the
                         page“s meaningful content = (including potential white space) as intended by the
                         page“s creator }
        );


    { Rectangle Type }
    TPDFRect =  record
        Left:ppReal;	{ Left border coordinate }
        Bottom:ppReal;	{ Bottom border coordinate }
        Right:ppReal;	{ Right border coordinate }
        Top:ppReal;		{ Top border coordinate }
    end;
    PPDFRect = ^ TPDFRect;

	{ Available Cos objects }
    TPDFCosType = (
        vctNull                                 , { NULL Cos object         }
        vctInteger                              , { Integer Cos Object      }
        vctReal                                 , { Real Cos Object         }
        vctBoolean                              , { Boolean Cos Object      }
        vctName                                 , { Name Cos Object         }
        vctString                               , { String Cos Object       }
        vctDict                                 , { Dictionary Cos Object   }
        vctArray                                , { Array Cos Object        }
        vctStream                                 { Stream Cos Object       }
    ) ;

	{ PDF explicit destination types }
    TPDFExplicitType = (
        vetXYZ,                                 {  Display the page, with the coordinates = (<i>left</i>, <i>top</i>) positioned
                                                    at the top-left corner of the window and the contents of the page magnified
                                                    by the factor <i>zoom</i>. A null value for any of the parameters <i>left</i>, <i>top</i>, or
                                                    <i>zoom</i> specifies that the current value of that parameter is to be retained unchanged. }
        vetFit,                                 {  Display the page, with its contents magnified just enough
                                                    to fit the entire page within the window both horizontally and vertically. If
                                                    the required horizontal and vertical magnification factors are different, use
                                                    the smaller of the two, centering the page within the window in the other dimension. }
        vetFitH,                                {  Display the page, with the vertical coordinate top positioned
                                                    at the <i>top</i> edge of the window and the contents of the page magnified
                                                    just enough to fit the entire width of the page within the window.                   }
        vetFitV,                                {  Display the page, with the horizontal coordinate left positioned
                                                    at the <i>left</i> edge of the window and the contents of the page magnified
                                                    just enough to fit the entire height of the page within the window.                  }
        vetFitR,                                {  Display the page, with its contents magnified just enough to fit the rectangle
                                                    specified by the coordinates <i>left</i>, <i>bottom</i>, <i>right</i>, and <i>top</i> entirely
                                                    within the window both horizontally and vertically. If the required horizontal and
                                                    vertical magnification factors are different, use the smaller of the two, centering
                                                    the rectangle within the window in the other dimension.                              }
        vetFitB,                                {  Display the page, with its contents magnified just enough to fit its bounding box
                                                    entirely within the window both horizontally and vertically. If the required horizontal
                                                    and vertical magnification factors are different, use the smaller of the two, centering
                                                    the bounding box within the window in the other dimension. }
        vetFitBH,                               {  Display the page, with the vertical coordinate top positioned at the <i>top</i>
                                                    edge of the window and the contents of the page magnified just enough to fit the entire
                                                    width of its bounding box within the window.                                         }
        vetFitBV                                {  Display the page, with the horizontal coordinate left positioned at the <i>left</i> edge of
                                                    the window and the contents of the page magnified just enough to fit the entire height
                                                    of its bounding box within the window.                                               }
    );

	{ This struct for specifying a destination explicitly in a PDF file }
    TPDFExplicitDestination = record
        ExType              :TPDFExplicitType;           {  Explicit destination type                           }
        Page                :ppInt32;          {  Index of the page in PDF document = (Begin from 0 )   }
        a                   :ppReal;              {  Value depended from type of the destination
                                            left for edtXYZ, top for edtFitH and edtFitBH, left for edtFitV,edtFitBV, and edtFitR }
        b                   :ppReal;              {  Value depended from type of the destination
                                            top for edtXYZ, bottom for edtFitR                  }
        c                   :ppReal;              {  Value depended from type of the destination
                                            zoom for edtXYZ, right for edtFitR                  }
        d                   :ppReal;              {  Value depended from type of the destination
                                            top for edtFitR                                     }
    end;


	{ This enumeration defines type of the store thread info in
	thread action                                             }
    TPDFThreadActionTypes = (
        vtatThreadClass,                 { Destination is stored in PDFThreadHandle }
        vtatThreadIndex,                  { Destination is stored in index of the threads in PDF
                                           document. The first thread in the document has index 0. }
        vtatThreadTitle                   { Destination is stored in the title of the thread. }
    ) ;

	{ This enumeration defines type of the store bead info in
	thread action                                             }
    TPDFBeadActionTypes= (
        vbatBeadClass,                   { Destination stored in PDFBeadHandle }
        vbatBeadIndex                     { Destination stored in index of the bead within its thread.
                                            The first bead in a thread has index 0. }
    ) ;

	{ This enumeration defines type of the operation for named
	action                                                   }
    TPDFNamedActionType= (
        vnatNextPage,     { Go to the next page of the document.     }
        vnatPrevPage,     { Go to the previous page of the document. }
        vnatFirstPage,    { Go to the first page of the document.    }
        vnatLastPage      { Go to the last page of the document.     }
    );

	{ Available annotations names }
	TPDFAnnotationTextName = (
		vatnNote ,         { Note annotation }
		vatnComment ,      { Comment annotation }
		vatnHelp ,         { Help annotation }
		vatnInsert ,       { Insert annotation }
		vatnKey ,          { Key annotation }
		vatnNewParagraph , { New paragraph annotation }
		vatnParagraph      { Paragraph annotation }
	);

	{ The annotationís highlighting mode, the visual effect to be
	used when the mouse button is pressed or held down inside its active area. }
	TPDFHighLightMode= (
		vhlmNoHightLight,	{ No highlighting. }
		vhlmInvert,			{ Invert the contents of the annotation rectangle. }
		vhlmOutline,			{ Invert the annotation's border. }
		vhlmPush				{ Display the annotation's down appearance. }
	);

	{ Available stamp icon types }
	TPDFAnnotationStampName= (
		vasnApproved,				{ Approved icon type }
		vasnAsIs,					{ AsIs icon type }
		vasnConfidential,			{ Confidential icon type }
		vasnDepartmental,			{ Departmental icon type }
		vasnDraft,				    { Draft icon type }
		vasnExperimental,			{ Experimental icon type }
		vasnExpired,				    { Expired icon type }
		vasnFinal,				    { Final icon type }
		vasnForComment,			    { For comment icon type }
		vasnForPublicRelease,		{ For public release icon type }
		vasnNotApproved,			    { Not approved icon type }
		vasnNotForPublicRelease,	    { Not for public release icon type }
		vasnSold,					{ Sold icon type }
		vasnTopSecret			    { Top secret icon type }
	);


	{ The annotation border style }
	TPDFBorderStyle= (
		vbsSolid,		{ A solid rectangle surrounding the annotation. }
		vbsDashed,		{ A dashed rectangle surrounding the annotation. }
		vbsBeveled,	{ A simulated embossed rectangle that appears to be raised above the surface of the page. }
		vbsInset,		{ A simulated engraved rectangle that appears to be recessed below the	surface of the page. }
		vbsUnderline	{ A single line along the bottom of the annotation rectangle. }
	);
	

	{ Acro Form Object Quadding Type. Text justification style. }
	TPDFAcroQuadding= (
		vaqLeftTop , { attach text to left top corner of field }
		vaqTop,         { attach text to top central site of field }
		vaqRightTop,    { attach text to right top corner of field }
		vaqLeft,        { attach text to left central site of field }
		vaqCenter,      { attach text to center of field }
		vaqRight,       { attach text to right central site of field }
		vaqLeftBottom,  { attach text to left bottom corner of field }
		vaqBottom,      { attach text to bottom central site of field }
		vaqRightBottom  { attach text to right bottom corner of field }
	);

	{ Action Event Type for Acro Form Objects. Set of events. }

	TPDFAcroEventType= (
		vaetActivate ,   { event on activate, primary action      }
		vaetEnter,          { event on enter in the active area      }
		vaetExit,           { event on exit from the active area     }
		vaetPress,          { event on press mouse button inside it  }
		vaetRelease,        { event on release mouse button inside   }
		vaetFocusOn,        { event on receive the input focus       }
		vaetFocusOff,       { event on lose the input focus          }
		vaetKeystroke,      { event on change text value in field    }
		vaetFormat,         { event on format value in the field     }
		vaetValidate,       { event on change fieldís value in field }
		vaetCalculate       { event on recalculate value             }
	);

	{ Type of CheckBox Style. }
	TPDFCheckBoxStyle= (
		vcbstRectangle,           { Rectangle style }
		vcbstCircle                   { Circle style }
	) ;
	{ Type of CheckBox Mark - code of Zapf Dingbats sign = ( 32 - 255 ).}
	TPDFCheckBoxSign= (
		vcbsFinger     , { 'Finger' mark }
		vcbsPen        , { 'Pen' mark }
		vcbsVmark      , { V-style mark }
		vcbsNike       , { V-style mark }
		vcbsCross      , { Cross-style mark }
		vcbsX          , { Cross-style mark }
		vcbsCheck      , { Cross-style mark }
		vcbsPlus       , { Cross-style mark }
		vcbsDiamond    , { Rhombus-style mark }
		vcbsStar       , { 'Star' mark }
		vcbsFlower     , { 'Flower' mark }
		vcbsSnow       , { 'Snowflake' mark }
		vcbsCircle     , { 'Circle' mark }
		vcbsRectangle  , { 'Rectangle' mark }
		vcbsRhombus      { Rhombus-style mark }
	);

	{ Acro Form Object Type. Interactive Control Type. }
	TPDFAcroType= (
		vatUnknown ,     {  Unknown Type, in failure case                           }
		vatPushButton,      {  Button for select single action                         }
		vatCheckBox,        {  Button for check single item                            }
		vatRadioButton,     {  Button from group for select only one item from ensemble}
		vatEditBox,         {  Variable text edit field for change text item           }
		vatComboBox,        {  Field for select one text item from list                }
		vatListBox,         {  Box for select item= (s) from list                        }
		vatSignature        {  Field for sign in document, maybe invisible             }
	) ;


implementation

end.
