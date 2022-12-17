//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "SmtpServer.h"
#include <FileCtrl.hpp>
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//

static inline void ValidCtrCheck(TSmtpServer *)
{
        new TSmtpServer(NULL);
}
//---------------------------------------------------------------------------
__fastcall TSmtpServer::TSmtpServer(TComponent* Owner)
        : TComponent(Owner)
{
  FActive = false;
  FLoggingLevel = SMTP_LOG_CONNECTS;
  FPort = 0;
  FMessagesHandled = 0;
  FBytesHandled = 0;

  FServerSocket = new TServerSocket(this);
  FServerSocket->ServerType = stNonBlocking;
  FServerSocket->ThreadCacheSize = 0;
  FServerSocket->OnClientConnect = EventClientConnect;
  FServerSocket->OnClientDisconnect = EventClientDisconnect;
  FServerSocket->OnClientRead = EventClientRead;
  FServerSocket->OnClientWrite = EventClientWrite;
  FServerSocket->OnClientError = EventClientError;

  FMyName = new AnsiString();
  FMailDir = new AnsiString();
  FMailFileExt = new AnsiString();
  FMailFilePrefix = new AnsiString();
  ClientName = new AnsiString();
  RecvData = new AnsiString();
  SendData = new AnsiString();

  MailMsg = new TMemoryStream();
  Tokens = new TTokenizer(this);
  Command = new AnsiString();
  Parm1 = new AnsiString();
  Parm2 = new AnsiString();
  MsgHeaders = new TMailHeaders(this);

  UserFrom = new TMailIdentity(this, false);
  UsersTo = new TAddressBook(this);

  Statistics = new TCmdStatsInfo();
}
//---------------------------------------------------------------------------
__fastcall TSmtpServer::~TSmtpServer()
{
  if (FServerSocket->Active)
    FServerSocket->Close();

  delete FMyName;
  delete FMailDir;
  delete FMailFileExt;
  delete FMailFilePrefix;
  delete ClientName;
  delete RecvData;
  delete SendData;
  delete MailMsg;
  delete Tokens;
  delete Command;
  delete Parm1;
  delete Parm2;
  delete MsgHeaders;
  delete UserFrom;
  delete UsersTo;
  delete Statistics;
}
//---------------------------------------------------------------------------
namespace Smtpserver
{
        void __fastcall PACKAGE Register()
        {
                 TComponentClass classes[1] = {__classid(TSmtpServer)};
                 RegisterComponents("Celestial", classes, 0);
        }
}
//---------------------------------------------------------------------------
void __fastcall TSmtpServer::SetActive(bool act)
{
  if (act) {
    FServerSocket->Port = FPort;
    FServerSocket->Open();
  } else
    FServerSocket->Close();

  FActive = FServerSocket->Active;
}
//---------------------------------------------------------------------------
void __fastcall TSmtpServer::SetLoggingLevel(int i)
{
  if ((i >= SMTP_LOG_NONE) && (i <= SMTP_LOG_DETAIL))
    FLoggingLevel = i;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TSmtpServer::GetMyName()
{
  return *FMyName;
}
//---------------------------------------------------------------------------
void __fastcall TSmtpServer::SetMyName(AnsiString mn)
{
  *FMyName = mn;
}
//---------------------------------------------------------------------------
void __fastcall TSmtpServer::SetPortA(int p)
{
  if (!FActive)
    FPort = p;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TSmtpServer::GetMailDir()
{
  return *FMailDir;
}
//---------------------------------------------------------------------------
void __fastcall TSmtpServer::SetMailDir(AnsiString md)
{
  if (!FActive) {
    *FMailDir = md;

    if (FMailDir->SubString(FMailDir->Length(), 1) == "\\")
      FMailDir->Delete(FMailDir->Length(), 1);
  }
}
//---------------------------------------------------------------------------
AnsiString __fastcall TSmtpServer::GetMailFileExt()
{
  return *FMailFileExt;
}
//---------------------------------------------------------------------------
void __fastcall TSmtpServer::SetMailFileExt(AnsiString mfe)
{
  if (!FActive) {
    *FMailFileExt = mfe;

    while (FMailFileExt->SubString(1, 1) == ".")
      FMailFileExt->Delete(1, 1);
  }
}
//---------------------------------------------------------------------------
AnsiString __fastcall TSmtpServer::GetMailFilePrefix()
{
  return *FMailFilePrefix;
}
//---------------------------------------------------------------------------
void __fastcall TSmtpServer::SetMailFilePrefix(AnsiString mfp)
{
  if (!FActive) {
    *FMailFilePrefix = mfp;

    if (FMailFilePrefix->Length() > 3)
      FMailFilePrefix->SetLength(3);
  }
}
//---------------------------------------------------------------------------
void __fastcall TSmtpServer::LogEvent(const AnsiString le)
{
  if (FOnLogEvent != NULL)
    FOnLogEvent(this, le);
}
//---------------------------------------------------------------------------
void __fastcall TSmtpServer::EventClientConnect(TObject* Sender, TCustomWinSocket* Socket)
{
  ResetState();
  ServerState = SMSS_INIT;

  *RecvData = "";
  *SendData = "220 " + UpperCase(Socket->LocalHost) + " SMTP server ready\r\n";

  if (FLoggingLevel > SMTP_LOG_NONE)
    LogEvent("[SMTP SRVR] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [CONNECT] Client connect from " + Socket->RemoteHost + ":" + IntToStr(Socket->RemotePort) + " (" + Socket->RemoteAddress + ")");

  if (OnClientConnected != NULL)
    OnClientConnected(this, Socket->RemoteHost, Socket->RemoteAddress, Socket->RemotePort);

  Socket->SendText(*SendData);
}
//---------------------------------------------------------------------------
void __fastcall TSmtpServer::EventClientDisconnect(TObject* Sender, TCustomWinSocket* Socket)
{
  *RecvData = "";
  *SendData = "";

  if (FLoggingLevel > SMTP_LOG_NONE)
    LogEvent("[SMTP SRVR] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [DISCONNECT] Client disconnect from " + Socket->RemoteHost + ":" + IntToStr(Socket->RemotePort) + " (" + Socket->RemoteAddress + ")");

  if (OnClientDisconnected != NULL)
    OnClientDisconnected(this, Socket->RemoteHost, Socket->RemoteAddress, Socket->RemotePort);
}
//---------------------------------------------------------------------------
void __fastcall TSmtpServer::EventClientRead(TObject* Sender, TCustomWinSocket* Socket)
{
  *SendData = "";

  try {
    *RecvData = Socket->ReceiveText();

    if (FLoggingLevel > SMTP_LOG_CONNECTS)
      if (RecvData->Length() > 128)
        LogEvent("[SMTP SRVR] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [READ] [" + Socket->RemoteAddress + ":" + Socket->RemotePort + "] Data size=" + IntToStr(RecvData->Length()));
      else
        LogEvent("[SMTP SRVR] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [READ] [" + Socket->RemoteAddress + ":" + Socket->RemotePort + "] " + *RecvData);

    if (!RecvData->IsEmpty())
      ProcessInput(Socket);
  } catch (Exception &e) {
    Socket->Close();
    ServerState = SMSS_INIT;
  }
}
//---------------------------------------------------------------------------
void __fastcall TSmtpServer::EventClientWrite(TObject* Sender, TCustomWinSocket* Socket)
{
}
//---------------------------------------------------------------------------
void __fastcall TSmtpServer::EventClientError(TObject* Sender, TCustomWinSocket* Socket, TErrorEvent ErrorEvent, int &ErrorCode)
{
  AnsiString *Ev = new AnsiString();

  switch (ErrorEvent) {
    case eeGeneral:
      *Ev = "GENERAL";
      break;
    case eeSend:
      *Ev = "SEND";
      break;
    case eeReceive:
      *Ev = "RECEIVE";
      break;
    case eeConnect:
      *Ev = "CONNECT";
      break;
    case eeDisconnect:
      *Ev = "DISCONNECT";
      break;
    case eeAccept:
      *Ev = "ACCEPT";
      break;
    default:
      *Ev = "UNKNOWN";
  }

  LogEvent("[SMTP SRVR] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [" + *Ev + "] Error occurred " + IntToStr(ErrorCode));

  ErrorCode = 0;
  Socket->Close();
  ServerState = SMSS_INIT;

  delete Ev;
}
//---------------------------------------------------------------------------
void __fastcall TSmtpServer::Start()
{
  if (FMyName->IsEmpty())
    throw Exception("Server Name is not assigned!");

  if (!FActive) {
    InitStatistics();

    if (!FMailDir->IsEmpty())
      if (!DirectoryExists(*FMailDir))
        ForceDirectories(*FMailDir);

    SetActive(true);

    if (FOnServerStarted != NULL)
      FOnServerStarted(this);
  }
}
//---------------------------------------------------------------------------
void __fastcall TSmtpServer::Stop()
{
  if (FActive) {
    SetActive(false);

    if (FOnServerStopped != NULL)
      FOnServerStopped(this);
  }
}
//---------------------------------------------------------------------------
void __fastcall TSmtpServer::InitStatistics()
{
  Statistics->ResetStats();

  Statistics->AddCommand("EHLO");
  Statistics->AddCommand("HELO");
  Statistics->AddCommand("MAIL");
  Statistics->AddCommand("RCPT");
  Statistics->AddCommand("DATA");
  Statistics->AddCommand("QUIT");
  Statistics->AddCommand("NOOP");
  Statistics->AddCommand("RSET");
  Statistics->AddCommand("HELP");
  Statistics->AddCommand("EXPN");
  Statistics->AddCommand("VRFY");
}
//---------------------------------------------------------------------------
bool __fastcall TSmtpServer::MsgCompleted(TMemoryStream *ms)
{
//  AnsiString *i = new AnsiString();
//
//  *i = RecvData->SubString(RecvData->Length() - 4, 5);
//  bool rc = (*i == "\r\n.\r\n");
//
//  delete i;
//
//  return rc;

  bool rc;
  int OldPosition = ms->Position;
  char buf[10];

  ms->Seek(-5, soFromEnd);
  memset(buf, 0, 10);
  ms->Read(&buf, 5);
  rc = (memcmp(buf, "\r\n.\r\n", 5) == 0);

  ms->Position = OldPosition;
  return rc;
}
//---------------------------------------------------------------------------
void __fastcall TSmtpServer::GetMailFilename(AnsiString *newname)
{
  *newname = "";

  TTimeStamp t = DateTimeToTimeStamp(Now());
  *newname = *FMailDir + "\\" + *FMailFilePrefix + IntToHex(t.Date, 1) + IntToHex(t.Time, 1) + "." + *FMailFileExt;
}
//---------------------------------------------------------------------------
void __fastcall TSmtpServer::ResetState()
{
  ResetMsg();
  UserFrom->Clear();
  UsersTo->Clear();
}
//---------------------------------------------------------------------------
void __fastcall TSmtpServer::ResetMsg()
{
  MailMsg->Clear();
  MailMsg->Position = 0;
  FCurrSize = 0;
}
//---------------------------------------------------------------------------
void __fastcall TSmtpServer::ProcessInput(TCustomWinSocket* Socket)
{
  *SendData = "";

  if (ServerState != SMSS_TRANS) {
    Tokens->Delimiters = " :";
    Tokens->Text = *RecvData;

    if (Tokens->TokenCount > 0)
      *Command = Trim(Tokens->GetToken(0));
    else
      *Command = "";

    if (Tokens->TokenCount > 1)
      *Parm1 = Trim(Tokens->GetToken(1));
    else
      *Parm1 = "";

    if (Tokens->TokenCount > 2)
      *Parm2 = Trim(Tokens->GetToken(2));
    else
      *Parm2 = "";


    if (Command->AnsiCompareIC("NOOP") == 0)
      *SendData = "250 Okay\r\n";


//    if (Command->AnsiCompareIC("HELP") == 0)
//    if (Command->AnsiCompareIC("EXPN") == 0) ;
//    if (Command->AnsiCompareIC("VRFY") == 0) ;


    if (Command->AnsiCompareIC("RSET") == 0) {
      ResetState();
      *SendData = "250 Okay, reset\r\n";
    }


    if (Command->AnsiCompareIC("QUIT") == 0) {
      ResetState();
      *SendData = "221 Bye\r\n";
      ServerState = SMSS_FINAL;
    }


    if (Command->AnsiCompareIC("DATA") == 0) {
      ServerState = SMSS_TRANS;
      ResetMsg();
      *SendData = "354 Okay, send mail data\r\n";
    }


    if (Command->AnsiCompareIC("RCPT") == 0) {
      if (!Parm1->IsEmpty()) {
        if (Parm1->AnsiCompareIC("TO") == 0) {
          if (!Parm2->IsEmpty()) {

            bool Allowed = true;
            if (FOnRecipientValidationRequired != NULL)
              FOnRecipientValidationRequired(this, *Parm2, Allowed);

            if (Allowed) {
              bool RcptOk = true;
              if (OnMaxRecipientsCheck != NULL)
                OnMaxRecipientsCheck(this, UsersTo->Count, RcptOk);

              if (RcptOk) {
                UsersTo->Add(*Parm2);
                *SendData = "250 Okay\r\n";
              } else
                *SendData = "452 Maximum number of allowed recipients exceeded\r\n";
            } else
              *SendData = "500 Sorry, recipient not allowed to get mail from this server\r\n";

          } else
            *SendData = "500 Error\r\n";
        } else
          *SendData = "500 Error\r\n";
      } else
        *SendData = "500 Error\r\n";
    }


    if (Command->AnsiCompareIC("MAIL") == 0) {
      if (!Parm1->IsEmpty()) {
        if (Parm1->AnsiCompareIC("FROM") == 0) {
          if (!Parm2->IsEmpty()) {

            bool Allowed = true;
            if (FOnSenderValidationRequired != NULL)
              FOnSenderValidationRequired(this, *Parm2, Allowed);

            if (Allowed) {
              UserFrom->AddAddress(*Parm2);
              *SendData = "250 Okay\r\n";
            } else
              *SendData = "500 User not allowed to use this server\r\n";

          } else
            *SendData = "500 Error\r\n";
        } else
          *SendData = "500 Error\r\n";
      } else
        *SendData = "500 Error\r\n";
    }


    if (Command->AnsiCompareIC("EHLO") == 0) {
      *SendData = "502 Command not implemented\r\n";
    }


    if (Command->AnsiCompareIC("HELO") == 0) {
      ServerState = SMSS_ROUTE;
      if (!Parm1->IsEmpty()) {
        ResetState();
        *ClientName = *Parm1;

        bool Allowed = true;
        if (FOnHostValidationRequired != NULL)
          FOnHostValidationRequired(this, *ClientName, Allowed);

        if (Allowed)
          *SendData = "250 Okay, send routing information\r\n";
        else
          *SendData = "500 Sorry, you are not allowed to use this server\r\n";
      } else
        *SendData = "500 Error\r\n";
    }

    if (SendData->IsEmpty())
      *SendData = "500 Command not recognised\r\n";

  } else {

    *Command = "";
    *Parm1 = "";
    *Parm2 = "";

    int length = RecvData->Length();
    MailMsg->Write(RecvData->c_str(), length);
    FCurrSize += length;


    bool MailSizeOk = true;
    if (OnMaxMsgSizeCheck != NULL)
      OnMaxMsgSizeCheck(this, FCurrSize, MailSizeOk);

    if (!MailSizeOk) {
      *SendData = "552 Mail data size limit exceeded\r\n";
    } else {
      *SendData = "220 Okay, received " + IntToStr((__int64)FCurrSize) + " bytes in total\r\n";

      if (MsgCompleted(MailMsg)) {
        AnsiString *mname = new AnsiString();
        AnsiString *sname = new AnsiString();

        *sname = Socket->RemoteHost + " [" + Socket->RemoteAddress + "]";

        GetMailFilename(mname);
        SaveFile(MailMsg, *sname, *mname);

        FMessagesHandled++;
        FBytesHandled += FCurrSize;

        *SendData = "250 Okay, received all mail data\r\n";
        ServerState = SMSS_ROUTE;

        ResetMsg();
        UsersTo->Clear();

        delete sname;
        delete mname;
      }
    }
  }


  Socket->SendText(*SendData);
  if (FLoggingLevel > SMTP_LOG_CONNECTS)
    if (SendData->Length() > 128)
      LogEvent("[SMTP SRVR] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [WRITE] [" + Socket->RemoteAddress + ":" + Socket->RemotePort + "] Data size=" + IntToStr(SendData->Length()));
    else
      LogEvent("[SMTP SRVR] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [WRITE] [" + Socket->RemoteAddress + ":" + Socket->RemotePort + "] " + *SendData);
}
//---------------------------------------------------------------------------
void __fastcall TSmtpServer::SaveFile(TMemoryStream *s, AnsiString src, AnsiString newname)
{
  TStringList *msg = new TStringList();
  TStringList *route = new TStringList();
  TStringList *newhdr = new TStringList();
  AnsiString *id = new AnsiString();
  AnsiString *rec = new AnsiString();
  AnsiString *rname = new AnsiString();

  TReplaceFlags rflags;
  rflags << rfIgnoreCase;

  s->Seek(0, soFromBeginning);
  msg->LoadFromStream(s);
  if (msg->Strings[msg->Count - 1].SubString(1, 2) == ".")
    msg->Delete(msg->Count - 1);

  *rname = StringReplace(newname, "." + *FMailFileExt, ".RTE", rflags);
  route->Add("HOST:" + Trim(*ClientName));
  route->Add("FROM:" + UserFrom->GetAddress(0));
  for (int k = 0; k < UsersTo->Count; k++) {
    TMailIdentity *ptr = UsersTo->GetEntry(k);
    for (int m = 0; m < ptr->GetAddressCount(); m++)
      route->Add("TO:" + ptr->GetAddress(m));
  }

  CreateMsgIdentifier(id);

  *rec = "from " + src;
  *rec += "\r\n     by " + *FMyName + " via SMTP";
  *rec += "\r\n     id " + *id + ";";
  *rec += "\r\n     " + FormatDateTime("ddd, dd mmm yyyy hh:nn:ss", Now());

  MsgHeaders->Clear();
  MsgHeaders->SetHeaders(msg);
  MsgHeaders->SetLatestReceived(*rec);
  MsgHeaders->GetHeaders(newhdr);
  *rec = newhdr->Strings[0];

  while ((msg->Count > 0) && (msg->Strings[0] != ""))
    msg->Delete(0);

  for (int i = newhdr->Count - 1; i >= 0; i--)
    msg->Insert(0, newhdr->Strings[i]);

  bool Cancel = false;
  long msize = msg->Text.Length() + route->Text.Length();
  if (FOnMessageBeforeSave != NULL)
    FOnMessageBeforeSave(this, newname, msize, Cancel);

  if (!Cancel) {
    route->SaveToFile(*rname);
    msg->SaveToFile(newname);
  }

  if (FOnMessageAfterSave != NULL)
    FOnMessageAfterSave(this, newname);

  delete rname;
  delete rec;
  delete id;
  delete newhdr;
  delete route;
  delete msg;
}
//---------------------------------------------------------------------------
void __fastcall TSmtpServer::CreateMsgIdentifier(AnsiString *mid)
{
  *mid = "";
  TTimeStamp t = DateTimeToTimeStamp(Now());
  *mid = IntToStr((__int64)Application->Handle) + "." + IntToHex(t.Date, 1) + "." + IntToHex(t.Time, 1) + "@" + *FMyName;
}
//---------------------------------------------------------------------------

