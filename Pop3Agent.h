//---------------------------------------------------------------------------
#ifndef Pop3AgentH
#define Pop3AgentH
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
class PACKAGE TPop3Agent : public TComponent
{
private:
        TNotifyEvent FOnInitRequired;
        TNotifyEvent FOnAgentBegun;
        TNotifyEvent FOnAgentCompleted;
        TNotifyEvent FOnConnected;
        TNotifyEvent FOnDisconnected;
        TLogNotifyEvent FOnLogEvent;
        TNotifyEvent FOnNoNewMessages;
        TNotifyEvent FOnRetrievalStart;
        TNotifyEvent FOnRetrievalEnd;
        TNotifyEvent FOnSessionClose;
        TMessageNotifyEvent FOnMessageStart;
        TMessageNotifyEvent FOnMessageEnd;
        TMessageChunkNotifyEvent FOnMessageChunk;
        TMessageNameNotifyEvent FOnMessageBeforeSave;
        TMessageSaveNotifyEvent FOnMessageSave;
        TMessageNotifyEvent FOnMessageAfterSave;

        bool FActive;
        int FPort;
        int FLoggingLevel;
        int FRetrievalOrder;
        AnsiString *FHost;
        AnsiString *FUserID;
        AnsiString *FPassword;
        AnsiString *FMailDir;
        AnsiString *FMailFilePrefix;
        AnsiString *FMailFileExt;

        TClientSocket *FMailSocket;

        bool FOverwriteExisting;
        bool FDeleteOnRetrieve;
        bool FAlternateMailstore;
        int FNumberOfMessages;  // Reported number of messages
        long FMailboxSize;    // Reported total bytes of messages
        int FCurrMsg;
        long FCurrSize;
        int CurrDelete;

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
        void __fastcall SaveMessage(TMemoryStream *ms, AnsiString filename);
        bool __fastcall ListEnd(TMemoryStream *ms);

protected:
        void __fastcall SetActive(bool act);
        void __fastcall SetLoggingLevel(int l);
        void __fastcall SetRetrievalOrder(int ro);
        void __fastcall SetOverwriteExisting(bool oe);
        int  __fastcall GetPort();
        void __fastcall SetPortA(int p);
        AnsiString __fastcall GetHost();
        void __fastcall SetHost(AnsiString h);
        AnsiString __fastcall GetMailDir();
        void __fastcall SetMailDir(AnsiString md);
        AnsiString _fastcall GetMailFileExt();
        void __fastcall SetMailFileExt(AnsiString mfe);
        AnsiString __fastcall GetMailFilePrefix();
        void __fastcall SetMailFilePrefix(AnsiString mfp);
        AnsiString __fastcall GetUserID();
        void __fastcall SetUserID(AnsiString uid);
        AnsiString __fastcall GetPassword();
        void __fastcall SetPassword(AnsiString pwd);
        void __fastcall SetAlternateMailstore(bool ams);

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
        void __fastcall ShowHiddenFiles();

public:
        __fastcall TPop3Agent(TComponent* Owner);
        __fastcall ~TPop3Agent();

        void __fastcall Execute();
        void __fastcall Abort();

__published:
        __property bool Active = {read=FActive, write=SetActive, default=false};
        __property int Port = {read=GetPort, write=SetPortA, default=0};
        __property int NumberOfMessages = {read=FNumberOfMessages};
        __property long MailboxSize = {read=FMailboxSize};
        __property int RetrievalOrder = {read=FRetrievalOrder, write=SetRetrievalOrder, default=P3RO_DEFAULT};
        __property bool OverwriteExisting = {read=FOverwriteExisting, write=SetOverwriteExisting, default=false};
        __property bool DeleteOnRetrieve = {read=FDeleteOnRetrieve, write=FDeleteOnRetrieve, default=false};
        __property bool AlternateMailstore = {read=FAlternateMailstore, write=SetAlternateMailstore, default=false};
        __property int LoggingLevel = {read=FLoggingLevel, write=SetLoggingLevel, default=P3A_LOG_CONNECTS};
        __property AnsiString Host = {read=GetHost, write=SetHost};
        __property AnsiString MailDir = {read=GetMailDir, write=SetMailDir};
        __property AnsiString MailFileExt = {read=GetMailFileExt, write=SetMailFileExt};
        __property AnsiString MailFilePrefix = {read=GetMailFilePrefix, write=SetMailFilePrefix};
        __property AnsiString UserID = {read=GetUserID, write=SetUserID};
        __property AnsiString Password = {read=GetPassword, write=SetPassword};

        __property TNotifyEvent OnInitRequired = {read=FOnInitRequired, write=FOnInitRequired};
        __property TNotifyEvent OnAgentBegun = {read=FOnAgentBegun, write=FOnAgentBegun};
        __property TNotifyEvent OnAgentCompleted = {read=FOnAgentCompleted, write=FOnAgentCompleted};
        __property TNotifyEvent OnConnected = {read=FOnConnected, write=FOnConnected};
        __property TNotifyEvent OnDisconnected = {read=FOnDisconnected, write=FOnDisconnected};
        __property TLogNotifyEvent OnLogEvent = {read=FOnLogEvent, write=FOnLogEvent};
        __property TNotifyEvent OnNoNewMessages = {read=FOnNoNewMessages, write=FOnNoNewMessages};
        __property TNotifyEvent OnRetrievalStart = {read=FOnRetrievalStart, write=FOnRetrievalStart};
        __property TNotifyEvent OnRetrievalEnd = {read=FOnRetrievalEnd, write=FOnRetrievalEnd};
        __property TNotifyEvent OnSessionClose = {read=FOnSessionClose,write=FOnSessionClose};
        __property TMessageNotifyEvent OnMessageStart = {read=FOnMessageStart, write=FOnMessageStart};
        __property TMessageNotifyEvent OnMessageEnd = {read=FOnMessageEnd, write=FOnMessageEnd};
        __property TMessageChunkNotifyEvent OnMessageChunk = {read=FOnMessageChunk, write=FOnMessageChunk};
        __property TMessageNameNotifyEvent OnMessageBeforeSave = {read=FOnMessageBeforeSave, write=FOnMessageBeforeSave};
        __property TMessageNotifyEvent OnMessageAfterSave = {read=FOnMessageAfterSave, write=FOnMessageAfterSave};
        __property TMessageSaveNotifyEvent OnMessageSave = {read=FOnMessageSave,write=FOnMessageSave};
};
//---------------------------------------------------------------------------
#endif
