//---------------------------------------------------------------------------
#ifndef Pop3ServerH
#define Pop3ServerH
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
#include "MailHeaders.h"
//---------------------------------------------------------------------------
class PACKAGE TPop3Server : public TComponent
{
private:
        TNotifyEvent FOnServerStarted;
        TNotifyEvent FOnServerStopped;
        TLogNotifyEvent FOnLogEvent;
        TClientNotifyEvent FOnClientConnected;
        TClientNotifyEvent FOnClientDisconnected;
        TUserValidationEvent FOnUserValidationRequired;
        TPassValidationEvent FOnPassValidationRequired;

        TServerSocket *FServerSocket;
        TTokenizer *Tokens;
        TPop3ServerState ServerState;

        AnsiString *FHostName;
        AnsiString *FMailDir;
        AnsiString *FMailFileExt;
        AnsiString *FMailFileArchive;
        bool FActive;
        bool FArchiveOnDelete;
        int FLoggingLevel;
        int FPort;

        bool UserOK;
        bool PassOK;
        AnsiString *Login;
        AnsiString *Password;

        int NumMsg;
        long ByteSize;
        TStringList *ServeMail;
        TStringList *DeleteMail;

        AnsiString *RecvData;
        AnsiString *SendData;
        AnsiString *Command;
        AnsiString *Parm1;
        AnsiString *Parm2;
        TStringList *MsgHeaders;
        TMailHeaders *MailHeaders;

        TCmdStatsInfo *Statistics;

protected:
        void __fastcall SetActive(bool act);
        void __fastcall SetArchiveOnDelete(bool aod);
        void __fastcall SetPortA(int p);
        void __fastcall SetLoggingLevel(int i);
        AnsiString __fastcall GetHostName();
        void __fastcall SetHostName(AnsiString hn);
        AnsiString __fastcall GetMailDir();
        void __fastcall SetMailDir(AnsiString md);
        AnsiString __fastcall GetMailFileExt();
        void __fastcall SetMailFileExt(AnsiString mfe);
        AnsiString __fastcall GetMailFileArchive();
        void __fastcall SetMailFileArchive(AnsiString mfa);

        void __fastcall EventClientConnect(TObject* Sender, TCustomWinSocket* Socket);
        void __fastcall EventClientDisconnect(TObject* Sender, TCustomWinSocket* Socket);
        void __fastcall EventClientRead(TObject* Sender, TCustomWinSocket* Socket);
        void __fastcall EventClientWrite(TObject* Sender, TCustomWinSocket* Socket);

        void __fastcall LogEvent(const AnsiString le);
        void __fastcall ProcessInput(TCustomWinSocket* Socket);
        void __fastcall ResetUser();
        void __fastcall GetMailboxStats();
        void __fastcall UpdateMailbox();
        void __fastcall LoadHeaders(AnsiString filename);
        void __fastcall GetMsgIdentifier(AnsiString *mid);

        void __fastcall InitStatistics();
        void __fastcall ArchiveFile(AnsiString fil);
        bool __fastcall MailboxIsLocked(AnsiString uid);
        void __fastcall LockMailbox(AnsiString uid);
        void __fastcall UnlockMailbox(AnsiString uid);
        void __fastcall UnlockAllMailboxes();

public:
        __fastcall TPop3Server(TComponent* Owner);
        __fastcall ~TPop3Server();

        void __fastcall Start();
        void __fastcall Stop();
        void __fastcall ResetServerStats();
        void __fastcall WriteServerStats(AnsiString fn);

__published:
        __property bool Active = {read=FActive, write=SetActive, default=false};
        __property bool ArchiveOnDelete = {read=FArchiveOnDelete, write=SetArchiveOnDelete, default=false};
        __property int Port = {read=FPort, write=SetPortA, default=0};
        __property int LoggingLevel = {read=FLoggingLevel, write=SetLoggingLevel, default=POP3_LOG_CONNECTS};
        __property AnsiString HostName = {read=GetHostName, write=SetHostName};
        __property AnsiString MailDir = {read=GetMailDir, write=SetMailDir};
        __property AnsiString MailFileArchive = {read=GetMailFileArchive, write=SetMailFileArchive};
        __property AnsiString MailFileExt = {read=GetMailFileExt, write=SetMailFileExt};

        __property TClientNotifyEvent OnClientConnected = {read=FOnClientConnected, write=FOnClientConnected};
        __property TClientNotifyEvent OnClientDisconnected = {read=FOnClientDisconnected, write=FOnClientDisconnected};
        __property TUserValidationEvent OnUserValidationRequired = {read=FOnUserValidationRequired, write=FOnUserValidationRequired};
        __property TPassValidationEvent OnPassValidationRequired = {read=FOnPassValidationRequired, write=FOnPassValidationRequired};
        __property TLogNotifyEvent OnLogEvent = {read=FOnLogEvent, write=FOnLogEvent};
        __property TNotifyEvent OnServerStarted = {read=FOnServerStarted, write=FOnServerStarted};
        __property TNotifyEvent OnServerStopped = {read=FOnServerStopped, write=FOnServerStopped};
};
//---------------------------------------------------------------------------
#endif
