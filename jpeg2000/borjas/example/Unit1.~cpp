//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"

#include "jasper.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{
  DoubleBuffered = TRUE;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::BitBtn1Click(TObject *Sender)
{
  BitBtn1->Enabled = FALSE;
  Screen->Cursor = crHourGlass;
  Label1->Caption = "";
  Application->ProcessMessages();

  if (OpenDialog1->Execute() == TRUE)
    if (ReadJPEG2000Image(OpenDialog1->FileName.c_str(), Image1->Picture->Bitmap, NULL) == -1)
      Label1->Caption = "Error Reading";

  Screen->Cursor = crDefault;
  BitBtn1->Enabled = TRUE;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::BitBtn2Click(TObject *Sender)
{
  BitBtn1->Enabled = FALSE;
  Screen->Cursor = crHourGlass;
  Label1->Caption = "";  
  Application->ProcessMessages();  

  if (SaveDialog1->Execute() == TRUE)
    if (WriteJPEG2000Image(Image1->Picture->Bitmap,
                           SaveDialog1->FileName.c_str(), NULL) == -1)
      Label1->Caption = "Error Writing";

  Screen->Cursor = crDefault;
  BitBtn1->Enabled = TRUE;
}
//---------------------------------------------------------------------------

