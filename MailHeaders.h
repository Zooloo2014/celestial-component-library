//---------------------------------------------------------------------------
#ifndef MailHeadersH
#define MailHeadersH
#include <stdlib.h>
#include <SysUtils.hpp>
#include <Controls.hpp>
#include <Classes.hpp>
#include <Forms.hpp>
#include "MailIdentity.h"
#include "AddressBook.h"
//---------------------------------------------------------------------------
class PACKAGE TMailHeaders : public TComponent
{
private:
         TStringList *Headers;

         AnsiString *MessageID;
         AnsiString *Priority;
         AnsiString *Subject;
         TMailIdentity *ReturnPath;
         TMailIdentity *From;
         TMailIdentity *ReplyTo;
         TAddressBook *Recipients;
         TDateTime MsgDate;
         double MimeVersion;

protected:
         void __fastcall Construct();
         void __fastcall DeleteMsgData();
         int  __fastcall GetHeaderIndex(AnsiString hdr);

         void __fastcall ParseHeaders();
         AnsiString * __fastcall GetHeaderInfo(AnsiString hdr);
         void __fastcall UpdateHeader(AnsiString hdr, AnsiString data);
         void __fastcall AddHeader(AnsiString hdr, AnsiString data);
         void __fastcall AddHeaderAt(int i, AnsiString hdr, AnsiString data);
         void __fastcall DeleteHeaderAt(int i);
         void __fastcall MoveHeaderTo(int from, int to);

public:
         __fastcall TMailHeaders(TComponent *Owner);
         __fastcall TMailHeaders(TComponent *Owner, TStringList *Message);
         __fastcall TMailHeaders(TComponent *Owner, AnsiString Message);
         __fastcall TMailHeaders(TComponent *Owner, TMemoryStream *Message);
         __fastcall ~TMailHeaders();

         void __fastcall SetHeaders(TStringList *Message);
         void __fastcall SetHeaders(AnsiString Filename);
         void __fastcall SetHeaders(TMemoryStream *Headers);
         void __fastcall GetHeaders(TStringList *HeaderList);
         void __fastcall Dump(AnsiString KeyFile, AnsiString ValuesFile);

         void __fastcall Clear();
         int  __fastcall GetHeaderCount();
         void __fastcall GetHeader(AnsiString hdr, TStringList *HeaderList);
         void __fastcall GetHeaderEx(AnsiString hdr, TStringList *HeaderList);
         void __fastcall GetAllHeaders(TStringList *Parms, TStringList *Values);
         void __fastcall SetReturnPath(AnsiString hdr);
         void __fastcall SetLatestReceived(AnsiString hdr);

         AnsiString __fastcall GetMessageID();
         AnsiString __fastcall GetPriority();
         AnsiString __fastcall GetSubject();
         TMailIdentity * __fastcall GetReturnPath();
         TMailIdentity * __fastcall GetFrom();
         TMailIdentity * __fastcall GetReplyTo();
         TAddressBook *  __fastcall GetRecipients();
         TDateTime __fastcall GetMsgDate();
         double __fastcall GetMimeVersion();
};
//---------------------------------------------------------------------------
#endif
