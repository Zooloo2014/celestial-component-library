//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "RemoteHostInfo.h"
#include "FrmRemoteHostInfo.h"
#include "Registry.hpp"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//

static inline void ValidCtrCheck(TRemoteHostInfo *)
{
        new TRemoteHostInfo(NULL);
}
//---------------------------------------------------------------------------
__fastcall TRemoteHostInfo::TRemoteHostInfo(TComponent *Owner)
            : TComponent(Owner)
{
  FRegistryKey = new AnsiString();
  FHostName = new AnsiString();
  FService = new AnsiString();
  FPreferredAccount = new AnsiString();

  Accounts = new THostAccountInfo(this);
  Accounts->OnUserIDValidation = ValidateUserID;
  Accounts->OnPasswordValidation = ValidatePassword;

  FPort = 0;
}
//---------------------------------------------------------------------------
__fastcall TRemoteHostInfo::~TRemoteHostInfo()
{
  delete FRegistryKey;
  delete FHostName;
  delete FService;
  delete FPreferredAccount;
  delete Accounts;
}
//---------------------------------------------------------------------------
namespace Remotehostinfo
{
        void __fastcall PACKAGE Register()
        {
                 TComponentClass classes[1] = {__classid(TRemoteHostInfo)};
                 RegisterComponents("LMTS", classes, 0);
        }
}
//---------------------------------------------------------------------------
AnsiString __fastcall TRemoteHostInfo::GetRegistryKey()
{
  return *FRegistryKey;
}
//---------------------------------------------------------------------------
void __fastcall TRemoteHostInfo::SetRegistryKey(AnsiString rk)
{
  AnsiString *newname = new AnsiString();
  bool Allowed = true;

  *newname = rk;
  if (FOnRegistryKeyChange != NULL)
    FOnRegistryKeyChange(this, *newname, Allowed);

  if (Allowed) {
    if (!FHostName->IsEmpty())
      *FRegistryKey = rk + "\\" + *FHostName;
    else
      *FRegistryKey = rk;

    Accounts->RegistryKey = *FRegistryKey + "\\Accounts";
  }

  delete newname;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TRemoteHostInfo::GetHostName()
{
  return *FHostName;
}
//---------------------------------------------------------------------------
void __fastcall TRemoteHostInfo::SetHostName(AnsiString hn)
{
  AnsiString *newname = new AnsiString();
  bool Allowed = true;

  *newname = hn;
  if (FOnHostNameChange != NULL)
    FOnHostNameChange(this, *newname, Allowed);

  if (Allowed)
    *FHostName = hn;

  delete newname;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TRemoteHostInfo::GetService()
{
  return *FService;
}
//---------------------------------------------------------------------------
void __fastcall TRemoteHostInfo::SetService(AnsiString hs)
{
  *FService = hs;
}
//---------------------------------------------------------------------------
void __fastcall TRemoteHostInfo::SetPort(int hp)
{
  FPort = hp;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TRemoteHostInfo::GetPreferredAccount()
{
  SetPreferredAccount(Accounts->PreferredAccount);
  return *FPreferredAccount;
}
//---------------------------------------------------------------------------
void __fastcall TRemoteHostInfo::SetPreferredAccount(AnsiString pa)
{
  if (!pa.IsEmpty()) {
    if (Accounts->IsUserAccount(pa)) {
      *FPreferredAccount = pa;
      Accounts->PreferredAccount = *FPreferredAccount;
    }
  } else
    *FPreferredAccount = "";
}
//---------------------------------------------------------------------------
void __fastcall TRemoteHostInfo::ClearAccounts()
{
  Accounts->ClearAccounts();
  SetPreferredAccount("");
}
//---------------------------------------------------------------------------
int  __fastcall TRemoteHostInfo::GetUserCount()
{
  if (Accounts->UserCount == 0)
    SetPreferredAccount("");

  return Accounts->UserCount;
}
//---------------------------------------------------------------------------
bool __fastcall TRemoteHostInfo::IsUserAccount(AnsiString uid)
{
  return Accounts->IsUserAccount(uid);
}
//---------------------------------------------------------------------------
bool __fastcall TRemoteHostInfo::IsPasswordValid(AnsiString uid, AnsiString pwd)
{
  return Accounts->IsPasswordValid(uid, pwd);
}
//---------------------------------------------------------------------------
void __fastcall TRemoteHostInfo::AddAccount(AnsiString uid, AnsiString pwd)
{
  Accounts->AddAccount(uid, pwd);
  SetPreferredAccount(Accounts->PreferredAccount);
}
//---------------------------------------------------------------------------
void __fastcall TRemoteHostInfo::AddAccount(AnsiString uid, AnsiString pwd, TObject *obj)
{
  AddAccount(uid, pwd);
  Accounts->SetObject(uid, obj);
}
//---------------------------------------------------------------------------
void __fastcall TRemoteHostInfo::RemoveAccount(AnsiString uid)
{
  Accounts->RemoveAccount(uid);
  SetPreferredAccount(Accounts->PreferredAccount);
}
//---------------------------------------------------------------------------
void __fastcall TRemoteHostInfo::SetAccountPassword(AnsiString uid, AnsiString pword)
{
  if (Accounts->IsUserAccount(uid))
    Accounts->SetAccountPassword(uid, pword);
}
//---------------------------------------------------------------------------
AnsiString __fastcall TRemoteHostInfo::GetPreferredUID()
{
  GetPreferredAccount();
  return *FPreferredAccount;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TRemoteHostInfo::GetPreferredPWD()
{
  AnsiString rv;

  if (Accounts->IsUserAccount(*FPreferredAccount))
    rv = Accounts->GetAccountPassword(*FPreferredAccount);
  else
    rv = "";

  return rv;
}
//---------------------------------------------------------------------------
void __fastcall TRemoteHostInfo::SaveToRegistry(int where)
{
  if (!FRegistryKey->IsEmpty()) {
    TRegistry *Reg = new TRegistry();

    switch (where) {
      case RHI_REG_USER:
        Reg->RootKey = HKEY_CURRENT_USER;
      case RHI_REG_MACHINE:
        Reg->RootKey = HKEY_LOCAL_MACHINE;
      default: ;
    }


    if (Reg->KeyExists(*FRegistryKey))
      Reg->DeleteKey(*FRegistryKey);

    Reg->CreateKey(*FRegistryKey);

    if (Reg->OpenKey(*FRegistryKey, true)) {
      Reg->WriteString("Name", *FHostName);
      Reg->WriteString("Srv", *FService);
      Reg->WriteInteger("Port", FPort);
      Reg->WriteString("PrefAcct", GetPreferredUID());
      Reg->CloseKey();
    }

    delete Reg;
  }
}
//---------------------------------------------------------------------------
void __fastcall TRemoteHostInfo::LoadFromRegistry(int where)
{
  if (!FRegistryKey->IsEmpty()) {
    TRegistry *Reg = new TRegistry();

    switch (where) {
      case RHI_REG_USER:
        Reg->RootKey = HKEY_CURRENT_USER;
      case RHI_REG_MACHINE:
        Reg->RootKey = HKEY_LOCAL_MACHINE;
      default: ;
    }


    if (Reg->OpenKey(*FRegistryKey, false)) {
      *FHostName = Reg->ReadString("Name");
      *FService = Reg->ReadString("Srv");
      FPort = Reg->ReadInteger("Port");
      *FPreferredAccount = "";
      SetPreferredAccount(Reg->ReadString("PrefAcct"));
      Reg->CloseKey();
    }

    delete Reg;
  }
}
//---------------------------------------------------------------------------
void __fastcall TRemoteHostInfo::SaveToRegistryUser()
{
  if (!FRegistryKey->IsEmpty()) {
    SaveToRegistry(RHI_REG_USER);
    Accounts->SaveToRegistryUser();
  }
}
//---------------------------------------------------------------------------
void __fastcall TRemoteHostInfo::SaveToRegistryMachine()
{
  if (!FRegistryKey->IsEmpty()) {
    SaveToRegistry(RHI_REG_MACHINE);
    Accounts->SaveToRegistryMachine();
  }
}
//---------------------------------------------------------------------------
void __fastcall TRemoteHostInfo::LoadFromRegistryUser()
{
  if (!FRegistryKey->IsEmpty()) {
    LoadFromRegistry(RHI_REG_USER);
    Accounts->LoadFromRegistryUser();
  }
}
//---------------------------------------------------------------------------
void __fastcall TRemoteHostInfo::LoadFromRegistryMachine()
{
  if (!FRegistryKey->IsEmpty()) {
    LoadFromRegistry(RHI_REG_MACHINE);
    Accounts->LoadFromRegistryMachine();
  }
}
//---------------------------------------------------------------------------
void __fastcall TRemoteHostInfo::ShowDialog()
{
  TFormRemoteHostInfo *frm = new TFormRemoteHostInfo(this);

  frm->LabelHost->Caption = *FHostName;
  frm->EditPort->Text = IntToStr(FPort);
  frm->EditService->Text = *FService;
  frm->SetMyHost(this);

  if (frm->ShowModal() == mrOk) {
    FPort = StrToInt(frm->EditPort->Text);
    *FService = frm->EditService->Text;
  }

  delete frm;
}
//---------------------------------------------------------------------------
THostAccountInfo * __fastcall TRemoteHostInfo::GetAccounts()
{
  return Accounts;
}
//---------------------------------------------------------------------------
void __fastcall TRemoteHostInfo::ValidateUserID(TObject* Sender, AnsiString Item, bool &Valid)
{
  if (FOnUserIDValidation != NULL)
    FOnUserIDValidation(this, Item, Valid);
}
//---------------------------------------------------------------------------
void __fastcall TRemoteHostInfo::ValidatePassword(TObject* Sender, AnsiString Item, bool &Valid)
{
  if (FOnPasswordValidation != NULL)
    FOnPasswordValidation(this, Item, Valid);
}
//---------------------------------------------------------------------------

