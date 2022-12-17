//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "PopPeeker.h"
#include <Filectrl.hpp>
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//

static inline void ValidCtrCheck(TPopPeeker *)
{
        new TPopPeeker(NULL);
}
//---------------------------------------------------------------------------
__fastcall TPopPeeker::TPopPeeker(TComponent* Owner)
        : TComponent(Owner)
{
  FActive = false;
  FPurge = false;
  FBruteSizeRule = false;
  FRetrievalOrder = P3RO_DEFAULT;

  FHost = new AnsiString();
  FPort = 0;
  FLoggingLevel = P3A_LOG_CONNECTS;

  FUserID = new AnsiString();
  FPassword = new AnsiString();

  Tokens = new TTokenizer(this);
  Tokens->Delimiters = " \n";


  FMailSocket = new TClientSocket(this);
  FMailSocket->ClientType = ctNonBlocking;
  FMailSocket->OnLookup = EventLookup;
  FMailSocket->OnConnect = EventConnect;
  FMailSocket->OnDisconnect = EventDisconnect;
  FMailSocket->OnConnecting = EventConnecting;
  FMailSocket->OnError = EventError;
  FMailSocket->OnRead = EventRead;
  FMailSocket->OnWrite = EventWrite;

  RecvData = new AnsiString();
  SendData = new AnsiString();

  MsgInfo = NULL;

  DeletedMsgs = new TStringList();
  MailMsg = new TMemoryStream();
  TempStr = new AnsiString();
}
//---------------------------------------------------------------------------
__fastcall TPopPeeker::~TPopPeeker()
{
  if (MsgInfo != NULL)
    free(MsgInfo);

  delete FMailSocket;
  delete FHost;
  delete Tokens;
  delete RecvData;
  delete SendData;
  delete DeletedMsgs;
  delete MailMsg;
  delete TempStr;
  delete FPassword;
  delete FUserID;
}
//---------------------------------------------------------------------------
namespace Poppeeker
{
        void __fastcall PACKAGE Register()
        {
                 TComponentClass classes[1] = {__classid(TPopPeeker)};
                 RegisterComponents("Celestial", classes, 0);
        }
}
//---------------------------------------------------------------------------
int __fastcall TPopPeeker::GetPort()
{
  return FPort;
}
//---------------------------------------------------------------------------
/*
void __fastcall TPopPeeker::SetPort(int p)
{
  if (!FActive)
    FPort = p;
}
*/
//---------------------------------------------------------------------------
void __fastcall TPopPeeker::SetPortA(int p)
{
  if (!FActive)
    FPort = p;
}
//---------------------------------------------------------------------------
void __fastcall TPopPeeker::SetRetrievalOrder(int ro)
{
  if (!FActive)
    if ((ro >= P3RO_DEFAULT) && (ro <= P3RO_SMALLEST_FIRST))
      FRetrievalOrder = ro;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TPopPeeker::GetUserID()
{
  return *FUserID;
}
//---------------------------------------------------------------------------
void __fastcall TPopPeeker::SetUserID(AnsiString uid)
{
  if (!FActive)
    *FUserID = uid;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TPopPeeker::GetPassword()
{
  return *FPassword;
}
//---------------------------------------------------------------------------
void __fastcall TPopPeeker::SetPassword(AnsiString pwd)
{
  if (!FActive)
    *FPassword = pwd;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TPopPeeker::GetHost()
{
  return *FHost;
}
//---------------------------------------------------------------------------
void __fastcall TPopPeeker::SetHost(AnsiString h)
{
  if (!FActive)
    *FHost = Trim(h);
}
//---------------------------------------------------------------------------
void __fastcall TPopPeeker::SetPurge(bool p)
{
  FPurge = p;
}
//---------------------------------------------------------------------------
void __fastcall TPopPeeker::SetBruteSizeRule(bool b)
{
  FBruteSizeRule = b;
}
//---------------------------------------------------------------------------
void __fastcall TPopPeeker::SetActive(bool act)
{
  if (!ComponentState.Contains(csDesigning)) {
    switch (act) {
      case true:
        if (!FActive) {

          if (FOnInitRequired != NULL)
            FOnInitRequired(this);

          if (FHost->IsEmpty())
            throw Exception("Host property not set");
          if (FPort == 0)
            throw Exception("Port property not set");
          if (FUserID->IsEmpty())
            throw Exception("UserID property not set");

          FMailSocket->Host = *FHost;
          FMailSocket->Port = FPort;
          FMailSocket->Open();

          FActive = true;
        } else
          throw Exception("Agent already active");
        break;

      case false:
        if (FActive) {
          FMailSocket->Close();
          FActive = false;
        } else
          throw Exception("Agent already inactive");
        break;

      default: ;
    }
  } else
    FActive = false;
}
//---------------------------------------------------------------------------
void __fastcall TPopPeeker::SetLoggingLevel(int l)
{
  if (!FActive)
    if ((l >= P3A_LOG_NONE) && (l <= P3A_LOG_DETAIL))
      FLoggingLevel = l;
}
//---------------------------------------------------------------------------
void __fastcall TPopPeeker::Execute()
{
  SetActive(true);
}
//---------------------------------------------------------------------------
void __fastcall TPopPeeker::Abort()
{
  if (FMailSocket->Socket->Connected == true) {
    FMailSocket->Close();

    while (FMailSocket->Socket->Connected == true)
      Application->ProcessMessages();

    LogEvent("[POP3 PEEK] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [ABORT] Transaction aborted");
  }

  FActive = FMailSocket->Socket->Connected;
}
//---------------------------------------------------------------------------
void __fastcall TPopPeeker::LogEvent(AnsiString le)
{
  if (FOnLogEvent != NULL)
    FOnLogEvent(this, le);
}
//---------------------------------------------------------------------------
void __fastcall TPopPeeker::EventLookup(TObject* Sender, TCustomWinSocket* Socket)
{
  if (FLoggingLevel > P3A_LOG_NONE)
    LogEvent("[POP3 PEEK] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [LOOKUP] Looking up host " + Host);
}
//---------------------------------------------------------------------------
void __fastcall TPopPeeker::EventConnect(TObject* Sender, TCustomWinSocket* Socket)
{
  if (FLoggingLevel > P3A_LOG_NONE)
    LogEvent("[POP3 PEEK] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [CONNECT] Connected to host " + Socket->RemoteHost + ":" + IntToStr(Socket->RemotePort) + " (" + Socket->RemoteAddress + ")");

  FNumberOfMessages = 0;
  FMailboxSize = 0;
  Step = 0;

  *RecvData = "";
  *SendData = "";

  if (FOnConnected != NULL)
    FOnConnected(this);

  Expect = EXP_LINE;
  EventRead(this, FMailSocket->Socket);
}
//---------------------------------------------------------------------------
void __fastcall TPopPeeker::EventDisconnect(TObject* Sender, TCustomWinSocket* Socket)
{
  if (FLoggingLevel > P3A_LOG_NONE)
    LogEvent("[POP3 PEEK] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [DISCONNECT] Disconnected from host " + Socket->RemoteHost + ":" + IntToStr(Socket->RemotePort) + " (" + Socket->RemoteAddress + ")");

  if (FOnDisconnected != NULL)
    FOnDisconnected(this);
}
//---------------------------------------------------------------------------
void __fastcall TPopPeeker::EventConnecting(TObject* Sender, TCustomWinSocket* Socket)
{
  if (FLoggingLevel > P3A_LOG_NONE)
    LogEvent("[POP3 PEEK] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [CONNECTING] Connecting to host...");
}
//---------------------------------------------------------------------------
void __fastcall TPopPeeker::EventError(TObject* Sender, TCustomWinSocket* Socket, TErrorEvent ErrorEvent, int &ErrorCode)
{
  switch (ErrorEvent) {
    case eeGeneral:
      LogEvent("[POP3 PEEK] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [ERROR] General - " + IntToStr(ErrorCode));
      break;

    case eeSend:
      LogEvent("[POP3 PEEK] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [ERROR] On Write - " + IntToStr(ErrorCode));
      break;

    case eeReceive:
      LogEvent("[POP3 PEEK] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [ERROR] On Read - " + IntToStr(ErrorCode));
      break;

    case eeConnect:
      LogEvent("[POP3 PEEK] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [ERROR] On Connect - " + IntToStr(ErrorCode));
      break;

    case eeDisconnect:
      LogEvent("[POP3 PEEK] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [ERROR] On Disconnect - " + IntToStr(ErrorCode));
      break;

    case eeAccept:
      LogEvent("[POP3 PEEK] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [ERROR] On Accept - " + IntToStr(ErrorCode));
      break;

    default:
      LogEvent("[POP3 PEEK] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [ERROR] Unknown - " + IntToStr(ErrorCode));
  }

  ErrorCode = 0;
  FActive = FMailSocket->Socket->Connected;
}
//---------------------------------------------------------------------------
void __fastcall TPopPeeker::EventRead(TObject* Sender, TCustomWinSocket* Socket)
{
  *SendData = "";
  *TempStr = "";

  switch (Expect) {
    case EXP_LIST:
      *TempStr = Socket->ReceiveText();
      *RecvData = *RecvData + *TempStr;
      Proceed = (RecvData->SubString(RecvData->Length() - 4, 5) == "\r\n.\r\n");
      break;

    case EXP_LINE:
    case EXP_MSG:
    default:
      *RecvData = Socket->ReceiveText();
  }


  switch (Expect) {
    case EXP_LIST:
      if (!Proceed)
        break;

    default:
      if (!RecvData->IsEmpty()) {
        int s = RecvData->Length();
        if (FLoggingLevel > P3A_LOG_CONNECTS) {
          if ((Expect == EXP_MSG) || (s > MAX_LOG_DETAIL_SIZE)) {
            LogEvent("[POP3 PEEK] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [READ] [" + Socket->RemoteAddress + ":" + IntToStr(Socket->RemotePort) + "] Data size=" + IntToStr(s));
          } else {
            if (UpperCase(RecvData->SubString(1, 4)) == "PASS")
              LogEvent("[POP3 PEEK] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [READ] [" + Socket->RemoteAddress + ":" + IntToStr(Socket->RemotePort) + "] " + RecvData->SubString(1, 5) + AnsiString::StringOfChar('*', RecvData->Length() - 5));
            else
              LogEvent("[POP3 PEEK] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [READ] [" + Socket->RemoteAddress + ":" + IntToStr(Socket->RemotePort) + "] " + *RecvData);
          }
        }

        ProcessInput(Socket);
      }
  }
}
//---------------------------------------------------------------------------
void __fastcall TPopPeeker::EventWrite(TObject* Sender, TCustomWinSocket* Socket)
{
}
//---------------------------------------------------------------------------
bool __fastcall TPopPeeker::MakeInfoList(AnsiString *text)
{
  bool rc;
  TTokenizer *Tok = new TTokenizer(this);
  TStringList *List = new TStringList();

  List->Text = *text;
  Tok->Delimiters = " \r";

  if (MsgInfo != NULL)
    free(MsgInfo);

  MsgInfo = (TMsgInfo *)malloc(FNumberOfMessages * sizeof(TMsgInfo));
  if (MsgInfo != NULL) {
    for (int i = 1; i <= FNumberOfMessages; i++) {
      Tok->Text = List->Strings[i];
      MsgInfo[i - 1].Number = StrToInt(Tok->GetToken(0));
      MsgInfo[i - 1].Size = StrToInt(Tok->GetToken(1));
      MsgInfo[i - 1].Retrieved = false;
      MsgInfo[i - 1].Deleted = false;
    }


    if (FRetrievalOrder == P3RO_SMALLEST_FIRST) {
      List->Clear();
      for (int i = 0; i < FNumberOfMessages; i++)
        List->AddObject(Format("%9d", ARRAYOFCONST(((int)MsgInfo[i].Size))), (TObject *)MsgInfo[i].Number);

      List->Sort();
      for (int i = 0; i < FNumberOfMessages; i++) {
        MsgInfo[i].Size = StrToInt(List->Strings[i]);
        MsgInfo[i].Number = (int)List->Objects[i];
      }
    }

    rc = true;
  } else
    rc = false;

  delete Tok;
  delete List;

  return rc;
}
//---------------------------------------------------------------------------
bool __fastcall TPopPeeker::Successful()
{
  bool rc;

  if (Proceed) {
    AnsiString *t = new AnsiString();

    Tokens->Text = *RecvData;
    *t = Tokens->GetFirstToken();
    rc = (t->AnsiCompare("+OK") == 0);

    delete t;
  } else
    rc = true;

  return rc;
}
//---------------------------------------------------------------------------
bool __fastcall TPopPeeker::ListEnd(TMemoryStream *ms)
{
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
void __fastcall TPopPeeker::ProcessInput(TCustomWinSocket* Socket)
{
  if (Successful()) {
    Tokens->Text = *RecvData;

    switch (Step) {
      case 0:
        *SendData = "USER " + GetUserID() + "\r\n";
        Expect = EXP_LINE;
        Step++;
        break;

      case 1:
        *SendData = "PASS " + GetPassword() + "\r\n";
        Expect = EXP_LINE;
        Step++;
        break;

      case 2:
        *SendData = "STAT\r\n";
        Expect = EXP_LINE;
        Step++;

        DeletedMsgs->Clear();
        DeletedMsgs->Sorted = true;
        DeletedMsgs->Duplicates = dupIgnore;
        break;

      case 3:
        FNumberOfMessages = 0;
        FMailboxSize = 0;

        try {
          FNumberOfMessages = StrToInt(Trim(Tokens->GetToken(1)));
          FMailboxSize = StrToInt(Trim(Tokens->GetToken(2)));

          if (FNumberOfMessages == 0) {
            if (FOnNoNewMessages != NULL)
              FOnNoNewMessages(this);

            Step = 9;
            *SendData = "QUIT\r\n";
            Expect = EXP_LINE;
          } else {
            *SendData = "LIST\r\n";
            Expect = EXP_LIST;
            Step++;
          }

        } catch (Exception &e) {
          *SendData = "RSET\r\n";
          Expect = EXP_LINE;
          Step = 7;
        }

        break;

      case 4:
        if (MakeInfoList(RecvData)) {
          if (FOnRetrievalStart != NULL)
            FOnRetrievalStart(this);


          for (FCurrMsg = 0; FCurrMsg < FNumberOfMessages; FCurrMsg++) {
            long mSize = MsgInfo[FCurrMsg].Size;
            bool mDel = false;

            if (FOnSizeCheck != NULL)
              FOnSizeCheck(this, MsgInfo[FCurrMsg].Number, mSize, mDel);

            if (FBruteSizeRule) {
              MsgInfo[FCurrMsg].Deleted = mDel;
              if (MsgInfo[FCurrMsg].Deleted)
                DeletedMsgs->Add(IntToStr(MsgInfo[FCurrMsg].Number));
            }
          }


          for (FCurrMsg = 0; FCurrMsg < FNumberOfMessages; FCurrMsg++) {
            if (!MsgInfo[FCurrMsg].Deleted)
              break;
          }

          if (FCurrMsg == FNumberOfMessages) {
            Step = 6;
            break;
          }


          Step++;
          if (FNumberOfMessages > 0) {
/*            FCurrMsg = 0;*/
            FCurrSize = 0;
            Proceed = false;
            *SendData = "TOP " + IntToStr(MsgInfo[FCurrMsg].Number) + " 0\r\n";
            Expect = EXP_LIST;
          }

          MailMsg->Clear();
        } else {
          *SendData = "RSET\r\n";
          Expect = EXP_LINE;
          Step = 7;
        }

        break;

      case 5:
        if (FCurrSize < MsgInfo[FCurrMsg].Size) {
          int length = RecvData->Length();
          MailMsg->Write(RecvData->c_str(), length);
          FCurrSize += length;

          Proceed = (FCurrSize >= MsgInfo[FCurrMsg].Size);
          if (!Proceed)
            Proceed = ListEnd(MailMsg);
        }


        if (Proceed) {
          MsgInfo[FCurrMsg].Retrieved = true;

          if (!MsgInfo[FCurrMsg].Deleted) {
            TMailHeaders *Headers = new TMailHeaders(NULL);
            AnsiString *EmailName = new AnsiString();
            AnsiString *EmailAddr = new AnsiString();
            TMailIdentity *Email;
            bool mDel = false;

            Headers->SetHeaders(MailMsg);

            if (FOnDateCheck != NULL) {
              TDateTime Sent = Headers->GetMsgDate();

              FOnDateCheck(this, MsgInfo[FCurrMsg].Number, Sent, mDel);
              if (mDel)
                DeletedMsgs->Add(IntToStr(MsgInfo[FCurrMsg].Number));
            }


            if (FOnSenderCheck != NULL) {
              Email = Headers->GetFrom();
              *EmailName = Email->DispName;
              if (Email->GetAddressCount() > 0)
                *EmailAddr = Email->GetAddress(0);
              else
                *EmailAddr = "";

              FOnSenderCheck(this, MsgInfo[FCurrMsg].Number, *EmailName, *EmailAddr, mDel);
              if (mDel)
                DeletedMsgs->Add(IntToStr(MsgInfo[FCurrMsg].Number));
            }


            if (FOnHeaderCheck != NULL) {
              FOnHeaderCheck(this, MsgInfo[FCurrMsg].Number, Headers, mDel);
              if (mDel)
                DeletedMsgs->Add(IntToStr(MsgInfo[FCurrMsg].Number));
            }


            delete EmailAddr;
            delete EmailName;
            delete Headers;
          }


          if (FCurrMsg < (FNumberOfMessages - 1)) {
            FCurrMsg++;
            MailMsg->Clear();
            if (MsgInfo[FCurrMsg].Deleted) {
              Proceed = true;
              *SendData = "NOOP\r\n";
              Expect = EXP_LINE;
              FCurrSize = MsgInfo[FCurrMsg].Size;
            } else {
              Proceed = false;
              *SendData = "TOP " + IntToStr(MsgInfo[FCurrMsg].Number) + " 0\r\n";
              Expect = EXP_LIST;
              FCurrSize = 0;
            }
          } else {
            Step++;
            *SendData = "NOOP\r\n";
            Expect = EXP_LINE;

            if (FOnRetrievalEnd != NULL)
              FOnRetrievalEnd(this);
          }
        }

        break;

      case 6:
        if ((DeletedMsgs->Count > 0) && (FPurge)) {
          *SendData = "DELE " + DeletedMsgs->Strings[0] + "\r\n";
          DeletedMsgs->Delete(0);
          break;
        } else
          Step++;  // Fall through to QUIT section...

      case 7:            // Done retrieve...
        *SendData = "QUIT\r\n";
        Expect = EXP_LINE;
        Step++;
        break;

      case 8:
      default: ;
        FMailSocket->Close();
        Step = 0;

        if (FOnSessionClose != NULL)
          FOnSessionClose(this);

        SetActive(false);
    }
  } else {
    *SendData = "QUIT\r\n";
    Expect = EXP_LINE;
    Step = 8;
  }


  if (Socket->Connected) {
    if (FLoggingLevel > P3A_LOG_CONNECTS) {
      if (SendData->Length() > MAX_LOG_DETAIL_SIZE) {
          LogEvent("[POP3 PEEK] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [WRITE] [" + Socket->RemoteAddress + ":" + Socket->RemotePort + "] Data size=" + IntToStr(SendData->Length()));
      } else {
        if (UpperCase(SendData->SubString(1, 5)) == "PASS ")
          LogEvent("[POP3 PEEK] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [WRITE] [" + Socket->RemoteAddress + ":" + Socket->RemotePort + "] " + SendData->SubString(1, 5) + AnsiString::StringOfChar('*', SendData->Length() - 7) + "\r\n");
        else
          LogEvent("[POP3 PEEK] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [WRITE] [" + Socket->RemoteAddress + ":" + Socket->RemotePort + "] " + *SendData);
      }
    }

    *RecvData = "";
    Socket->SendText(*SendData);
  }
}
//---------------------------------------------------------------------------

