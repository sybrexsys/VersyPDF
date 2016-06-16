program Security;

{$APPTYPE CONSOLE}

uses
  SysUtils, VersyPDF, VersyPDFTypes;
var
 Lib: TPDFLibrary;
 Doc: TPDFDocument;
 PassFound: Boolean;

begin
  try
    Lib := TPDFLibrary.Create('','');
    try
        { Open exists uncrypted document }
        Doc := TPDFDocument.Create( Lib );
        try
            Doc.LoadFromFile('../../../data/in/pdf/island.pdf');
            Doc.AutoLaunch := true;
            { Set security on pdf-file with permissions to print document, 128-bits crypt key and passwords :
            user - "123", owner - "12345" }
            Doc.SetSecurity([vdrPrint,vdrPrintHiResolution],vptIs128BitProtection,'123', '12345');
            Doc.SaveToFile( '../../../data/out/pdf/crypted.pdf');
        finally
            Doc.Free;
        end;
        { Open exists crypted document }
        Doc := TPDFDocument.Create( Lib );
        try
            Doc.LoadFromFile('../../../data/out/pdf/crypted.pdf');
   			{ Checking to crypted file }
			if ( Doc.IsCrypted ) then
            begin
				{ Check to Password "1" }
				if ( Doc.CheckPassword('1')<>vkvInvalidPassword ) then
                begin
					Writeln(' Valid Password is : 1 ');
                    PassFound := True;
				end else
                begin
					{ Check to Password "12" }
					if ( Doc.CheckPassword('12')<>vkvInvalidPassword) then
					begin
						Writeln(' Valid Password is : 12 ');
                        PassFound := True;
					end	else
					begin
						{ Check to Password "123" }
						if ( Doc.CheckPassword('123')<>vkvInvalidPassword) then
						begin
							Writeln(' Valid Password is : 123 ');
                            PassFound := True;
						end	else
						begin
							Writeln (' Valid Password not was found ');
                            PassFound := False;
						end
					end
				end
			end else
            begin
                PassFound := True;
            end;
            if PassFound then
            begin
                Doc.AutoLaunch := true;
                Doc.RemoveSecurity;
                Doc.SaveToFile( '../../../data/out/pdf/uncrypted.pdf');
            end;
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
