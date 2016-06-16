program FontsAndText;

{$APPTYPE CONSOLE}

uses
  SysUtils, VersyPDF, VersyPDFTypes;

const
    PageWidth = 595;
    PageHeight = 842;



var
    Lib: TPDFLibrary ;
    Doc: TPDFDocument ;
    PB:	 TPDFPaintBox ;
    Page:	 TPDFPage;
    FontIndex, PageIndex, i: Cardinal;
    Radius: Double;

function cx( x: Double ):Double;
begin
    Result := PageWidth*x/100;
end;

function cy( y: Double ):Double;
begin
    Result := PageHeight*y/100;
end;

begin
  try
    Lib := TPDFLibrary.Create('','');
    try
        Doc :=TPDFDocument.Create( Lib );
        try
   			Doc.AutoLaunch := true;
			Doc.AppendPage ( vpsA4, vpoPortrait );
			Page := Doc.Pages[0];
            PB := Page.PaintBox;
            PB.SetActiveFont( Doc.FontAppendStandard( vsfHelvetica, vetWinAnsiEncoding ), cy(4), false, false );
            PB.SetFillColor( GrayToColor( 0.5 ) );
            PB.TextShow ( cx(50) - PB.GetTextWidth( 'Fonts' ) / 2, cy(4), 0, 'Fonts');

            { Set active font as Helvetica, size is 1% of page height }
            FontIndex := Doc.FontAppendStandard( vsfHelvetica, vetWinAnsiEncoding );
            PB.SetActiveFont(FontIndex , cy(1), false, false );
            { Draw text label 'Standard fonts' and underline it }
            PB.TextShow( cx(10), cy(8), 0, 'Standard fonts' );
            { Set active font as Helvetica, size is 3% of page height }
            PB.SetActiveFont( FontIndex, cy(3), false, false );
            { Draw text label 'Helvetica' }
            PB.TextShow(  cx(10), cy(10), 0 , 'Helvetica' );

            { Set active font as Helvetica Bold, size is 3% of page height }
            FontIndex := Doc.FontAppendStandard ( vsfHelveticaBold, vetWinAnsiEncoding  );
            PB.SetActiveFont( FontIndex, cy(3), false, false );
            { Draw text label 'Helvetica Bold' }
            PB.TextShow( cx(10), cy(15), 0 , 'Helvetica Bold' );

            { Set active font as Helvetica Oblique, size is 3% of page height }
            FontIndex := Doc.FontAppendStandard ( vsfHelveticaOblique, vetWinAnsiEncoding  );
            PB.SetActiveFont( FontIndex, cy(3), false, false );
            { Draw text label 'Helvetica Oblique' }
            PB.TextShow( cx(10), cy(20), 0 , 'Helvetica Oblique' );

            { Set active font as Helvetica Bold Oblique, size is 3% of page height }
            FontIndex := Doc.FontAppendStandard ( vsfHelveticaBoldOblique, vetWinAnsiEncoding  );
            PB.SetActiveFont( FontIndex, cy(3), false, false );
            { Draw text label 'Helvetica Bold Oblique' }
            PB.TextShow( cx(10), cy(25), 0 , 'Helvetica Bold Oblique' );
            { Set active font as Times New Roman, size is 3% of page height }
            FontIndex := Doc.FontAppendStandard ( vsfTimesRoman, vetWinAnsiEncoding  );
            PB.SetActiveFont( FontIndex, cy(3), false, false );
            { Draw text label 'Times New Roman' }
            PB.TextShow(  cx(10), cy(30), 0 , 'Times New Roman' );

            { Set active font as Times New Roman Bold, size is 3% of page height }
            FontIndex := Doc.FontAppendStandard ( vsfTimesBold, vetWinAnsiEncoding  );
            PB.SetActiveFont(  FontIndex, cy(3), false, false );
            { Draw text label 'Times New Roman Bold' }
            PB.TextShow(  cx(10), cy(35), 0 , 'Times New Roman Bold' );

            { Set active font as Times New Roman Italic, size is 3% of page height }
            FontIndex := Doc.FontAppendStandard ( vsfTimesItalic, vetWinAnsiEncoding  );
            PB.SetActiveFont(  FontIndex, cy(3), false, false );
            { Draw text label 'Times New Roman Italic' }
            PB.TextShow(  cx(10), cy(40), 0 , 'Times New Roman Italic' );

            { Set active font as Times New Roman Bold Italic, size is 3% of page height }
            FontIndex := Doc.FontAppendStandard ( vsfTimesBoldItalic, vetWinAnsiEncoding  );
            PB.SetActiveFont(  FontIndex, cy(3), false, false );
            { Draw text label 'Times New Roman Bold Italic' }
            PB.TextShow(  cx(10), cy(45), 0 , 'Times New Roman Bold Italic' );

            { Set active font as Courier, size is 3% of page height }
            FontIndex := Doc.FontAppendStandard ( vsfCourier, vetWinAnsiEncoding  );
            PB.SetActiveFont(  FontIndex, cy(3), false, false );
            { Draw text label 'Courier' }
            PB.TextShow(  cx(10), cy(50), 0 , 'Courier' );

            { Set active font as Courier Bold, size is 3% of page height }
            FontIndex := Doc.FontAppendStandard ( vsfCourierBold, vetWinAnsiEncoding  );
            PB.SetActiveFont(  FontIndex, cy(3), false, false );
            { Draw text label 'Courier Bold' }
            PB.TextShow(  cx(10), cy(55), 0 , 'Courier Bold' );

            { Set active font as Courier Oblique, size is 3% of page height }
            FontIndex := Doc.FontAppendStandard ( vsfCourierOblique, vetWinAnsiEncoding  );
            PB.SetActiveFont(  FontIndex, cy(3), false, false );
            { Draw text label 'Courier Oblique' }
            PB.TextShow(  cx(10), cy(60), 0 , 'Courier Oblique' );

            { Set active font as Courier Bold Oblique, size is 3% of page height }
            FontIndex := Doc.FontAppendStandard ( vsfCourierBoldOblique, vetWinAnsiEncoding  );
            PB.SetActiveFont(  FontIndex, cy(3), false, false );
            { Draw text label 'Courier Bold Oblique' }
            PB.TextShow(  cx(10), cy(65), 0 , 'Courier Bold Oblique' );

            { Set active font as Symbol, size is 3% of page height }
            FontIndex := Doc.FontAppendStandard ( vsfSymbol, vetWinAnsiEncoding  );
            PB.SetActiveFont(  FontIndex, cy(3), false, false );
            { Draw text label with Symbol's characters }
            PB.TextShow(  cx(10), cy(70), 0 , 'abcdefghijklmnopqrstuvwxyz' );
            { Set active font as Helvetica, size is 1% of page height }
            FontIndex := Doc.FontAppendStandard ( vsfHelvetica, vetWinAnsiEncoding  );
            PB.SetActiveFont(  FontIndex, cy(1), false, false );
            { Draw text label '(Symbol Font)' }
            PB.TextShow(  cx(90) - PB.GetTextWidth(  '(Symbol Font)' ),
                cy(70), 0, '(Symbol Font)' );

            { Set active font as Zapf Dingbats, size is 3% of page height }
            FontIndex := Doc.FontAppendStandard ( vsfZapfDingbats, vetWinAnsiEncoding  );
            PB.SetActiveFont(  FontIndex, cy(3), false, false );
            { Draw text label with Zapf Dingbats characters }
            PB.TextShow(  cx(10), cy(75), 0 , '1234567890-=[];./,abcdef' );
            { Set active font as Helvetica, size is 1% of page height }
            FontIndex := Doc.FontAppendStandard ( vsfHelvetica, vetWinAnsiEncoding  );
            PB.SetActiveFont(  FontIndex, cy(1), false, false );
            { Draw text label '(Zapf Dingbats Font)' }
            PB.TextShow(  cx(90) - PB.GetTextWidth(  '(Zapf Dingbats Font)' ),
                cy(75), 0, '(Zapf Dingbats Font)' );

            { Set active font as Helvetica, size is 1% of page height, and underline }
            PB.SetActiveFont(  FontIndex, cy(1), true, false );
            { Draw text label 'Non-standard fonts' }
            PB.TextShow(  cx(10), cy(82), 0, 'Non-standard fonts' );
            FontIndex := Doc.FontAppendTrueType('Arial',false, true );
            PB.SetActiveFont(  FontIndex, cy(3), false, false );
            PB.TextShow(  cx(10), cy(85), 0, 'True Type Font loaded from memory' );
            FontIndex := Doc.FontAppendType1('../../../data/in/fonts/macoron.pfb');
            PB.SetActiveFont(  FontIndex, cy(3), false, false );
            PB.TextShow(  cx(10), cy(90), 0, 'Type 1 Font loaded from file' );
            PB.Close;
			{**********************************************************************}
			{ Text Spacing                                                         }
			{**********************************************************************}
			{ Create new page in PDF document }
			PageIndex := Doc.AppendPage ( vpsA4, vpoPortrait );
			Page := Doc.Pages[PageIndex];
            PB := Page.PaintBox;
            { Set active font as Helvetica, size is 4% of page height }
            PB.SetActiveFont(  Doc.FontAppendStandard ( vsfHelvetica, vetWinAnsiEncoding  ),
                cy(4), false, false );
            { Font color is black }
            PB.SetFillColor( GrayToColor( 0 ) );
            { Draw title 'Character Spacing' }
            PB.TextShow(  cx(50) - PB.GetTextWidth(  'Character Spacing' ) / 2,
                cy(4), 0, 'Character Spacing' );

            { Draw blank table for text examples, size is 4 x 2 }
            { Set line width as 0.5 point }
            PB.SetLineWidth(  0.5 );
            { Set stroke color as black }
            PB.SetStrokeColor(  GrayToColor( 0 ) );
            { Set background color as light green }
            PB.SetFillColor(  RGBToColor( 0.95, 1.0, 0.95 ) );
            { Draw 4 rectangles and colorize them }
            PB.Rectangle(  cx(10), cy(10), cx(50), cy(16) );
            PB.FillAndStroke( );
            PB.Rectangle(  cx(50), cy(16), cx(90), cy(22) );
            PB.FillAndStroke( );
            PB.Rectangle(  cx(10), cy(22), cx(50), cy(28) );
            PB.FillAndStroke( );
            PB.Rectangle(  cx(50), cy(28), cx(90), cy(34) );
            PB.FillAndStroke( );

            { Set background color as light yellow }
            PB.SetFillColor(  RGBToColor( 1.0, 1.0, 0.95 ) );
            { Draw 4 rectangles and colorize them }
            PB.Rectangle(  cx(50), cy(10), cx(90), cy(16) );
            PB.FillAndStroke( );
            PB.Rectangle(  cx(10), cy(16), cx(50), cy(22) );
            PB.FillAndStroke( );
            PB.Rectangle(  cx(50), cy(22), cx(90), cy(28) );
            PB.FillAndStroke( );
            PB.Rectangle(  cx(10), cy(28), cx(50), cy(34) );
            PB.FillAndStroke( );

            { Set active font as Helvetica, size is 2% of page height }
            PB.SetActiveFont(  Doc.FontAppendStandard ( vsfHelvetica, vetWinAnsiEncoding  ),
                cy(2), false, false );
            { Set fill color as black }
            PB.SetFillColor(  GrayToColor( 0.0 ) );
            { Draw 1st column's text labels }
            PB.TextShow(  cx(15), cy(12), 0, '= -0.1' );
            PB.TextShow(  cx(15), cy(18), 0, '=  0.0 (Default)' );
            PB.TextShow(  cx(15), cy(24), 0, '=  0.1' );
            PB.TextShow(  cx(15), cy(30), 0, '=  0.5' );

            { Set Character's Spacing value as -0.1 }
            PB.SetCharacterSpacing(  -0.1 );
            { Draw text label as spacing example }
            PB.TextShow(  cx(55), cy(12), 0, 'Test String' );
            { Set Character's Spacing value as 0 ( default ) }
            PB.SetCharacterSpacing(  0 );
            { Draw text label as spacing example }
            PB.TextShow(  cx(55), cy(18), 0, 'Test String' );
            { Set Character's Spacing value as 0.1 }
            PB.SetCharacterSpacing(  0.1 );
            { Draw text label as spacing example }
            PB.TextShow(  cx(55), cy(24), 0, 'Test String' );
            { Set Character's Spacing value as 0.5 }
            PB.SetCharacterSpacing(  0.5 );
            { Draw text label as spacing example }
            PB.TextShow(  cx(55), cy(30), 0, 'Test String' );
            { Set Default Character's Spacing }
            PB.SetCharacterSpacing(  0 );

            { Set active font as Helvetica, size is 4% of page height }
            PB.SetActiveFont( Doc.FontAppendStandard ( vsfHelvetica, vetWinAnsiEncoding  ),
                cy(4), false, false );
            { Draw title 'Word Spacing' in 36% of page height from top }
            PB.TextShow(  cx(50) - PB.GetTextWidth(  'Word Spacing' ) / 2,
                cy(36), 0, 'Word Spacing' );

            { Draw blank table for text examples, size is 4 x 2 }
            { Set line width as 0.5 point }
            PB.SetLineWidth(  0.5 );
            { Set stroke color as black }
            PB.SetStrokeColor(  RGBToColor( 0.0, 0.0, 0.0 ) );
            { Set background color as light blue }
            PB.SetFillColor(  RGBToColor( 0.95, 0.95, 1.0 ) );
            { Draw 4 rectangles and colorize them }
            PB.Rectangle(  cx(10), cy(40), cx(50), cy(46) );
            PB.FillAndStroke( );
            PB.Rectangle(  cx(50), cy(46), cx(90), cy(52) );
            PB.FillAndStroke( );
            PB.Rectangle(  cx(10), cy(52), cx(50), cy(58) );
            PB.FillAndStroke( );
            PB.Rectangle(  cx(50), cy(58), cx(90), cy(64) );
            PB.FillAndStroke( );

            { Set background color as light red }
            PB.SetFillColor(  RGBToColor( 1.0, 0.95, 0.95 ) );			{ Draw 4 rectangles and colorize them }
            PB.Rectangle(  cx(50), cy(40), cx(90), cy(46) );
            PB.FillAndStroke( );
            PB.Rectangle(  cx(10), cy(46), cx(50), cy(52) );
            PB.FillAndStroke( );
            PB.Rectangle(  cx(50), cy(52), cx(90), cy(58) );
            PB.FillAndStroke( );
            PB.Rectangle(  cx(10), cy(58), cx(50), cy(64) );
            PB.FillAndStroke( );

            { Set active font as Helvetica, size is 2% of page height }
            PB.SetActiveFont(  Doc.FontAppendStandard ( vsfHelvetica, vetWinAnsiEncoding  ),
                cy(2), false, false );
            { Set fill color as black }
            PB.SetFillColor(  RGBToColor( 0.0, 0.0, 0.0 ) );
            { Draw 1st column's text labels }
            PB.TextShow(  cx(15), cy(42), 0, '= -0.5' );
            PB.TextShow(  cx(15), cy(48), 0, '=  0.0 (Default)' );
            PB.TextShow(  cx(15), cy(54), 0, '=  0.5' );
            PB.TextShow(  cx(15), cy(60), 0, '=  1.5' );

            { Set Word's Spacing value as -0.5 }
            PB.SetWordSpacing(  -0.5 );
            { Draw text label as spacing example in second column }
            PB.TextShow(  cx(55), cy(42), 0, 'Test String' );
            { Set Word's Spacing value as 0 ( default ) }
            PB.SetWordSpacing(  0 );
            { Draw text label as spacing example in second column }
            PB.TextShow(  cx(55), cy(48), 0, 'Test String' );
            { Set Word's Spacing value as 0.5 }
            PB.SetWordSpacing(  0.5 );
            { Draw text label as spacing example in second column }
            PB.TextShow(  cx(55), cy(54), 0, 'Test String' );
            { Set Word's Spacing value as 1.5 }
            PB.SetWordSpacing(  1.5 );
            { Draw text label as spacing example in second column }
            PB.TextShow(  cx(55), cy(60), 0, 'Test String' );
            { Set Default Word's Spacing }
            PB.SetWordSpacing(  0 );

            { Set active font as Helvetica, size is 4% of page height }
            PB.SetActiveFont(  Doc.FontAppendStandard ( vsfHelvetica, vetWinAnsiEncoding  ),
                cy(4), false, false );
            { Draw title 'Text Scaling' in 66% of page height from top }
            PB.TextShow(  cx(50) - PB.GetTextWidth(  'Text Scaling' ) / 2,
                cy(66), 0, 'Text Scaling' ); { Draw text title on top-center of page }

            { Draw blank table for text examples, size is 4 x 2 }
            { Set line width as 0.5 point }
            PB.SetLineWidth(  0.5 );
            { Set stroke color as black }
            PB.SetStrokeColor(  RGBToColor( 0.0, 0.0, 0.0 ) );
            { Set background color as light violet }
            PB.SetFillColor(  RGBToColor( 0.95, 0.9, 1.0 ) );
            { Draw 4 rectangles and colorize them }
            PB.Rectangle(  cx(10), cy(70), cx(50), cy(76) );
            PB.FillAndStroke( );
            PB.Rectangle(  cx(50), cy(76), cx(90), cy(82) );
            PB.FillAndStroke( );
            PB.Rectangle(  cx(10), cy(82), cx(50), cy(88) );
            PB.FillAndStroke( );
            PB.Rectangle(  cx(50), cy(88), cx(90), cy(94) );
            PB.FillAndStroke( );

            { Set background color as light orange }
            PB.SetFillColor(  RGBToColor( 1.0, 0.95, 0.9 ) );
            { Draw 4 rectangles and colorize them }
            PB.Rectangle(  cx(50), cy(70), cx(90), cy(76) );
            PB.FillAndStroke( );
            PB.Rectangle(  cx(10), cy(76), cx(50), cy(82) );
            PB.FillAndStroke( );
            PB.Rectangle(  cx(50), cy(82), cx(90), cy(88) );
            PB.FillAndStroke( );
            PB.Rectangle(  cx(10), cy(88), cx(50), cy(94) );
            PB.FillAndStroke( );

            { Set active font as Helvetica, size is 2% of page height }
            PB.SetActiveFont(  Doc.FontAppendStandard ( vsfHelvetica, vetWinAnsiEncoding  ),
                cy(2), false, false );
            { Set fill color as black }
            PB.SetFillColor(  RGBToColor( 0.0, 0.0, 0.0 ) );
            { Draw 1st column's text labels }
            PB.TextShow(  cx(15), cy(72), 0, '=   25' );
            PB.TextShow(  cx(15), cy(78), 0, '=   50' );
            PB.TextShow(  cx(15), cy(84), 0, '=  100 (Default)' );
            PB.TextShow(  cx(15), cy(90), 0, '=  250' );

            { Set Horizontal Scaling value as 25% }
            PB.SetHorizontalScaling(  25 );
            { Draw text label as spacing example in second column }
            PB.TextShow(  cx(55), cy(72), 0, 'Test String' );
            { Set Horizontal Scaling value as 50% }
            PB.SetHorizontalScaling(  50 );
            PB.TextShow(  cx(55), cy(78), 0, 'Test String' );
            { Set Horizontal Scaling value as 100% ( default ) }
            PB.SetHorizontalScaling(  100 );
            PB.TextShow(  cx(55), cy(84), 0, 'Test String' );
            { Set Horizontal Scaling value as 250% }
            PB.SetHorizontalScaling(  250 );
            PB.TextShow(  cx(55), cy(90), 0, 'Test String' );
            { Set Default Horizontal Scaling }
            PB.SetHorizontalScaling(  100 );
            PB.Close;

			{**********************************************************************}
			{ Text Rendering Mode and Rotation Examples                            }
			{**********************************************************************}

			PageIndex := Doc.AppendPage ( vpsA4, vpoPortrait );
			Page := Doc.Pages[PageIndex];
            PB := Page.PaintBox;
            { Set active as Helvetica, size is 4% of page height }
            PB.SetActiveFont(  Doc.FontAppendStandard ( vsfHelvetica, vetWinAnsiEncoding  ),
                cy(4), false, false );
            { Font color is black }
            PB.SetFillColor(  RGBToColor( 0.0, 0.0, 0.0 ) );
            { Draw title 'Text rendering modes' }
            PB.TextShow(  cx(50) - PB.GetTextWidth(  'Text rendering modes' ) / 2,
                cy(4), 0, 'Text rendering modes' );

            { Draw blank table for text examples, size is 4 x 2 }
            { Set line width as 0.5 point }
            PB.SetLineWidth(  0.5 );
            { Set stroke color as black }
            PB.SetStrokeColor(  RGBToColor( 0.0, 0.0, 0.0 ) );
            { Set background color as white }
            PB.SetFillColor(  RGBToColor( 1.0, 1.0, 1.0 ) );
            { Draw 8 rectangles and colorize them }
            PB.Rectangle(  cx(10), cy(10), cx(50), cy(20) );
            PB.FillAndStroke( );
            PB.Rectangle(  cx(50), cy(10), cx(90), cy(20) );
            PB.FillAndStroke( );
            PB.Rectangle(  cx(10), cy(20), cx(50), cy(30) );
            PB.FillAndStroke( );
            PB.Rectangle(  cx(50), cy(20), cx(90), cy(30) );
            PB.FillAndStroke( );
            PB.Rectangle(  cx(10), cy(30), cx(50), cy(40) );
            PB.FillAndStroke( );
            PB.Rectangle(  cx(50), cy(30), cx(90), cy(40) );
            PB.FillAndStroke( );
            PB.Rectangle(  cx(10), cy(40), cx(50), cy(50) );
            PB.FillAndStroke( );
            PB.Rectangle(  cx(50), cy(40), cx(90), cy(50) );
            PB.FillAndStroke( );

            { Set active font as Helvetica, size is 2% of page height }
            PB.SetActiveFont(  Doc.FontAppendStandard ( vsfHelvetica, vetWinAnsiEncoding  ),
                cy(2), false, false );
            { Set fill color as black }
            PB.SetFillColor(  RGBToColor( 0.0, 0.0, 0.0 ) );
            { Draw 1st column's text labels }
            PB.TextShow(  cx(15), cy(14), 0, '= 0 ( fill, default mode )' );
            PB.TextShow(  cx(15), cy(24), 0, '= 1 ( stroke )' );
            PB.TextShow(  cx(15), cy(34), 0, '= 2 ( fill and stroke )' );
            PB.TextShow(  cx(15), cy(44), 0, '= 3 ( invisible )' );

            { Set active font as Helvetica, size is 5% of page height }
            PB.SetActiveFont(  Doc.FontAppendStandard ( vsfHelveticaBold, vetWinAnsiEncoding  ),
                cy(5), false, false );
            { Set stroke color as blue }
            PB.SetStrokeColor(  RGBToColor( 0.0, 0.0, 1.0 ) );
            { Set fill color as red }
            PB.SetFillColor(  RGBToColor( 1.0, 0.0, 0.0 ) );
            { Set stroke line width as 2 points }
            PB.SetLineWidth(  2 );

            { Set Text Rendering Mode as 0 ( default ) }
            PB.SetTextRenderingMode( vrmFillText );
            { Draw text label as spacing example in second column }
            PB.TextShow(  cx(52), cy(13), 0, 'Test String' );
            PB.SetTextRenderingMode( vrmStrokeText);
            PB.TextShow(  cx(52), cy(23), 0, 'Test String' );
            PB.SetTextRenderingMode(  vrmFillAndStrokeText );
            PB.TextShow(  cx(52), cy(33), 0, 'Test String' );
            PB.SetTextRenderingMode(  vrmInvisibleText );
            PB.TextShow(  cx(52), cy(43), 0, 'Test String' );
            PB.SetTextRenderingMode(  vrmFillText );

            { Set active font as Helvetica, size is 4% of page height }
            PB.SetActiveFont(  Doc.FontAppendStandard ( vsfHelvetica, vetWinAnsiEncoding  ),
                cy(4), false, false );
            { Set Fill color as black }
            PB.SetFillColor(  RGBToColor( 0.0, 0.0, 0.0 ) );
            { Draw title 'Text Rotation' in 53% of page height from top }
            PB.TextShow(  cx(50) - PB.GetTextWidth(  'Text Rotation' ) / 2,
                cy(53), 0, 'Text Rotation' );

            { Set active font as Helvetica, size is 2% of page height }
            PB.SetActiveFont(  Doc.FontAppendStandard ( vsfHelvetica, vetWinAnsiEncoding  ),
                cy(2), false, false );
            { Set Radius of example's circle as Label Width plus 1% of page width}
            Radius := PB.GetTextWidth(  'Test String' ) + cx(1);
            { Draw Text Rotation on first third of circle in red color }
             for i := 0 to 11 do
            begin
                { Set Fill color as red tone }
                PB.SetFillColor(  RGBToColor( Cos((i - 6)/3.83), 0.0, 0.0 ) );
                { Draw text label 'Test String' with rotation on angle from 0 to 120 degrees }
                PB.TextShow(  cx(50) + Radius * Cos( i * 0.1745 ),
                    cy(77) - Radius * Sin (i * 0.1745 ), i * 10, 'Test String' );
            end;
            { Draw Text Rotation on second third of circle in green color }
            for i := 12 to 23 do
            begin
                { Set Fill color as green tone }
                PB.SetFillColor(  RGBToColor( 0.0, Cos((i - 18)/3.83), 0.0 ) );
                { Draw text label 'Test String' with rotation on angle from 120 to 240 degrees }
                PB.TextShow(  cx(50) + Radius * Cos( i * 0.1745 ),
                    cy(77) - Radius * Sin ( i * 0.1745 ), i * 10, 'Test String' );
            end;
            { Draw Text Rotation on last third of circle in blue color }
            for i := 24 to 35 do
            begin
                { Set Fill color as blue tone }
                PB.SetFillColor(  RGBToColor( 0.0, 0.0, Cos((i - 30)/3.83) ) );
                { Draw text label 'Test String' with rotation on angle from 240 to 360 degrees }
                PB.TextShow(  cx(50) + Radius * Cos( i * 0.1745 ),
                    cy(77) - Radius * Sin ( i * 0.1745 ), i * 10, 'Test String' );
            end;
            PB.Close;
			Doc.SaveToFile( '../../../data/out/pdf/FontsAndText.pdf');
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
