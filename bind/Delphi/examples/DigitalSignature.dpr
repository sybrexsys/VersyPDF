program DigitalSignature;

{$APPTYPE CONSOLE}

uses
  SysUtils, VersyPDF, VersyPDFTypes, VersyCommon;
var
 Lib: TPDFLibrary;
 Doc: TPDFDocument;
{$l ../obj/x64/test.obj}
function Get:Pointer;register; external;
procedure Clear( P: Pointer ); register; external;
var
Z:Pointer;
begin
  Z   := Get;
  Clear(Z);
  try
    Lib := TPDFLibrary.Create('','');
    try
        { Open exists document}
        Doc := TPDFDocument.Create( Lib );
        try
            Doc.LoadFromFile( '../../../data/in/pdf/island.pdf' );
   			Doc.AutoLaunch := true;
			{ Append digital signature from PFX file }
            Doc.AppendSignature('../../../data/in/other/dig-signature.pfx', 'Document Signature', 'Only for test...', false, '123456' );
   			Doc.SaveToFile( '../../../data/out/pdf/signed.pdf');
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
