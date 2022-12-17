//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "FrmMatrix.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "DotMatrix"
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button1Click(TObject *Sender)
{
  Close();        
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormCreate(TObject *Sender)
{
  DotMatrix1->Text = Edit1->Text;        
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Edit1KeyUp(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
  if (Key == 0x0d)
    DotMatrix1->Text = Edit1->Text;        
}
//---------------------------------------------------------------------------

void __fastcall TForm1::CheckBox1Click(TObject *Sender)
{
  Timer1->Enabled = CheckBox1->Checked;        
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Timer1Timer(TObject *Sender)
{
  DotMatrix1->Scroll();        
}
//---------------------------------------------------------------------------

