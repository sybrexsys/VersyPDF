program Outlines;

{$APPTYPE CONSOLE}

uses
  SysUtils, VersyPDF, VersyPDFTypes;
const
    PagesCount = 164;
    PageWidth = 595;
    PageHeight = 842;

    PageContentName : array [0.. PagesCount-1 ] of string = (
                    'Contents',
                    'Preface',
                    '1 Introduction',
                        '1.1 About PDF format',
                        '1.2 PDF 1.6 Features',
                        '1.3 Related Publications',
                        '1.4 About Versy PDF Library',
                    '2 Overview',
                        '2.1 Imaging Model',
                        '2.2 Other General Properties',
                        '2.3 Creating PDF',
                        '2.4 PDF and the PostScript Language',
                    '3 Versy PDF Library',
                        '3.1 Configuration',
                            '3.1.1 Set Platform',
                            '3.1.2 Set Library Properties',
                        '3.2 Structures',
                            '3.2.1 PDF Library Structure',
                            '3.2.2 PDF Document Structure',
                            '3.2.3 Base PDF Data Types',
                            '3.2.4 Base PDF Data Objects',
                            '3.2.5 Error Types',
                            '3.2.6 Exception Types',
                            '3.2.7 Base processing routines',
                            '3.2.8 Versy PDF Name Space',
                        '3.3 Models',
                            '3.3.1 Graphics',
                                '3.3.1.1 Graphics Objects',
                                '3.3.1.2 Coordinate Systems',
                                    '3.3.1.2.1 Coordinate Spaces',
                                        '3.3.1.2.1.1 Device Space',
                                        '3.3.1.2.1.2 User Space',
                                        '3.3.1.2.1.3 Other Coordinate Spaces',
                                        '3.3.1.2.1.4 Relationships among Coordinate Spaces',
                                    '3.3.1.2.2 Common Transformations',
                                    '3.3.1.2.3 Transformation Matrices',
                                '3.3.1.3 Graphics State',
                                    '3.3.1.3.1 Graphics State Stack',
                                    '3.3.1.3.2 Details of Graphics State Parameters',
                                        '3.3.1.3.2.1 Line Width',
                                        '3.3.1.3.2.2 Line Cap Style',
                                        '3.3.1.3.2.3 Line Join Style',
                                        '3.3.1.3.2.4 Miter Limit',
                                        '3.3.1.3.2.5 Line Dash Pattern',
                                    '3.3.1.3.3 Graphics State Operators',
                                    '3.3.1.3.4 Graphics State Parameter Dictionaries',
                                '3.3.1.4 Path Construction and Painting',
                                '3.3.1.5 Color Spaces',
                                '3.3.1.6 Patterns',
                                '3.3.1.7 External Objects',
                                '3.3.1.8 Images',
                                '3.3.1.9 Form XObjects',
                                '3.3.1.10 Optional Content',
                            '3.3.2 Text',
                                '3.3.2.1 Organization and Use of Fonts',
                                    '3.3.2.1.1 Basics of Showing Text',
                                    '3.3.2.1.2 Achieving Special Graphical Effects',
                                    '3.3.2.1.3 Glyph Positioning and Metrics',
                                '3.3.2.2 Text State Parameters and Operators',
                                    '3.3.2.2.1 Character Spacing',
                                    '3.3.2.2.2 Word Spacing',
                                    '3.3.2.2.3 Horizontal Scaling',
                                    '3.3.2.2.4 Leading',
                                    '3.3.2.2.5 Text Rendering Mode',
                                    '3.3.2.2.6 Text Rise',
                                    '3.3.2.2.7 Text Knockout',
                                '3.3.2.3 Text Objects',
                                    '3.3.2.3.1 Text-Positioning Operators',
                                    '3.3.2.3.2 Text-Showing Operators',
                                    '3.3.2.3.3 Text Space Details',
                                '3.3.2.4 Variable text',
                            '3.3.3 Fonts',
                                '3.3.3.1 Introduction to Font Data Structures',
                                '3.3.3.2 Simple Fonts',
                                    '3.3.3.2.1 Type 1 Fonts',
                                    '3.3.3.2.2 TrueType Fonts',
                                    '3.3.3.2.3 Font Subsets',
                                    '3.3.3.2.4 Type 3 Fonts',
                                    '3.3.3.2.5 Character Encoding',
                                '3.3.3.3 Composite Fonts',
                                '3.3.3.4 Font Descriptors',
                            '3.3.4 Drawing',
                                '3.3.4.1 Graphics Operator categories',
                                    '3.3.4.1.1 General graphics state operators',
                                    '3.3.4.1.2 Special graphics state operators',
                                    '3.3.4.1.3 Path construction operators',
                                    '3.3.4.1.4 Path painting operators',
                                    '3.3.4.1.5 Clipping paths operators',
                                    '3.3.4.1.6 Text objects operators',
                                    '3.3.4.1.7 Text state operators',
                                    '3.3.4.1.8 Text positioning operators',
                                    '3.3.4.1.9 Text showing operators',
                                    '3.3.4.1.10 Type 3 fonts operators',
                                    '3.3.4.1.11 Color operators',
                                    '3.3.4.1.12 Shading patterns operators',
                                    '3.3.4.1.13 Inline images operators',
                                    '3.3.4.1.14 XObjects operators',
                                    '3.3.4.1.15 Marked content operators',
                                    '3.3.4.1.16 Compatibility operators',
                                '3.3.4.2 Page Content',
                                '3.3.4.3 Control Content',
                                '3.3.4.4 Appearance Stream',
                                '3.3.4.5 Operator Summary',
                            '3.3.5 Images',
                                '3.3.5.1 Image Parameters',
                                '3.3.5.2 Sample Representation',
                                '3.3.5.3 Image Coordinate System',
                                '3.3.5.4 Image Dictionaries',
                                    '3.3.5.4.1 Decode Arrays',
                                    '3.3.5.4.2 Image Interpolation',
                                    '3.3.5.4.3 Alternate Images',
                                '3.3.5.5 Masked Images',
                                    '3.3.5.5.1 Stencil Masking',
                                    '3.3.5.5.2 Explicit Masking',
                                    '3.3.5.5.3 Color Key Masking',
                                '3.3.5.6 Inline Images',
                            '3.3.6 Composite models',
                        '3.4 Application Program Interface (API)',
                            '3.4.1 PDF Library API',
                            '3.4.2 PDF Document API',
                            '3.4.3 PDF Page API',
                            '3.4.4 PDF Exception API',
                            '3.4.5 PDF Files API',
                            '3.4.6 PDF Data API',
                            '3.4.7 PDF Connection API',
                        '3.5 Features',
                            '3.5.1 Graphic States',
                            '3.5.2 Actions',
                            '3.5.3 Annotations',
                            '3.5.4 AcroForm (Interactive Objects)',
                                '3.5.4.1 Edit Box',
                                '3.5.4.2 Push Button',
                                '3.5.4.3 Check Box',
                                '3.5.4.4 Radio Button',
                                '3.5.4.5 Combo Box',
                                '3.5.4.6 List Box',
                                '3.5.4.7 Signatures',
                                    '3.5.4.7.1 Invisible Document Signature',
                                    '3.5.4.7.2 Signature Stamp',
                                    '3.5.4.7.3 Encode filters',
                                    '3.5.4.7.4 RSA encrypting and rights',
                                '3.5.4.8 Compositing interactive forms',
                            '3.5.5 Outlines',
                            '3.5.6 Threads and Articles',
                            '3.5.8 Security',
                                '3.5.8.1 Permissions',
                                '3.5.8.2 Encrypting key',
                                '3.5.8.3 Passwords',
                                '3.5.8.4 User''s rights',
                                '3.5.8.5 Owner''s rights',
                            '3.5.9 Packing PDF Objects',
                            '3.5.10 Linearizing PDF Document',
                            '3.5.11 Multimedia',
                                '3.5.11.1 Sound',
                                '3.5.11.2 Video',
                            '3.5.12 Other Features',
                        '3.6 Versy PDF Library Conception',
                    '4 Appendix',
                        '4.1 Versy PDF Library Functions List',
                        '4.2 Implementation Limits',
                        '4.3 PDF Application Samples',
                        '4.4 PDF Files Examples',
                    'Bibliography',
                    'Index' );



