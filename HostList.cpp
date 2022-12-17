//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include <Registry.hpp>
#include "HostList.h"
#include "FrmHostList.h" 
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//

static inline void ValidCtrCheck(THostList *)
{
        new THostList(NULL);
}
//---------------------------------------------------------------------------
__fastcall THostList::THostList(TComponent *Owner)
               : TComponent(Owner)
{
  Construct();
}
//---------------------------------------------------------------------------
__fastcall THostList::THostList(TComponent *Owner, AnsiString ListName)
               : TComponent(Owner)
{
  Construct();
  SetListName(ListName);
}
//---------------------------------------------------------------------------
void __fastcall THostList::Construct()
{
  FCapacity = BLOCK_SIZE;
  FPrefHost = -1;

  FListName = new AnsiString();
  FRegistryKey = new AnsiString();
  FPreferredHost = new AnsiString();
  FPreferredAccount = new AnsiString();
  FPreferredPassword = new AnsiString();

  FHosts = (TRemoteHostInfoPtr)malloc(sizeof(TRemoteHostInfoPtr) * BLOCK_SIZE);
  FCount = 0;

  if (FHosts != NULL)
    memset(FHosts, 0, sizeof(TRemoteHostInfoPtr) * BLOCK_SIZE);
}
//---------------------------------------------------------------------------
__fastcall THostList::~THostList()
{
  delete FListName;
  delete FRegistryKey;
  delete FPreferredHost;
  delete FPreferredAccount;
  delete FPreferredPassword;

  free(FHosts);
}
//---------------------------------------------------------------------------
namespace Hostlist
{
        void __fastcall PACKAGE Register()
        {
                 TComponentClass classes[1] = {__classid(THostList)};
                 RegisterComponents("LMTS", classes, 0);
        }
}
//---------------------------------------------------------------------------
void __fastcall THostList::Clear()
{
  TRemoteHostInfo *ptr;

  for (int i = 0; i < FCount; i++) {
    ptr = FHosts[i];
    if (ptr != NULL)
      delete ptr;
  }

  memset(FHosts, 0, sizeof(TRemoteHostInfoPtr) * FCount);
  FCount = 0;
  FPrefHost = -1;
}
//---------------------------------------------------------------------------
bool __fastcall THostList::IsInList(AnsiString host)
{
  return (GetHostIndex(host) > -1);
}
//---------------------------------------------------------------------------
void __fastcall THostList::Resize(int newsize)
{
  TRemoteHostInfoPtr newptr;

  newptr = (TRemoteHostInfoPtr)malloc(sizeof(TRemoteHostInfoPtr) * newsize);
  if (newptr == NULL)
    throw Exception("Couldn\'t resize memory block!");

  memset(newptr, 0, sizeof(TRemoteHostInfoPtr) * newsize);
  memcpy(newptr, FHosts, sizeof(TRemoteHostInfoPtr) * FCapacity);

  free(FHosts);
  FHosts = newptr;
  FCapacity = newsize;
}
//---------------------------------------------------------------------------
void __fastcall THostList::SetCapacity(int i)
{
  FCapacity = i;
}
//---------------------------------------------------------------------------
AnsiString __fastcall THostList::GetRegistryKey()
{
  return *FRegistryKey;
}
//---------------------------------------------------------------------------
void __fastcall THostList::SetRegistryKey(AnsiString rk)
{
  *FRegistryKey = rk;
}
//---------------------------------------------------------------------------
AnsiString __fastcall THostList::GetListName()
{
  return *FListName;
}
//---------------------------------------------------------------------------
void __fastcall THostList::SetListName(AnsiString ln)
{
  *FListName = ln;
}
//---------------------------------------------------------------------------
AnsiString __fastcall THostList::GetPreferredHost()
{
  TRemoteHostInfo *ptr;

  if (FPrefHost > -1) {
    ptr = FHosts[FPrefHost];
    if (ptr != NULL) {
      *FPreferredHost = ptr->HostName;
      *FPreferredAccount = ptr->GetPreferredUID();
      *FPreferredPassword = ptr->GetPreferredPWD();
    }
  } else
    *FPreferredHost = "";

  return *FPreferredHost;
}
//---------------------------------------------------------------------------
void __fastcall THostList::SetPreferredHost(AnsiString ph)
{
  TRemoteHostInfo *ptr;

  if (!ph.IsEmpty()) {
    FPrefHost = GetHostIndex(ph);
    if (FPrefHost > -1) {
      ptr = GetHost(ph);
      if (ptr != NULL) {
        *FPreferredHost = ptr->HostName;
        *FPreferredAccount = ptr->GetPreferredUID();
        *FPreferredPassword = ptr->GetPreferredPWD();
      }
    }
  } else {
    FPrefHost = -1;
    *FPreferredHost = "";
    *FPreferredAccount = "";
    *FPreferredPassword = "";
  }
}
//---------------------------------------------------------------------------
void __fastcall THostList::SetPreferredHost(int h)
{
  if ((h >= 0) && (h < FCount)) {
    TRemoteHostInfo *ptr = FHosts[h];
    if (ptr != NULL)
      SetPreferredHost(ptr->HostName);
  }
}
//---------------------------------------------------------------------------
int __fastcall THostList::GetPreferredPort()
{
  int rc;
  TRemoteHostInfo *ptr;

  if (!FPreferredHost->IsEmpty()) {
    ptr = GetHost(GetPreferredHost());
    rc = ptr->Port;
  } else
    rc = 0;

  return rc;
}
//---------------------------------------------------------------------------
AnsiString __fastcall THostList::GetPreferredAccount()
{
  return *FPreferredAccount;
}
//---------------------------------------------------------------------------
AnsiString __fastcall THostList::GetPreferredPassword()
{
  return *FPreferredPassword;
}
//---------------------------------------------------------------------------
int __fastcall THostList::GetHostIndex(AnsiString host)
{
  int rc;
  AnsiString *hn = new AnsiString(host);
  TRemoteHostInfo *ptr;
  bool Found = false;

  for (int i = 0; i < FCount; i++) {
    ptr = FHosts[i];
    if (ptr != NULL) {
      Found = (ptr->HostName.AnsiCompareIC(host) == 0);
      if (Found) {
        rc = i;
        break;
      }
    }
  }

  if (!Found)
    rc = -1;

  delete hn;

  return rc;
}
//---------------------------------------------------------------------------
TRemoteHostInfo * __fastcall THostList::GetHost(AnsiString host)
{
  TRemoteHostInfo *ptr;
  bool Found = false;

  for (int i = 0; i < FCount; i++) {
    ptr = FHosts[i];
    if (ptr != NULL) {
      Found = (ptr->HostName.AnsiCompareIC(host) == 0);
      if (Found)
        break;
    }
  }

  if (!Found)
    ptr = NULL;

  return ptr;
}
//---------------------------------------------------------------------------
void __fastcall THostList::Add(AnsiString host)
{
  TRemoteHostInfo *ptr;

  ptr = GetHost(host);
  if (ptr == NULL) {
    if ((FCount + 1) > FCapacity)
      Resize(FCapacity + BLOCK_SIZE);

    ptr = new TRemoteHostInfo(this);
    ptr->HostName = host;
    FHosts[FCount] = ptr;
    FCount++;
  }

  if (FCount == 0)
    SetPreferredHost("");
  if (FCount == 1)
    SetPreferredHost(0);
}
//---------------------------------------------------------------------------
void __fastcall THostList::Delete(AnsiString host)
{
  TRemoteHostInfo *ptr;

  int i = GetHostIndex(host);
  if (i != -1) {
    ptr = FHosts[i];
    delete ptr;
    FHosts[i] = NULL;

    for (int j = (i + i); j < FCount; j++)
      FHosts[i] = FHosts[j];

    FHosts[FCount] = NULL;
    FCount--;
  }

  if (FCount == 0)
    SetPreferredHost("");
  if (FCount == 1)
    SetPreferredHost(0);
}
//---------------------------------------------------------------------------
void __fastcall THostList::SaveToRegistryUser()
{
  SaveToRegistry(HLR_REG_USER);
}
//---------------------------------------------------------------------------
void __fastcall THostList::SaveToRegistryMachine()
{
  SaveToRegistry(HLR_REG_MACHINE);
}
//---------------------------------------------------------------------------
void __fastcall THostList::LoadFromRegistryUser()
{
  LoadFromRegistry(HLR_REG_USER);
}
//---------------------------------------------------------------------------
void __fastcall THostList::LoadFromRegistryMachine()
{
  LoadFromRegistry(HLR_REG_MACHINE);
}
//---------------------------------------------------------------------------
void __fastcall THostList::SaveToRegistry(int where)
{
  if (!FRegistryKey->IsEmpty()) {
    TRegistry *Reg = new TRegistry();
    AnsiString *rk = new AnsiString();

    if (!FListName->IsEmpty())
      *rk = *FRegistryKey + "\\" + *FListName;
    else
      *rk = *FRegistryKey;

    switch (where) {
      case HLR_REG_USER:
        Reg->RootKey = HKEY_CURRENT_USER;
        break;
      case HLR_REG_MACHINE:
        Reg->RootKey = HKEY_LOCAL_MACHINE;
        break;
      default: ;
    }

    if (Reg->KeyExists(*rk))
      Reg->DeleteKey(*rk);

    Reg->CreateKey(*rk);

    if (Reg->OpenKey(*rk, false)) {
      Reg->WriteString("PrefHost", *FPreferredHost);
      Reg->CloseKey();
    }


    if (Reg->OpenKey(*rk + "\\Hosts", true)) {
      TRemoteHostInfo *ptr;

      for (int i = 0; i < FCount; i++) {
        ptr = FHosts[i];
        if (ptr != NULL) {
          ptr->RegistryKey = *rk + "\\Hosts";
          if (where == HLR_REG_USER)
            ptr->SaveToRegistryUser();
          if (where == HLR_REG_MACHINE)
            ptr->SaveToRegistryMachine();
        }
      }

      Reg->CloseKey();
    }


    delete rk;
    delete Reg;
  }
}
//---------------------------------------------------------------------------
void __fastcall THostList::LoadFromRegistry(int where)
{
  if (!FRegistryKey->IsEmpty()) {
    TRegistry *Reg = new TRegistry();
    AnsiString *rk = new AnsiString();

    if (!FListName->IsEmpty())
      *rk = *FRegistryKey + "\\" + *FListName;
    else
      *rk = *FRegistryKey;

    switch (where) {
      case HLR_REG_USER:
        Reg->RootKey = HKEY_CURRENT_USER;
        break;
      case HLR_REG_MACHINE:
        Reg->RootKey = HKEY_LOCAL_MACHINE;
        break;
      default: ;
    }


    if (Reg->OpenKey(*rk + "\\Hosts", true)) {
      TStringList *names = new TStringList();
      TRemoteHostInfo *ptr;

      Reg->GetKeyNames(names);

      for (int i = 0; i < names->Count; i++) {
        Add(names->Strings[i]);
        ptr = GetHost(names->Strings[i]);
        if (ptr != NULL) {
          ptr->RegistryKey = *rk + "\\Hosts";
          if (where == HLR_REG_USER)
            ptr->LoadFromRegistryUser();
          if (where == HLR_REG_MACHINE)
            ptr->LoadFromRegistryMachine();
        }
      }

      Reg->CloseKey();

      delete names;
    }


    if (Reg->OpenKey(*rk, false)) {
      SetPreferredHost(Reg->ReadString("PrefHost"));
      Reg->CloseKey();
    }

    delete rk;
    delete Reg;
  }
}
//---------------------------------------------------------------------------
void __fastcall THostList::ShowDialog()
{
  TFormHostList *frm = new TFormHostList(this);

  if (!FListName->IsEmpty())
    frm->Caption = "Host List - " + *FListName;
  else
    frm->Caption = "Host List - Unnamed";

  frm->CheckListBox->Items->Clear();
  for (int i = 0; i < FCount; i++) {
    TRemoteHostInfo *ptr = FHosts[i];
    if (ptr != NULL) {
      frm->CheckListBox->Items->Add(ptr->HostName);
    }
  }

  if (!FPreferredHost->IsEmpty()) {
    int i = frm->CheckListBox->Items->IndexOf(*FPreferredHost);
    if (i > -1)
      frm->CheckListBox->Checked[i] = true;
  }

  frm->SetMyHostList(this, FHosts, FCount);

  if (frm->ShowModal() == mrOk) {
    for (int i = 0; i < frm->CheckListBox->Items->Count; i++) {
      if (frm->CheckListBox->Checked[i])
        SetPreferredHost(frm->CheckListBox->Items->Strings[i]);

      if (!IsInList(frm->CheckListBox->Items->Strings[i]))
        Add(frm->CheckListBox->Items->Strings[i]);
    }


    TStringList *RedList = new TStringList();

    for (int i = 0; i < FCount; i++) {
      TRemoteHostInfo *ptr = FHosts[i];
      if (ptr != NULL)
        if (frm->CheckListBox->Items->IndexOf(ptr->HostName) == -1)
          RedList->Add(ptr->HostName);
    }

    for (int i = 0; i < RedList->Count; i++)
      Delete(RedList->Strings[i]);

    delete RedList;
  }

  delete frm;
}
//---------------------------------------------------------------------------
AnsiString __fastcall THostList::GetHostName(int i)
{
  TRemoteHostInfo *ptr;

  if ((i >= 0) && (i < FCount)) {
    ptr = FHosts[i];
    return ptr->HostName;
  } else
    return "";
}
//---------------------------------------------------------------------------

