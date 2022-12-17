//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "MailUser.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
__fastcall TMailUser::TMailUser(TCollection* Collection)
        : TCollectionItem(Collection)
{
  Name = new AnsiString();

  Email = new TStringList();
  Email->Sorted = true;
  Email->Duplicates = dupIgnore;

  AddressCount = 0;

  ValidEmailChars = new AnsiString();
  *ValidEmailChars = "@._";

  ValidNameChars = new AnsiString();
  *ValidNameChars = " \'";
}
//---------------------------------------------------------------------------
__fastcall TMailUser::~TMailUser()
{
  delete ValidNameChars;
  delete ValidEmailChars;

  delete Email;
  delete Name;
}
//---------------------------------------------------------------------------
void __fastcall TMailUser::SetName(AnsiString name) {
  ExtractName(name, Name);
}
//---------------------------------------------------------------------------
void __fastcall TMailUser::AddAddress(AnsiString addr) {
  AnsiString *a = new AnsiString();

  ExtractAddress(addr, a);
  if (*a != "")
    Email->Add(*a);

  AddressCount = Email->Count;

  delete a;
}
//---------------------------------------------------------------------------
void __fastcall TMailUser::ExtractAddress(AnsiString s, AnsiString *addr) {
  AnsiString *t = new AnsiString();
  AnsiString *n1 = new AnsiString();
  AnsiString *n2 = new AnsiString();

  *t = s;
  *t = t->Trim();

  int p = t->AnsiPos("@");
  if (p > 0) {
    for (int i = p + 1; i <= t->Length(); i++) {
      if (SelectEmailChar(t->SubString(i, 1)))
        *n2 = *n2 + t->SubString(i, 1);
      else
        break;
    }

    for (int i = p; i > 0; i--) {
      if (SelectEmailChar(t->SubString(i, 1)))
        *n1 = t->SubString(i, 1) + *n1;
      else
        break;
    }
  }

  *addr = *n1 + *n2;

  delete n2;
  delete n1;
  delete t;
}
//---------------------------------------------------------------------------
bool __fastcall TMailUser::SelectEmailChar(AnsiString c) {
  bool rv = false;

  if (((c.SubString(1, 1) >= "0") && (c.SubString(1, 1) <= "9")) || ((c.SubString(1, 1) >= "a") && (c.SubString(1, 1) <= "z")) || ((c.SubString(1, 1) >= "A") && (c.SubString(1, 1) <= "Z")))
    rv = true;

  if (!rv) {
    for (int i = 1; i <= ValidEmailChars->Length(); i++) {
      rv = (c.SubString(1, 1) == ValidEmailChars->SubString(i, 1));
      if (rv) break;
    }
  }

  return rv;
}
//---------------------------------------------------------------------------
bool __fastcall TMailUser::SelectNameChar(AnsiString c) {
  bool rv = false;

  if (((c.SubString(1, 1) >= "0") && (c.SubString(1, 1) <= "9")) || ((c.SubString(1, 1) >= "a") && (c.SubString(1, 1) <= "z")) || ((c.SubString(1, 1) >= "A") && (c.SubString(1, 1) <= "Z")))
    rv = true;

  if (!rv) {
    for (int i = 1; i <= ValidNameChars->Length(); i++) {
      rv = (c.SubString(1, 1) == ValidNameChars->SubString(i, 1));
      if (rv) break;
    }
  }

  return rv;
}
//---------------------------------------------------------------------------
void __fastcall TMailUser::ExtractName(AnsiString s, AnsiString *n) {
  AnsiString *ta = new AnsiString();
  AnsiString *tn = new AnsiString();

  *tn = s;

  ExtractAddress(s, ta);
  tn->Delete(tn->AnsiPos(*ta), ta->Length());

  *tn = TrimRight(*tn);
  int ms = 0;
  int me = 0;

  if (tn->AnsiPos("\"") > 0) { // Name is in quotes...
    for (int i = tn->Length(); i > 0; i--) {
      if (tn->SubString(i, 1) == "\"")
        if (me == 0)
          me = i;
        else
          ms = i;

      if ((ms != 0) && (me != 0))
        break;
    }

    if (ms == 0) ms = 1;
    if (me == 0) me = tn->Length();

    *tn = tn->SubString(ms, (me - ms + 1));

    while (tn->AnsiPos("\"") > 0)
      tn->Delete(tn->AnsiPos("\""), 1);
  }
  else { // Name is not quoted...
    for (int i = tn->Length(); i > 0; i--) {
      if (SelectNameChar(tn->SubString(i, 1))) {
        me = i;
        break;
      }
    }

    *tn = TrimRight(tn->SubString(1, me));
    me = tn->Length();

    for (int i = tn->Length(); i > 0; i--) {
      if (SelectNameChar(tn->SubString(i, 1)))
        ms = i;
      else
        break;
    }

    if (ms == 0) ms = 1;
    if (me == 0) me = tn->Length();

    *tn = Trim(tn->SubString(ms, (me - ms + 1)));
  }

  *n = *tn;

  delete tn;
  delete ta;
}
//---------------------------------------------------------------------------
int __fastcall TMailUser::GetAddressCount() {
  return AddressCount;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TMailUser::GetAddress(int Num) {
  AnsiString rv;

  if ((Num >= 0) && (Num < GetAddressCount()))
    rv = Email->Strings[Num];
  else
    rv = NULL;

  return rv;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TMailUser::GetName() {
  AnsiString rv;

  if (*Name != "")
    rv = *Name;
  else
    rv = NULL;

  return rv;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TMailUser::GetValidEmailChars() {
  return *ValidEmailChars;
}
//---------------------------------------------------------------------------
void __fastcall TMailUser::SetValidEmailChars(AnsiString chars) {
  *ValidEmailChars = chars;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TMailUser::GetValidNameChars() {
  return *ValidNameChars;
}
//---------------------------------------------------------------------------
void __fastcall TMailUser::SetValidNameChars(AnsiString chars) {
  *ValidNameChars = chars;
}
//---------------------------------------------------------------------------
void __fastcall TMailUser::Clear() {
  *Name = "";
  Email->Clear();
  AddressCount = 0;
}
//---------------------------------------------------------------------------

