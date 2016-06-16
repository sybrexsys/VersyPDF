program Images;

{$APPTYPE CONSOLE}

uses
  SysUtils, VersyPDF, VersyPDFTypes, Graphics;

var
    Lib:TPDFLibrary;
    Doc:TPDFDocument;
    PB:TPDFPaintBox;
    i,j:Integer;
    MF:TMetafile;
    Img : TPDFImage;

begin
  try
    Lib := TPDFLibrary.Create( '','' );
    try
        Doc := TPDFDocument.Create(Lib);
        try
   			{ Load jpeg image and append to PDF document with jpeg compression }
            Img := TPDFImage.CreateFromFile(Lib, '../../../data/In/Images/snow.jpg',0);
            try
    			i := Doc.ImageAppend( Img,victJPEG );
            finally
                Img.Free;
            end;
			{ Load jpeg image and append to PDF document with flate compression }
			j := Doc.ImageAppend( '../../../data/In/Images/snowtrees.jpg',0, victFlate );
            PB := Doc.Pages[Doc.AppendPage ( vpsA4, vpoPortrait )].PaintBox;
   			PB.ShowImage(i, 100, 100, 350, 600, 0 );
            PB := Doc.Pages[Doc.AppendPage ( vpsA4, vpoPortrait )].PaintBox;
   			PB.ShowImage(j, 100, 100, 350, 400, 0 );
			Doc.SaveToFile( '../../../data/Out/PDF/JpegFiles.pdf');
        finally
            Doc.Free;
        end;

        Doc := TPDFDocument.Create(Lib);
        try
   			{ Load black/white image from tiff file and append to PDF document with flate compression }
			i := Doc.ImageAppend( '../../../data/In/Images/islandsbw.tif',0,victFlate  );
            PB := Doc.Pages[Doc.AppendPage ( vpsA4, vpoPortrait )].PaintBox;
   			PB.ShowImage(i, 0, 0, 590, 840, 0 );
 			Doc.SaveToFile( '../../../data/Out/PDF/BW(Flate).pdf');
        finally
            Doc.Free;
        end;

        Doc := TPDFDocument.Create(Lib);
        try
   			{ Load black/white image from tiff file and append to PDF document with CCITT compression }
			i := Doc.ImageAppend( '../../../data/In/Images/islandsbw.tif',0,victCCITT42D  );
            PB := Doc.Pages[Doc.AppendPage ( vpsA4, vpoPortrait )].PaintBox;
            PB.ShowImage(i, 0, 0, 590, 840, 0 );
			Doc.SaveToFile( '../../../data/Out/PDF/BW(CCITT).pdf');
        finally
            Doc.Free;
        end;

        Doc := TPDFDocument.Create(Lib);
        try
   			{ Load black/white image from tiff file and append to PDF document with JBIG2 compression }
			i := Doc.ImageAppend( '../../../data/In/Images/islandsbw.tif',0,victJBIG2  );
            PB := Doc.Pages[Doc.AppendPage ( vpsA4, vpoPortrait )].PaintBox;
            PB.ShowImage(i, 0, 0, 590, 840, 0 );
			Doc.SaveToFile( '../../../data/Out/PDF/BW(JBIG2).pdf');
        finally
            Doc.Free;
        end;

        Doc := TPDFDocument.Create(Lib);
        try
            MF := TMetafile.Create;
            try
                MF.LoadFromFile('../../../data/in/Images/page1.emf');
                Doc.Resolution := 96;
                PB := Doc.Pages[Doc.AppendPage ( vpsA4, vpoPortrait )].PaintBox;
                PB.PlayMetaFile(MF, 10,20,1,1 );
                MF.LoadFromFile('../../../data/in/Images/page2.emf');
                PB := Doc.Pages[Doc.AppendPage ( vpsA4, vpoPortrait )].PaintBox;
                PB.PlayMetaFile(MF, 0,0,1,1);
                MF.LoadFromFile('../../../data/in/Images/page3.emf');
                PB := Doc.Pages[Doc.AppendPage ( vpsA4, vpoPortrait )].PaintBox;
                PB.PlayMetaFile(MF, 10,20,1,1 );
                MF.LoadFromFile('../../../data/in/Images/page4.emf');
                PB := Doc.Pages[Doc.AppendPage ( vpsA4, vpoPortrait )].PaintBox;
                PB.PlayMetaFile(MF, 10,20,0.6,1);
            finally
                MF.Free;
            end;
            Doc.SaveToFile( '../../../data/Out/PDF/EMFImages.pdf');
        finally
            Doc.Free;
        end;
    finally
        Lib.Free;
    end;
  except
    on e: exception do
      Writeln('Error:',e.message);
  end;
    writeln('Exit');
end.
