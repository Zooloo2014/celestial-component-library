//---------------------------------------------------------------------------
#ifndef NewMailNotifierH
#define NewMailNotifierH
//---------------------------------------------------------------------------
#include <stdlib.h>
#include <SysUtils.hpp>
#include <Controls.hpp>
#include <Classes.hpp>
#include <Forms.hpp>
#include <ScktComp.hpp>
#include "LMTSTypes.h"
#include "Tokenizer.h"
//---------------------------------------------------------------------------
#define P3RO_DEFAULT            0
#define P3RO_SMALLEST_FIRST     1
//---------------------------------------------------------------------------
typedef void __fastcall (__closure *TNotifierErrorEvent)(System::TObject* Sender, TErrorEvent ErrorEvent, int ErrorCode);
//---------------------------------------------------------------------------
class PACKAGE TNewMailNotifier : public TComponent
{
private:
        TNotifyEvent FOnConnected;
        TNotifyEvent FOnDisconnected;
        TNotifierErrorEvent FOnNotifierError;
        TLogNotifyEvent FOnLogEvent;
        TNotifyEvent FOnGotInfo;

        bool FActive;
        int FPort;
        int FLoggingLevel;
        int FRetrievalOrder;
        AnsiString *FHost;
        AnsiString *FUserID;
        AnsiString *FPassword;

        TClientSocket *FMailSocket;

        int FNumberOfMessages;  // Reported number of messages
        long FMailboxSize;    // Reported total bytes of messages
        int FCurrMsg;
        long FCurrSize;
        int CurrDelete;

        int Step;
        TTokenizer *Tokens;
        AnsiString *RecvData;
        AnsiString *SendData;

protected:
        void __fastcall SetActive(bool act);
        void __fastcall SetLoggingLevel(int l);
        void __fastcall SetPort(int p);
        AnsiString __fastcall GetHost();
        void __fastcall SetHost(AnsiString h);
        AnsiString __fastcall GetUserID();
        void __fastcall SetUserID(AnsiString uid);
        AnsiString __fastcall GetPassword();
        void __fastcall SetPassword(AnsiString pwd);

        void __fastcall EventLookup(TObject* Sender, TCustomWinSocket* Socket);
        void __fastcall EventConnect(TObject* Sender, TCustomWinSocket* Socket);
        void __fastcall EventDisconnect(TObject* Sender, TCustomWinSocket* Socket);
        void __fastcall EventConnecting(TObject* Sender, TCustomWinSocket* Socket);
        void __fastcall EventError(TObject* Sender, TCustomWinSocket* Socket, TErrorEvent ErrorEvent, int &ErrorCode);
        void __fastcall EventRead(TObject* Sender, TCustomWinSocket* Socket);
        void __fastcall EventWrite(TObject* Sender, TCustomWinSocket* Socket);

        bool __fastcall Successful();
        bool __fastcall Unsuccessful();
        void __fastcall LogEvent(const AnsiString le);
        void __fastcall ProcessInput(TCustomWinSocket* Socket);

public:
        __fastcall TNewMailNotifier(TComponent* Owner);
        __fastcall ~TNewMailNotifier();

        void __fastcall Execute();
        void __fastcall Abort();

__published:
        __property bool Active = {read=FActive, write=SetActive, default=false};
        __property int Port = {read=FPort, write=SetPort, default=0};
        __property int NumberOfMessages = {read=FNumberOfMessages};
        __property long MailboxSize = {read=FMailboxSize};
        __property int LoggingLevel = {read=FLoggingLevel, write=SetLoggingLevel, default=P3A_LOG_CONNECTS};
        __property AnsiString Host = {read=GetHost, write=SetHost};
        __property AnsiString UserID = {read=GetUserID, write=SetUserID};
        __property AnsiString Password = {read=GetPassword, write=SetPassword};

        __property TNotifyEvent OnConnected = {read=FOnConnected, write=FOnConnected};
        __property TNotifyEvent OnDisconnected = {read=FOnDisconnected, write=FOnDisconnected};
        __property TNotifierErrorEvent OnNotifierError = {read=FOnNotifierError, write=FOnNotifierError};
        __property TLogNotifyEvent OnLogEvent = {read=FOnLogEvent, write=FOnLogEvent};
        __property TNotifyEvent OnGotInfo = {read=FOnGotInfo, write=FOnGotInfo};
};
//---------------------------------------------------------------------------
#endif
