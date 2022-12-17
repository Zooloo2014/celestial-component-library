//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "CascadedList.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//

static inline void ValidCtrCheck(TCascadedList *)
{
        new TCascadedList(NULL);
}
//---------------------------------------------------------------------------
__fastcall TCascadedList::TCascadedList(TComponent* Owner)
        : TCustomPanel(Owner)
{
  FOrientation = ORIENT_VERTICAL;
  FItemCaption = new AnsiString();
  FListCaption = new AnsiString();

  FItem = new AnsiString();
  FList = new TStringList();
  FListSelection = new AnsiString();

  if (FOrientation == ORIENT_VERTICAL) {
    Height = 120;
    Width = 100;
  } else {
    Height = 100;
    Width = 220;
  }

  Visible = true;

  Name1 = new TLabel(this);
  Name1->Parent = this;
  Name2 = new TLabel(this);
  Name2->Parent = this;

  List1 = new TComboBox(this);
  List1->Style = csDropDownList;
  List1->Parent = this;
  List1->OnChange = List1Change;
  List1->Sorted = true;
  List2 = new TComboBox(this);
  List2->Style = csDropDownList;
  List2->Parent = this;
  List2->OnChange = List2Change;
  List2->Sorted = true;

  Name1->Caption = "";
  Name2->Caption = "";

  List1->Width = Width;
  List2->Width = Width;
  List1->Visible = Visible;
  List2->Visible = Visible;
  List1->Height = 20;
  List2->Height = 20;

  Layout();
}
//---------------------------------------------------------------------------
namespace Cascadedlist
{
        void __fastcall PACKAGE Register()
        {
                 TComponentClass classes[1] = {__classid(TCascadedList)};
                 RegisterComponents("Celestial", classes, 0);
        }
}
//---------------------------------------------------------------------------
void __fastcall TCascadedList::SetOrientation(TOrientation o)
{
  if ((o == ORIENT_VERTICAL) || (o == ORIENT_HORIZONTAL)) {
    FOrientation = o;
    Layout();
  }
}
//---------------------------------------------------------------------------
void __fastcall TCascadedList::Layout(void)
{
  Name1->Caption = *FItemCaption;
  Name1->Font = Font;
  Name2->Caption = *FListCaption;
  Name2->Font = Font;

  if (FOrientation == ORIENT_VERTICAL) {
    Name1->Top = 0;
    List1->Top = Name1->Top + Name1->Height + 2;

    Name2->Top = List1->Top + List1->Height + 5;
    List2->Top = Name2->Top + Name2->Height + 2;
    Name2->Left = List1->Left;
    Name1->Left = List1->Left;
    List2->Left = List1->Left;
  } else {
    Name1->Top = 0;
    List1->Top = Name1->Top + Name1->Height + 2;

    Name2->Top = 0;
    List2->Top = Name2->Top + Name2->Height + 2;
    List2->Left = List1->Left + List1->Width + 2;
    Name2->Left = List2->Left;
  }
}
//---------------------------------------------------------------------------
AnsiString __fastcall TCascadedList::GetItemCaption()
{
  return *FItemCaption;
}
//---------------------------------------------------------------------------
void __fastcall TCascadedList::SetItemCaption(AnsiString c)
{
  *FItemCaption = c;
  Layout();
}
//---------------------------------------------------------------------------
AnsiString __fastcall TCascadedList::GetListCaption()
{
  return *FListCaption;
}
//---------------------------------------------------------------------------
void __fastcall TCascadedList::SetListCaption(AnsiString c)
{
  *FListCaption = c;
  Layout();
}
//---------------------------------------------------------------------------
AnsiString __fastcall TCascadedList::GetItem()
{
  return *FItem;
}
//---------------------------------------------------------------------------
void __fastcall TCascadedList::SetItem(AnsiString i)
{
  if (!i.IsEmpty()) {
    *FItem = i;
    if (List1->Items->IndexOf(*FItem) == -1)
      List1->Items->AddObject(*FItem, (TObject*)new TStringList());

    List1->ItemIndex = List1->Items->IndexOf(*FItem);
  }
}
//---------------------------------------------------------------------------
AnsiString __fastcall TCascadedList::GetListSelection()
{
  return *FListSelection;
}
//---------------------------------------------------------------------------
TStringList* __fastcall TCascadedList::GetList()
{
  return FList;
}
//---------------------------------------------------------------------------
void __fastcall TCascadedList::SetList(TStringList *i)
{
  if (List1->ItemIndex != -1) {
    FList->Clear();
    for (int k = 0; k < i->Count; k++)
      FList->Add(i->Strings[k]);

    TStringList *l = (TStringList*)List1->Items->Objects[List1->ItemIndex];
    if (l != NULL) {
      l->Clear();
      for (int k = 0; k < FList->Count; k++)
        l->Add(FList->Strings[k]);
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TCascadedList::List1Change(TObject* Sender)
{
  int i = List1->ItemIndex;
  if (i > -1) {
    TStringList *l = (TStringList*)List1->Items->Objects[i];
    if (l != NULL) {
      List2->Clear();
      for (int j = 0; j < l->Count; j++)
        List2->Items->Add(l->Strings[j]);
    }

    *FItem = List1->Items->Strings[List1->ItemIndex];
    *FListSelection = "";
    List2->ItemIndex = -1;

    if (FOnChange != NULL)
      FOnChange(this);
  }
}
//---------------------------------------------------------------------------
void __fastcall TCascadedList::List2Change(TObject* Sender)
{
  int i = List2->ItemIndex;
  if (i > -1) {
    *FListSelection = List2->Items->Strings[i];

    if (FOnChange != NULL)
      FOnChange(this);
  }
}
//---------------------------------------------------------------------------
void __fastcall TCascadedList::Paint(void)
{
}
//---------------------------------------------------------------------------

