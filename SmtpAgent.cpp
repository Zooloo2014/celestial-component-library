//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "SmtpAgent.h"
#include <Filectrl.hpp>
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//

static inline void ValidCtrCheck(TSmtpAgent *)
{
        new TSmtpAgent(NULL);
}
//---------------------------------------------------------------------------
__fastcall TSmtpAgent::TSmtpAgent(TComponent* Owner)
        : TComponent(Owner)
{
  FActive = false;

  FMailSocket = new TClientSocket(this);
  FMailSocket->ClientType = ctNonBlocking;
  FMailSocket->OnLookup = EventLookup;
  FMailSocket->OnConnect = EventConnect;
  FMailSocket->OnDisconnect = EventDisconnect;
  FMailSocket->OnConnecting = EventConnecting;
  FMailSocket->OnError = EventError;
  FMailSocket->OnRead = EventRead;
  FMailSocket->OnWrite = EventWrite;

  FAgentName = new AnsiString();
  FMessageNames = new TStringList();
  MailRoute = new TStringList();
  MailMsg = new TStringList();

  FHost = new AnsiString();
  FPort = 0;
  FLoggingLevel = SMA_LOG_CONNECTS;

  Tokens = new TTokenizer(this);
  Tokens->Delimiters = " \n";

  RecvData = new AnsiString();
  SendData = new AnsiString();

  FMailDir = new AnsiString();
  FMailFileExt = new AnsiString();

  FUserID = new AnsiString();
  FPassword = new AnsiString();
}
//---------------------------------------------------------------------------
__fastcall TSmtpAgent::~TSmtpAgent()
{
  delete FMailSocket;
  delete FAgentName;
  delete FMessageNames;
  delete MailMsg;
  delete MailRoute;
  delete FHost;
  delete Tokens;
  delete RecvData;
  delete SendData;
  delete FMailDir;
  delete FMailFileExt;
  delete FPassword;
  delete FUserID;
}
//---------------------------------------------------------------------------
namespace Smtpagent
{
        void __fastcall PACKAGE Register()
        {
                 TComponentClass classes[1] = {__classid(TSmtpAgent)};
                 RegisterComponents("Celestial", classes, 0);
        }
}
//---------------------------------------------------------------------------
void __fastcall TSmtpAgent::SetPort(int p)
{
  if (!FActive)
    FPort = p;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TSmtpAgent::GetUserID()
{
  return *FUserID;
}
//---------------------------------------------------------------------------
void __fastcall TSmtpAgent::SetUserID(AnsiString uid)
{
  if (!FActive)
    *FUserID = uid;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TSmtpAgent::GetPassword()
{
  return *FPassword;
}
//---------------------------------------------------------------------------
void __fastcall TSmtpAgent::SetPassword(AnsiString pwd)
{
  if (!FActive)
    *FPassword = pwd;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TSmtpAgent::GetAgentName()
{
  return *FAgentName;
}
//---------------------------------------------------------------------------
void __fastcall TSmtpAgent::SetAgentName(AnsiString an)
{
  if (!FActive)
    *FAgentName = an;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TSmtpAgent::GetHost()
{
  return *FHost;
}
//---------------------------------------------------------------------------
void __fastcall TSmtpAgent::SetHost(AnsiString h)
{
  if (!FActive)
    *FHost = Trim(h);
}
//---------------------------------------------------------------------------
AnsiString __fastcall TSmtpAgent::GetMailDir()
{
  return *FMailDir;
}
//---------------------------------------------------------------------------
void __fastcall TSmtpAgent::SetMailDir(AnsiString md)
{
  if (!FActive) {
    *FMailDir = Trim(md);

    if (!FMailDir->IsEmpty()) {
      while (FMailDir->SubString(FMailDir->Length(), 1) == "\\")
        FMailDir->Delete(FMailDir->Length(), 1);

      if (!DirectoryExists(*FMailDir))
        ForceDirectories(*FMailDir);
    }
  }
}
//---------------------------------------------------------------------------
AnsiString __fastcall TSmtpAgent::GetMailFileExt()
{
  return *FMailFileExt;
}
//---------------------------------------------------------------------------
void __fastcall TSmtpAgent::SetMailFileExt(AnsiString mfe)
{
  if (!FActive) {
    *FMailFileExt = Trim(mfe);

    if (!FMailFileExt->IsEmpty())
      while (FMailFileExt->SubString(1, 1) == ".")
        FMailFileExt->Delete(1, 1);
  }
}
//---------------------------------------------------------------------------
void __fastcall TSmtpAgent::SetActive(bool act)
{
  if (!ComponentState.Contains(csDesigning)) {
    FActive = FMailSocket->Socket->Connected;

    if (FActive != act) {
      if (act) {
        if (FAgentName->IsEmpty())
          throw Exception("AgentName property not set!");

          if (FOnInitRequired != NULL)
            FOnInitRequired(this);

          if (FHost->IsEmpty())
            throw Exception("Host property not set");
          if (FPort == 0)
            throw Exception("Port property not set");

          if (FOnAgentBegun != NULL)
            FOnAgentBegun(this);

        GetMessageNames(FMessageNames);

        if (FMessageNames->Count > 0) {
          FMailSocket->Host = *FHost;
          FMailSocket->Port = FPort;
          FMailSocket->Open();
          FActive = act;
        }
      } else {
        Abort();

        if (FOnAgentCompleted != NULL)
          FOnAgentCompleted(this);
      }

      FActive = FMailSocket->Socket->Connected;
    } else
      LogEvent("[SMTP AGNT] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [ACTIVATE] Transaction in progress");
  }
}
//---------------------------------------------------------------------------
void __fastcall TSmtpAgent::SetLoggingLevel(int l)
{
  if (!FActive)
    if ((l >= SMA_LOG_NONE) && (l <= SMA_LOG_DETAIL))
      FLoggingLevel = l;
}
//---------------------------------------------------------------------------
void __fastcall TSmtpAgent::Execute()
{
  SetActive(true);
}
//---------------------------------------------------------------------------
void __fastcall TSmtpAgent::Abort()
{
  if (FMailSocket->Socket->Connected) {
    FMailSocket->Close();

    while (FMailSocket->Socket->Connected) {}

    LogEvent("[SMTP AGNT] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [ABORT] Transaction aborted");
  }

  SetActive(FMailSocket->Socket->Connected);
}
//---------------------------------------------------------------------------
void __fastcall TSmtpAgent::LogEvent(AnsiString le)
{
  if (FOnLogEvent != NULL)
    FOnLogEvent(this, le);
}
//---------------------------------------------------------------------------
void __fastcall TSmtpAgent::EventLookup(TObject* Sender, TCustomWinSocket* Socket)
{
  if (FLoggingLevel > SMA_LOG_NONE)
    LogEvent("[SMTP AGNT] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [LOOKUP] Looking up host " + Host);
}
//---------------------------------------------------------------------------
void __fastcall TSmtpAgent::EventConnect(TObject* Sender, TCustomWinSocket* Socket)
{
  if (FLoggingLevel > SMA_LOG_NONE)
    LogEvent("[SMTP AGNT] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [CONNECT] Connect to host " + Socket->RemoteHost + ":" + IntToStr(Socket->RemotePort) + " (" + Socket->RemoteAddress + ")");

  Step = 0;
  FCurrMsg = 0;
  FCurrSize = 0;

  *RecvData = "";
  *SendData = "";

  if (FOnConnected != NULL)
    FOnConnected(this);

  EventRead(this, FMailSocket->Socket);
}
//---------------------------------------------------------------------------
void __fastcall TSmtpAgent::EventDisconnect(TObject* Sender, TCustomWinSocket* Socket)
{
  if (FLoggingLevel > SMA_LOG_NONE)
    LogEvent("[SMTP AGNT] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [DISCONNECT] Disconnect from host " + Socket->RemoteHost + ":" + IntToStr(Socket->RemotePort) + " (" + Socket->RemoteAddress + ")");

  if (FOnDisconnected != NULL)
    FOnDisconnected(this);
}
//---------------------------------------------------------------------------
void __fastcall TSmtpAgent::EventConnecting(TObject* Sender, TCustomWinSocket* Socket)
{
  if (FLoggingLevel > SMA_LOG_NONE)
    LogEvent("[SMTP AGNT] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [CONNECTING] Connecting to host...");
}
//---------------------------------------------------------------------------
void __fastcall TSmtpAgent::EventError(TObject* Sender, TCustomWinSocket* Socket, TErrorEvent ErrorEvent, int &ErrorCode)
{
  switch (ErrorEvent) {
    case eeGeneral:
      LogEvent("[SMTP AGNT] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [ERROR] General - " + IntToStr(ErrorCode));
      break;

    case eeSend:
      LogEvent("[SMTP AGNT] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [ERROR] On Write - " + IntToStr(ErrorCode));
      break;

    case eeReceive:
      LogEvent("[SMTP AGNT] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [ERROR] On Read - " + IntToStr(ErrorCode));
      break;

    case eeConnect:
      LogEvent("[SMTP AGNT] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [ERROR] On Connect - " + IntToStr(ErrorCode));
      break;

    case eeDisconnect:
      LogEvent("[SMTP AGNT] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [ERROR] On Disconnect - " + IntToStr(ErrorCode));
      break;

    case eeAccept:
      LogEvent("[SMTP AGNT] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [ERROR] On Accept - " + IntToStr(ErrorCode));
      break;

    default:
      LogEvent("[SMTP AGNT] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [ERROR] Unknown - " + IntToStr(ErrorCode));
  }

  ErrorCode = 0;
}
//---------------------------------------------------------------------------
void __fastcall TSmtpAgent::EventRead(TObject* Sender, TCustomWinSocket* Socket)
{
  *SendData = "";
  *RecvData = Socket->ReceiveText();

  int s = RecvData->Length();
  if (FLoggingLevel > SMA_LOG_CONNECTS) {
    if (s > 128) {
      LogEvent("[SMTP AGNT] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [READ] [" + Socket->RemoteAddress + ":" + IntToStr(Socket->RemotePort) + "] Data size=" + IntToStr(s));
    } else {
      if (UpperCase(RecvData->SubString(1, 4)) == "PASS")
        LogEvent("[SMTP AGNT] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [READ] [" + Socket->RemoteAddress + ":" + IntToStr(Socket->RemotePort) + "]" + RecvData->SubString(1, 5) + AnsiString::StringOfChar('*', RecvData->Length() - 5));
      else
        LogEvent("[SMTP AGNT] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [READ] [" + Socket->RemoteAddress + ":" + IntToStr(Socket->RemotePort) + "]" + *RecvData);
    }
  }

  if (!RecvData->IsEmpty())
    ProcessInput(Socket);
}
//---------------------------------------------------------------------------
void __fastcall TSmtpAgent::EventWrite(TObject* Sender, TCustomWinSocket* Socket)
{
  if (FLoggingLevel > SMA_LOG_CONNECTS)
    LogEvent("[SMTP AGNT] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [WRITE] [" + Socket->RemoteAddress + ":" + IntToStr(Socket->RemotePort) + "]" + *SendData);
}
//---------------------------------------------------------------------------
bool __fastcall TSmtpAgent::Successful()
{
  bool rc;

  if (Proceed) {
    AnsiString *t = new AnsiString();

    Tokens->Text = *RecvData;
    *t = Tokens->GetFirstToken();
    try {
      int v = StrToInt(*t);
      rc = ((v >= 200) && (v <= 299));
    } catch (Exception &e) {
      rc = false;
    }

    delete t;
  } else
    rc = true;

  return rc;
}
//---------------------------------------------------------------------------
void __fastcall TSmtpAgent::GetMessageNames(TStringList *mn)
{
  TSearchRec sr;

  FCurrMsg = 0;
  FCurrSize = 0;
  FNumberOfMessages = 0;
  FMailboxSize = 0;

  sr.ExcludeAttr = (faVolumeID | faDirectory | faHidden);
  mn->Clear();

  if (FindFirst(*FMailDir + "\\*." + *FMailFileExt, faAnyFile, sr) == 0) {
    if ((sr.Attr & faHidden) != faHidden) {
      mn->Add(*FMailDir + "\\" + sr.Name);
      FMailboxSize += sr.Size;
      FNumberOfMessages++;
    }

    while (FindNext(sr) == 0) {
      if ((sr.Attr & faHidden) != faHidden) {
        mn->Add(*FMailDir + "\\" + sr.Name);
        FMailboxSize += sr.Size;
        FNumberOfMessages++;
      }
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TSmtpAgent::LoadMessage(AnsiString mn)
{
  AnsiString *rname = new AnsiString();
  TReplaceFlags rflags;


  rflags << rfIgnoreCase;
  *rname = StringReplace(mn, "." + *FMailFileExt, ".RTE", rflags);

  MailRoute->Clear();
  MailRoute->LoadFromFile(*rname);
  while (MailRoute->Strings[0].AnsiPos("HOST:") != 0)
    MailRoute->Delete(0);


  MailMsg->Clear();
  MailMsg->LoadFromFile(mn);

  for (int i = 0; i < MailMsg->Count; i++)
    if (MailMsg->Strings[i].SubString(1, 1) == ".")
      MailMsg->Strings[i].Insert(".", 1);

  MailMsg->Append(".");


  delete rname;
}
//---------------------------------------------------------------------------
void __fastcall TSmtpAgent::ProcessInput(TCustomWinSocket* Socket)
{
  AnsiString *Parm = new AnsiString();

  if (Successful()) {
    Tokens->Text = *RecvData;

    switch (Step) {
      case 0:
        *SendData = "HELO " + *FAgentName + "\r\n";
        Step++;

        if (FMessageNames->Count > 0) {
          LoadMessage(FMessageNames->Strings[0]);
          FCurrMsg++;
          FCurrSize = MailMsg->Text.Length();
        }

        break;

      case 1:
        while (MailRoute->Strings[0].AnsiPos("FROM:") == 0) {
          *Parm = MailRoute->Strings[0];
          MailRoute->Delete(0);
        }

        Parm->Delete(1, 5);
        *SendData = "MAIL FROM: <" + Trim(*Parm) + ">\r\n";
        Step++;

        break;

      case 2:
        if (MailRoute->Count > 0) {
          if (MailRoute->Strings[0].AnsiPos("TO:") > 0) {
            *Parm = MailRoute->Strings[0];
            MailRoute->Delete(0);
          }

          Parm->Delete(1, 3);

          bool Valid = true;
          if (FOnValidateRecipient != NULL)
            FOnValidateRecipient(this, *Parm, Valid);

          if (Valid)
            *SendData = "RCPT TO: <" + Trim(*Parm) + ">\r\n";
          else
            *SendData = "NOOP\r\n";

          break;
        } else
          Step++;

      case 3:
        if (FOnTransmissionStart != NULL)
          FOnTransmissionStart(this, FCurrMsg, FCurrSize);

        *SendData = "DATA\r\n";
        Step++;

        break;

      case 4:
        *SendData = MailMsg->Text;
        Step++;

        break;

      case 5:
        if (FOnTransmissionEnd != NULL)
          FOnTransmissionEnd(this, FCurrMsg, FCurrSize);

        if (FMessageNames->Count > 0)
          FMessageNames->Delete(0);

        if (FMessageNames->Count > 0) {
          LoadMessage(FMessageNames->Strings[0]);
          FCurrMsg++;
          FCurrSize = MailMsg->Text.Length();
          Step = 1;
          *SendData = "NOOP\r\n";
          break;
        } else
          Step++;

      case 6:
        *SendData = "QUIT\r\n";
        Step++;
        break;

      case 7:
      default: ;
        FMailSocket->Close();
        Step = 0;
    }
  } else {
    *SendData = "QUIT\r\n";
    Step = 8;
  }


  if (Socket->Connected)
    Socket->SendText(*SendData);

  delete Parm;
}
//---------------------------------------------------------------------------
void __fastcall TSmtpAgent::ShowHiddenFiles()
{
  TSearchRec sr;

  if (FindFirst(*FMailDir + "\\*.*", faHidden, sr) == 0) {
    if ((sr.Attr & faHidden) == faHidden)
      FileSetAttr(*FMailDir + "\\" + sr.Name, sr.Attr & (!faHidden));

    while (FindNext(sr) == 0)
      if ((sr.Attr & faHidden) == faHidden)
        FileSetAttr(*FMailDir + "\\" + sr.Name, sr.Attr & (!faHidden));
  }

  FindClose(sr);
}
//---------------------------------------------------------------------------

