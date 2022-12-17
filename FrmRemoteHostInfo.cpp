//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "FrmRemoteHostInfo.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormRemoteHostInfo *FormRemoteHostInfo;
//---------------------------------------------------------------------------
__fastcall TFormRemoteHostInfo::TFormRemoteHostInfo(TComponent* Owner)
        : TForm(Owner)
{
  MyHost = NULL;
  MyAccounts = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TFormRemoteHostInfo::BtnApplyClick(TObject *Sender)
{
  if (MyHost != NULL) {
    MyHost->Port = StrToInt(EditPort->Text);
    MyHost->Service = EditService->Text;
  }

  ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TFormRemoteHostInfo::BtnCancelClick(TObject *Sender)
{
  ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TFormRemoteHostInfo::BtnEditClick(TObject *Sender)
{
  if (MyAccounts != NULL) {
    MyAccounts->ShowDialog();
    UpdateAccounts();
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormRemoteHostInfo::SetMyHost(TRemoteHostInfo *host)
{
  MyHost = host;

  if (MyHost != NULL) {
    LabelHost->Caption = host->HostName;
    EditPort->Text = host->Port;
    EditService->Text = host->Service;

    MyAccounts = MyHost->GetAccounts();
    if (MyAccounts != NULL)
      UpdateAccounts();
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormRemoteHostInfo::UpdateAccounts()
{
  if (MyAccounts != NULL) {
    LabelCount->Caption = "(" + Trim(IntToStr(MyAccounts->UserCount)) + ")";

    CheckListBox->Clear();
    for (int j = 0; j < MyAccounts->UserID->Count; j++)
      CheckListBox->Items->Add(MyAccounts->ExtractUserID(MyAccounts->UserID->Strings[j]));

    if (MyAccounts->PreferredAccount != "") {
      int i = CheckListBox->Items->IndexOf(MyAccounts->PreferredAccount);

      for (int j = 0; j < CheckListBox->Items->Count; j++)
        if (j == i)
          CheckListBox->Checked[j] = true;
        else
          CheckListBox->Checked[j] = false;
    }
  } else
    LabelCount->Caption = "(0)";
}
//---------------------------------------------------------------------------
void __fastcall TFormRemoteHostInfo::FormShow(TObject *Sender)
{
  UpdateAccounts();
}
//---------------------------------------------------------------------------
void __fastcall TFormRemoteHostInfo::CheckListBoxMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
  if (Button == mbLeft)
    LastClick = CheckListBox->ItemAtPos(TPoint(X, Y), true);
}
//---------------------------------------------------------------------------
void __fastcall TFormRemoteHostInfo::CheckListBoxClickCheck(
      TObject *Sender)
{
  if (LastClick != -1) {
    for (int i = 0; i < CheckListBox->Items->Count; i++) {
      CheckListBox->Checked[i] = false;
      if (LastClick == i) {
        CheckListBox->Checked[i] = true;
        if (MyAccounts != NULL)
          MyAccounts->PreferredAccount = CheckListBox->Items->Strings[i];
      }
    }
  }
}
//---------------------------------------------------------------------------

