//---------------------------------------------------------------------------
#ifndef HostListH
#define HostListH
//---------------------------------------------------------------------------
#include <stdlib.h>
#include <SysUtils.hpp>
#include <Classes.hpp>
#include "RemoteHostInfo.h"
//---------------------------------------------------------------------------
#define BLOCK_SIZE              10
#define HLR_REG_USER            0
#define HLR_REG_MACHINE         1
//---------------------------------------------------------------------------
typedef TRemoteHostInfo * * TRemoteHostInfoPtr;
//---------------------------------------------------------------------------
class THostList : public TComponent
{
private:
           AnsiString *FRegistryKey;
           AnsiString *FListName;
           AnsiString *FPreferredHost;
           AnsiString *FPreferredAccount;
           AnsiString *FPreferredPassword;

           TRemoteHostInfoPtr FHosts;

           int FCount;
           int FCapacity;
           int FPrefHost;

protected:
           void __fastcall Construct();
           void __fastcall Resize(int newsize);
           void __fastcall SetCapacity(int i);
           AnsiString __fastcall GetRegistryKey();
           void __fastcall SetRegistryKey(AnsiString rk);
           AnsiString __fastcall GetListName();
           void __fastcall SetListName(AnsiString ln);
           AnsiString __fastcall GetPreferredHost();
           void __fastcall SetPreferredHost(AnsiString ph);
           void __fastcall SetPreferredHost(int h);
           AnsiString __fastcall GetPreferredAccount();
           AnsiString __fastcall GetPreferredPassword();
           int __fastcall GetPreferredPort();
           int __fastcall GetHostIndex(AnsiString host);
           void __fastcall SaveToRegistry(int where);
           void __fastcall LoadFromRegistry(int where);

public:
           __fastcall THostList(TComponent *Owner);
           __fastcall THostList(TComponent *Owner, AnsiString ListName);
           __fastcall ~THostList();

           void __fastcall Clear();
           bool __fastcall IsInList(AnsiString host);

           TRemoteHostInfo * __fastcall GetHost(AnsiString host);
           AnsiString __fastcall GetHostName(int i);
           void __fastcall Add(AnsiString host);
           void __fastcall Delete(AnsiString host);

           void __fastcall ShowDialog();
           void __fastcall SaveToRegistryUser();
           void __fastcall SaveToRegistryMachine();
           void __fastcall LoadFromRegistryUser();
           void __fastcall LoadFromRegistryMachine();

__published:
           __property int Count = {read=FCount};
           __property int Capacity = {read=FCapacity, write=SetCapacity, default=BLOCK_SIZE};
           __property AnsiString RegistryKey = {read=GetRegistryKey, write=SetRegistryKey};
           __property AnsiString ListName = {read=GetListName, write=SetListName};
           __property AnsiString PreferredHost = {read=GetPreferredHost, write=SetPreferredHost};
           __property AnsiString PreferredAccount = {read=GetPreferredAccount};
           __property AnsiString PreferredPassword = {read=GetPreferredPassword};
           __property int PreferredPort = {read=GetPreferredPort};
};
//---------------------------------------------------------------------------
#endif
