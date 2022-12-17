//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "MailHeaders.h"
#include "Tokenizer.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//

static inline void ValidCtrCheck(TMailHeaders *)
{
        new TMailHeaders(NULL);
}
//---------------------------------------------------------------------------
__fastcall TMailHeaders::TMailHeaders(TComponent *Owner)
           : TComponent(Owner)
{
  Construct();
}
//---------------------------------------------------------------------------
__fastcall TMailHeaders::TMailHeaders(TComponent *Owner, TStringList *Message)
           : TComponent(Owner)
{
  Construct();
  SetHeaders(Message);
}
//---------------------------------------------------------------------------
__fastcall TMailHeaders::TMailHeaders(TComponent *Owner, AnsiString Message)
           : TComponent(Owner)
{
  Construct();
  Headers->Text = Message;
  DeleteMsgData();
  ParseHeaders();
}
//---------------------------------------------------------------------------
__fastcall TMailHeaders::TMailHeaders(TComponent *Owner, TMemoryStream *Message)
           : TComponent(Owner)
{
  Construct();
  Headers->LoadFromStream(Message);
  DeleteMsgData();
  ParseHeaders();
}
//---------------------------------------------------------------------------
void __fastcall TMailHeaders::Construct()
{
  Headers = new TStringList();
  MessageID = new AnsiString();
  Priority = new AnsiString();
  Subject = new AnsiString();
  ReturnPath = new TMailIdentity(this);
  From = new TMailIdentity(this);
  ReplyTo = new TMailIdentity(this);
  Recipients = new TAddressBook(this);
}
//---------------------------------------------------------------------------
__fastcall TMailHeaders::~TMailHeaders()
{
  delete Headers;
  delete MessageID;
  delete Priority;
  delete Subject;
  delete ReturnPath;
  delete From;
  delete ReplyTo;
  delete Recipients;
}
//---------------------------------------------------------------------------
namespace Mailheaders
{
        void __fastcall PACKAGE Register()
        {
                 TComponentClass classes[1] = {__classid(TMailHeaders)};
                 RegisterComponents("Celestial", classes, 0);
        }
}
//---------------------------------------------------------------------------
void __fastcall TMailHeaders::SetHeaders(TStringList *Message)
{
  Clear();

  for (int i = 0; i < Message->Count; i++) {
    if (Message->Strings[i] == "") break;
    Headers->Add(Message->Strings[i]);
  }

  DeleteMsgData();
  ParseHeaders();
}
//---------------------------------------------------------------------------
void __fastcall TMailHeaders::SetHeaders(TMemoryStream *Headers)
{
  TStringList *tl = new TStringList();
  int OldPos = Headers->Position;

  Headers->Seek(0, soFromBeginning);
  tl->LoadFromStream(Headers);
  SetHeaders(tl);

  Headers->Seek(OldPos, soFromBeginning);

  delete tl;
}
//---------------------------------------------------------------------------
void __fastcall TMailHeaders::SetHeaders(AnsiString Filename)
{
  TFileStream *fs = new TFileStream(Filename, fmOpenRead);
  TMemoryStream *ms = new TMemoryStream();
  int StreamSize = 16384;
  char *Buf;

  Buf = (char *)malloc(StreamSize);
  ms->SetSize(StreamSize);

  if (Buf != NULL) {
    Clear();
    memset((char *)Buf, 0, StreamSize);

    bool Found = false;
    int r = StreamSize;
    int s;
    unsigned int f = 0;

    while ((!Found) && (r >= StreamSize)) {
      r = fs->Read((char *)Buf, StreamSize);
      s = 0;

      while ((!Found) && (s < r)) {
        ms->Write(&Buf[s], 1);

        if ((char)Buf[s] == '\r')
          f |= ((f & 0x0008) == 0x0008 ? 0x0002 : 0x0008);
        else
          if ((char)Buf[s] == '\n')
            f |= ((f & 0x0004) == 0x0004 ? 0x0001 : 0x0004);
          else
            f = 0;

        Found = (f == 0x0000000f);
        s++;
      }
    }

    if (Found) {
      ms->Position = 0;
      SetHeaders(ms);
    }

    free(Buf);
  }

  delete ms;
  delete fs;
}
//---------------------------------------------------------------------------
void __fastcall TMailHeaders::DeleteMsgData()
{
  if (Headers->Count > 0) {
    int StopLine = -1;
    for (int i = 0; i < Headers->Count; i++) {
      if (Headers->Strings[i] == "") {
        StopLine = i;
        break;
      }
    }

    if (StopLine > -1) {
      while (Headers->Count > StopLine)
        Headers->Delete(StopLine);
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TMailHeaders::ParseHeaders()
{
  TStringList *tl = new TStringList();
  AnsiString *data = new AnsiString();


  for (int i = 0; i < Headers->Count; i++) {
    if (Headers->Strings[i] == "")
      break;

    if ((Headers->Strings[i].SubString(1, 1) > " ") && (Headers->Strings[i].SubString(1, 1) < "z")) {
      if (*data != "")
        tl->Add(*data);
      *data = Headers->Strings[i];
    } else
      *data += Headers->Strings[i] + "\t";
  }

  if (*data != "")
    tl->Add(*data);


  Headers->Clear();
  for (int i = 0; i < tl->Count; i++) {
    if ((i == (tl->Count - 1)) && (tl->Strings[i] == "."))
      continue;
    if (tl->Strings[i].SubString(1, 4) != "+OK ")
      Headers->Add(tl->Strings[i]);
  }


  int j;
  for (int i = 0; i < Headers->Count; i++) {
    *data = Headers->Strings[i];
    j = data->AnsiPos(":");
    if (j == 0)
      throw Exception("[Line " + IntToStr(i + 1) + "] Invalid mail header! [" + *data + "]");

    Headers->Strings[i] = Trim(data->SubString(1, j));
    data->Delete(1, j);

    AnsiString *ptr = new AnsiString();
    *ptr = Trim(*data);
    Headers->Objects[i] = (TObject *)ptr;
  }


  AnsiString *inf;

  int f = Headers->IndexOf("Return-Path:");
  if (f > -1) {
    inf = (AnsiString *)Headers->Objects[f];
    ReturnPath->AddAddress(*inf);
    ReturnPath->DispName = *inf;
  }

  f = Headers->IndexOf("From:");
  if (f > -1) {
    inf = (AnsiString *)Headers->Objects[f];
    From->AddAddress(*inf);
    From->DispName = *inf;
  }

  f = Headers->IndexOf("To:");
  if (f > -1) {
    inf = (AnsiString *)Headers->Objects[f];
    Recipients->Add(*inf);
  }

  f = Headers->IndexOf("Reply-To:");
  if (f > -1) {
    inf = (AnsiString *)Headers->Objects[f];
    ReplyTo->AddAddress(*inf);
    ReplyTo->DispName = *inf;
  }

  f = Headers->IndexOf("Message-ID:");
  if (f > -1) {
    inf = (AnsiString *)Headers->Objects[f];
    *MessageID = *inf;
  }

  f = Headers->IndexOf("Date:");
  if (f > -1) {
    inf = (AnsiString *)Headers->Objects[f];

    TTokenizer *tok = new TTokenizer(NULL);
    TTokenizer *toktim = new TTokenizer(NULL);
    tok->Delimiters = ", ";
    tok->Text = inf->LowerCase();

    int month;
    if (tok->Text.Pos("jan") > 0) month = 1;
    if (tok->Text.Pos("feb") > 0) month = 2;
    if (tok->Text.Pos("mar") > 0) month = 3;
    if (tok->Text.Pos("apr") > 0) month = 4;
    if (tok->Text.Pos("may") > 0) month = 5;
    if (tok->Text.Pos("jun") > 0) month = 6;
    if (tok->Text.Pos("jul") > 0) month = 7;
    if (tok->Text.Pos("aug") > 0) month = 8;
    if (tok->Text.Pos("sep") > 0) month = 9;
    if (tok->Text.Pos("oct") > 0) month = 10;
    if (tok->Text.Pos("nov") > 0) month = 11;
    if (tok->Text.Pos("dec") > 0) month = 12;

    int day;
    int year;
    int j = 0;
    for (int i = 0; i < tok->TokenCount; i++) {
      if (tok->GetToken(i).Length() == 4) {
        if (tok->TokenIsNumeric(i)) {
          year = StrToInt(tok->GetToken(i));
          if (year < 100) year += 2000; 
        }
      }

      if ((tok->GetToken(i).Length() == 1) || (tok->GetToken(i).Length() == 2))
        if (tok->TokenIsNumeric(i))
          day = StrToInt(tok->GetToken(i));

      if ((j == 0) && (tok->GetToken(i).Pos(":") > 0))
        j = i;
    }

    MsgDate = EncodeDate((unsigned short)year, (unsigned short)month, (unsigned short)day);


    toktim->Delimiters = " :";
    toktim->Text = tok->GetToken(j);

    int hour;
    int min;
    int sec;
    if (toktim->TokenCount > 0)
      hour = StrToInt(toktim->GetToken(0));
    else
      hour = 0;
    if (toktim->TokenCount > 1)
      min = StrToInt(toktim->GetToken(1));
    else
      min = 0;
    if (toktim->TokenCount > 2)
      sec = StrToInt(toktim->GetToken(2));
    else
      sec = 0;

    TDateTime t = EncodeTime((unsigned short)hour, (unsigned short)min, (unsigned short)sec, 0);
    MsgDate += t;

    delete toktim;
    delete tok;
  }

  f = Headers->IndexOf("Priority:");
  if (f > -1) {
    inf = (AnsiString *)Headers->Objects[f];
    *Priority = *inf;
  }

  f = Headers->IndexOf("Subject:");
  if (f > -1) {
    inf = (AnsiString *)Headers->Objects[f];
    *Subject = *inf;
  }

  f = Headers->IndexOf("MIME-Version:");
  if (f > -1) {
    inf = (AnsiString *)Headers->Objects[f];

    TTokenizer *t2 = new TTokenizer(NULL);
    t2->Delimiters = " ";
    t2->Text = Trim(*inf);
    MimeVersion = StrToFloat(t2->GetFirstToken());
    delete t2;
  }


  delete data;
  delete tl;
}
//---------------------------------------------------------------------------
void __fastcall TMailHeaders::Clear()
{
  AnsiString *ptr;

  for (int i = 0; i < Headers->Count; i++) {
    ptr = (AnsiString *)Headers->Objects[i];
    if (ptr != NULL)
      delete ptr;
  }

  Headers->Clear();
}
//---------------------------------------------------------------------------
int __fastcall TMailHeaders::GetHeaderCount()
{
  return Headers->Count;
}
//---------------------------------------------------------------------------
int __fastcall TMailHeaders::GetHeaderIndex(AnsiString hdr)
{
  return Headers->IndexOf(LowerCase(Trim(hdr)));
}
//---------------------------------------------------------------------------
AnsiString * __fastcall TMailHeaders::GetHeaderInfo(AnsiString hdr)
{
  AnsiString *rc;

  int i = GetHeaderIndex(hdr);
  if (i > -1)
    rc = (AnsiString *)Headers->Objects[i];

  return rc;
}
//---------------------------------------------------------------------------
void __fastcall TMailHeaders::UpdateHeader(AnsiString hdr, AnsiString data)
{
  int i = GetHeaderIndex(hdr);
  if (i == -1)
    i = Headers->Add(hdr);

  AnsiString *j = (AnsiString *)Headers->Objects[i];
  if (j == NULL) {
    j = new AnsiString();
    Headers->Objects[i] = (TObject *)j;
  }

  *j = data;
}
//---------------------------------------------------------------------------
void __fastcall TMailHeaders::AddHeader(AnsiString hdr, AnsiString data)
{
  int i = Headers->Add(hdr);

  AnsiString *j = (AnsiString *)Headers->Objects[i];
  if (j == NULL) {
    j = new AnsiString();
    Headers->Objects[i] = (TObject *)j;
  }

  *j = data;
}
//---------------------------------------------------------------------------
void __fastcall TMailHeaders::AddHeaderAt(int i, AnsiString hdr, AnsiString data)
{
  Headers->Insert(i, hdr);

  AnsiString *j = (AnsiString *)Headers->Objects[i];
  if (j == NULL) {
    j = new AnsiString();
    Headers->Objects[i] = (TObject *)j;
  }

  *j = data;
}
//---------------------------------------------------------------------------
void __fastcall TMailHeaders::DeleteHeaderAt(int i)
{
  if ((i > -1) && (i < Headers->Count)) {
    AnsiString *j = (AnsiString *)Headers->Objects[i];
    if (j != NULL)
      delete j;

    Headers->Delete(i);
  }
}
//---------------------------------------------------------------------------
void __fastcall TMailHeaders::Dump(AnsiString KeyFile, AnsiString ValuesFile)
{
  TStringList *Keys = new TStringList();
  TStringList *Values = new TStringList();
  AnsiString *ptr;

  for (int i = 0; i < Headers->Count; i++) {
    Keys->Add(Headers->Strings[i]);

    ptr = (AnsiString *)Headers->Objects[i];
    if (ptr != NULL)
      Values->Add(*ptr);
    else
      Values->Add("");
  }

  Keys->SaveToFile(KeyFile);
  Values->SaveToFile(ValuesFile);

  delete Values;
  delete Keys;
}
//---------------------------------------------------------------------------
void __fastcall TMailHeaders::GetHeader(AnsiString hdr, TStringList *HeaderList)
{
  HeaderList->Clear();
  HeaderList->Sorted = false;

  for (int i = 0; i < Headers->Count; i++) {
    if (LowerCase(Headers->Strings[i]) == LowerCase(hdr)) {
      AnsiString *ptr = (AnsiString *)Headers->Objects[i];
      if (ptr != NULL)
        HeaderList->Add(Headers->Strings[i] + " " + *ptr);
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TMailHeaders::GetHeaderEx(AnsiString hdr, TStringList *HeaderList)
{
  HeaderList->Clear();
  HeaderList->Sorted = false;

  for (int i = 0; i < Headers->Count; i++) {
    if (LowerCase(Headers->Strings[i]) == LowerCase(hdr)) {
      AnsiString *ptr = (AnsiString *)Headers->Objects[i];
      if (ptr != NULL)
        HeaderList->Add(*ptr);
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TMailHeaders::GetAllHeaders(TStringList *Parms, TStringList *Values)
{
  Parms->Clear();
  Parms->Sorted = false;
  Values->Clear();
  Values->Sorted = false;

  for (int i = 0; i < Headers->Count; i++) {
    AnsiString *ptr = (AnsiString *)Headers->Objects[i];
    if (ptr != NULL) {
      Parms->Add(Headers->Strings[i]);
      Values->Add(*ptr);
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TMailHeaders::GetHeaders(TStringList *HeaderList)
{
  TStringList *tl = new TStringList();
  AnsiString *th = new AnsiString();
  AnsiString *ts = new AnsiString();
  AnsiString *td;

  for (int i = 0; i < Headers->Count; i++) {
    td = (AnsiString *)Headers->Objects[i];
    *th = Headers->Strings[i] + " " + *td;

    *ts = "";

    tl->Add(*th);
  }

  HeaderList->Clear();
  HeaderList->AddStrings(tl);

  delete ts;
  delete th;
  delete tl;
}
//---------------------------------------------------------------------------
void __fastcall TMailHeaders::MoveHeaderTo(int from, int to)
{
  int dest;

  if (from != to) {
    AnsiString *fhdr = new AnsiString();
    AnsiString *fdat;

    if ((from >= 0) && (from < Headers->Count)) {
      *fhdr = Headers->Strings[from];
      fdat = (AnsiString *)Headers->Objects[from];

      AddHeaderAt(to, *fhdr, *fdat);

      if (to < from)
        dest = from + 1;
      else
        dest = from;

      if (fdat != NULL)
        delete fdat;
      Headers->Delete(dest);
    }

    delete fhdr;
  }
}
//---------------------------------------------------------------------------
void __fastcall TMailHeaders::SetReturnPath(AnsiString hdr)
{
  AnsiString *data = new AnsiString();
  *data = Trim(hdr);

  if (data->SubString(1, 1) != "<")
    *data = "<" + *data;
  if (data->SubString(data->Length(), 1) != ">")
    *data += ">";

  int i = GetHeaderIndex("Return-Path:");
  if (i > -1) {
    UpdateHeader("Return-Path:", *data);
    if (i > 0)
      MoveHeaderTo(i, 0);
  } else
    AddHeaderAt(0, "Return-Path:", *data);

  delete data;
}
//---------------------------------------------------------------------------
void __fastcall TMailHeaders::SetLatestReceived(AnsiString data)
{
  AddHeaderAt(0, "Received:", data);

  int i = GetHeaderIndex("Return-Path:");
  if (i > 0)
    MoveHeaderTo(i, 0);
}
//---------------------------------------------------------------------------
AnsiString __fastcall TMailHeaders::GetMessageID()
{
  return *MessageID;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TMailHeaders::GetPriority()
{
  return *Priority;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TMailHeaders::GetSubject()
{
  return *Subject;
}
//---------------------------------------------------------------------------
TMailIdentity * __fastcall TMailHeaders::GetReturnPath()
{
  return ReturnPath;
}
//---------------------------------------------------------------------------
TMailIdentity * __fastcall TMailHeaders::GetFrom()
{
  return From;
}
//---------------------------------------------------------------------------
TMailIdentity * __fastcall TMailHeaders::GetReplyTo()
{
  return ReplyTo;
}
//---------------------------------------------------------------------------
TAddressBook *  __fastcall TMailHeaders::GetRecipients()
{
  return Recipients;
}
//---------------------------------------------------------------------------
double __fastcall TMailHeaders::GetMimeVersion()
{
  return MimeVersion;
}
//---------------------------------------------------------------------------
TDateTime __fastcall TMailHeaders::GetMsgDate()
{
  return MsgDate;
}
//---------------------------------------------------------------------------

