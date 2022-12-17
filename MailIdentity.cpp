//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "MailIdentity.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//

static inline void ValidCtrCheck(TMailIdentity *)
{
        new TMailIdentity(NULL);
}
//---------------------------------------------------------------------------
__fastcall TMailIdentity::TMailIdentity(TComponent *Owner)
        : TComponent(Owner)
{
  Construct();
}
//---------------------------------------------------------------------------
__fastcall TMailIdentity::TMailIdentity(TComponent *Owner, bool DispNameReqd)
        : TComponent(Owner)
{
  Construct();
  SetRequireDispName(DispNameReqd);
}
//---------------------------------------------------------------------------
void __fastcall TMailIdentity::Construct()
{
  FRequireDispName = false;
  FDispName = new AnsiString();

  FEmails = new TStringList();
  FEmails->Sorted = true;
  FEmails->Duplicates = dupIgnore;

  FValidEmailChars = new AnsiString();
  *FValidEmailChars = "@._-";

  FValidNameChars = new AnsiString();
  *FValidNameChars = "- \'";
}
//---------------------------------------------------------------------------
__fastcall TMailIdentity::~TMailIdentity()
{
  delete FValidNameChars;
  delete FValidEmailChars;

  delete FEmails;
  delete FDispName;
}
//---------------------------------------------------------------------------
namespace Mailidentity
{
        void __fastcall PACKAGE Register()
        {
                 TComponentClass classes[1] = {__classid(TMailIdentity)};
                 RegisterComponents("Celestial", classes, 0);
        }
}
//---------------------------------------------------------------------------
AnsiString __fastcall TMailIdentity::GetValidEmailChars()
{
  return *FValidEmailChars;
}
//---------------------------------------------------------------------------
void __fastcall TMailIdentity::SetValidEmailChars(AnsiString chars)
{
  if (!chars.IsEmpty())
    *FValidEmailChars = chars;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TMailIdentity::GetValidNameChars()
{
  return *FValidNameChars;
}
//---------------------------------------------------------------------------
void __fastcall TMailIdentity::SetValidNameChars(AnsiString chars)
{
  if (!chars.IsEmpty())
    *FValidNameChars = chars;
}
//---------------------------------------------------------------------------
void __fastcall TMailIdentity::Clear()
{
  *FDispName = "";
  FEmails->Clear();
}
//---------------------------------------------------------------------------
void __fastcall TMailIdentity::ClearAddresses()
{
  FEmails->Clear();
}
//---------------------------------------------------------------------------
void __fastcall TMailIdentity::SetRequireDispName(bool req)
{
  FRequireDispName = req;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TMailIdentity::GetDispName()
{
  return *FDispName;
}
//---------------------------------------------------------------------------
void __fastcall TMailIdentity::SetDispName(AnsiString name)
{
  AnsiString *extrname = new AnsiString();

  ExtractName(name, extrname);

  if (FRequireDispName) {
    if (!extrname->IsEmpty())
      *FDispName = *extrname;
  } else
    *FDispName = *extrname;

  delete extrname;
}
//---------------------------------------------------------------------------
bool __fastcall TMailIdentity::SelectEmailChar(AnsiString c)
{
  bool rv = false;

  if (((c.SubString(1, 1) >= "0") && (c.SubString(1, 1) <= "9")) || ((c.SubString(1, 1) >= "a") && (c.SubString(1, 1) <= "z")) || ((c.SubString(1, 1) >= "A") && (c.SubString(1, 1) <= "Z")))
    rv = true;

  if (!rv) {
    for (int i = 1; i <= FValidEmailChars->Length(); i++) {
      rv = (c.SubString(1, 1) == FValidEmailChars->SubString(i, 1));
      if (rv) break;
    }
  }

  return rv;
}
//---------------------------------------------------------------------------
bool __fastcall TMailIdentity::SelectNameChar(AnsiString c)
{
  bool rv = false;

  if (((c.SubString(1, 1) >= "0") && (c.SubString(1, 1) <= "9")) || ((c.SubString(1, 1) >= "a") && (c.SubString(1, 1) <= "z")) || ((c.SubString(1, 1) >= "A") && (c.SubString(1, 1) <= "Z")))
    rv = true;

  if (!rv) {
    for (int i = 1; i <= FValidNameChars->Length(); i++) {
      rv = (c.SubString(1, 1) == FValidNameChars->SubString(i, 1));
      if (rv) break;
    }
  }

  return rv;
}
//---------------------------------------------------------------------------
int __fastcall TMailIdentity::GetAddressCount()
{
  return FEmails->Count;
}
//---------------------------------------------------------------------------
void __fastcall TMailIdentity::ExtractAddress(AnsiString str, AnsiString *addr)
{
  AnsiString *t = new AnsiString();
  AnsiString *n1 = new AnsiString();
  AnsiString *n2 = new AnsiString();
  AnsiString *t2 = new AnsiString();

  *t2 = str.Trim();

  for (int i = t2->Length(); i > 0; i--) {
    if (t2->SubString(i, 1) == " ") {
      *t = t2->SubString(i + 1, t2->Length() - i + 1);
      break;
    }

    if (i < 2)
      *t = str.Trim();
  }


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

  delete t2;
  delete n2;
  delete n1;
  delete t;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TMailIdentity::GetAddress(int Num)
{
  AnsiString rv;

  if ((Num >= 0) && (Num < GetAddressCount()))
    rv = FEmails->Strings[Num];
  else
    rv = NULL;

  return rv;
}
//---------------------------------------------------------------------------
void __fastcall TMailIdentity::AddAddress(AnsiString addr)
{
  AnsiString *name = new AnsiString();
  AnsiString *email = new AnsiString();

  ExtractAddress(addr, email);
  if (!email->IsEmpty())
    FEmails->Add(*email);

  if (FRequireDispName) {
    if (FDispName->IsEmpty()) {
      ExtractName(addr, name);
      if (!name->IsEmpty())
        *FDispName = *name;
      else
        *FDispName = *email;
    }
  }

  delete name;
  delete email;
}
//---------------------------------------------------------------------------
void __fastcall TMailIdentity::AddAddresses(TStringList *addrlist)
{
  for (int i = 0; i < addrlist->Count; i++)
    if (ContainsAddress(addrlist->Strings[i]) != -1)
      AddAddress(addrlist->Strings[i]);
}
//---------------------------------------------------------------------------
void __fastcall TMailIdentity::ExtractName(AnsiString str, AnsiString *name)
{
  AnsiString *ta = new AnsiString();
  AnsiString *tn = new AnsiString();

  *tn = str;
  *name = "";

  ExtractAddress(*tn, ta);
  tn->Delete(tn->AnsiPos(*ta), ta->Length());

  *tn = Trim(*tn);
  int q = tn->Pos("\"");
  if (q > 0) {  // Get text up to next quote as the display name...
    *name = *tn;
    name->Delete(1, q);
    q = name->Pos("\"");
    if (q > 0)
      *name = name->SubString(1, q - 1);
  } else {

    for (int w = tn->Length(); w > 0; w--) {
      if (tn->SubString(w, 1) == " ")
        continue;

      if (SelectNameChar(tn->SubString(w, 1))) {
        *name = Trim(tn->SubString(1, w));
        break;
      }
    }

  }

  delete tn;
  delete ta;
}
//---------------------------------------------------------------------------
int __fastcall TMailIdentity::ContainsAddress(AnsiString addr)
{
  int rv = -1;

  if (!addr.IsEmpty()) {
    for (int i = 0; i < GetAddressCount(); i++) {
      if (LowerCase(FEmails->Strings[i]) == LowerCase(addr)) {
        rv = i;
        break;
      }
    }
  }

  return rv;
}
//---------------------------------------------------------------------------
void __fastcall TMailIdentity::Export(AnsiString fname)
{
  TStringList *tl = new TStringList();

  tl->Add("[" + *FDispName + "]");

  for (int i = 0; i < GetAddressCount(); i++)
    tl->Add(GetAddress(i));

  tl->Add("");

  tl->SaveToFile(fname);

  delete tl;
}
//---------------------------------------------------------------------------

