//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "HostAccountInfo.h"
#include "Registry.hpp"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//

static inline void ValidCtrCheck(THostAccountInfo *)
{
        new THostAccountInfo(NULL);
}
//---------------------------------------------------------------------------
__fastcall THostAccountInfo::THostAccountInfo(TComponent *Owner)
            : TComponent(Owner)
{
  PrefAcct = -1;

  FRegistryKey = new AnsiString();
  FPreferredAccount = new AnsiString();
  FUserID = new TStringList();
  FFullName = new TStringList();

  FIsDirty = false;
}
//---------------------------------------------------------------------------
__fastcall THostAccountInfo::~THostAccountInfo()
{
  delete FRegistryKey;
  delete FPreferredAccount;
  delete FUserID;
  delete FFullName;
}
//---------------------------------------------------------------------------
namespace Hostaccountinfo
{
        void __fastcall PACKAGE Register()
        {
                 TComponentClass classes[1] = {__classid(THostAccountInfo)};
                 RegisterComponents("LMTS", classes, 0);
        }
}
//---------------------------------------------------------------------------
AnsiString __fastcall THostAccountInfo::GetRegistryKey()
{
  return *FRegistryKey;
}
//---------------------------------------------------------------------------
void __fastcall THostAccountInfo::SetRegistryKey(AnsiString rk)
{
  if (*FRegistryKey != rk) {
    *FRegistryKey = rk;
    FIsDirty = true;
  }
}
//---------------------------------------------------------------------------
int  __fastcall THostAccountInfo::GetUserCount()
{
  return FUserID->Count;
}
//---------------------------------------------------------------------------
TObject * __fastcall THostAccountInfo::GetObject(AnsiString uid)
{
  TObject *rv;

  int i = GetAccountIndex(uid);
  if (i > -1)
    rv = FUserID->Objects[i];
  else
    rv = NULL;

  return rv;
}
//---------------------------------------------------------------------------
void __fastcall THostAccountInfo::SetObject(AnsiString uid, TObject *obj)
{
  int i = GetAccountIndex(uid);
  if (i > -1) {
    FUserID->Objects[i] = obj;
    FIsDirty = true;
  }
}
//---------------------------------------------------------------------------
AnsiString __fastcall THostAccountInfo::ExtractUserID(AnsiString str)
{
  AnsiString rv;

  if (str.Length() > 0) {
    int k = str.AnsiPos("[");
    if (k > 0) {
      if (k > 1)
        rv = str.SubString(1, k - 1);
      else
        rv = "";
    } else
      rv = Trim(str);
  } else
    rv = "";

  return rv;
}
//---------------------------------------------------------------------------
AnsiString __fastcall THostAccountInfo::ExtractPassword(AnsiString str)
{
  AnsiString rv;
  AnsiString *tmp = new AnsiString();

  *tmp = str;
  int k = tmp->AnsiPos("[");
  int m = tmp->AnsiPos("]");
  if ((k > 0) && (m > 0)) {
    rv = tmp->SubString(k + 1, (m - 1) - (k + 1) + 1);
  } else
    rv = "";

  delete tmp;

  return rv;
}
//---------------------------------------------------------------------------
AnsiString __fastcall THostAccountInfo::MakeEntry(AnsiString uid, AnsiString pwd)
{
  if (!pwd.IsEmpty())
    return (Trim(uid) + "[" + pwd + "]");
  else
    return Trim(uid);
}
//---------------------------------------------------------------------------
TStringList * __fastcall THostAccountInfo::GetUserID()
{
  return FUserID;
}
//---------------------------------------------------------------------------
void __fastcall THostAccountInfo::SetUserID(TStringList *uid)
{
  AnsiString *usr = new AnsiString();

  FIsDirty = true;
  ClearAccounts();

  for (int j = 0; j < uid->Count; j++) {
    *usr = uid->Strings[j];
    AddAccount(ExtractUserID(*usr), ExtractPassword(*usr));
  }

  delete usr;
}
//---------------------------------------------------------------------------
AnsiString __fastcall THostAccountInfo::GetAccountPassword(AnsiString uid)
{
  AnsiString rv;

  int i = GetAccountIndex(uid);
  if (i > -1) {
    rv = ExtractPassword(FUserID->Strings[i]);
  } else
    rv = NULL;

  return rv;
}
//---------------------------------------------------------------------------
void __fastcall THostAccountInfo::SetAccountPassword(AnsiString uid, AnsiString pword)
{
  int i = GetAccountIndex(uid);
  if (i > -1) {
    FUserID->Strings[i] = MakeEntry(ExtractUserID(FUserID->Strings[i]), pword);
    FIsDirty = true;

    if (FOnPasswordChanged != NULL)
      FOnPasswordChanged(this, uid);
  }
}
//---------------------------------------------------------------------------
void __fastcall THostAccountInfo::ClearPreferredAccount()
{
  FIsDirty = true;
  PrefAcct = -1;
  *FPreferredAccount = "";
}
//---------------------------------------------------------------------------
AnsiString __fastcall THostAccountInfo::GetPreferredAccount()
{
  if ((PrefAcct > -1) && (PrefAcct < FUserID->Count))
    *FPreferredAccount = ExtractUserID(FUserID->Strings[PrefAcct]);
  else
    *FPreferredAccount = "";

  FIsDirty = true;

  return *FPreferredAccount;
}
//---------------------------------------------------------------------------
void __fastcall THostAccountInfo::SetPreferredAccount(int i)
{
  if ((i >= 0) && (i < FUserID->Count)) {
    PrefAcct = i;
    FIsDirty = true;
    *FPreferredAccount = ExtractUserID(FUserID->Strings[PrefAcct]);
  }
}
//---------------------------------------------------------------------------
void __fastcall THostAccountInfo::SetPreferredAccount(AnsiString pa)
{
  int i = GetAccountIndex(pa);
  if (i > -1)
    SetPreferredAccount(i);
  else
    ClearPreferredAccount();
}
//---------------------------------------------------------------------------
void __fastcall THostAccountInfo::ClearAccounts()
{
  for (int i = 0; i < FUserID->Count; i++) {
    TObject *ptr = FUserID->Objects[i];
    if (ptr != NULL)
      delete ptr;
  }

  FUserID->Clear();
  FFullName->Clear();
  ClearPreferredAccount();
  FIsDirty = true;

  if (FOnAccountsChange != NULL)
    FOnAccountsChange(this);
}
//---------------------------------------------------------------------------
int __fastcall THostAccountInfo::GetAccountIndex(AnsiString uid)
{
  int rv = -1;

  for (int i = 0; i < FUserID->Count; i++) {
    if (uid.AnsiCompareIC(ExtractUserID(FUserID->Strings[i])) == 0) {
      rv = i;
      break;
    }
  }

  return rv;
}
//---------------------------------------------------------------------------
bool __fastcall THostAccountInfo::IsUserAccount(AnsiString uid)
{
  return (GetAccountIndex(uid) > -1);
}
//---------------------------------------------------------------------------
bool __fastcall THostAccountInfo::UserHasPassword(AnsiString uid)
{
  bool rc;

  int i = GetAccountIndex(uid);
  if (i > -1)
    rc = (ExtractPassword(FUserID->Strings[i]) != "");
  else
    rc = false;

  return rc;
}
//---------------------------------------------------------------------------
bool __fastcall THostAccountInfo::IsPasswordValid(AnsiString uid, AnsiString pwd)
{
  bool rc;

  int i = GetAccountIndex(uid);
  if (i > -1)
    rc = (pwd.AnsiCompareIC(ExtractPassword(FUserID->Strings[i])) == 0);
  else
    rc = false;

  return rc;
}
//---------------------------------------------------------------------------
void __fastcall THostAccountInfo::AddAccount(AnsiString uid, AnsiString pwd, AnsiString fname)
{
  AddAccount(uid, pwd);
  int k = GetAccountIndex(uid);
  if (k != -1)
    FFullName->Strings[k] = fname;
}
//---------------------------------------------------------------------------
void __fastcall THostAccountInfo::AddAccount(AnsiString uid, AnsiString pwd)
{
  if (!IsUserAccount(uid)) {
    AnsiString *Item = new AnsiString();

    bool UValid = true;
    *Item = Trim(uid);
    if (FOnUserIDValidation != NULL)
      FOnUserIDValidation(this, *Item, UValid);

    bool PValid = true;
    *Item = pwd;
    if (FOnPasswordValidation != NULL)
      FOnPasswordValidation(this, *Item, PValid);

    if ((UValid) && (PValid)) {
      FUserID->AddObject(MakeEntry(Trim(uid), pwd), NULL);
      FFullName->Add("");
    }

    delete Item;
  }

  FIsDirty = true;

  if (FUserID->Count == 0)
    ClearPreferredAccount();
  if (PrefAcct == -1)
    if (FUserID->Count == 1)
      SetPreferredAccount(0);

  if (FOnAccountsChange != NULL)
    FOnAccountsChange(this);
}
//---------------------------------------------------------------------------
void __fastcall THostAccountInfo::RemoveAccount(AnsiString uid)
{
  int i = GetAccountIndex(uid);
  if (i > -1) {
    if (FUserID->Objects[i] != NULL)
      delete FUserID->Objects[i];

    FUserID->Delete(i);
  }

  FIsDirty = true;

  if (FUserID->Count == 0)
    ClearPreferredAccount();
  if (FUserID->Count == 1)
    SetPreferredAccount(0);

  if (FOnAccountsChange != NULL)
    FOnAccountsChange(this);
}
//---------------------------------------------------------------------------
void __fastcall THostAccountInfo::SaveToRegistry(int where)
{
  bool Cancel = false;
  if (FOnBeforeSave != NULL)
    FOnBeforeSave(this, Cancel);

  if ((!Cancel) && (!FRegistryKey->IsEmpty())) {
    TRegistry *Reg = new TRegistry();

    if ((where >= HAI_REG_USER) && (where <= HAI_REG_MACHINE)) {
      switch (where) {
        case HAI_REG_USER:
          Reg->RootKey = HKEY_CURRENT_USER;
          break;
        case HAI_REG_MACHINE:
          Reg->RootKey = HKEY_LOCAL_MACHINE;
          break;
        default: ;
      }
    } else
      throw Exception("Invalid registry location on save");

    if (Reg->KeyExists(*FRegistryKey)) {
      TStringList *vn = new TStringList();

      Reg->GetValueNames(vn);
      for (int p = 0; p < vn->Count; p++)
        Reg->DeleteValue(vn->Strings[p]);

      delete vn;
    }

    if (Reg->OpenKey(*FRegistryKey, true)) {
      Reg->WriteString("PrefAcct", *FPreferredAccount);
      Reg->CloseKey();
    }


    AnsiString *uid = new AnsiString();
    AnsiString *pwd = new AnsiString();
    AnsiString *fn = new AnsiString();

    for (int i = 0; i < FUserID->Count; i++) {
      *uid = ExtractUserID(FUserID->Strings[i]);
      Reg->CreateKey(*FRegistryKey + "\\" + *uid);
      if (Reg->OpenKey(*FRegistryKey + "\\" + *uid, false)) {
        *pwd = ExtractPassword(FUserID->Strings[i]);
        *fn = FFullName->Strings[i];
        Reg->WriteString("Pwd", *pwd);
        Reg->WriteString("Fname", *fn);
        Reg->CloseKey();
      }
    }

    delete uid;
    delete pwd;
    delete fn;

    FIsDirty = false;

    delete Reg;
  }

   if (FOnAfterSave != NULL)
     FOnAfterSave(this);
}
//---------------------------------------------------------------------------
void __fastcall THostAccountInfo::LoadFromRegistry(int where)
{
  bool Cancel = false;
  if (FOnBeforeLoad != NULL)
    FOnBeforeLoad(this, Cancel);

  if ((!Cancel) && (!FRegistryKey->IsEmpty())) {
    TRegistry *Reg = new TRegistry();

    if ((where >= HAI_REG_USER) && (where <= HAI_REG_MACHINE)) {
      switch (where) {
        case HAI_REG_USER:
          Reg->RootKey = HKEY_CURRENT_USER;
          break;
        case HAI_REG_MACHINE:
          Reg->RootKey = HKEY_LOCAL_MACHINE;
          break;
        default: ;
      }
    } else
      throw Exception("Invalid registry location on load");


    if (Reg->OpenKey(*FRegistryKey, false)) {
      TStringList *users = new TStringList();
      Reg->GetKeyNames(users);
      Reg->CloseKey();

      ClearAccounts();
      AnsiString *pwd = new AnsiString();
      AnsiString *fn = new AnsiString();

      for (int i = 0; i < users->Count; i++) {
        if (Reg->OpenKey(*FRegistryKey + "\\" + users->Strings[i], false)) {
          *pwd = Reg->ReadString("Pwd");
          *fn = Reg->ReadString("Fname");
          AddAccount(users->Strings[i], *pwd, *fn);
          Reg->CloseKey();
        }
      }

      delete pwd;
      delete fn;
      delete users;
    }


    if (Reg->OpenKey(*FRegistryKey, false)) {
      SetPreferredAccount(Reg->ReadString("PrefAcct"));
      Reg->CloseKey();
    }

    FIsDirty = true;

    delete Reg;
  }

  if (FOnAfterLoad != NULL)
    FOnAfterLoad(this);
}
//---------------------------------------------------------------------------
void __fastcall THostAccountInfo::SaveToRegistryUser()
{
  if (!FRegistryKey->IsEmpty())
    SaveToRegistry(HAI_REG_USER);
}
//---------------------------------------------------------------------------
void __fastcall THostAccountInfo::SaveToRegistryMachine()
{
  if (!FRegistryKey->IsEmpty())
    SaveToRegistry(HAI_REG_MACHINE);
}
//---------------------------------------------------------------------------
void __fastcall THostAccountInfo::LoadFromRegistryUser()
{
  if (!FRegistryKey->IsEmpty())
    LoadFromRegistry(HAI_REG_USER);
}
//---------------------------------------------------------------------------
void __fastcall THostAccountInfo::LoadFromRegistryMachine()
{
  if (!FRegistryKey->IsEmpty())
    LoadFromRegistry(HAI_REG_MACHINE);
}
//---------------------------------------------------------------------------
void __fastcall THostAccountInfo::ShowDialog()
{
  TFormHostAccountInfo *frm = new TFormHostAccountInfo(this);

  for (int i = 0; i < FUserID->Count; i++) {
    frm->StringGrid->Cells[0][i + 1] = ExtractUserID(FUserID->Strings[i]);
    frm->StringGrid->Cells[1][i + 1] = ExtractPassword(FUserID->Strings[i]);
    frm->StringGrid->Cells[2][i + 1] = FFullName->Strings[i];
  }

  if (frm->ShowModal() == mrOk) {
    TStringList *RedList = new TStringList();

    for (int i = 0; i < FUserID->Count; i++) {
      bool Found = false;

      for (int k = 1; k <= frm->StringGrid->RowCount; k++) {
        if (frm->StringGrid->Cells[0][k] == "")
          break;

        Found = (Trim(ExtractUserID(FUserID->Strings[i])) == Trim(frm->StringGrid->Cells[0][k]));
        if (Found) {
          FUserID->Strings[i] = MakeEntry(Trim(ExtractUserID(FUserID->Strings[i])), frm->StringGrid->Cells[1][k]);
          FFullName->Strings[i] = frm->StringGrid->Cells[2][k];
          break;
        }
      }

      if (!Found)
        RedList->Add(ExtractUserID(FUserID->Strings[i]));
    }

    for (int k = 0; k < RedList->Count; k++)
      RemoveAccount(RedList->Strings[k]);

    delete RedList;


    for (int i = 1; i <= frm->StringGrid->RowCount; i++) {
      if (frm->StringGrid->Cells[0][i] == "")
        break;

      if (!IsUserAccount(Trim(frm->StringGrid->Cells[0][i])))
        AddAccount(frm->StringGrid->Cells[0][i], frm->StringGrid->Cells[1][i], frm->StringGrid->Cells[2][i]);
    }
  }

  delete frm;
}
//---------------------------------------------------------------------------

