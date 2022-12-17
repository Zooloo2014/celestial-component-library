//---------------------------------------------------------------------------
#ifndef HostAccountInfoH
#define HostAccountInfoH
//---------------------------------------------------------------------------
#include <stdlib.h>
#include <SysUtils.hpp>
#include <Classes.hpp>
#include "FrmHostAccountInfo.h"
//---------------------------------------------------------------------------
#define HAI_REG_USER                    0
#define HAI_REG_MACHINE                 1
//---------------------------------------------------------------------------
typedef void __fastcall (__closure *THostAcctPasswordNotifyEvent)(System::TObject* Sender, AnsiString Account);
typedef void __fastcall (__closure *THostAcctValidationRequiredEvent)(System::TObject* Sender, AnsiString Item, bool &Valid);
typedef void __fastcall (__closure *THostAcctBeforeRegistryEvent)(System::TObject* Sender, bool &Cancel);
typedef void __fastcall (__closure *THostAcctAfterRegistryEvent)(System::TObject* Sender);
//---------------------------------------------------------------------------
class THostAccountInfo : public TComponent
{
private:
          TNotifyEvent FOnAccountsChange;
          THostAcctPasswordNotifyEvent FOnPasswordChanged;
          THostAcctValidationRequiredEvent FOnUserIDValidation;
          THostAcctValidationRequiredEvent FOnPasswordValidation;
          THostAcctBeforeRegistryEvent FOnBeforeSave;
          THostAcctAfterRegistryEvent FOnAfterSave;
          THostAcctBeforeRegistryEvent FOnBeforeLoad;
          THostAcctAfterRegistryEvent FOnAfterLoad;

          bool FIsDirty;
          AnsiString *FRegistryKey;
          AnsiString *FPreferredAccount;

          TStringList *FUserID;
          TStringList *FFullName;

          int PrefAcct;

protected:
          AnsiString __fastcall GetRegistryKey();
          void __fastcall SetRegistryKey(AnsiString rk);
          int  __fastcall GetUserCount();
          TStringList * __fastcall GetUserID();
          void __fastcall SetUserID(TStringList *uid);
          AnsiString __fastcall GetPreferredAccount();
          void __fastcall ClearPreferredAccount();
          void __fastcall SetPreferredAccount(int i);

          int  __fastcall GetAccountIndex(AnsiString uid);
          void __fastcall SaveToRegistry(int where);
          void __fastcall LoadFromRegistry(int where);
          AnsiString __fastcall ExtractPassword(AnsiString str);
          AnsiString __fastcall MakeEntry(AnsiString uid, AnsiString pwd);

public:
          __fastcall THostAccountInfo(TComponent *Owner);
          __fastcall ~THostAccountInfo();

          TObject * __fastcall GetObject(AnsiString uid);
          void __fastcall SetObject(AnsiString uid, TObject *obj);
          AnsiString __fastcall GetAccountPassword(AnsiString uid);
          void __fastcall SetAccountPassword(AnsiString uid, AnsiString pword);

          void __fastcall SaveToRegistryUser();
          void __fastcall SaveToRegistryMachine();
          void __fastcall LoadFromRegistryUser();
          void __fastcall LoadFromRegistryMachine();

          void __fastcall ClearAccounts();
          bool __fastcall IsUserAccount(AnsiString uid);
          bool __fastcall UserHasPassword(AnsiString uid);
          bool __fastcall IsPasswordValid(AnsiString uid, AnsiString pwd);
          void __fastcall THostAccountInfo::AddAccount(AnsiString uid, AnsiString pwd, AnsiString fname);
          void __fastcall AddAccount(AnsiString uid, AnsiString pwd);
          void __fastcall RemoveAccount(AnsiString uid);
          void __fastcall SetPreferredAccount(AnsiString pa);

          void __fastcall ShowDialog();
          AnsiString __fastcall ExtractUserID(AnsiString str);

__published:
          __property bool IsDirty = {read=FIsDirty};
          __property AnsiString RegistryKey = {read=GetRegistryKey, write=SetRegistryKey};
          __property int UserCount = {read=GetUserCount};
          __property TStringList *UserID = {read=GetUserID, write=SetUserID};
          __property AnsiString PreferredAccount = {read=GetPreferredAccount, write=SetPreferredAccount};

          __property TNotifyEvent OnAccountsChange = {read=FOnAccountsChange, write=FOnAccountsChange};
          __property THostAcctPasswordNotifyEvent OnPasswordChanged = {read=FOnPasswordChanged, write=FOnPasswordChanged};
          __property THostAcctValidationRequiredEvent OnUserIDValidation = {read=FOnUserIDValidation, write=FOnUserIDValidation};
          __property THostAcctValidationRequiredEvent OnPasswordValidation = {read=FOnPasswordValidation, write=FOnPasswordValidation};
          __property THostAcctBeforeRegistryEvent OnBeforeSave = {read=FOnBeforeSave, write=FOnBeforeSave};
          __property THostAcctAfterRegistryEvent OnAfterSave = {read=FOnAfterSave, write=FOnAfterSave};
          __property THostAcctBeforeRegistryEvent OnBeforeLoad = {read=FOnBeforeLoad, write=FOnBeforeLoad};
          __property THostAcctAfterRegistryEvent OnAfterLoad = {read=FOnAfterLoad, write=FOnAfterLoad};
};
//---------------------------------------------------------------------------
#endif