var
    Doc:TPDFDocument;
    Lib:TPDFLibrary;
    I, j , PageIndex:Integer;
    PB:TPDFPaintBox;
    TextString: string;
    PDFRootOutline, PDFParentOutline, PDFChildOutline: TPDFOutline;
    Dest:TPDFDestination;

function cx( x: Double ):Double;
begin
    Result := PageWidth*x/100;
end;

function cy( y: Double ):Double;
begin
    Result := PageHeight*y/100;
end;


function ParseOutlineStrings ( PrevOutline, CurrOutline:string ):Integer;
var
    Len, i :Integer;
begin
    i:= 1;
    if ( Length ( PrevOutline ) < Length ( CurrOutline ) ) then
        Len := Length ( PrevOutline )
    else
        Len := Length ( CurrOutline );
    while ( ( i <= Len ) and (  PrevOutline[i] = CurrOutline[i]) ) do
        Inc ( i ) ;
    if ( i = 1) then
    begin
        Result := -2;
        Exit;
    end;
    if ( i > Len ) then
    begin
        Result := 0;
        Exit;
    end;
    if ( ( PrevOutline[i] = ' ' ) and ( CurrOutline[i] = '.' ) ) then
    begin
        Result :=1;
        Exit;
    end;
    if ( ( PrevOutline[i] = '.' ) and ( CurrOutline[i] = ' ' ) ) then
    begin
        Result :=-1;
        Exit;
    end;
    if ( ( PrevOutline[i+1] = '.' ) and ( CurrOutline[i+1] = ' ' ) ) then
    begin
        Result :=-1 ;
        Exit;
    end;
    Result := 0;
