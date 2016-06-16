program PagesAPI;

{$APPTYPE CONSOLE}

uses
  SysUtils,
  VersyPDF,
  VersyPDFTypes;

var
    Lib : TPDFLibrary;
    SourceDocument, DestinationDocument: TPDFDocument;
    Connector: TPDFPagesConnector;
    I:Integer;

begin
  try
    Lib := TPDFLibrary.Create ('','');
    try
    { Create new destination PDF file }
        DestinationDocument := TPDFDocument.Create( Lib );
        try
            DestinationDocument.AutoLaunch := true;   { Launch destination PDF file after work }
            SourceDocument := TPDFDocument.Create ( Lib );{ Load existing source PDF file }
            try
                SourceDocument.LoadFromFile( '../../../data/In/PDF/island.pdf');
            { Initialize source and destination documents connection }
                Connector := TPDFPagesConnector.Create( SourceDocument, DestinationDocument );
                try
                { Take each page from source PDF document, by increase page number and select it for copy }
                for i := 0 to  SourceDocument.PageCount -1 do
                    Connector.AppendPage( i );
                { Copy all selected in connection pages from source to destination PDF Document }
                Connector.Execute;
                { Take each page from source PDF document, by decrease page number and select it for copy }
                for i := SourceDocument.PageCount downto 0 do
                    Connector.AppendPage ( i - 1 );
                { Copy all selected in connection pages from source to destination PDF Document }
                Connector.Execute;
                finally
                    Connector.Free;
                end;
            finally
                SourceDocument.Free;
            end;

            SourceDocument := TPDFDocument.Create ( Lib );{ Load existing source PDF file }
            try
                SourceDocument.LoadFromFile( '../../../data/In/PDF/Romeo And Juliet.pdf');
            { Initialize source and destination documents connection }
                Connector := TPDFPagesConnector.Create( SourceDocument, DestinationDocument );
                try
                { Take each page from source PDF document, by increase page number and select it for copy }
                for i := 0 to  SourceDocument.PageCount -1 do
                    Connector.AppendPage( i );
                { Copy all selected in connection pages from source to destination PDF Document }
                Connector.Execute;
                { Take each page from source PDF document, by decrease page number and select it for copy }
                for i := SourceDocument.PageCount downto 0 do 
                    Connector.AppendPage ( i - 1 );
                { Copy all selected in connection pages from source to destination PDF Document }
                Connector.Execute;
                finally
                    Connector.Free;
                end;
            finally
                SourceDocument.Free;
            end;
            { Save destination PDF file as 'pages-api-test.pdf' in current path }
            DestinationDocument.SaveToFile( '../../../data/Out/PDF/PagesAPITest.pdf' );
        finally
            DestinationDocument.Free;
        end;
    finally
        Lib.Free;
    end;
  except
    on e: Exception do
      Writeln('Error:',e.message);
  end;
end.
