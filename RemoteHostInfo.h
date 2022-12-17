//---------------------------------------------------------------------------
#ifndef RemoteHostInfoH
#define RemoteHostInfoH
//---------------------------------------------------------------------------
#include <stdlib.h>
#include <SysUtils.hpp>
#include <Classes.hpp>
#include "HostAccountInfo.h"
//---------------------------------------------------------------------------
#define RHI_REG_USER                    0
#define RHI_REG_MACHINE                 1
//---------------------------------------------------------------------------
typedef void __fastcall (__closure *TRemoteHostChange)(System::TObject* Sender, AnsiString NewValue, bool &Allowed);
typedef void __fastcall (__closure *TRemoteHostValidationRequired)(System::TObject* Sender, AnsiString Item, bool &Valid);
//---------------------------------------------------------------------------
class TRemoteHostInfo : public TComponent
{
private:
          TRemoteHostValidationRequired FOnUserIDValidation;
          TRemoteHostValidationRequired FOnPasswordValidation;
          TRemoteHostChange FOnRegistryKeyChange;
          TRemoteHostChange FOnHostNameChange;

          AnsiString *FRegistryKey;
          AnsiString *FHostName;
          AnsiString *FService;
          AnsiString *FPreferredAccount;
          int FPort;

          THostAccountInfo *Accounts;

protected:
          AnsiString __fastcall GetRegistryKey();
          void __fastcall SetRegistryKey(AnsiString rk);
          AnsiString __fastcall GetHostName();
          void __fastcall SetHostName(AnsiString hn);
          AnsiString __fastcall GetService();
          void __fastcall SetService(AnsiString hs);
          void __fastcall SetPort(int hp);
          int  __fastcall GetUserCount();
          AnsiString __fastcall GetPreferredAccount();
          void __fastcall SetPreferredAccount(AnsiString pa);
          void __fastcall SaveToRegistry(int where);
          void __fastcall LoadFromRegistry(int where);
          void __fastcall ValidateUserID(TObject* Sender, AnsiString Item, bool &Valid);
          void __fastcall ValidatePassword(TObject* Sender, AnsiString Item, bool &Valid);

public:
          __fastcall TRemoteHostInfo(TComponent *Owner);
          __fastcall ~TRemoteHostInfo();

          void __fastcall SaveToRegistryUser();
          void __fastcall SaveToRegistryMachine();
          void __fastcall LoadFromRegistryUser();
          void __fastcall LoadFromRegistryMachine();

          void __fastcall ClearAccounts();
          THostAccountInfo * __fastcall GetAccounts();
          bool __fastcall IsUserAccount(AnsiString uid);
          bool __fastcall IsPasswordValid(AnsiString uid, AnsiString pwd);
          void __fastcall AddAccount(AnsiString uid, AnsiString pwd);
          void __fastcall AddAccount(AnsiString uid, AnsiString pwd, TObject *obj);
          void __fastcall RemoveAccount(AnsiString uid);
          void __fastcall SetAccountPassword(AnsiString uid, AnsiString pword);

          void __fastcall ShowDialog();
          AnsiString __fastcall GetPreferredUID();
          AnsiString __fastcall GetPreferredPWD();

__published:
          __property int Port = {read=FPort, write=SetPort};
          __property AnsiString RegistryKey = {read=GetRegistryKey, write=SetRegistryKey};
          __property AnsiString HostName = {read=GetHostName, write=SetHostName};
          __property AnsiString Service = {read=GetService, write=SetService};
          __property AnsiString PreferredAccount = {read=GetPreferredAccount, write=SetPreferredAccount};
          __property int UserCount = {read=GetUserCount};

          __property TRemoteHostValidationRequired OnUserIDValidation = {read=FOnUserIDValidation, write=FOnUserIDValidation};
          __property TRemoteHostValidationRequired OnPasswordValidation = {read=FOnPasswordValidation, write=FOnPasswordValidation};
          __property TRemoteHostChange OnRegistryKeyChange = {read=FOnRegistryKeyChange, write=FOnRegistryKeyChange};
          __property TRemoteHostChange OnHostNameChange = {read=FOnHostNameChange, write=FOnHostNameChange};
};
//---------------------------------------------------------------------------
#endif

