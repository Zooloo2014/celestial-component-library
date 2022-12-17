//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "FrmHostList.h"
#include "FrmRemoteHostInfo.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormHostList *FormHostList;
//---------------------------------------------------------------------------
__fastcall TFormHostList::TFormHostList(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormHostList::BtnCancelClick(TObject *Sender)
{
  ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TFormHostList::BtnApplyClick(TObject *Sender)
{
  ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TFormHostList::CheckListBoxMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
  if (Button == mbLeft)
    LastClick = CheckListBox->ItemAtPos(TPoint(X, Y), true);
}
//---------------------------------------------------------------------------
void __fastcall TFormHostList::CheckListBoxClickCheck(TObject *Sender)
{
  if (LastClick != -1) {
    for (int i = 0; i < CheckListBox->Items->Count; i++) {
      CheckListBox->Checked[i] = false;
      if (LastClick == i)
        CheckListBox->Checked[i] = true;
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormHostList::SetMyHostList(THostList *src, TRemoteHostInfoPtr hl, int Count)
{
  Source = src;
  MyHostList = hl;
  HostCount = Count;
}
//---------------------------------------------------------------------------
void __fastcall TFormHostList::BtnEditClick(TObject *Sender)
{
  TFormRemoteHostInfo *frm = new TFormRemoteHostInfo(this);
  AnsiString *host = new AnsiString();

  if (CheckListBox->ItemIndex > -1) {
    *host = CheckListBox->Items->Strings[CheckListBox->ItemIndex];

    TRemoteHostInfo *ptr;
    for (int i = 0; i < HostCount; i++) {
      ptr = MyHostList[i];
      if (ptr->HostName == *host) {
        frm->SetMyHost(ptr);
        frm->ShowModal();
        break;
      }
    }
  }

  delete host;
  delete frm;
}
//---------------------------------------------------------------------------
void __fastcall TFormHostList::FormShow(TObject *Sender)
{
  BtnEdit->Enabled = (CheckListBox->Items->Count > 0);
  BtnRemove->Enabled = BtnEdit->Enabled;
}
//---------------------------------------------------------------------------
void __fastcall TFormHostList::BtnAddClick(TObject *Sender)
{
  AnsiString *nh = new AnsiString();

  *nh = InputBox("Add a host", "Hostname to add?", "");

  if (!Source->IsInList(*nh)) {
    Source->Add(*nh);
    CheckListBox->Items->Add(*nh);
    HostCount++;
  }

  delete nh;
}
//---------------------------------------------------------------------------
void __fastcall TFormHostList::BtnRemoveClick(TObject *Sender)
{
  if (CheckListBox->ItemIndex != -1) {
    Source->Delete(CheckListBox->Items->Strings[CheckListBox->ItemIndex]);
    CheckListBox->Items->Delete(CheckListBox->ItemIndex);
    HostCount--;
  }
}
//---------------------------------------------------------------------------

