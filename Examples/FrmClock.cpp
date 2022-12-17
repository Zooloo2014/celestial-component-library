//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "FrmClock.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "DigiClock"
#pragma link "Lcd"
#pragma link "CSPIN"
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

void __fastcall TForm1::Timer1Timer(TObject *Sender)
{
  DigiClock1->Update();        
}
//---------------------------------------------------------------------------
void __fastcall TForm1::CheckBox1Click(TObject *Sender)
{
  DigiClock1->International = CheckBox1->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::CheckBox2Click(TObject *Sender)
{
  if (CheckBox3->Checked) {
    DigiClock1->CountDownHours = CSpinEdit1->Value;
    DigiClock1->CountDownMinutes = CSpinEdit2->Value;
    DigiClock1->CountDownSeconds = CSpinEdit3->Value;
    DigiClock1->CountDown = true;
  } else {
    DigiClock1->CountDown = false;
  }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::CheckBox3Click(TObject *Sender)
{
  if (CheckBox3->Checked) {
    DigiClock1->CountDownHours = CSpinEdit1->Value;
    DigiClock1->CountDownMinutes = CSpinEdit2->Value;
    DigiClock1->CountDownSeconds = CSpinEdit3->Value;
    DigiClock1->CountDown = true;
  } else {
    DigiClock1->CountDown = false;
  }      
}
//---------------------------------------------------------------------------

void __fastcall TForm1::CheckBox4Click(TObject *Sender)
{
  DigiClock1->International = CheckBox1->Checked;
}
//---------------------------------------------------------------------------

