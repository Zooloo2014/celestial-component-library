//---------------------------------------------------------------------------
#ifndef AgentInfoH
#define AgentInfoH
#include <stdlib.h>
#include <SysUtils.hpp>
#include <Controls.hpp>
#include <Classes.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
#define WINSOCK_VER_REQD        2
#define HOSTNAME_BUF            8192
//---------------------------------------------------------------------------
class TAgentInfo : public TComponent
{
private:	// User declarations
        bool FLoadedFromRegistry;
        int  FHostPort;
        AnsiString *FHostName;
        AnsiString *FMyName;
        AnsiString *FMachineName;
        AnsiString *FRegistryKey;

        bool Dirty;
        bool WinSockStarted;

protected:      // User declarations
        void __fastcall SetHostPort(int p);
        AnsiString __fastcall GetHostName();
        void __fastcall SetHostName(const AnsiString n);
        AnsiString __fastcall GetMachineName();
        void __fastcall SetMachineName(const AnsiString n);
        AnsiString __fastcall GetMyName();
        void __fastcall SetMyName(const AnsiString n);
        AnsiString __fastcall GetRegistryKey();
        void __fastcall SetRegistryKey(const AnsiString r);
        bool __fastcall GetLoadedFromRegistry();

        bool __fastcall IsDirty();
        void __fastcall SetDirty(bool dirt);
        void __fastcall GetWinSockInfo();


public:		// User declarations
        __fastcall TAgentInfo(TComponent *Owner);
        __fastcall ~TAgentInfo();

        void __fastcall SaveInfo();
        void __fastcall LoadInfo();
        void __fastcall GenerateAgentName();
        void __fastcall SetAgentName(AnsiString n);

__published:		// User declarations
        __property bool LoadedFromRegistry = {read=GetLoadedFromRegistry};
        __property int  HostPort = {read=FHostPort, write=SetHostPort};
        __property AnsiString HostName = {read=GetHostName, write=SetHostName};
        __property AnsiString MyName = {read=GetMyName, write=SetMyName};
        __property AnsiString MachineName = {read=GetMachineName};
        __property AnsiString RegistryKey = {read=GetRegistryKey, write=SetRegistryKey};
};
//---------------------------------------------------------------------------
#endif
