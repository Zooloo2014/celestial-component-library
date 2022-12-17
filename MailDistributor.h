//---------------------------------------------------------------------------
#ifndef MailDistributorH
#define MailDistributorH
//---------------------------------------------------------------------------
#include <stdlib.h>
#include <SysUtils.hpp>
#include <Controls.hpp>
#include <Classes.hpp>
#include <Forms.hpp>
#include "MailIdentity.h"
#include "MailHeaders.h"
#include "AddressBook.h"
//---------------------------------------------------------------------------
#define BUF_SIZE (8 * 1024)
//---------------------------------------------------------------------------
typedef void __fastcall (__closure *TDistMsgFileExistsEvent)(System::TObject* Sender, AnsiString *Filename);
typedef void __fastcall (__closure *TDistMsgDeadEvent)(System::TObject* Sender, AnsiString Filename);
typedef void __fastcall (__closure *TDistMsgSaveEvent)(System::TObject* Sender, AnsiString Recipient, AnsiString *Filename, bool& Save);
typedef void __fastcall (__closure *TDistMsgUserEvent)(System::TObject* Sender, AnsiString Recipient, AnsiString *UserID, bool &Valid);
typedef void __fastcall (__closure *TDistMsgLoadedEvent)(System::TObject* Sender, long MessageSize);
typedef void __fastcall (__closure *TDistMsgDistEvent)(System::TObject* Sender, int MessageNumber);
typedef void __fastcall (__closure *TDistLogEvent)(System::TObject* Sender, const AnsiString le);
//---------------------------------------------------------------------------
class PACKAGE TMailDistributor : public TComponent
{
private:
        TDistMsgLoadedEvent FOnMessageLoaded;
        TDistMsgDistEvent FOnMessageDistributed;
        TDistMsgUserEvent FOnMessageValidateRecipient;
        TDistMsgSaveEvent FOnMessageBeforeSave;
        TDistMsgFileExistsEvent FOnMessageFileExists;
        TDistMsgDeadEvent FOnDeadLetter;
        TDistLogEvent FOnLogEvent;

        bool FActive;
        bool FParseOnly;
        bool FAllowEmptyUID;
        bool FArchiveAsDelete;
        AnsiString *FSourceDir;
        AnsiString *FDestinationDir;
        AnsiString *FArchiveDir;
        AnsiString *FDeadLetterDir;
        AnsiString *FMailFileExt;
        int FNumberOfMessages;

        TStringList *FileList;
        TMailHeaders *FMailHeaders;

        TStringList *MessageHeaders;
        TMailIdentity *AddrFrom;
        TMailIdentity *AddrDeliveredTo;
        TAddressBook *AddrTo;
        TAddressBook *AddrCc;
        TAddressBook *AddrBcc;

protected:
        void __fastcall GetMailFiles();
        void __fastcall SetParseOnly(bool po);
        void __fastcall SetAllowEmptyUID(bool eu);
        void __fastcall SetArchiveAsDelete(bool aad);
        AnsiString __fastcall GetSourceDir();
        void __fastcall SetSourceDir(AnsiString sf);
        AnsiString __fastcall GetDestinationDir();
        void __fastcall SetDestinationDir(AnsiString df);
        AnsiString __fastcall GetArchiveDir();
        void __fastcall SetArchiveDir(AnsiString ad);
        AnsiString __fastcall GetDeadLetterDir();
        void __fastcall SetDeadLetterDir(AnsiString dld);
        AnsiString __fastcall GetMailFileExt();
        void __fastcall SetMailFileExt(AnsiString me);
        void __fastcall DistributeMessage(AnsiString oldname);
        void __fastcall DeleteMsgFile(AnsiString fname);
        void __fastcall LogEvent(AnsiString le);

public:
        __fastcall TMailDistributor(TComponent* Owner);
        __fastcall ~TMailDistributor();

        void __fastcall Clear();
        void __fastcall Distribute();
        void __fastcall AssignToUser(AnsiString MsgFile, AnsiString uid);

__published:
        __property bool Active = {read=FActive};
        __property AnsiString SourceDir = {read=GetSourceDir, write=SetSourceDir};
        __property AnsiString DestinationDir = {read=GetDestinationDir, write=SetDestinationDir};
        __property AnsiString ArchiveDir = {read=GetArchiveDir, write=SetArchiveDir};
        __property AnsiString DeadLetterDir = {read=GetDeadLetterDir, write=SetDeadLetterDir};
        __property AnsiString MailFileExt = {read=GetMailFileExt, write=SetMailFileExt};
        __property bool ParseOnly = {read=FParseOnly, write=SetParseOnly, default=true};
        __property bool AllowEmptyUID = {read=FAllowEmptyUID, write=SetAllowEmptyUID, default=false};
        __property bool ArchiveAsDelete = {read=FArchiveAsDelete, write=SetArchiveAsDelete, default=false};
        __property int NumberOfMessages = {read=FNumberOfMessages};

        __property TDistMsgLoadedEvent OnMessageLoaded = {read=FOnMessageLoaded, write=FOnMessageLoaded};
        __property TDistMsgDistEvent OnMessageDistributed = {read=FOnMessageDistributed, write=FOnMessageDistributed};
        __property TDistMsgUserEvent OnMessageValidateRecipient = {read=FOnMessageValidateRecipient, write=FOnMessageValidateRecipient};
        __property TDistMsgSaveEvent OnMessageBeforeSave = {read=FOnMessageBeforeSave, write=FOnMessageBeforeSave};
        __property TDistMsgFileExistsEvent OnMessageFileExists = {read=FOnMessageFileExists, write=FOnMessageFileExists};
        __property TDistMsgDeadEvent OnDeadLetter = {read=FOnDeadLetter, write=FOnDeadLetter};
        __property TDistLogEvent OnLogEvent = {read=FOnLogEvent, write=FOnLogEvent};
};
//---------------------------------------------------------------------------
#endif
