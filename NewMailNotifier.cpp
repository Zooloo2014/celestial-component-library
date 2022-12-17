//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "NewMailNotifier.h"
#include <Filectrl.hpp>
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//

static inline void ValidCtrCheck(TNewMailNotifier *)
{
        new TNewMailNotifier(NULL);
}
//---------------------------------------------------------------------------
__fastcall TNewMailNotifier::TNewMailNotifier(TComponent* Owner)
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

  FHost = new AnsiString();
  FPort = 0;
  FLoggingLevel = P3A_LOG_CONNECTS;

  Tokens = new TTokenizer(this);
  Tokens->Delimiters = " \n";

  RecvData = new AnsiString();
  SendData = new AnsiString();

  FUserID = new AnsiString();
  FPassword = new AnsiString();
}
//---------------------------------------------------------------------------
__fastcall TNewMailNotifier::~TNewMailNotifier()
{
  delete FMailSocket;
  delete FHost;
  delete Tokens;
  delete RecvData;
  delete SendData;
  delete FPassword;
  delete FUserID;
}
//---------------------------------------------------------------------------
namespace Newmailnotifier
{
        void __fastcall PACKAGE Register()
        {
                 TComponentClass classes[1] = {__classid(TNewMailNotifier)};
                 RegisterComponents("Celestial", classes, 0);
        }
}
//---------------------------------------------------------------------------
void __fastcall TNewMailNotifier::SetPort(int p)
{
  if (!FActive)
    FPort = p;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TNewMailNotifier::GetUserID()
{
  return *FUserID;
}
//---------------------------------------------------------------------------
void __fastcall TNewMailNotifier::SetUserID(AnsiString uid)
{
  if (!FActive)
    *FUserID = uid;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TNewMailNotifier::GetPassword()
{
  return *FPassword;
}
//---------------------------------------------------------------------------
void __fastcall TNewMailNotifier::SetPassword(AnsiString pwd)
{
  if (!FActive)
    *FPassword = pwd;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TNewMailNotifier::GetHost()
{
  return *FHost;
}
//---------------------------------------------------------------------------
void __fastcall TNewMailNotifier::SetHost(AnsiString h)
{
  if (!FActive)
    *FHost = Trim(h);
}
//---------------------------------------------------------------------------
void __fastcall TNewMailNotifier::SetActive(bool act)
{
  if (!ComponentState.Contains(csDesigning)) {
    FActive = FMailSocket->Socket->Connected;

    if (FActive != act) {
      if (act) {
        FMailSocket->Host = *FHost;
        FMailSocket->Port = FPort;
        FMailSocket->Open();
      } else
        Abort();

      FActive = act;
    } else
      LogEvent("[POP3 NOTI] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [ACTIVATE] Transaction in progress");
  }
}
//---------------------------------------------------------------------------
void __fastcall TNewMailNotifier::SetLoggingLevel(int l)
{
  if (!FActive)
    if ((l >= P3A_LOG_NONE) && (l <= P3A_LOG_DETAIL))
      FLoggingLevel = l;
}
//---------------------------------------------------------------------------
void __fastcall TNewMailNotifier::Execute()
{
  SetActive(true);
}
//---------------------------------------------------------------------------
void __fastcall TNewMailNotifier::Abort()
{
  if (FMailSocket->Socket->Connected) {
    FMailSocket->Close();

    while (FMailSocket->Socket->Connected) {}

    LogEvent("[POP3 NOTI] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [ABORT] Transaction aborted");
  }

  FActive = FMailSocket->Socket->Connected;
}
//---------------------------------------------------------------------------
void __fastcall TNewMailNotifier::LogEvent(AnsiString le)
{
  if (FOnLogEvent != NULL)
    FOnLogEvent(this, le);
}
//---------------------------------------------------------------------------
void __fastcall TNewMailNotifier::EventLookup(TObject* Sender, TCustomWinSocket* Socket)
{
  if (FLoggingLevel > P3A_LOG_NONE)
    LogEvent("[POP3 NOTI] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [LOOKUP] Looking up host " + Host);
}
//---------------------------------------------------------------------------
void __fastcall TNewMailNotifier::EventConnect(TObject* Sender, TCustomWinSocket* Socket)
{
  if (FLoggingLevel > P3A_LOG_NONE)
    LogEvent("[POP3 NOTI] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [CONNECT] Connect to host " + Socket->RemoteHost + ":" + IntToStr(Socket->RemotePort) + " (" + Socket->RemoteAddress + ")");

  FNumberOfMessages = 0;
  FMailboxSize = 0;
  Step = 0;

  *RecvData = "";
  *SendData = "";

  if (FOnConnected != NULL)
    FOnConnected(this);

  EventRead(this, FMailSocket->Socket);
}
//---------------------------------------------------------------------------
void __fastcall TNewMailNotifier::EventDisconnect(TObject* Sender, TCustomWinSocket* Socket)
{
  if (FLoggingLevel > P3A_LOG_NONE)
    LogEvent("[POP3 NOTI] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [DISCONNECT] Disconnect from host " + Socket->RemoteHost + ":" + IntToStr(Socket->RemotePort) + " (" + Socket->RemoteAddress + ")");

  if (FOnDisconnected != NULL)
    FOnDisconnected(this);
}
//---------------------------------------------------------------------------
void __fastcall TNewMailNotifier::EventConnecting(TObject* Sender, TCustomWinSocket* Socket)
{
  if (FLoggingLevel > P3A_LOG_NONE)
    LogEvent("[POP3 NOTI] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [CONNECTING] Connecting to host...");
}
//---------------------------------------------------------------------------
void __fastcall TNewMailNotifier::EventError(TObject* Sender, TCustomWinSocket* Socket, TErrorEvent ErrorEvent, int &ErrorCode)
{
  if (FOnNotifierError != NULL)
    FOnNotifierError(this, ErrorEvent, ErrorCode);

  switch (ErrorEvent) {
    case eeGeneral:
      LogEvent("[POP3 NOTI] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [ERROR] General - " + IntToStr(ErrorCode));
      break;

    case eeSend:
      LogEvent("[POP3 NOTI] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [ERROR] On Write - " + IntToStr(ErrorCode));
      break;

    case eeReceive:
      LogEvent("[POP3 NOTI] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [ERROR] On Read - " + IntToStr(ErrorCode));
      break;

    case eeConnect:
      LogEvent("[POP3 NOTI] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [ERROR] On Connect - " + IntToStr(ErrorCode));
      break;

    case eeDisconnect:
      LogEvent("[POP3 NOTI] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [ERROR] On Disconnect - " + IntToStr(ErrorCode));
      break;

    case eeAccept:
      LogEvent("[POP3 NOTI] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [ERROR] On Accept - " + IntToStr(ErrorCode));
      break;

    default:
      LogEvent("[POP3 NOTI] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [ERROR] Unknown - " + IntToStr(ErrorCode));
  }

  ErrorCode = 0;
}
//---------------------------------------------------------------------------
void __fastcall TNewMailNotifier::EventRead(TObject* Sender, TCustomWinSocket* Socket)
{
  *SendData = "";
  *RecvData = Socket->ReceiveText();

  int s = RecvData->Length();
  if (FLoggingLevel > P3A_LOG_CONNECTS) {
    if (s > 128) {
      LogEvent("[POP3 NOTI] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [READ] [" + Socket->RemoteAddress + ":" + IntToStr(Socket->RemotePort) + "] Data size=" + IntToStr(s));
    } else {
      if (UpperCase(RecvData->SubString(1, 4)) == "PASS")
        LogEvent("[POP3 NOTI] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [READ] [" + Socket->RemoteAddress + ":" + IntToStr(Socket->RemotePort) + "] " + RecvData->SubString(1, 5) + AnsiString::StringOfChar('*', RecvData->Length() - 5));
      else
        LogEvent("[POP3 NOTI] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [READ] [" + Socket->RemoteAddress + ":" + IntToStr(Socket->RemotePort) + "] " + *RecvData);
    }
  }

  if (!RecvData->IsEmpty())
    ProcessInput(Socket);
}
//---------------------------------------------------------------------------
void __fastcall TNewMailNotifier::EventWrite(TObject* Sender, TCustomWinSocket* Socket)
{
  if (FLoggingLevel > P3A_LOG_CONNECTS)
    LogEvent("[POP3 NOTI] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [WRITE] [" + Socket->RemoteAddress + ":" + IntToStr(Socket->RemotePort) + "] " + *SendData);
}
//---------------------------------------------------------------------------
bool __fastcall TNewMailNotifier::Successful()
{
  bool rc;
  AnsiString *t = new AnsiString();

  Tokens->Text = *RecvData;
  *t = Tokens->GetFirstToken();
  rc = (t->AnsiCompare("+OK") == 0);

  delete t;

  return rc;
}
//---------------------------------------------------------------------------
bool __fastcall TNewMailNotifier::Unsuccessful()
{
  bool rc;
  AnsiString *t = new AnsiString();

  Tokens->Text = *RecvData;
  *t = Tokens->GetFirstToken();
  rc = (t->AnsiCompare("-ERR") == 0);

  delete t;

  return rc;
}
//---------------------------------------------------------------------------
void __fastcall TNewMailNotifier::ProcessInput(TCustomWinSocket* Socket)
{
  if (Successful()) {
    Tokens->Text = *RecvData;

    switch (Step) {
      case 0:
        *SendData = "USER " + GetUserID() + "\r\n";
        Step++;
        break;

      case 1:
        *SendData = "PASS " + GetPassword() + "\r\n";
        Step++;
        break;

      case 2:
        *SendData = "STAT\r\n";
        Step++;
        break;

      case 3:
        FNumberOfMessages = 0;
        FMailboxSize = 0;

        try {
          FNumberOfMessages = StrToInt(Trim(Tokens->GetToken(1)));
          FMailboxSize = StrToInt(Trim(Tokens->GetToken(2)));

          Step = 9;
          *SendData = "QUIT\r\n";

        } catch (Exception &e) {
          *SendData = "RSET\r\n";
          Step = 9;
        }

        break;

      case 9:
      default: ;
        FMailSocket->Close();
        Step = 0;

        if (FOnGotInfo != NULL)
          FOnGotInfo(this);
    }
  } else {
    if (Unsuccessful()) {
      *SendData = "QUIT\r\n";
      Step = 9;
    }
  }


  if (Socket->Connected)
    if (!SendData->IsEmpty())
      Socket->SendText(*SendData);
}
//---------------------------------------------------------------------------

