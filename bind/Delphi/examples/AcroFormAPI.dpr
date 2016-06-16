program AcroFormAPI;

{$APPTYPE CONSOLE}

uses
  SysUtils, VersyPDF, VersyPDFTypes;

const
    PageWidth = 1000;
	PageHeight = 500;
function PX( X: Double  ):Double;
begin
    Result := PageWidth*X/100;
end;
function PY( Y: Double  ):Double;
begin
    Result := PageHeight*Y/100;
end;
function OX( X: Double  ):Double;
begin
    Result := PageWidth*X/100;
end;
function OY( Y: Double  ):Double;
begin
    Result := PageHeight*(100 - Y ) /100;
end;

function GetPDFRect ( xl, yl, xr, yr: Double ): TPDFRect;
begin
    Result.Left := xl;
    Result.Bottom := yl;
    Result.Right := xr;
    Result.Top := yr;
end;

var
    Doc:TPDFDocument;
    Lib:TPDFLibrary;
    Page: TPDFPage;
    PB:TPDFPaintBox;
    PageIndex: Integer;
    fi:Integer;
    AL: TPDFAnnotationLink;
    URL: TPDFActionURL;
    wrk:String;
    A:TPDFAction;
begin
  try
    Lib := TPDFLibrary.Create( '','' );
    try
        Doc := TPDFDocument.Create(Lib);
        try
            Doc.AutoLaunch := True;
			PageIndex := Doc.AppendPage ( PageWidth, PageHeight );
			Page :=Doc.Pages[ PageIndex ];
            PB := Page.PaintBox;
            PB.StateStore( );
            { Set the line 1 point width. }
            PB.SetLineWidth( 1 );
            { Set the line cap style in the graphics state. }
            PB.SetLineCap( vlcRound );
            { Set the line join style in the graphics state. }
            PB.SetLineJoin( vljRound );
            { Set the filling Color space to DeviceRGB and Color as White to use for filling operations }
            PB.SetFillColor( RGBToColor( 0.98, 0.95, 0.92 ) );
            { Append a rectangle to the current path as all active area. }
            PB.Rectangle( 0, 0, PX(100), PY(100) );
            { Fill the path, using the nonzero winding number rule to determine the region to fill.
            Any subpaths that are open are implicitly closed before being filled.}
            PB.Fill( );
            { Set the stroking Color as Dark Maroon }
            PB.SetStrokeColor( RGBToColor( 0.3, 0.0, 0.0 ) );
            { Set Rectangle Border around whole area }
            PB.Rectangle( 0, 0, PX(100), PY(100) );
            { Draw Rectangle Border }
            PB.Stroke( );
            { Set the stroking Color as Light Maroon }
            PB.SetStrokeColor( RGBToColor( 0.6, 0.0, 0.0 ) );
            { Set Rectangle Border in previous Rectangle }
            PB.Rectangle( 1, 1, PX(100) - 1, PY(100) - 1 );
            { Draw Rectangle Border }
            PB.Stroke( );
            { Set the stroking Color as Red }
            PB.SetStrokeColor( RGBToColor( 1.0, 0.0, 0.0 ) );
            { Set Rectangle Border in previous Rectangle }
            PB.Rectangle( 2, 2, PX(100) - 2, PY(100) - 2 );
            { Draw Rectangle Border }
            PB.Stroke( );

            fi := Doc.FontAppendStandard( vsfHelvetica, vetWinAnsiEncoding );
            PB.SetActiveFont( fi, PY(8), false, false );
            PB.SetFillColor( RGBToColor( 0.3, 0.0, 0.0 ) );
            PB.TextShow( PX(02), PY(02), 0, 'Intention Form' );
            { Set filling color as Dark Maroon }
            PB.SetFillColor( RGBToColor( 0.3, 0.0, 0.0 ) );
            { Set narrow rectangle under text caption }
            PB.Rectangle( PX(02), PY(10) + 1, PX(98), PY(10) + 2 );
            { Fill rectangle by Dark Maroon Color }
            PB.Fill( );

            PB.SetActiveFont( fi, PY(2), false, false );
            PB.SetFillColor( RGBToColor( 0.0, 0.5, 0.0 ) );
            PB.TextShow( PX(75) , PY(5), 0, 'Thallium Software Group (c) 2005' );
            URL := TPDFActionURL.Create( Doc, 'http://www.thallium.com', False );
            try
                TPDFAnnotationLink.Create(Page, GetPDFRect(  OX(75), OY(5)-PY(2), OX(75)+PB.GetTextWidth('Thallium Software Group (c) 2005') , OY(5) ),
                URl,vhlmNoHightLight, False ).Free;
            finally
                URL.Free
            end;
            fi := Doc.FontAppendStandard( vsfCourier, vetWinAnsiEncoding );
            PB.SetActiveFont( fi, PY(2.5), false, false );
            PB.SetFillColor( RGBToColor( 0.3, 0.2, 0.0 ) );
            PB.TextShow( PX(02), PY(12), 0, 'Contact Person' );
            PB.TextShow( PX(02), PY(22), 0, 'Company Name' );
            PB.TextShow( PX(02), PY(32), 0, 'Department' );
            PB.TextShow( PX(21), PY(32), 0, 'Phone' );
            PB.TextShow( PX(41), PY(32), 0, 'E-Mail' );
            PB.TextShow( PX(2), PY(42), 0, 'Street Address' );
            PB.TextShow( PX(2), PY(62), 0, 'City' );
            PB.TextShow( PX(31), PY(62), 0, 'State' );
            PB.TextShow( PX(41), PY(62), 0, 'ZIP' );
            PB.TextShow( PX(2), PY(71), 0, 'Which purposes are you pursuing?' );

            fi := Doc.FontAppendStandard( vsfHelvetica, vetWinAnsiEncoding );
            PB.SetActiveFont( fi, PY(3), false, false );
            PB.SetFillColor( RGBToColor( 0.8, 0.1, 0.1  ) );
            PB.TextShow( PX(4), PY(91.5), 0, 'I wanna to subscribe news about yours products' );
            PB.TextShow( PX(4), PY(95.5), 0, 'I wanna to contact with yours managers' );

            PB.SetActiveFont( fi, PY(5), false, false );
            PB.SetFillColor( RGBToColor(  0.3, 0.0, 0.0  ) );
            PB.TextShow( PX(62), PY(21), 0, 'Which library pack' );
            PB.TextShow( PX(62), PY(25), 0, 'are you interested?' );
            {**********************************************************************}
            {  Draw Contact person edit box                                        }
            {**********************************************************************}
            with  TPDFAcroEditBox.Create ( Page, GetPDFRect( OX(02), OY(15), OX(60), OY(20) ), 'cp' ) do
            begin
                SetBorder ( RGBToColor( 0.0, 0.3, 0.0 ), RGBToColor( 0.95, 1.0, 0.9 ), 2.0 );
                MaxLen := 40;
                Align := vaqLeft;
                SetFont( Doc.FontAppendStandard( vsfHelvetica, vetWinAnsiEncoding ),
                    PY(3), RGBToColor( 0.0, 0.4, 0.0 ) );
                Free;
            end;

            {**********************************************************************}
            {  Draw Company name                                                   }
            {**********************************************************************}
            with TPDFAcroEditBox.Create ( Page, GetPDFRect( OX(02), OY(25), OX(60), OY(30) ), 'cn' ) do
            begin
                SetBorder ( RGBToColor( 0.0, 0.3, 0.0 ), RGBToColor( 0.95, 1.0, 0.9 ), 2.0 );
                Align := vaqLeft;
                SetFont(  Doc.FontAppendStandard( vsfHelvetica, vetWinAnsiEncoding ),
                    PY(3), RGBToColor( 0.0, 0.4, 0.0 ) );
                Free;
            end;


            {**********************************************************************}
            {  Draw phone edit box                                      }
            {**********************************************************************}
            with TPDFAcroEditBox.Create (Page, GetPDFRect( OX(21), OY(35), OX(40), OY(40) ), 'ph' ) do
            begin
                SetBorder (  RGBToColor( 0.0, 0.3, 0.0 ), RGBToColor( 0.95, 1.0, 0.9 ), 2.0 );
                Align := vaqLeft ;
                SetFont(  Doc.FontAppendStandard( vsfHelvetica, vetWinAnsiEncoding ),
                    PY(3), RGBToColor( 0.0, 0.4, 0.0 ) );
                Free;
            end;

            {**********************************************************************}
            {  Draw phone edit box                                      }
            {**********************************************************************}
            with TPDFAcroEditBox.Create (Page, GetPDFRect( OX(41), OY(35), OX(60), OY(40) ), 'em' ) do
            begin
                SetBorder (  RGBToColor( 0.0, 0.3, 0.0 ), RGBToColor( 0.95, 1.0, 0.9 ), 2.0 );
                Align := vaqLeft ;
                SetFont(  Doc.FontAppendStandard( vsfHelvetica, vetWinAnsiEncoding ),
                    PY(3), RGBToColor( 0.0, 0.4, 0.0 ) );
                Free;
            end;


            {**********************************************************************}
            {  Draw address edit box                                               }
            {**********************************************************************}
            with TPDFAcroEditBox.Create (Page, GetPDFRect( OX(2), OY(45), OX(60), OY(60) ), 'ad' ) do
            begin
                SetBorder (  RGBToColor( 0.0, 0.3, 0.0 ), RGBToColor( 0.95, 1.0, 0.9 ), 2.0 );
                Flag := [vaofMultiLine, vaofDoNotSpellCheck];
                Align := vaqLeft;
                SetFont(  Doc.FontAppendStandard( vsfHelvetica, vetWinAnsiEncoding ),
                    PY(3), RGBToColor( 0.0, 0.4, 0.0 ) );
                Free;
            end;

            {**********************************************************************}
            {  Draw city    edit box                                               }
            {**********************************************************************}
            with TPDFAcroEditBox.Create (Page, GetPDFRect( OX(2), OY(65), OX(30), OY(70) ), 'ct' ) do
            begin
                SetBorder (  RGBToColor( 0.0, 0.3, 0.0 ), RGBToColor( 0.95, 1.0, 0.9 ), 2.0 );
                Align := vaqLeft;
                SetFont(  Doc.FontAppendStandard( vsfHelvetica, vetWinAnsiEncoding ),
                    PY(3), RGBToColor( 0.0, 0.4, 0.0 ) );
                Free;
            end;

            {**********************************************************************}
            {  Draw zip edit box                                                   }
            {**********************************************************************}
            with  TPDFAcroEditBox.Create (Page, GetPDFRect( OX(40), OY(65), OX(60), OY(70) ), 'zp' ) do
            begin
                SetBorder (  RGBToColor( 0.0, 0.3, 0.0 ), RGBToColor( 0.95, 1.0, 0.9 ), 2.0 );
                Align := vaqLeft ;
                SetFont(  Doc.FontAppendStandard( vsfHelvetica, vetWinAnsiEncoding ),
                    PY(3), RGBToColor( 0.0, 0.4, 0.0 ) );
                Free;
            end;

            {**********************************************************************}
            {  Draw department combo box                                           }
            {**********************************************************************}
            with TPDFAcroComboBox.Create ( Page, GetPDFRect( OX(02), OY(35), OX(20), OY(40) ), 'dp' ) do
            begin
                SetBorder (  RGBToColor( 0.0, 0.3, 0.0 ), RGBToColor( 0.95, 1.0, 0.9 ), 2.0 );
                SetFont(  Doc.FontAppendStandard( vsfHelvetica, vetWinAnsiEncoding ),
                    PY(2.5), RGBToColor( 0.0, 0.4, 0.0 ) );
                Caption :=  '< select department, please >' ;
                AppendItem (  '100 Management' );
                AppendItem (  '101 Administration' );
                AppendItem (  '102 Accounting');
                AppendItem (  '103 Information Department' );
                AppendItem (  '104 Technology Department' );
                AppendItem (  '105 Personal Management' );
                AppendItem (  '110 Customer Service' );
                AppendItem (  '120 External Relations' );
                AppendItem (  '121 Analog Proofs' );
                AppendItem (  '150 System Analytic Department' );
                AppendItem (  '200 Research Department' );
                AppendItem (  '201 Development' );
                AppendItem (  '202 Software Department' );
                AppendItem (  '999 Other' );
                Free;
            end;


            {**********************************************************************}
            {  Draw state combo box                                           }
            {**********************************************************************}
            with  TPDFAcroComboBox.Create ( Page, GetPDFRect( OX(31), OY(65), OX(39), OY(70) ), 'st' ) do
            begin
                SetBorder (  RGBToColor( 0.0, 0.3, 0.0 ), RGBToColor( 0.95, 1.0, 0.9 ), 2.0 );
                SetFont(  Doc.FontAppendStandard( vsfHelvetica, vetWinAnsiEncoding ),
                    PY(2.5), RGBToColor( 0.0, 0.4, 0.0 ) );
                Caption :=  '<select>';
                AppendItem (  'AL' );
                AppendItem (  'AR' );
                AppendItem (  'AZ');
                AppendItem (  'CA' );
                AppendItem (  'CO' );
                AppendItem (  'HA' );
                AppendItem (  'KE' );
                AppendItem (  'MA' );
                AppendItem (  'NE' );
                AppendItem (  'NY' );
                AppendItem (  'OH' );
                AppendItem (  'OL' );
                AppendItem (  'OR' );
                AppendItem (  'PE' );
                AppendItem (  'TX' );
                Free;
            end;



            with  TPDFAcroListBox.Create ( Page, GetPDFRect( OX(2), OY(90), OX(60), OY(75) ), 'pu' ) do
            begin
                SetBorder (  RGBToColor( 0.0, 0.3, 0.0 ), RGBToColor( 0.95, 1.0, 0.9 ), 2.0 );
                Flag := [vaofMultiSelect] ;
                SetFont(  Doc.FontAppendStandard( vsfHelvetica, vetWinAnsiEncoding ),
                    PY(2.5), RGBToColor( 0.0, 0.4, 0.0 ) );
                AppendItem (  'I wanna buy one copy for me' );
                AppendItem (  'I wanna buy library for my company' );
                AppendItem (  'I wanna to be reseller in own region');
                AppendItem (  'I wanna testing yours library' );
                AppendItem (  'I looking for similar product in internet' );
                AppendItem (  'I have some questions about yours PDF products' );
                Free;
            end;
            with TPDFAcroCheckBox.Create ( Page, GetPDFRect( OX(2), OY(94), OX(2) + PY(3), OY(91) ), 'sb', false ) do
            begin
                SetBorder (  RGBToColor( 0.0, 0.3, 0.0 ), RGBToColor( 0.95, 1.0, 0.9 ), 2.0 );
                SetFont(  0, 0, RGBToColor( 0.0, 0.4, 0.0 ) );
                SetStyle ( vcbsCheck, vcbstRectangle );
                Free;
            end;

            with TPDFAcroCheckBox.Create ( Page, GetPDFRect(  OX(2), OY(98), OX(2) + PY(3), OY(95)), 'cm', false ) do
            begin
                SetBorder (  RGBToColor( 0.0, 0.3, 0.0 ), RGBToColor( 0.95, 1.0, 0.9 ), 2.0 );
                SetFont(  0, 0, RGBToColor( 0.3, 0.0, 0.0 ) );
                SetStyle ( vcbsVmark, vcbstRectangle );
                Free;
            end;

            PB.SetActiveFont( fi, PY(2), false, false );
            PB.SetFillColor( RGBToColor(  0.0, 0.3, 0.0  ) );
           PB.TextShow( PX(90), PY(23), 0, 'Demo' );

           with TPDFAcroRadioButton.Create ( Page, GetPDFRect(  OX(87), OY(26), OX(87) + PY(04), OY(22) ), 'Demo', 'pd', true ) do
            begin
                SetBorder (  RGBToColor( 0.0, 0.3, 0.0 ), RGBToColor( 0.95, 1.0, 0.9 ), 2.0 );
                SetFont(  0,	0, RGBToColor( 0.3, 0.0, 0.0 ) );
                SetStyle ( vcbsDiamond , vcbstCircle );
                Free;
            end;

           PB.TextShow( PX(83), PY(33), 0, 'Home Edition' );

           with TPDFAcroRadioButton.Create (  Page, GetPDFRect(  OX(80), OY(36), OX(80) + PY(04), OY(32) ), 'Home', 'pd', false ) do
            begin
                SetBorder (  RGBToColor( 0.0, 0.3, 0.0 ), RGBToColor( 0.95, 1.0, 0.9 ), 2.0 );
                SetFont(  0,	0, RGBToColor( 0.3, 0.0, 0.0 ) );
                SetStyle ( vcbsDiamond , vcbstCircle );
                Free;
            end;

           PB.TextShow( PX(77), PY(43), 0, 'Standard Edition' );

           with TPDFAcroRadioButton.Create (  Page, GetPDFRect(  OX(74), OY(46), OX(74) + PY(04), OY(42) ), 'Standard', 'pd', false ) do
            begin
            SetBorder (  RGBToColor( 0.0, 0.3, 0.0 ), RGBToColor( 0.95, 1.0, 0.9 ), 2.0 );
            SetFont(  0,	0, RGBToColor( 0.3, 0.0, 0.0 ) );
            SetStyle ( vcbsDiamond , vcbstCircle );
                Free;
            end;

           PB.TextShow( PX(72), PY(53), 0, 'Professional Edition' );

           with TPDFAcroRadioButton.Create (  Page, GetPDFRect(  OX(69), OY(56), OX(69) + PY(04), OY(52) ), 'Professional', 'pd', false ) do
            begin
                SetBorder (  RGBToColor( 0.0, 0.3, 0.0 ), RGBToColor( 0.95, 1.0, 0.9 ), 2.0 );
                SetFont(  0,	0, RGBToColor( 0.3, 0.0, 0.0 ) );
                SetStyle ( vcbsDiamond , vcbstCircle );
                Free;
            end;

           PB.TextShow( PX(68), PY(63), 0, 'Enterprise Edition' );

           with  TPDFAcroRadioButton.Create ( Page, GetPDFRect(  OX(65), OY(66), OX(65) + PY(04), OY(62) ), 'Enterprise', 'pd', false ) do
            begin
               SetBorder (  RGBToColor( 0.0, 0.3, 0.0 ), RGBToColor( 0.95, 1.0, 0.9 ), 2.0 );
               SetFont(  0,	0, RGBToColor( 0.3, 0.0, 0.0 ) );
               SetStyle ( vcbsDiamond , vcbstCircle );
                Free;
            end;

           PB.TextShow( PX(65), PY(73), 0, 'Corporate Edition' );

           with TPDFAcroRadioButton.Create ( Page, GetPDFRect(  OX(62), OY(76), OX(62) + PY(04), OY(72) ), 'Corporate', 'pd', false ) do
            begin
               SetBorder (  RGBToColor( 0.0, 0.3, 0.0 ), RGBToColor( 0.95, 1.0, 0.9 ), 2.0 );
               SetFont(  0,	0, RGBToColor( 0.3, 0.0, 0.0 ) );
               SetStyle ( vcbsDiamond , vcbstCircle );
                Free;
            end;
           with TPDFAcroPushButton.Create ( Page, GetPDFRect( 0.9 * PageWidth, 10, PageWidth - 10, 10 + 0.05 * PageHeight ),
            'SubmitButton' ) do
            begin
            SetBorder (  NoColor(), RGBToColor( 0.95, 1.0, 0.9 ), 2.0 );
            Caption :=  'Submit Form';
            SetFont(  Doc.FontAppendStandard( vsfHelvetica, vetWinAnsiEncoding ),
                PY(2), RGBToColor( 0.0, 0.0, 0.0 ) );
            Miter := 0.006 * PageHeight;
            A:= TPDFActionSubmitForm.Create( Doc, '//LocalHost/PDF/fdfserv.php', [] );
            try
                AddAction ( A , vaetActivate );
            finally
                A.Free;
            end;
            Free;
            end;

           with TPDFAcroPushButton.Create ( Page, GetPDFRect( 0.8 * PageWidth, 10, 0.9 * PageWidth - 10, 10 + 0.05 * PageHeight ),
            'ExportButton' ) do
            begin
                SetBorder (  NoColor(), RGBToColor( 0.95, 1.0, 0.9 ), 2.0 );
                Caption :=  'Export Data' ;
                SetFont(  Doc.FontAppendStandard( vsfHelvetica, vetWinAnsiEncoding ),
                    PY(2), RGBToColor( 0.0, 0.0, 0.0 ) );
                Miter := 0.006 * PageHeight;
                A:= TPDFActionSubmitForm.Create( Doc, 'http://www.thallium.com/lib/pdf/fdfserv.php', [vsffExportFormat] );
                try
                    AddAction ( A , vaetActivate );
                finally
                    A.Free;
                end;
                Free;
            end;
            with TPDFAcroPushButton.Create ( Page, GetPDFRect( 0.7 * PageWidth, 10, 0.8 * PageWidth - 10, 10 + 0.05 * PageHeight ),
            'ResetButton' ) do
            begin
                SetBorder (  NoColor(), RGBToColor( 0.95, 1.0, 0.9 ), 2.0 );
                Caption :=  'Reset Form';
                SetFont(  Doc.FontAppendStandard( vsfHelvetica, vetWinAnsiEncoding ),
                    PY(2), RGBToColor( 0.0, 0.0, 0.0 ) );
                Miter := 0.006 * PageHeight;
                A:= TPDFActionResetForm.Create ( Doc, False );
                try
                    AddAction ( A, vaetActivate );
                finally
                    A.Free;
                end;
                Free;
            end;
            TPDFAcroSignatureBox.Create ( Page, GetPDFRect(  OX(80), OY(70), OX(80) + PY(14), OY(62) ),'SGN').Free;
            PB.StateRestore( );
            with TPDFAnnotationText.Create( Page,  GetPDFRect(  OX(0), OY(10), OX(30) , OY(0) ), false, vatnComment, 'Demo of Text Annotation' ) do
            begin
                SetTitle('Text');
                Color := ToRGB( 1,0,0 );
                Free;
            end;
            with  TPDFAnnotationStamp.Create ( Page,GetPDFRect(  OX(70), OY(90), OX(90) , OY(70) ), vasnForComment, 'Example' ) do
            begin
                Color := ToRGB( 0.8,0.8,0.8 );
                Free;
            end;
            PB.Close;
			Doc.SaveToFile( '../../../Data/Out/PDF/AcroFormAPI.pdf');
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
