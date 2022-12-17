//---------------------------------------------------------------------------
#ifndef PopPeekerH
#define PopPeekerH
//---------------------------------------------------------------------------
#include <stdlib.h>
#include <SysUtils.hpp>
#include <Controls.hpp>
#include <Classes.hpp>
#include <Forms.hpp>
#include <ScktComp.hpp>
#include "LMTSTypes.h"
#include "Tokenizer.h"
#include "MailHeaders.h"
//---------------------------------------------------------------------------
class PACKAGE TPopPeeker : public TComponent
{
private:
        TNotifyEvent FOnInitRequired;
        TNotifyEvent FOnConnected;
        TNotifyEvent FOnDisconnected;
        TLogNotifyEvent FOnLogEvent;
        TNotifyEvent FOnNoNewMessages;
        TNotifyEvent FOnRetrievalStart;
        TNotifyEvent FOnRetrievalEnd;
        TNotifyEvent FOnSessionClose;
        TPeekerSizeEvent FOnSizeCheck;
        TPeekerDateEvent FOnDateCheck;
        TPeekerHeaderEvent FOnHeaderCheck;
        TPeekerEmailEvent FOnSenderCheck;

        bool FActive;
        bool FPurge;
        bool FBruteSizeRule;
        int FPort;
        int FLoggingLevel;
        int FRetrievalOrder;
        AnsiString *FHost;
        AnsiString *FUserID;
        AnsiString *FPassword;

        TClientSocket *FMailSocket;
        TStringList *DeletedMsgs;

        int FNumberOfMessages;  // Reported number of messages
        long FMailboxSize;    // Reported total bytes of messages
        int FCurrMsg;
        long FCurrSize;

        int Step;
        TTokenizer *Tokens;
        AnsiString *RecvData;
        AnsiString *SendData;

        AnsiString *TempStr;
        TMemoryStream *MailMsg;
        TMsgInfo *MsgInfo;
        TExpectType Expect;
        bool Proceed;

        bool __fastcall MakeInfoList(AnsiString *text);
        bool __fastcall ListEnd(TMemoryStream *ms);

protected:
        void __fastcall SetActive(bool act);
        void __fastcall SetPurge(bool p);
        void __fastcall SetBruteSizeRule(bool b);
        void __fastcall SetLoggingLevel(int l);
        void __fastcall SetRetrievalOrder(int ro);
        void __fastcall SetOverwriteExisting(bool oe);
        int  __fastcall GetPort();
//        void __fastcall SetPort(int p);
        void __fastcall SetPortA(int p);
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
        void __fastcall LogEvent(const AnsiString le);
        void __fastcall ProcessInput(TCustomWinSocket* Socket);
        void __fastcall GenerateFilename(AnsiString *Newname);

public:
        __fastcall TPopPeeker(TComponent* Owner);
        __fastcall ~TPopPeeker();

        void __fastcall Execute();
        void __fastcall Abort();

__published:
        __property bool Active = {read=FActive, write=SetActive, default=false};
        __property bool Purge = {read=FPurge, write=SetPurge, default=false};
        __property bool BruteSizeRule = {read=FBruteSizeRule, write=SetBruteSizeRule, default=false};
        __property int Port = {read=GetPort, write=SetPortA, default=0};
        __property int NumberOfMessages = {read=FNumberOfMessages};
        __property long MailboxSize = {read=FMailboxSize};
        __property int RetrievalOrder = {read=FRetrievalOrder, write=SetRetrievalOrder, default=P3RO_DEFAULT};
        __property int LoggingLevel = {read=FLoggingLevel, write=SetLoggingLevel, default=P3A_LOG_CONNECTS};
        __property AnsiString Host = {read=GetHost, write=SetHost};
        __property AnsiString UserID = {read=GetUserID, write=SetUserID};
        __property AnsiString Password = {read=GetPassword, write=SetPassword};

        __property TNotifyEvent OnInitRequired = {read=FOnInitRequired, write=FOnInitRequired};
        __property TNotifyEvent OnConnected = {read=FOnConnected, write=FOnConnected};
        __property TNotifyEvent OnDisconnected = {read=FOnDisconnected, write=FOnDisconnected};
        __property TLogNotifyEvent OnLogEvent = {read=FOnLogEvent, write=FOnLogEvent};
        __property TNotifyEvent OnNoNewMessages = {read=FOnNoNewMessages, write=FOnNoNewMessages};
        __property TNotifyEvent OnRetrievalStart = {read=FOnRetrievalStart, write=FOnRetrievalStart};
        __property TNotifyEvent OnRetrievalEnd = {read=FOnRetrievalEnd, write=FOnRetrievalEnd};
        __property TNotifyEvent OnSessionClose = {read=FOnSessionClose, write=FOnSessionClose};
        __property TPeekerSizeEvent OnSizeCheck = {read=FOnSizeCheck, write=FOnSizeCheck};
        __property TPeekerHeaderEvent OnHeaderCheck = {read=FOnHeaderCheck, write=FOnHeaderCheck};
        __property TPeekerEmailEvent OnSenderCheck = {read=FOnSenderCheck, write=FOnSenderCheck};
        __property TPeekerDateEvent OnDateCheck = {read=FOnDateCheck, write=FOnDateCheck};
};
//---------------------------------------------------------------------------
#endif
