program Graph;

{$APPTYPE CONSOLE}

uses
  SysUtils, VersyPDF, VersyPDFTypes;

Const
    PageWidth = 595;
    PageHeight = 842;
    FiguresCount = 200;
    LineDashTypes :array [0..8] of String = (
				'[2 2] 0', '[4 4] 0', '[8 8] 0', '[8 8] 4', '[8 8] 8', '[12 4] 0',
	    		'[16 3 4 3] 0', '[13 3 2 3 2 3] 0', '[ ] 0' );
    function cx( X:Double ):Double ;
    begin
        Result :=  PageWidth*X/100;
    end;
    function cy( Y:Double ):Double ;
    begin
        Result :=  PageHeight*Y/100;
    end;
var
    Lib: TPDFLibrary ;
    Doc: TPDFDocument ;
    PB: TPDFPaintBox ;
    Page: TPDFPage ;
    GState:TPDFExtGraphicState;
    PageIndex, i: Integer;
    a, b, c, d, Radius : Double;
    TextString: string;
begin
  try
    Radius := (cx(10)+cy(10))/2;
    Randomize;
    Lib := TPDFLibrary.Create('','');
    try
        Doc := TPDFDocument.Create ( Lib );
        try
            Doc.AutoLaunch := true;
            PageIndex := Doc.AppendPage ( PageWidth, PageHeight );
            // First page
            Page := Doc.Pages[PageIndex];
            PB := Page.PaintBox;
            { Save the current graphics state on the graphics state stack. }
            PB.StateStore();
            { Set the line 1 point width. }
            PB.SetLineWidth( 1 );
            { Set the line cap style in the graphics state. }
            PB.SetLineCap( vlcRound);
            { Set the line join style in the graphics state. }
            PB.SetLineJoin( vljRound );
            { Set the filling Color space to DeviceRGB and Color as White to use for filling operations }
            PB.SetFillColor( RGBToColor( 0.92, 0.95, 0.98 ) );
            { Append a rectangle to the current path as all active area. }
            PB.Rectangle( cx(0), cy(0), cx(100), cy(100) );
            { Fill the path, using the nonzero winding number rule to determine the region to fill.
            Any subpaths that are open are implicitly closed before being filled.}
            PB.Fill();
            { Set the stroking Color as Dark Blue }
            PB.SetStrokeColor( RGBToColor( 0.0, 0.0, 0.3 ) );
            { Set Rectangle Border around whole area }
            PB.Rectangle( cx(0), cy(0), cx(100), cy(100) );
            { Draw Rectangle Border }
            PB.Stroke();
            { Set the stroking Color as Medium Blue }
            PB.SetStrokeColor( RGBToColor( 0.0, 0.0, 0.6 ) );
            { Set Rectangle Border in previous Rectangle }
            PB.Rectangle( cx(1), cy(1), cx(99), cy(99) );
            { Draw Rectangle Border }
            PB.Stroke();
            { Set the stroking Color as Light Blue }
            PB.SetStrokeColor( RGBToColor( 0.0, 0.0, 1.0 ) );
            { Set Rectangle Border in previous Rectangle }
            PB.Rectangle( cx(2), cy(2), cx(98), cy(98) );
            { Draw Rectangle Border }
            PB.Stroke();

            { Set the text font as Helvetica Oblique, size is 2,5% of page height, and underline text }
            PB.SetActiveFont( Doc.FontAppendStandard ( vsfHelvetica, vetWinAnsiEncoding  ),
                cy(2.5), true, false );
            { Text Color set a dark brown }
            PB.SetFillColor( RGBToColor( 0.1, 0.1, 0.0 ) );
            { Output text of title in upper-left corner of page }
            PB.TextShow( cx(3), cy(3), 0, 'Graphics Primitives. Figures.' );

            { Border color is dark maroon }
            PB.SetStrokeColor( RGBToColor( 0.3, 0.0, 0.0 ) );
            { Filling color is light rose }
            PB.SetFillColor( RGBToColor( 1.0, 0.9, 0.9 ) );
            { Set circle centered at ( 50% of page, 25% of page) with radius as 'Radius' }
            PB.Circle( cx(50), cy(25), Radius );
            { Draw circle }
            PB.FillAndStroke();

            { Set the text font as Helvetica, size is 4% of page height }
            PB.SetActiveFont( Doc.FontAppendStandard ( vsfHelvetica, vetWinAnsiEncoding  ),
                cy(4), false, false );
            { Text Color set a maroon }
            PB.SetFillColor( RGBToColor( 0.5, 0.0, 0.0 ) );
            { Output caption in center of circle }
            PB.TextShow( cx(50) - Radius / 2, cy(23) , 0, 'Circle' );

            { Border color is dark green }
            PB.SetStrokeColor( RGBToColor( 0.0, 0.3, 0.0 ) );
            { Filling color is light green }
            PB.SetFillColor( RGBToColor( 0.9, 1.0, 0.9 ) );
            { Set rectangle centered at ( 25% of page, 50% of page) with side as double 'Radius' }
            PB.Rectangle( cx(25) - Radius, cy(50) - Radius, cx(25) + Radius, cy(50) + Radius );
            { Draw rectangle }
            PB.FillAndStroke();

            { Text Color set a dark green }
            PB.SetFillColor( RGBToColor( 0.0, 0.5, 0.0 ) );
            { Output caption set on left of left-bottom corner of rectangle }
            PB.TextShow( cx(25) - Radius - 2, cy(51), -90, 'Rect' );
            { Output caption set on bottom of left-bottom corner of rectangle }
            PB.TextShow( cx(25) - Radius, cy(50) + Radius + 2, 0, 'angle' );

            { Border color is dark blue }
            PB.SetStrokeColor( RGBToColor( 0.0, 0.0, 0.3 ) );
            { Filling color is light blue }
            PB.SetFillColor( RGBToColor( 0.9, 0.9, 1.0 ) );
            { Set pie centered at ( 75% of page, 50% of page) with radius as 'Radius',
            starting angle is 45 degrees, ending angle is -45 degrees }
            PB.Pie( cx(75) - Radius, cy(50) - Radius, cx(75) + Radius, cy(50) + Radius, 45, -45 );
            { Draw pie }
            PB.FillAndStroke();

            { Text Color set a dark blue }
            PB.SetFillColor( RGBToColor( 0.0, 0.0, 0.5 ) );
            { Output caption set on right of pie center, and rotate text on 45 degree }
            PB.TextShow( cx(75) + Radius / 2, cy(50), 45, 'Pie' );

            { Border color is dark brown }
            PB.SetStrokeColor( RGBToColor( 0.3, 0.3, 0.0 ) );
            { Filling color is light yellow }
            PB.SetFillColor( RGBToColor( 1.0, 1.0, 0.9 ) );
            { Set start of path to upper top of triangle }
            PB.MoveTo( cx(50), cy(75) - 0.866 * Radius );
            { Move path cursor to right top of triangle }
            PB.LineTo( cx(50) + Radius, cy(75) + 0.866 * Radius );
            { Move path cursor to left top of triangle }
            PB.LineTo( cx(50) - Radius, cy(75) + 0.866 * Radius );
            { Return path cursor to upper top of triangle }
            PB.LineTo( cx(50), cy(75) - 0.866 * Radius );
            { Draw triangle }
            PB.FillAndStroke();

            { Text Color set a brown }
            PB.SetFillColor( RGBToColor( 0.5, 0.5, 0.0 ) );
            { Output caption set on left side of triangle, and rotate text on 60 degree }
            PB.TextShow( cx(50) - Radius, cy(75), 60, 'Tri' );
            { Output caption set on right side of triangle, and rotate text on -60 degree }
            PB.TextShow( cx(50) + Radius / 1.7, cy(75) - Radius / 1.7, -60, 'angle' );

            { Border color is teal }
            PB.SetStrokeColor( RGBToColor( 0.0, 0.3, 0.3 ) );
            { Filling color is light aqua }
            PB.SetFillColor( RGBToColor( 0.9, 1.0, 1.0 ) );
            { Set start of path to center of page }
            PB.MoveTo( cx(49), cy(49) );
            { Add Bezier cubic curve segment to graphics path }
            PB.CurveTo( cx(49) - Radius, cy(49) + Radius, cx(49) - Radius,
                cy(49) - Radius, cx(49) + Radius, cy(49) - Radius );
            { Draw it }
            PB.ClosePath();

            { Curve is symmetrical for center of page }
            PB.MoveTo( cx(51), cy(51) );
            PB.CurveTo( cx(51) + Radius, cy(51) - Radius, cx(51) + Radius,
                cy(51) + Radius, cx(51) - Radius, cy(51) + Radius );
            PB.ClosePath();
            PB.FillAndStroke();

            { Text Color set a teal }
            PB.SetFillColor( RGBToColor( 0.0, 0.5, 0.5 ) );
            { Output caption set on first curve figure, and rotate text on 45 degree }
            PB.TextShow( cx(49) - Radius / 2, cy(49), 45, 'Bezier' );
            { Output caption set on second curve figure, and rotate text on 45 degree }
            PB.TextShow( cx(51) - Radius / 2, cy(51) + Radius / 2, 45, 'curve' );

            { Draw 40 arcs, with different widths, colors and arc's parameters }
            for i := 1 to 10 do
            begin
                { With increase line width }
                PB.SetLineWidth( cy( i * i / 20 ) );
                { Set arc color as red tones }
                PB.SetStrokeColor( RGBToColor( 1.0, 1 - i/10, 1 - i/10 ) );
                { Set graphical path in right-top side of page }
                PB.Arc( cx(5), cy(5), cx(95), cy(95), 4*i, 90 - 4*i );
                { Draw one arc }
                PB.Stroke();
                { Set arc color as green tones }
                PB.SetStrokeColor( RGBToColor( 1 - i/10, 1.0, 1 - i/10 ) );
                { Set graphical path in left-top side of page }
                PB.Arc( cx(5), cy(5), cx(95), cy(95), 90 + 4*i, 180 - 4*i );
                { Draw one arc }
                PB.Stroke();
                { Set arc color as yellow tones }
                PB.SetStrokeColor( RGBToColor( 1.0, 1.0, 1 - i/10 ));
                { Set graphical path in left-bottom side of page }
                PB.Arc( cx(5), cy(5), cx(95), cy(95), - 180 + 4*i, -90 - 4*i );
                { Draw one arc }
                PB.Stroke();
                { Set arc color as blue tones }
                PB.SetStrokeColor( RGBToColor( 1 - i/10, 1 - i/10, 1.0 ) );
                { Set graphical path in right-bottom side of page }
                PB.Arc( cx(5), cy(5), cx(95), cy(95), -90 + 4*i, - 4*i );
                { Draw one arc }
                PB.Stroke();
            end;
            { Restore the graphics state by removing the most recently saved state
            from the stack and making it the current state.}
            PB.StateRestore();
            PB.Close;
            PageIndex := Doc.AppendPage ( PageWidth, PageHeight );
            Page := Doc.Pages[PageIndex ];
            PB := Page.PaintBox;
            { Save the current graphics state on the graphics state stack. }
            PB.StateStore( );
            { Set active font as Helvetica, size is 4% of page height }
            PB.SetActiveFont( Doc.FontAppendStandard ( vsfHelvetica, vetWinAnsiEncoding  ),
                cy(4), false, false );
            { Font color is black }
            PB.SetFillColor( RGBToColor( 0.0, 0.0, 0.0 ) );
            { Draw title of current page }
            PB.TextShow( cx(50) - PB.GetTextWidth( 'Lines' )/2, cy(4), 0, 'Lines' );
            { Set active font as Helvetica, size is 1.2% of page height }
            PB.SetActiveFont( Doc.FontAppendStandard ( vsfHelveticaBold, vetWinAnsiEncoding  ),
                cy(1.2), false, false );
            { Draw title of current example near right border of page }
            PB.TextShow( cx(80), cy(9), 0, 'Dash Types' );

            { Set stroke color as black }
            PB.SetStrokeColor( RGBToColor( 0.0, 0.0, 0.0 ) );
            { Draw 9 types of dashed lines from dash type array - LineDashTypes }
            for  i := 0 to 8 do
            begin
                { Set dash type from string's array }
                PB.SetDash( LineDashTypes[ i ] );
                { Move current position on in start of line }
                PB.MoveTo( cx( 10 ), cy( 10 + 3 * i ) );
                { Draw line from left side to right }
                PB.LineTo( cx( 60 ), cy( 10 + 3 * i ) );
                { ... and colorize it }
                PB.Stroke( );
                { Draw dash type as text near example line }
                PB.TextShow( cx(62), cy( 9.5 + 3 * i ), 0, LineDashTypes[ i ] );
            end;

            { Draw title of current example near right border of page,
            and in 39% of page height from top }
            PB.TextShow( cx(80), cy(39), 0, 'Width Values' );
            for i := 0 to 18 do
            begin
                { Increase line width from 0 to 4.5 by 0.25 points }
                PB.SetLineWidth( 0.25 * i );
                { Set line color as dark blue }
                PB.SetStrokeColor( RGBToColor( 0.0, 0.0, 0.5 ) );
                { Move current position on in start of line }
                PB.MoveTo( cx( 10 ), cy( 40 + 3 * i ) );
                { Draw line from left side to right }
                PB.LineTo( cx( 60 ), cy( 40 + 3 * i ) );
                { ... and colorize it }
                PB.Stroke( );
                { Create line width value as text ... }
                TextString := 'Line width = ' + FloatToStr ( 0.25 * i );
                { ... and draw text label near example line }
                PB.TextShow( cx( 62 ), cy( 39.5 + 3 * i ), 0, TextString );
            end;
            { Restore the graphics state by removing the most recently saved state
            from the stack and making it the current state.}
            PB.StateRestore( );
            PB.Close;
            PageIndex := Doc.AppendPage ( PageWidth, PageHeight );
            Page := Doc.Pages[ PageIndex ];
            PB := Page.PaintBox;
            {******************************************************************************}
            { Draw graphic primitives as rectangles, rotated rectangles, ellipses, circles }
            {******************************************************************************}
            { Create new page in PDF document }
            { Save the current graphics state on the graphics state stack. }
            PB.StateStore( );
            { Set active font as Helvetica, size is 3% of page height }
            PB.SetActiveFont( Doc.FontAppendStandard ( vsfHelvetica, vetWinAnsiEncoding  ),
                cy(3), false, false );
            { Text Color set a black }
            PB.SetFillColor( RGBToColor( 0.0, 0.0, 0.0 ) );
            { Draw title 'Rectangles' }
            PB.TextShow( cx(28) - PB.GetTextWidth( 'Rectangles' )/2, cy(4), 0, 'Rectangles' );
            { End the path object without filling or stroking it. }
            PB.NewPath( );
            { Set big rectangle for inserting other rectangles }
            PB.Rectangle( cx(10), cy(10), cx(45), cy(45) );
            { Check path for clipping }
            PB.Clip( );
            { Start new path for drawing }
            PB.NewPath( );
            { Set line width as 0.5 points }
            PB.SetLineWidth( 0.5 );
            { Initialize number generation : Random  is number from 0 to 1 }
            { Draw rectangles in clipping path area }
            for i := 0 to FiguresCount -1 do
            begin
                { Set randomize stroke color }
                PB.SetStrokeColor( RGBToColor( Random , Random , Random  ) );
                { Set randomize fill color }
                PB.SetFillColor( RGBToColor( Random , Random , Random  ) );
                { Random X-coordinate of left border }
                a := cx( 5 ) + cx( 35 * Random  );
                { Random Y-coordinate of top border }
                b := cy( 5 ) + cy( 35 * Random );
                { Random width of rectangle }
                c := cx( 10 * Random  );
                { Random height of rectangle }
                d := cy( 10 * Random  );
                { Draw this rectangle }
                PB.Rectangle( a, b, a + c, b + d );
                { ... and colorize it }
                PB.FillAndStroke( );
            end;
            { Restore the graphics state by removing the most recently saved state
            from the stack and making it the current state.}
            PB.StateRestore( );
            { Start new path for drawing without old clipping path }
            PB.NewPath( );
            { Set line width as 3 points }
            PB.SetLineWidth( 3 );
            { Set line color of rectangle as black }
            PB.SetStrokeColor( RGBToColor( 0.0, 0.0, 0.0 ) );
            { Draw dribble rectangle }
            PB.Rectangle( cx(10), cy(10), cx(45), cy(45) );
            { ... and colorize it }
            PB.Stroke( );
            { Save the current graphics state on the graphics state stack. }
            PB.StateStore( );
            { Set the text font as Helvetica, size is 3% of page height }
            PB.SetActiveFont( Doc.FontAppendStandard ( vsfHelvetica, vetWinAnsiEncoding  ),
                cy(3), false, false );
            { Text Color set a black }
            PB.SetFillColor( RGBToColor( 0.0, 0.0, 0.0 ) );
            { Draw title 'Rotated rectangles' }
            PB.TextShow( cx(74) - PB.GetTextWidth( 'Rotated rectangles' )/2, cy(4), 0, 'Rotated rectangles' );
            { End the path object without filling or stroking it. }
            PB.NewPath( );
            { Set big rectangle for inserting other rectangles }
            PB.Rectangle( cx(55), cy(10), cx(90), cy(45) );
            { Check path for clipping }
            PB.Clip( );
            { Start new path for drawing }
            PB.NewPath( );
            { Set line width as 0.5 points }
            PB.SetLineWidth( 0.5 );
            { Initialize number generation : Random  is number from 0 to 1 }
            { Draw rotated rectangles in clipping path area }
            for i := 0 to FiguresCount -1 do
            begin
                { Set randomize stroke color }
                PB.SetStrokeColor( RGBToColor( Random , Random , Random  ) );
                { Set randomize fill color }
                PB.SetFillColor( RGBToColor( Random , Random , Random  ) );
                { Random X-coordinate of left border }
                a := cx( 50 ) + cx( 35 * Random  );
                { Random Y-coordinate of top border }
                b := cy( 90 ) - cy( 35 * Random  );
                { Random width of rectangle }
                c := cx( 10 * Random  );
                { Random height of rectangle }
                d := cy( 10 * Random  );
                { Draw this rectangle which rotated on randomize angle form -45 to 45 degrees }
                PB.RectRotated( a, b, c, d, 90 * Random  - 45 );
                { ... and colorize it }
                PB.FillAndStroke( );
            end;
            { Restore the graphics state by removing the most recently saved state
            from the stack and making it the current state.}
            PB.StateRestore( );
            { Start new path for drawing without old clipping path }
            PB.NewPath( );
            { Set line width as 3 points }
            PB.SetLineWidth( 3 );
            { Set line color of rectangle as black }
            PB.SetStrokeColor( RGBToColor( 0.0, 0.0, 0.0 ) );
            { Draw dribble rectangle }
            PB.Rectangle( cx(55), cy(10), cx(90), cy(45) );
            { ... and colorize it }
            PB.Stroke( );

            { Save the current graphics state on the graphics state stack. }
            PB.StateStore( );
            { Set the text font as Helvetica, size is 3% of page height }
            PB.SetActiveFont( Doc.FontAppendStandard ( vsfHelvetica, vetWinAnsiEncoding  ),
                cy(3), false, false );
            { Text Color set a black }
            PB.SetFillColor( RGBToColor( 0.0, 0.0, 0.0 ) );
            { Draw title 'Ellipses' }
            PB.TextShow( cx(28) - PB.GetTextWidth( 'Ellipses' )/2, cy(49), 0, 'Ellipses' );
            { End the path object without filling or stroking it. }
            PB.NewPath( );
            { Draw big ellipse for inserting other ellipses }
            PB.Ellipse( cx(10), cy(55), cx(45), cy(90) );
            { Check path for clipping }
            PB.Clip( );
            { Start new path for drawing }
            PB.NewPath( );
            { Set line width as 0.5 points }
            PB.SetLineWidth( 0.5 );
            { Draw ellipses in clipping path area }
            for i := 0 to FiguresCount -1 do
            begin
                { Set randomize stroke color }
                PB.SetStrokeColor( RGBToColor( Random , Random , Random  ) );
                { Set randomize fill color }
                PB.SetFillColor( RGBToColor( Random , Random , Random  ) );
                { Random X-coordinate of left border }
                a := cx( 5 ) + cx( 35 * Random  );
                { Random Y-coordinate of top border }
                b := cy( 50 ) + cy( 35 * Random  );
                { Random width of ellipse }
                c := cx( 10 * Random  );
                { Random height of ellipse }
                d := cy( 10 * Random  );
                { Draw this ellipse }
                PB.Ellipse( a, b, a + c, b + d );
                { ... and colorize it }
                PB.FillAndStroke( );
            end;
            { Restore the graphics state by removing the most recently saved state
            from the stack and making it the current state.}
            PB.StateRestore( );
            { Start new path for drawing without old slipping path }
            PB.NewPath( );
            { Set line width as 3 points }
            PB.SetLineWidth( 3 );
            { Set line color of rectangle as black }
            PB.SetStrokeColor( RGBToColor( 0.0, 0.0, 0.0 ) );
            { Draw dribble ellipse }
            PB.Ellipse( cx(10), cy(55), cx(45), cy(90) );
            { ... and colorize it }
            PB.Stroke( );

            { Save the current graphics state on the graphics state stack. }
            PB.StateStore( );
            { Set the text font as Helvetica, size is 3% of page height }
            PB.SetActiveFont( Doc.FontAppendStandard ( vsfHelvetica, vetWinAnsiEncoding  ),
                cy(3), false, false );
            { Text Color set a black }
            PB.SetFillColor( RGBToColor( 0.0, 0.0, 0.0 ) );
            { Draw title 'Circles' }
            PB.TextShow( cx(74) - PB.GetTextWidth( 'Circles' )/2, cy(49), 0, 'Circles' );
            { End the path object without filling or stroking it. }
            PB.NewPath( );
            { Draw big circle for inserting other circles }
            PB.Circle( cx(73), cy(73), cx(18) );
            { Check path for clipping }
            PB.Clip( );
            { Start new path for drawing }
            PB.NewPath( );
            { Set line width as 0.5 points }
            PB.SetLineWidth( 0.5 );
            { Draw circles in clipping path area }
            for  i := 0 to FiguresCount -1 do
            begin
                { Set randomize stroke color }
                PB.SetStrokeColor( RGBToColor( Random , Random , Random  ) );
                { Set randomize fill color }
                PB.SetFillColor( RGBToColor( Random , Random , Random  ) );
                { Random X-coordinate of circle center }
                a := cx( 55 ) + cx( 35 * Random  );
                { Random Y-coordinate of circle center }
                b := cy( 55 ) + cy( 35 * Random  );
                { Random radius of circle }
                c := cx( 5 * Random  );
                { Draw this circle }
                PB.Circle( a, b, c );
                { ... and colorize it }
                PB.FillAndStroke( );
            end;
            { Restore the graphics state by removing the most recently saved state
            from the stack and making it the current state.}
            PB.StateRestore( );
            { Start new path for drawing without old slipping path }
            PB.NewPath( );
            { Set line width as 3 points }
            PB.SetLineWidth( 3 );
            { Set line color of rectangle as black }
            PB.SetStrokeColor( RGBToColor( 0.0, 0.0, 0.0 ) );
            { Draw dribble circle }
            PB.Circle( cx(73), cy(73), cx(18) );
            { ... and colorize it }
            PB.Stroke( );
            PB.Close;
            PageIndex := Doc.AppendPage ( PageWidth, PageHeight );
            Page := Doc.Pages[PageIndex];
            PB := Page.PaintBox;
            {**********************************************************************}
            { Draw round rectangles, pies and Bezier curves. Example.              }
            {**********************************************************************}
            { Save the current graphics state on the graphics state stack. }
            PB.StateStore( );
            { Set active font as Helvetica, size is 3% of page height }
            PB.SetActiveFont( Doc.FontAppendStandard ( vsfHelvetica, vetWinAnsiEncoding  ),
                cy(3), false, false );
            { Text Color set a black }
            PB.SetFillColor( RGBToColor( 0.0, 0.0, 0.0 ) );
            { Draw title 'Round rectangles' }
            PB.TextShow( cx(28) - PB.GetTextWidth( 'Round rectangles' )/2, cy(4), 0, 'Round rectangles' );

            { Stroke color set a maroon }
            PB.SetStrokeColor( RGBToColor( 0.6, 0.0, 0.0 ) );
            { Fill color set a white }
            PB.SetFillColor( RGBToColor( 1.0, 1.0, 1.0 ) );
            { Draw 8 round rectangles with variable parameters }
            for  i := 0 to 7 do
            begin
                { Change line width from 0 to 5 points }
                PB.SetLineWidth( 2*i/3 );
                { Draw round rectangle }
                PB.RoundRect( cx( 10 + i), cy(10 + i), cx(45 - i), cy(45 - i), cx(3 * i), cy(3 * i) );
                { ... and colorize it }
                PB.FillAndStroke( );
            end;
            { Stroke color set a white }
            PB.SetStrokeColor( RGBToColor( 1.0, 1.0, 1.0 ) );
            { Fill color set a maroon }
            PB.SetFillColor( RGBToColor( 0.6, 0.0, 0.0 ) );
            { Draw 8 round rectangles with variable parameters }
            for  i := 8 to 15 DO
            begin
                { Change line width from 5 to 0 points }
                PB.SetLineWidth( 10 - 2*i/3 );
                { Draw round rectangle }
                PB.RoundRect( cx( 10 + i), cy(10 + i), cx(45 - i), cy(45 - i), cx( 45 - 3 * i ), cy( 45 - 3 * i ) );
                { ... and colorize it }
                PB.FillAndStroke( );
            end;
            { Restore the graphics state by removing the most recently saved state
            from the stack and making it the current state.}
            PB.StateRestore( );

            { Save the current graphics state on the graphics state stack. }
            PB.StateStore( );
            { Set the text font as Helvetica, size is 3% of page height }
            PB.SetActiveFont( Doc.FontAppendStandard ( vsfHelvetica, vetWinAnsiEncoding  ),
                cy(3), false, false );
            { Text Color set a black }
            PB.SetFillColor( RGBToColor( 0.0, 0.0, 0.0 ) );
            { Draw title 'Pies' }
            PB.TextShow( cx(74) - PB.GetTextWidth( 'Pies' )/2, cy(4), 0, 'Pies' );
            { Stroke color set a maroon }
            PB.SetStrokeColor( RGBToColor( 0.6, 0.0, 0.0 ) );
            { Fill color set a red }
            PB.SetFillColor( RGBToColor( 1.0, 0.0, 0.0 ) );
            { Set line width as 1 point }
            PB.SetLineWidth( 1 );
            { Set join style as Mode 0 }
            PB.SetLineJoin( vljRound );
            { Draw big pie }
            PB.Pie( cx(50), cy(15), cx(85), cy(40), cx(90), cy(15), cx(90), cy(40) );
            { ... and colorize it }
            PB.FillAndStroke( );
            { Stroke color set a dark green }
            PB.SetStrokeColor( RGBToColor( 0.0, 0.6, 0.0 ) );
            { Fill color set a green }
            PB.SetFillColor( RGBToColor( 0.0, 1.0, 0.0 ) );
            { Draw small pie }
            PB.Pie( cx(60), cy(15), cx(95), cy(40), cx(100), cy(40), cx(100), cy(15) );
            { ... and colorize it }
            PB.FillAndStroke( );
            { Restore the graphics state by removing the most recently saved state
            from the stack and making it the current state.}
            PB.StateRestore( );

            { Save the current graphics state on the graphics state stack. }
            PB.StateStore( );
            { Set the text font as Helvetica, size is 3% of page height }
            PB.SetActiveFont( Doc.FontAppendStandard ( vsfHelvetica, vetWinAnsiEncoding  ),
                cy(3), false, false );
            { Text Color set a black }
            PB.SetFillColor( RGBToColor( 0.0, 0.0, 0.0 ) );
            { Draw title 'Cubic Bezier curves' }
            PB.TextShow( cx(50) - PB.GetTextWidth( 'Cubic Bezier curves' )/2, cy(49),
                0, 'Cubic Bezier curves' );

            { Set line color as black }
            PB.SetStrokeColor( RGBToColor( 0.0, 0.0, 0.0 ) );
            { Set line width as 1 point }
            PB.SetLineWidth( 1 );
            { Set dash line style as Solid ( default )}
            PB.NoDash( );
            { Move current position on in start of Bezier Curve }
            PB.MoveTo( cx(65), cy(90) );
            { Draw Bezier Curve with cubic interpolation by 2 points and 1 destination point }
            PB.CurveTo( cx(50), cy(70), cx(90), cy(70), cx(75), cy(85) );
            { ... and colorize it }
            PB.Stroke( );

            { Set line color as gray }
            PB.SetStrokeColor( RGBToColor( 0.5, 0.5, 0.5 ) );
            { Set line width as 0.25 points }
            PB.SetLineWidth( 0.25 );
            { Set dash line style as dash-dotted mode }
            PB.SetDash( '[3 3] 0' );
            { Move current position on in start of Bezier Curve }
            PB.MoveTo( cx(65), cy(90) );
            { Draw interpolation vector from start to first interpolation point }
            PB.LineTo( cx(50), cy(70) );
            { ... and colorize it }
            PB.Stroke( );
            { Move current position on in finish of Bezier Curve }
            PB.MoveTo( cx(75), cy(85) );
            { Draw interpolation vector from finish to second interpolation point }
            PB.LineTo( cx(90), cy(70) );
            { ... and colorize it }
            PB.Stroke( );

            { Draw bold maroon point as circle in start of Bezier Curve }
            PB.SetFillColor( RGBToColor( 0.6, 0.0, 0.0 ) );
            PB.Circle( cx(65), cy(90), 3 );
            PB.Fill( );

            { Draw bold dark blue point as circle in first interpolation point }
            PB.SetFillColor( RGBToColor( 0.0, 0.0, 0.3 ) );
            PB.Circle( cx(50), cy(70), 2 );
            PB.Fill( );

            { Draw bold dark blue point as circle in second interpolation point }
            PB.SetFillColor( RGBToColor( 0.0, 0.0, 0.3 ) );
            PB.Circle( cx(90), cy(70), 2 );
            PB.Fill( );

            { Draw bold dark green point as circle in finish of Bezier Curve }
            PB.SetFillColor( RGBToColor( 0.0, 0.6, 0.0 ) );
            PB.Circle( cx(75), cy(85), 2.5 );
            PB.Fill( );

            { Save the current graphics state on the graphics state stack. }
            PB.StateStore( );
            { Set the text font as Helvetica, size is 1% of page height }
            PB.SetActiveFont( Doc.FontAppendStandard ( vsfHelvetica, vetWinAnsiEncoding  ),
                cy(1), false, false );
            { Text Color set a black }
            PB.SetFillColor( RGBToColor( 0.0, 0.0, 0.0 ) );
            { Draw caption of point in start of Bezier Curve as 'x0, y0 - current point' }
            PB.TextShow( cx(67) - PB.GetTextWidth( 'x0, y0 - current point' )/2, cy(91),
                0, 'x0, y0 - current point' );
            { Draw caption of first interpolation point as 'x1, y1 - interpolation point' }
            PB.TextShow( cx(52) - PB.GetTextWidth( 'x1, y1 - interpolation point' )/2, cy(71),
                0, 'x1, y1 - interpolation point' );
            { Draw caption of second interpolation point as 'x2, y2 - interpolation point' }
            PB.TextShow( cx(92) - PB.GetTextWidth( 'x2, y2 - interpolation point' )/2, cy(71),
                0, 'x2, y2 - interpolation point' );
            { Draw caption of point in finish of Bezier Curve as 'x3, y3 - destination point' }
            PB.TextShow( cx(77) - PB.GetTextWidth( 'x3, y3 - destination point' )/2, cy(86),
                0, 'x3, y3 - destination point' );
            { Restore the graphics state by removing the most recently saved state
            from the stack and making it the current state.}
            PB.StateRestore( );


            { One more example of Cubic Bezier curves }
            PB.SetStrokeColor( RGBToColor( 0.0, 0.0, 0.0 ) );
            PB.SetLineWidth( 1 );
            PB.NoDash( );
            PB.MoveTo( cx(10), cy(60) );
            PB.CurveTo( cx(20), cy(50), cx(40), cy(75), cx(45), cy(60) );
            PB.Stroke( );

            PB.SetStrokeColor( RGBToColor( 0.5, 0.5, 0.5 ) );
            PB.SetLineWidth( 0.25 );
            PB.SetDash( '[3 3] 0' );
            PB.MoveTo( cx(10), cy(60) );
            PB.LineTo( cx(20), cy(50) );
            PB.Stroke( );
            PB.MoveTo( cx(45), cy(60) );
            PB.LineTo( cx(40), cy(75) );
            PB.Stroke( );


            { One more example of Cubic Bezier curves }
            PB.SetStrokeColor( RGBToColor( 0.0, 0.0, 0.0 ) );
            PB.SetLineWidth( 1 );
            PB.NoDash( );
            PB.MoveTo( cx(10), cy(90) );
            PB.CurveTo( cx(35), cy(88), cx(30), cy(75), cx(45), cy(90) );
            PB.Stroke( );

            PB.SetStrokeColor( RGBToColor( 0.5, 0.5, 0.5 ) );
            PB.SetLineWidth( 0.25 );
            PB.SetDash( '[3 3] 0' );
            PB.MoveTo( cx(10), cy(90) );
            PB.LineTo( cx(35), cy(88) );
            PB.Stroke( );
            PB.MoveTo( cx(45), cy(90) );
            PB.LineTo( cx(30), cy(75) );
            PB.Stroke( );
            PB.Close;
            PageIndex := Doc.AppendPage ( PageWidth, PageHeight );
            Page := Doc.Pages[PageIndex];
            PB := Page.PaintBox;
            {**********************************************************************}
            { Alpha blending example.                                              }
            {**********************************************************************}
            { Save the current graphics state on the graphics state stack. }
            PB.StateStore( );
            { Set the text font as Helvetica, size is 3% of page height }
            PB.SetActiveFont( Doc.FontAppendStandard ( vsfHelvetica, vetWinAnsiEncoding  ),
                cy(3), false, false );
            { Text Color set a black }
            PB.SetFillColor( RGBToColor( 0.0, 0.0, 0.0 ) );
            { Draw title of current page }
            PB.TextShow( cx(50) - PB.GetTextWidth( 'Alpha blending' )/2, cy(4),
                0, 'Alpha blending' );
            GState  := TPDFExtGraphicState.Create ( Doc ); { Create Graphic state }
            GState.AlphaFill := 0.5 ;              { Set fill alpha blending to 50 % }
            GState.AlphaStroke := 0.5;            { Set lines alpha blending to 50 % }
            GState.AlphaIsShape := true;          { Set alpha blending to shapes }
            PB.SetGState ( GState );                               { Set graphic state as current }
            PB.SetColor( RGBToColor( 0.0, 0.0, 0.0 ) );     { Set line color as black }
            PB.SetFillColor( RGBToColor( 1.0, 1.0, 0.0 ) ); { Set fill color as yellow  }
            PB.Rectangle( cx(10), cy(10), cx(60), cy(60) );   { Draw rectangle }
            PB.FillAndStroke( );                               { Close and fill rectangle }
            PB.SetColor( RGBToColor( 0.0, 0.0, 0.0 ) );     { Set line color as black  }
            PB.SetFillColor( RGBToColor( 1.0, 0.0, 0.0 ) ); { Set fill color as red }
            PB.Rectangle( cx(20), cy(20), cx(70), cy(70) );   { Draw rectangle }
            PB.FillAndStroke( );                               { Close and fill rectangle }
            PB.SetColor( RGBToColor( 0.0, 0.0, 0.0 ) );     { Set line color as black  }
            PB.SetFillColor( RGBToColor( 0.0, 0.0, 1.0 ) ); { Set fill color as blue  }
            PB.Rectangle( cx(30), cy(30), cx(80), cy(80) );   { Draw rectangle }
            PB.FillAndStroke( );                               { Close and fill rectangle }
            PB.SetColor( RGBToColor( 0.0, 0.0, 0.0 ) );     { Set line color as black  }
            PB.SetFillColor( RGBToColor( 0.0, 1.0, 0.0 ) ); { Set fill color as green  }
            PB.Rectangle( cx(40), cy(40), cx(90), cy(90) );   { Draw rectangle }
            PB.FillAndStroke( );                               { Close and fill rectangle }
            Doc.SaveToFile( '../../../data/Out/PDF/DrawFigures.pdf' );
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
