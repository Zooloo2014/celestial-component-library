//---------------------------------------------------------------------------
#ifndef DialupAlerterH
#define DialupAlerterH
#define TAPI_CURRENT_VERSION 0x00020000
//---------------------------------------------------------------------------
#include <stdlib.h>
#include <SysUtils.hpp>
#include <Controls.hpp>
#include <Classes.hpp>
#include <Forms.hpp>
#include <Tapi.h>
//---------------------------------------------------------------------------
typedef void __fastcall (__closure *TDialupNotifyEvent)(System::TObject* Sender, AnsiString *Msg);
//---------------------------------------------------------------------------
class PACKAGE TDialupAlerter : public TComponent
{
private:
        TNotifyEvent FOnNoLineAvailable;
        TDialupNotifyEvent FOnEventNotification;
        bool FActive;
        bool FConnected;
        int FLineDeviceID;

        LPHLINEAPP MyTAPIHandle;
        AnsiString *LastErrorMsg;
        DWORD LastErrorCode;
        int LinkCount;

        LPLINEINITIALIZEEXPARAMS InitParms;
        LPLINEMESSAGE LineMsg;
        HLINE LineHandle;
        HANDLE EventHandle;
        unsigned long APIVersion;
        unsigned long NumDevices;

        HCALL CallHandle;
        LPLINECALLINFO CallInfo;

        void __fastcall GetErrorMessage(AnsiString Tag, int res);

protected:
        virtual void __fastcall SetActive(bool act);
        virtual void __fastcall SetLineDeviceID(int lid);

        void __fastcall Startup();
        void __fastcall Shutdown();
        void __fastcall OpenLine(int i);
        void __fastcall CloseLine(HLINE i);
        void __fastcall SetStatusChangeRequests(HLINE l);

public:
        __fastcall TDialupAlerter(TComponent* Owner);
        __fastcall ~TDialupAlerter();

        void __fastcall CheckMessages();
        
__published:
        __property bool Active = {read=FActive, write=SetActive, default=false};
        __property bool Connected = {read=FConnected, default=false};
        __property int LineDeviceID = {read=FLineDeviceID, write=SetLineDeviceID, default=-1};
        __property TNotifyEvent OnNoLineAvailable = {read=FOnNoLineAvailable, write=FOnNoLineAvailable, nodefault};
        __property TDialupNotifyEvent OnEventNotification = {read=FOnEventNotification, write=FOnEventNotification, nodefault};
};
//---------------------------------------------------------------------------
#endif
