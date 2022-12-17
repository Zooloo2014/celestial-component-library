//---------------------------------------------------------------------------
#ifndef SmtpServerH
#define SmtpServerH
//---------------------------------------------------------------------------
#include <stdlib.h>
#include <SysUtils.hpp>
#include <Controls.hpp>
#include <Classes.hpp>
#include <Forms.hpp>
#include <scktComp.hpp>
#include "LMTSTypes.h"
#include "Tokenizer.h"
#include "CmdStatsInfo.h"
#include "MailIdentity.h"
#include "AddressBook.h"
#include "MailHeaders.h"
//---------------------------------------------------------------------------
class PACKAGE TSmtpServer : public TComponent
{
private:
        TNotifyEvent FOnServerStarted;
        TNotifyEvent FOnServerStopped;
        TLogNotifyEvent FOnLogEvent;
        TClientNotifyEvent FOnClientConnected;
        TClientNotifyEvent FOnClientDisconnected;
        THostValidationEvent FOnHostValidationRequired;
        TUserValidationEvent FOnSenderValidationRequired;
        TUserValidationEvent FOnRecipientValidationRequired;
        TMessageBeforeSaveEvent FOnMessageBeforeSave;
        TMessageAfterSaveEvent FOnMessageAfterSave;
        TValidateMaxRecipients FOnMaxRecipientsCheck;
        TValidateMaxMsgSize FOnMaxMsgSizeCheck;

        TServerSocket *FServerSocket;
        TTokenizer *Tokens;
        TSmtpServerState ServerState;
        int FLoggingLevel;
        int FMessagesHandled;
        long FBytesHandled;

        AnsiString *FMailDir;
        AnsiString *FMailFileExt;
        AnsiString *FMailFilePrefix;
        AnsiString *FMyName;
        bool FActive;
        int FPort;

        TMailIdentity *UserFrom;
        TAddressBook *UsersTo;

        long FCurrSize;
        TMemoryStream *MailMsg;
        TMailHeaders *MsgHeaders;

        AnsiString *ClientName;
        AnsiString *RecvData;
        AnsiString *SendData;
        AnsiString *Command;
        AnsiString *Parm1;
        AnsiString *Parm2;

        TCmdStatsInfo *Statistics;

protected:
        void __fastcall SetActive(bool act);
        void __fastcall SetLoggingLevel(int i);
        void __fastcall SetPortA(int p);
        AnsiString __fastcall GetMailDir();
        AnsiString __fastcall GetMyName();
        void __fastcall SetMyName(AnsiString mn);
        void __fastcall SetMailDir(AnsiString md);
        AnsiString __fastcall GetMailFileExt();
        void __fastcall SetMailFileExt(AnsiString mfe);
        AnsiString __fastcall GetMailFilePrefix();
        void __fastcall SetMailFilePrefix(AnsiString mfp);

        void __fastcall EventClientConnect(TObject* Sender, TCustomWinSocket* Socket);
        void __fastcall EventClientDisconnect(TObject* Sender, TCustomWinSocket* Socket);
        void __fastcall EventClientRead(TObject* Sender, TCustomWinSocket* Socket);
        void __fastcall EventClientWrite(TObject* Sender, TCustomWinSocket* Socket);
        void __fastcall EventClientError(TObject* Sender, TCustomWinSocket* Socket, TErrorEvent ErrorEvent, int &ErrorCode);

        void __fastcall LogEvent(const AnsiString le);
        void __fastcall ProcessInput(TCustomWinSocket* Socket);

        void __fastcall InitStatistics();
//        bool __fastcall MsgCompleted();
        bool __fastcall MsgCompleted(TMemoryStream *ms);

        void __fastcall GetMailFilename(AnsiString *newname);
        void __fastcall SaveFile(TMemoryStream *s, AnsiString src, AnsiString newname);
        void __fastcall ResetState();
        void __fastcall ResetMsg();
        void __fastcall CreateMsgIdentifier(AnsiString *mid);

public:
        __fastcall TSmtpServer(TComponent* Owner);
        __fastcall ~TSmtpServer();

        void __fastcall Start();
        void __fastcall Stop();

__published:
        __property bool Active = {read=FActive, write=SetActive, default=false};
        __property int Port = {read=FPort, write=SetPortA, default=0};
        __property int LoggingLevel = {read=FLoggingLevel, write=SetLoggingLevel, default=SMTP_LOG_CONNECTS};
        __property int MessagesHandled = {read=FMessagesHandled};
        __property long BytesHandled = {read=FBytesHandled};
        __property AnsiString MyName = {read=GetMyName, write=SetMyName};
        __property AnsiString MailDir = {read=GetMailDir, write=SetMailDir};
        __property AnsiString MailFileExt = {read=GetMailFileExt, write=SetMailFileExt};
        __property AnsiString MailFilePrefix = {read=GetMailFilePrefix, write=SetMailFilePrefix};

        __property TClientNotifyEvent OnClientConnected = {read=FOnClientConnected, write=FOnClientConnected};
        __property TClientNotifyEvent OnClientDisconnected = {read=FOnClientDisconnected, write=FOnClientDisconnected};
        __property THostValidationEvent OnHostValidationRequired = {read=FOnHostValidationRequired, write=FOnHostValidationRequired};
        __property TUserValidationEvent OnSenderValidationRequired = {read=FOnSenderValidationRequired, write=FOnSenderValidationRequired};
        __property TUserValidationEvent OnRecipientValidationRequired = {read=FOnRecipientValidationRequired, write=FOnRecipientValidationRequired};
        __property TMessageBeforeSaveEvent OnMessageBeforeSave = {read=FOnMessageBeforeSave, write=FOnMessageBeforeSave};
        __property TMessageAfterSaveEvent OnMessageAfterSave = {read=FOnMessageAfterSave, write=FOnMessageAfterSave};
        __property TLogNotifyEvent OnLogEvent = {read=FOnLogEvent, write=FOnLogEvent};
        __property TNotifyEvent OnServerStarted = {read=FOnServerStarted, write=FOnServerStarted};
        __property TNotifyEvent OnServerStopped = {read=FOnServerStopped, write=FOnServerStopped};
        __property TValidateMaxRecipients OnMaxRecipientsCheck = {read=FOnMaxRecipientsCheck, write=FOnMaxRecipientsCheck};
        __property TValidateMaxMsgSize OnMaxMsgSizeCheck = {read=FOnMaxMsgSizeCheck, write=FOnMaxMsgSizeCheck};
};
//---------------------------------------------------------------------------
#endif
