//---------------------------------------------------------------------------
#ifndef ServiceControllerH
#define ServiceControllerH
//---------------------------------------------------------------------------
#include <stdlib.h>
#include <SysUtils.hpp>
#include <Controls.hpp>
#include <Classes.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
enum ServiceState {None, Stopped, Starting, Stopping, Running, Continuing, Pausing, Paused};
enum StartType {Automatic = SERVICE_AUTO_START, Manual = SERVICE_DEMAND_START, Disabled = SERVICE_DISABLED};
//---------------------------------------------------------------------------
class PACKAGE TServiceController : public TComponent
{
private:
        bool ManagerOpen;
        bool CanInstall;
        SC_HANDLE ManHandle;
        SC_HANDLE SvcHandle;
        LPSERVICE_STATUS Status;

        AnsiString *FServiceName;
        ServiceState FStatus;
        AnsiString *FStatusString;

        void __fastcall ThrowException(int i);
        void __fastcall OpenManager(bool InstallAccess);
        void __fastcall CloseManager();
        ServiceState __fastcall MapState(DWORD i);

protected:
        virtual AnsiString __fastcall GetServiceName();
        virtual void __fastcall SetServiceName(AnsiString sn);
        virtual ServiceState __fastcall GetStatus();
        virtual AnsiString __fastcall GetStatusString();

public:
        __fastcall TServiceController(TComponent *Owner);
        __fastcall ~TServiceController();

        void __fastcall OpenServ(AnsiString ServName);
        void __fastcall CloseServ();
        void __fastcall Install(AnsiString SvcName, AnsiString DispName, StartType st, AnsiString FilePath, bool Interactive);
        void __fastcall Uninstall();
        void __fastcall Start();
        void __fastcall Stop();
        void __fastcall Pause();
        void __fastcall Continue();
        void __fastcall QueryStatus();

__published:
        __property AnsiString ServiceName = {read=GetServiceName, nodefault};
        __property ServiceState ServiceStatus = {read=GetStatus, nodefault};
        __property AnsiString ServiceStatusString = {read=GetStatusString, nodefault};
};
//---------------------------------------------------------------------------
#endif
