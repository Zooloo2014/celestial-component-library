//---------------------------------------------------------------------------
#ifndef SmtpAgentH
#define SmtpAgentH
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
class PACKAGE TSmtpAgent : public TComponent
{
private:
        TNotifyEvent FOnInitRequired;
        TNotifyEvent FOnAgentBegun;
        TNotifyEvent FOnAgentCompleted;
        TNotifyEvent FOnConnected;
        TNotifyEvent FOnDisconnected;
        TLogNotifyEvent FOnLogEvent;
        TMessageNotifyEvent FOnTransmissionStart;
        TMessageNotifyEvent FOnTransmissionEnd;
        TValidateAddressNotifyEvent FOnValidateRecipient;
        TMessageChunkNotifyEvent FOnMessageChunk;

        bool FActive;
        int FPort;
        int FLoggingLevel;
        AnsiString *FAgentName;
        AnsiString *FHost;
        AnsiString *FUserID;
        AnsiString *FPassword;
        AnsiString *FMailDir;
        AnsiString *FMailFileExt;

        TClientSocket *FMailSocket;
        TStringList *FMessageNames;
        TStringList *MailRoute;
        TStringList *MailMsg;

        int FNumberOfMessages;  // Reported number of messages
        long FMailboxSize;    // Reported total bytes of messages
        int FCurrMsg;
        long FCurrSize;
        int CurrDelete;

        int Step;
        TTokenizer *Tokens;
        AnsiString *RecvData;
        AnsiString *SendData;

        bool Proceed;

protected:
        void __fastcall SetActive(bool act);
        void __fastcall SetLoggingLevel(int l);
        void __fastcall SetPort(int p);
        AnsiString __fastcall GetAgentName();
        void __fastcall SetAgentName(AnsiString an);
        AnsiString __fastcall GetHost();
        void __fastcall SetHost(AnsiString h);
        AnsiString __fastcall GetMailDir();
        void __fastcall SetMailDir(AnsiString md);
        AnsiString _fastcall GetMailFileExt();
        void __fastcall SetMailFileExt(AnsiString mfe);
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

        void __fastcall GetMessageNames(TStringList *mn);
        void __fastcall LoadMessage(AnsiString mn);
        bool __fastcall Successful();
        void __fastcall LogEvent(const AnsiString le);
        void __fastcall ProcessInput(TCustomWinSocket* Socket);
        void __fastcall ShowHiddenFiles();

public:
        __fastcall TSmtpAgent(TComponent* Owner);
        __fastcall ~TSmtpAgent();

        void __fastcall Execute();
        void __fastcall Abort();

__published:
        __property bool Active = {read=FActive, write=SetActive, default=false};
        __property int Port = {read=FPort, write=SetPort, default=0};
        __property int NumberOfMessages = {read=FNumberOfMessages};
        __property long MailboxSize = {read=FMailboxSize};
        __property int LoggingLevel = {read=FLoggingLevel, write=SetLoggingLevel, default=P3A_LOG_CONNECTS};
        __property AnsiString AgentName = {read=GetAgentName, write=SetAgentName};
        __property AnsiString Host = {read=GetHost, write=SetHost};
        __property AnsiString MailDir = {read=GetMailDir, write=SetMailDir};
        __property AnsiString MailFileExt = {read=GetMailFileExt, write=SetMailFileExt};
        __property AnsiString UserID = {read=GetUserID, write=SetUserID};
        __property AnsiString Password = {read=GetPassword, write=SetPassword};

        __property TNotifyEvent OnInitRequired = {read=FOnInitRequired, write=FOnInitRequired};
        __property TNotifyEvent OnAgentBegun = {read=FOnAgentBegun, write=FOnAgentBegun};
        __property TNotifyEvent OnAgentCompleted = {read=FOnAgentCompleted, write=FOnAgentCompleted};
        __property TNotifyEvent OnConnected = {read=FOnConnected, write=FOnConnected};
        __property TNotifyEvent OnDisconnected = {read=FOnDisconnected, write=FOnDisconnected};
        __property TLogNotifyEvent OnLogEvent = {read=FOnLogEvent, write=FOnLogEvent};
        __property TMessageNotifyEvent OnTransmissionStart = {read=FOnTransmissionStart, write=FOnTransmissionStart};
        __property TMessageNotifyEvent OnTransmissionEnd = {read=FOnTransmissionEnd, write=FOnTransmissionEnd};
        __property TValidateAddressNotifyEvent OnValidateRecipient = {read=FOnValidateRecipient, write=FOnValidateRecipient};
        __property TMessageChunkNotifyEvent OnMessageChunk = {read=FOnMessageChunk, write=FOnMessageChunk};
};
//---------------------------------------------------------------------------
#endif
