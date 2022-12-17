//---------------------------------------------------------------------------
#ifndef TransmissionServerH
#define TransmissionServerH
//---------------------------------------------------------------------------
#include <stdlib.h>
#include <SysUtils.hpp>
#include <Controls.hpp>
#include <Classes.hpp>
#include <Forms.hpp>
#include <ScktComp.hpp>
#include "TransmissionCommon.h"
#include "TransmissionMessage.h"
//---------------------------------------------------------------------------
#define LOG_LEVEL_NONE          0
#define LOG_LEVEL_CONNECTS      1
#define LOG_LEVEL_CONTENT       2
//---------------------------------------------------------------------------
typedef void __fastcall (__closure *TNotifyNewMsgEvent)(System::TObject* Sender, TTransmissionMessage *Msg, bool &DontQueue);
//---------------------------------------------------------------------------
class PACKAGE TTransmissionServer : public TComponent
{
private:
        TNotifyNewMsgEvent FOnNewMessage;
        TNotifyEvent FOnMessageQueued;
        TLogNotifyEvent FOnLogEvent;

        bool FActive;
        int FPort;
        int FConnectCount;
        int FBlockedConnectCount;
        int FQueuedMsgCount;
        int FDroppedMsgCount;
        int FLoggingLevel;
        bool FEnableBlockList;
        TStringList *FBlockedHosts;

        TServerSocket *MySocket;
        TStringList *Queue;

        bool HostIsBlocked;
        AnsiString *RecvData;
        AnsiString *SendData;

protected:
        void __fastcall SetActive(bool act);
        void __fastcall SetPort(int port);
        void __fastcall SetEnableBlockList(bool act);
        TStringList * __fastcall GetBlockedHosts();
        void __fastcall SetBlockedHosts(TStringList *bh);

        void __fastcall ClientConnect(TObject* Sender, TCustomWinSocket* Socket);
        void __fastcall ClientDisconnect(TObject* Sender, TCustomWinSocket* Socket);
        void __fastcall ClientRead(TObject* Sender, TCustomWinSocket* Socket);
        void __fastcall ClientWrite(TObject* Sender, TCustomWinSocket* Socket);
        void __fastcall ClientError(TObject* Sender, TCustomWinSocket* Socket, TErrorEvent ErrorEvent, int &ErrorCode);
        void __fastcall Accept(TObject* Sender, TCustomWinSocket* Socket);
        void __fastcall LogEvent(const AnsiString le);
        void __fastcall ProcessInput(const AnsiString remname, const AnsiString ipaddr);
        void __fastcall SetLoggingLevel(int i);

public:
        __fastcall TTransmissionServer(TComponent* Owner);
        __fastcall ~TTransmissionServer();

        void __fastcall Start();
        void __fastcall Stop();
        void __fastcall AddToQueue(TTransmissionMessage *m);
        TTransmissionMessage * __fastcall Dequeue();

__published:
        __property bool Active = {read=FActive};
        __property bool EnableBlockList = {read=FEnableBlockList, write=SetEnableBlockList, default=false};
        __property int Port = {read=FPort, write=SetPort, default=0};
        __property int QueuedMsgCount = {read=FQueuedMsgCount};
        __property int DroppedMsgCount = {read=FDroppedMsgCount};
        __property int LoggingLevel = {read=FLoggingLevel, write=SetLoggingLevel, default=LOG_LEVEL_CONNECTS};
        __property int ConnectCount = {read=FConnectCount};
        __property int BlockedConnectCount = {read=FBlockedConnectCount};
        __property TStringList *BlockedHosts = {read=GetBlockedHosts, write=SetBlockedHosts};

        __property TNotifyNewMsgEvent OnNewMessage = {read=FOnNewMessage, write=FOnNewMessage};
        __property TNotifyEvent OnMessageQueued = {read=FOnMessageQueued, write=FOnMessageQueued};
        __property TLogNotifyEvent OnLogEvent = {read=FOnLogEvent, write=FOnLogEvent};
};
//---------------------------------------------------------------------------
#endif
