//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "FrmHostInformation.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormHostInformation *FormHostInformation;
//---------------------------------------------------------------------------
__fastcall TFormHostInformation::TFormHostInformation(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TFormHostInformation::BtnCloseClick(TObject *Sender)
{
  ModalResult = mrOk;        
}
//---------------------------------------------------------------------------

