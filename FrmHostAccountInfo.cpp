//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "FrmHostAccountInfo.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormHostAccountInfo *FormHostAccountInfo;
//---------------------------------------------------------------------------
__fastcall TFormHostAccountInfo::TFormHostAccountInfo(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormHostAccountInfo::FormCreate(TObject *Sender)
{
  StringGrid->RowCount = (StringGrid->Height / StringGrid->DefaultRowHeight) + 5;

  StringGrid->Cells[0][0] = "UserID";
  StringGrid->Cells[1][0] = "Password";
  StringGrid->Cells[2][0] = "Full Name";

  StringGrid->ColWidths[0] = (StringGrid->GridWidth * 0.25);
  StringGrid->ColWidths[1] = (StringGrid->GridWidth * 0.25);
  StringGrid->ColWidths[2] = (StringGrid->GridWidth * 0.50);

  SortedID = new TStringList();
  Pass = new TStringList();
  Pass->Sorted = false;
  FName = new TStringList();
  FName->Sorted = false;
}
//---------------------------------------------------------------------------
void __fastcall TFormHostAccountInfo::FormDestroy(TObject *Sender)
{
  delete SortedID;
  delete Pass;
  delete FName;
}
//---------------------------------------------------------------------------
void __fastcall TFormHostAccountInfo::BtnCancelClick(TObject *Sender)
{
  ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TFormHostAccountInfo::BtnOkClick(TObject *Sender)
{
  SortGridInfo();
  ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TFormHostAccountInfo::StringGridSetEditText(
      TObject *Sender, int ACol, int ARow, const AnsiString Value)
{
  int c = CountUsers();
  LabelUsrCnt->Caption = IntToStr(c);
  StringGrid->RowCount = (StringGrid->Height / StringGrid->DefaultRowHeight) + 5 + c;
}
//---------------------------------------------------------------------------
void __fastcall TFormHostAccountInfo::SortGridInfo()
{
  SortedID->Clear();
  Pass->Clear();
  FName->Clear();

  for (int i = 1; i <= StringGrid->RowCount; i++)
    if (StringGrid->Cells[0][i] != "") {
      int j = Pass->Add(StringGrid->Cells[1][i]);
      FName->Add(StringGrid->Cells[2][i]);
      SortedID->AddObject(StringGrid->Cells[0][i], (TObject *)j);
    }

  for (int i = 1; i <= StringGrid->RowCount; i++) {
    StringGrid->Cells[0][i] = "";
    StringGrid->Cells[1][i] = "";
    StringGrid->Cells[2][i] = "";
  }

  SortedID->Sort();
  for (int i = 0; i < SortedID->Count; i++) {
    StringGrid->Cells[0][i + 1] = SortedID->Strings[i];
    StringGrid->Cells[1][i + 1] = Pass->Strings[(int)SortedID->Objects[i]];
    StringGrid->Cells[2][i + 1] = FName->Strings[(int)SortedID->Objects[i]];
  }
}
//---------------------------------------------------------------------------
int __fastcall TFormHostAccountInfo::CountUsers()
{
  int uc = 0;

  for (int i = 1; i <= StringGrid->RowCount; i++)
    if (StringGrid->Cells[0][i] != "")
      uc++;

  return uc;
}
//---------------------------------------------------------------------------
void __fastcall TFormHostAccountInfo::FormShow(TObject *Sender)
{
  SortGridInfo();
  LabelUsrCnt->Caption = IntToStr(CountUsers());
}
//---------------------------------------------------------------------------