end;
begin
  try

    Randomize;
    Lib := TPDFLibrary.Create('','');
    try
        { Create PDF document}
        Doc := TPDFDocument.Create( Lib );
        try
   			Doc.AutoLaunch := true;
            PDFRootOutline := Doc.GetOutlineRoot.AddNewChild;
            PDFParentOutline:= PDFRootOutline;
            PDFChildOutline := PDFRootOutline;
            PDFRootOutline.SetTitle('VersyPDF Sample');
            for I:= 0 to PagesCount -1 do
            begin
                PageIndex := Doc.AppendPage ( vpsA4, vpoPortrait );
                PB := Doc.Pages[PageIndex].PaintBox;
                { Save the current graphics state on the graphics state stack. }
                PB.StateStore( );
                { Out text with render mode }
                PB.SetTextRenderingMode( vrmFillAndStrokeText );
                { Set the text font as Helvetica Bold, size is 4% of page height }
                PB.SetActiveFont( Doc.FontAppendStandard( vsfHelveticaBold, vetWinAnsiEncoding ),
                    cy(3), false, false );
                { Set the stroking Color as Dark Green }
                PB.SetStrokeColor( RGBToColor( Random, Random, Random ) );
                { Text Color set a Light Yellow }
                PB.SetFillColor( RGBToColor( Random, Random, Random ) );
                { Output text of title on top-center site of page }
                PB.TextShow( cx(50) - PB.GetTextWidth( PageContentName[ i ] ) / 2, cy(3), 0,
                    PageContentName[ i ] );
                { Set random line width }
                PB.SetLineWidth( cy( 3 * Random ) );
                { Set random line RGB-color }
                PB.SetStrokeColor( RGBToColor( Random, Random, Random ) );
                { Set random dash mode for drawing lines }
                { Generate dash string }
                TextString := '[' + IntToStr ( Random(20) )+ ' '+ IntToStr ( Random(20)+1) +' '+
                   IntToStr ( Random(20) ) + ' '+ IntToStr ( Random(20)+1) +' ] 0';
                { Apply dash style }
                PB.SetDash( TextString );
                PB.NewPath();
                { Draw 40 lines on each page with different style }
                for j := 5 to 44 do
                begin
                    { Set point on page content }
                    PB.MoveTo( cx( 10 ), cy( 2 * j ) );
                    { Draw horizontal line }
                    PB.LineTo( cx( 90 ), cy( 2 * j ) );
                end;
                { Paint lines }
                PB.Stroke( );
                { Restore the graphics state by removing the most recently saved state
                from the stack and making it the current state.}
                PB.StateRestore( );
                pb.Close;
                if I >0 then
                begin
                    j:= ParseOutlineStrings( PageContentName[i-1], PageContentName[i]);
                    case j of
                        -2 :
                            { Current outline link to outline root }
                            PDFParentOutline := PDFRootOutline;
                        -1 :
                            { Current outline link to grand-Parent }
                            PDFParentOutline := PDFParentOutline.Parent;
                        1 :
                            { Current outline link to child }
                            PDFParentOutline := PDFChildOutline;
                        { else link to simple parent }
                    end;
                end;
                PDFChildOutline := PDFParentOutline.AddNewChild;
                PDFChildOutline.SetTitle(PageContentName[i]);
                PDFChildOutline.Color:= ToRGB( Random, Random, Random );
                Dest := Doc.CreateDestination( False, FitHToDest( I, cy ( 100 ) ) , False );
                PDFChildOutline.Destination := Dest;
            end;
            Doc.SaveToFile( '../../../data/out/pdf/outlines.pdf');
        finally
            Doc.Free;
        end;
    finally
        Lib.Free;
    end;
  except
    on e: Exception do
      Writeln('Error:',e.message);
  end;
end.

