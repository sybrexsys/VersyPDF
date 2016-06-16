program AcroFormInfo;

{$APPTYPE CONSOLE}

uses
  SysUtils, VersyPDF, VersyPDFTypes, Classes;


var
    Doc:TPDFDocument;
    Lib:TPDFLibrary;
    Info: TPDFAcroInformation;
    Count, I : Integer;
    SettingName, FieldValue, FieldName:string;
    FS: TFileStream;
    FN, FV:PDFString;
begin
  try
    FS := TFileStream.Create('../../../data/in/PDF/AcroFormAPITest.pdf', fmOpenRead	);
    try
        Lib := TPDFLibrary.Create('','');
        try
            { Open exists document}
            Doc := TPDFDocument.Create ( Lib );
            try
                Doc.LoadFromStream( FS );
                Doc.AutoLaunch := True;
                Info := Doc.AcroInformation;
                Count := Info.Count; { Receive count of the acroform objects }
                for  i := 0 to Count -1 do
                begin
                    FN := Info.GetNameByIndex( i ); { Receive name of the acroform object }
                    FV := Info.GetValueByIndex ( i ); { Receive value of acroform object }
                    if ( FV = nil) then
                        FieldValue := '<empty>'
                    else
                      if PDFStringIsUnicode(FV) then
                        FieldValue := PDFStringToUnicode( FV )
                      else
                        FieldValue := PDFStringToString( FV );
                    if PDFStringIsUnicode( FN ) then
                      FieldName := PDFStringToUnicode( FN )
                    else
                      FieldName := PDFStringToString( FN );
                    Writeln('Field with name ', FieldName,' have value ', FieldValue ,' yet.');
                end;
                SettingName := 'cp';
                I := Info.GetIndexByName(SettingName, false);
                Info.SetValueByIndex(I,'Ted Thompson');
                FV := Info.GetValueByIndex(i);
                if ( FV = nil) then
                        FieldValue := '<empty>'
                    else
                      if PDFStringIsUnicode(FV) then
                        FieldValue := PDFStringToUnicode( FV )
                      else
                        FieldValue := PDFStringToString( FV );
                Writeln('Field with name ', SettingName,' have value ', FieldValue ,' now.');

                SettingName := 'cn';
                I := Info.GetIndexByName(SettingName, false);
                Info.SetValueByIndex(I,'Thallium Inc.');
                FV := Info.GetValueByIndex(i);
                if ( FV = nil) then
                        FieldValue := '<empty>'
                    else
                      if PDFStringIsUnicode(FV) then
                        FieldValue := PDFStringToUnicode( FV )
                      else
                        FieldValue := PDFStringToString( FV );
                Writeln('Field with name ', SettingName,' have value ', FieldValue ,' now.');


                SettingName := 'dp';
                I := Info.GetIndexByName(SettingName, false);
                Info.SetValueByIndex(i,'202 Software Department');
                FV := Info.GetValueByIndex(i);
                if ( FV = nil) then
                        FieldValue := '<empty>'
                    else
                      if PDFStringIsUnicode(FV) then
                        FieldValue := PDFStringToUnicode( FV )
                      else
                        FieldValue := PDFStringToString( FV );
                Writeln('Field with name ', SettingName,' have value ', FieldValue ,' now.');

                Doc.SaveToFile( '../../../data/out/pdf/AcroModified.pdf');
            finally
                Doc.Free;
            end;
        finally
            Lib.Free;
        end;
    finally
        FS.Free;
    end;
  except
    on e: Exception do
      Writeln('Error:',e.message);
  end;
end.
