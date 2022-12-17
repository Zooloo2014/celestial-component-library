//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "AddressBook.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//

static inline void ValidCtrCheck(TAddressBook *)
{
        new TAddressBook(NULL);
}
//---------------------------------------------------------------------------
__fastcall TAddressBook::TAddressBook(TComponent *Owner)
        : TComponent(Owner)
{
  FDuplicatesAllowed = true;
  FListDelimiters = new AnsiString();
  *FListDelimiters = ",;\t";

  Addresses = new TStringList();
  LastEntry = new TStringList();
  LastAddress = new TStringList();
  LastEntryCtr = -1;
  LastAddressCtr = -1;
}
//---------------------------------------------------------------------------
__fastcall TAddressBook::~TAddressBook()
{
  delete FListDelimiters;
  delete Addresses;
  delete LastEntry;
  delete LastAddress;
}
//---------------------------------------------------------------------------
namespace Addressbook
{
        void __fastcall PACKAGE Register()
        {
                 TComponentClass classes[1] = {__classid(TAddressBook)};
                 RegisterComponents("Celestial", classes, 0);
        }
}
//---------------------------------------------------------------------------
AnsiString __fastcall TAddressBook::GetListDelimiters()
{
  return *FListDelimiters;
}
//---------------------------------------------------------------------------
void __fastcall TAddressBook::SetListDelimiters(AnsiString ld)
{
  *FListDelimiters = ld;
}
//---------------------------------------------------------------------------
int __fastcall TAddressBook::GetCount()
{
  return Addresses->Count;
}
//---------------------------------------------------------------------------
bool __fastcall TAddressBook::IsDelimiter(AnsiString s) {
  bool rv = false;

  for (int i = 1; i < FListDelimiters->Length(); i++) {
    if (FListDelimiters->SubString(i, 1) == s.SubString(1, 1)) {
      rv = true;
      break;
    }
  }

  return rv;
}
//---------------------------------------------------------------------------
void __fastcall TAddressBook::ParseList(AnsiString s) {
  AnsiString *is = new AnsiString();
  AnsiString *ss = new AnsiString();

  *is = s;
  *is = *is + FListDelimiters->SubString(1, 1) + " ";

  for (int i = 1; i < is->Length(); i++) {
    if (IsDelimiter(is->SubString(i, 1))) {
      TMailIdentity *mi = new TMailIdentity(this, true);
      if (!ss->IsEmpty()) {
        mi->AddAddress(*ss);
        mi->DispName = *ss;
        if (mi->DispName != "")
          Add(mi);
      }
      *ss = "";
    }
    else
      *ss += is->SubString(i, 1);
  }

  delete ss;
  delete is;
}
//---------------------------------------------------------------------------
void __fastcall TAddressBook::Clear()
{
  TMailIdentity *ptr;

  for (int i = 0; i < Addresses->Count; i++) {
    ptr = (TMailIdentity *)Addresses->Objects[i];
    delete ptr;
    Addresses->Objects[i] = NULL;
  }

  Addresses->Clear();
}
//---------------------------------------------------------------------------
bool __fastcall TAddressBook::FindIdentity(TMailIdentity *mid)
{
  bool rc;

  if (!mid->DispName.IsEmpty()) {
    for (int i = 0; i < Addresses->Count; i++) {
      TMailIdentity *ptr = (TMailIdentity *)Addresses->Objects[i];
      if (!ptr->DispName.IsEmpty()) {
        if (!FCaseSensitive)
          rc = (ptr->DispName.AnsiCompareIC(mid->DispName) == 0);
        else
          rc = (ptr->DispName.AnsiCompare(mid->DispName) == 0);
      } else
        rc = false;
    }
  } else
    rc = false;

  return rc;
}
//---------------------------------------------------------------------------
void __fastcall TAddressBook::Add(TMailIdentity *mid)
{
  if (mid->DispName != "") {
    if (!FDuplicatesAllowed) {
      if (!FindIdentity(mid))
        Addresses->AddObject(mid->DispName, mid);
    } else
      Addresses->AddObject(mid->DispName, mid);
  }

  GetAllEntries();
  GetAllAddresses();
}
//---------------------------------------------------------------------------
void __fastcall TAddressBook::Add(AnsiString mid)
{
  ParseList(mid);
  GetAllEntries();
  GetAllAddresses();
}
//---------------------------------------------------------------------------
TMailIdentity * __fastcall TAddressBook::GetEntry(int i)
{
  TMailIdentity *ptr;

  if ((i >= 0) && (i < GetCount()))
    ptr = (TMailIdentity *)Addresses->Objects[i];

  return ptr;
}
//---------------------------------------------------------------------------
TMailIdentity * __fastcall TAddressBook::FindEntry(AnsiString str)
{
  TMailIdentity *rv = NULL;
  TMailIdentity *ptr;

  for (int i = 0; i < GetCount(); i++) {
    ptr = GetEntry(i);

    if (ptr->DispName != str) {
      int k = ptr->ContainsAddress(str);
      if (k > -1)
        rv = ptr;
    } else
      rv = ptr;

    if (rv != NULL)
      break;
  }

  return rv;
}
//---------------------------------------------------------------------------
void __fastcall TAddressBook::Export(AnsiString fname)
{
  TStringList *tl = new TStringList();

  for (int i = 0; i < GetCount(); i++) {
    tl->Add("[" + Addresses->Strings[i] + "]");

    TMailIdentity *ptr = GetEntry(i);
    for (int j = 0; j < ptr->GetAddressCount(); j++)
      tl->Add(ptr->GetAddress(j));

    tl->Add("");
  }

  tl->SaveToFile(fname);

  delete tl;
}
//---------------------------------------------------------------------------
void __fastcall TAddressBook::Append(TAddressBook *ab)
{
  for (int i = 0; i < ab->Count; i++)
    Add(ab->GetEntry(i));
}
//---------------------------------------------------------------------------
void __fastcall TAddressBook::Append(TMailIdentity *mi)
{
  Add(mi);
}
//---------------------------------------------------------------------------
void __fastcall TAddressBook::SetDuplicatesAllowed(bool dup)
{
  FDuplicatesAllowed = dup;
}
//---------------------------------------------------------------------------
void __fastcall TAddressBook::SetCaseSensitive(bool cs)
{
  FCaseSensitive = cs;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TAddressBook::GetFirstEntry()
{
  AnsiString rv;

  if (LastEntry->Count > 0)
    rv = LastEntry->Strings[0];
  else
    rv = NULL;

  return rv;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TAddressBook::GetNextEntry()
{
  AnsiString rv;

  if (LastEntryCtr + 1 < LastEntry->Count)
    rv = LastEntry->Strings[++LastEntryCtr];
  else
    rv = NULL;

  return rv;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TAddressBook::GetFirstAddress()
{
  AnsiString rv;

  if (LastAddress->Count > 0)
    rv = LastAddress->Strings[0];
  else
    rv = NULL;

  return rv;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TAddressBook::GetNextAddress()
{
  AnsiString rv;

  if (LastAddressCtr + 1 < LastAddress->Count)
    rv = LastAddress->Strings[++LastAddressCtr];
  else
    rv = NULL;

  return rv;
}
//---------------------------------------------------------------------------
void __fastcall TAddressBook::GetAllEntries()
{
  LastEntry->Clear();
  for (int i = 0; i < Addresses->Count; i++)
    LastEntry->Add(Addresses->Strings[i]);

  if (LastEntry->Count > 0)
    LastEntryCtr = 0;
}
//---------------------------------------------------------------------------
void __fastcall TAddressBook::GetAllAddresses()
{
  TMailIdentity *ptr;

  LastAddress->Clear();
  for (int i = 0; i < Addresses->Count; i++) {
    ptr = (TMailIdentity *)Addresses->Objects[i];
    if (ptr != NULL)
      for (int j = 0; j < ptr->GetAddressCount(); j++)
        LastAddress->Add(ptr->GetAddress(j));
  }

  if (LastAddress->Count > 0)
    LastAddressCtr = 0;
}
//---------------------------------------------------------------------------

