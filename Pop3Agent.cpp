//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "Pop3Agent.h"
#include <Filectrl.hpp>
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//

static inline void ValidCtrCheck(TPop3Agent *)
{
        new TPop3Agent(NULL);
}
//---------------------------------------------------------------------------
__fastcall TPop3Agent::TPop3Agent(TComponent* Owner)
        : TComponent(Owner)
{
  FActive = false;
  FRetrievalOrder = P3RO_DEFAULT;
  FDeleteOnRetrieve = false;
  FOverwriteExisting = false;
  FAlternateMailstore = false;

  FHost = new AnsiString();
  FPort = 0;
  FLoggingLevel = P3A_LOG_CONNECTS;

  FMailDir = new AnsiString();
  FMailFileExt = new AnsiString();
  FMailFilePrefix = new AnsiString();

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

  MailMsg = new TMemoryStream();
  TempStr = new AnsiString();
}
//---------------------------------------------------------------------------
__fastcall TPop3Agent::~TPop3Agent()
{
  if (MsgInfo != NULL)
    free(MsgInfo);

  delete FMailSocket;
  delete FHost;
  delete Tokens;
  delete RecvData;
  delete SendData;
  delete FMailDir;
  delete FMailFileExt;
  delete FMailFilePrefix;
  delete MailMsg;
  delete TempStr;
  delete FPassword;
  delete FUserID;
}
//---------------------------------------------------------------------------
namespace Pop3agent
{
        void __fastcall PACKAGE Register()
        {
                 TComponentClass classes[1] = {__classid(TPop3Agent)};
                 RegisterComponents("Celestial", classes, 0);
        }
}
//---------------------------------------------------------------------------
int __fastcall TPop3Agent::GetPort()
{
  return FPort;
}
//---------------------------------------------------------------------------
void __fastcall TPop3Agent::SetPortA(int p)
{
  if (!FActive)
    FPort = p;
}
//---------------------------------------------------------------------------
void __fastcall TPop3Agent::SetRetrievalOrder(int ro)
{
  if (!FActive)
    if ((ro >= P3RO_DEFAULT) && (ro <= P3RO_SMALLEST_FIRST))
      FRetrievalOrder = ro;
}
//---------------------------------------------------------------------------
void __fastcall TPop3Agent::SetOverwriteExisting(bool oe)
{
  if (!FActive)
    FOverwriteExisting = oe;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TPop3Agent::GetUserID()
{
  return *FUserID;
}
//---------------------------------------------------------------------------
void __fastcall TPop3Agent::SetUserID(AnsiString uid)
{
  if (!FActive)
    *FUserID = uid;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TPop3Agent::GetPassword()
{
  return *FPassword;
}
//---------------------------------------------------------------------------
void __fastcall TPop3Agent::SetPassword(AnsiString pwd)
{
  if (!FActive)
    *FPassword = pwd;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TPop3Agent::GetHost()
{
  return *FHost;
}
//---------------------------------------------------------------------------
void __fastcall TPop3Agent::SetHost(AnsiString h)
{
  if (!FActive)
    *FHost = Trim(h);
}
//---------------------------------------------------------------------------
AnsiString __fastcall TPop3Agent::GetMailDir()
{
  return *FMailDir;
}
//---------------------------------------------------------------------------
void __fastcall TPop3Agent::SetMailDir(AnsiString md)
{
  if (!FActive) {
    *FMailDir = Trim(md);

    if (!FMailDir->IsEmpty()) {
      while (FMailDir->SubString(FMailDir->Length(), 1) == "\\")
        FMailDir->Delete(FMailDir->Length(), 1);
    }
  }
}
//---------------------------------------------------------------------------
AnsiString __fastcall TPop3Agent::GetMailFileExt()
{
  return *FMailFileExt;
}
//---------------------------------------------------------------------------
void __fastcall TPop3Agent::SetMailFileExt(AnsiString mfe)
{
  if (!FActive) {
    *FMailFileExt = Trim(mfe);

    if (!FMailFileExt->IsEmpty())
      while (FMailFileExt->SubString(1, 1) == ".")
        FMailFileExt->Delete(1, 1);
  }
}
//---------------------------------------------------------------------------
AnsiString __fastcall TPop3Agent::GetMailFilePrefix()
{
  return *FMailFilePrefix;
}
//---------------------------------------------------------------------------
void __fastcall TPop3Agent::SetMailFilePrefix(AnsiString mfp)
{
  if (!FActive) {
    *FMailFilePrefix = Trim(mfp);

    if (!FMailFilePrefix->IsEmpty()) {
      if (FMailFilePrefix->Length() > 3)
        FMailFilePrefix->SetLength(3);
    } else
      *FMailFilePrefix = "LMT";
  }
}
//---------------------------------------------------------------------------
void __fastcall TPop3Agent::SetActive(bool act)
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

          if (!FMailDir->IsEmpty())
            if (!DirectoryExists(*FMailDir))
              ForceDirectories(*FMailDir);

          FMailSocket->Host = *FHost;
          FMailSocket->Port = FPort;
          FMailSocket->Open();

          if (FOnAgentBegun != NULL)
            FOnAgentBegun(this);

          FActive = true;
        } else
          throw Exception("Agent already active");
        break;

      case false:
        if (FActive) {
          FMailSocket->Close();
          FActive = false;

          if (FOnAgentCompleted != NULL)
            FOnAgentCompleted(this);
        } else
          throw Exception("Agent already inactive");
        break;

      default: ;
    }
  } else
    FActive = false;
}
//---------------------------------------------------------------------------
void __fastcall TPop3Agent::SetLoggingLevel(int l)
{
  if (!FActive)
    if ((l >= P3A_LOG_NONE) && (l <= P3A_LOG_DETAIL))
      FLoggingLevel = l;
}
//---------------------------------------------------------------------------
void __fastcall TPop3Agent::SetAlternateMailstore(bool ams)
{
  FAlternateMailstore = ams;
}
//---------------------------------------------------------------------------
void __fastcall TPop3Agent::Execute()
{
  SetActive(true);
}
//---------------------------------------------------------------------------
void __fastcall TPop3Agent::Abort()
{
  if (FMailSocket->Socket->Connected == true) {
    FMailSocket->Close();

    while (FMailSocket->Socket->Connected == true)
      Application->ProcessMessages();

    LogEvent("[POP3 AGNT] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [ABORT] Transaction aborted");
  }

  FActive = FMailSocket->Socket->Connected;
}
//---------------------------------------------------------------------------
void __fastcall TPop3Agent::LogEvent(AnsiString le)
{
  if (FOnLogEvent != NULL)
    FOnLogEvent(this, le);
}
//---------------------------------------------------------------------------
void __fastcall TPop3Agent::EventLookup(TObject* Sender, TCustomWinSocket* Socket)
{
  if (FLoggingLevel > P3A_LOG_NONE)
    LogEvent("[POP3 AGNT] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [LOOKUP] Looking up host " + Host);
}
//---------------------------------------------------------------------------
void __fastcall TPop3Agent::EventConnect(TObject* Sender, TCustomWinSocket* Socket)
{
  if (FLoggingLevel > P3A_LOG_NONE)
    LogEvent("[POP3 AGNT] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [CONNECT] Connected to host " + Socket->RemoteHost + ":" + IntToStr(Socket->RemotePort) + " (" + Socket->RemoteAddress + ")");

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
void __fastcall TPop3Agent::EventDisconnect(TObject* Sender, TCustomWinSocket* Socket)
{
  if (FLoggingLevel > P3A_LOG_NONE)
    LogEvent("[POP3 AGNT] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [DISCONNECT] Disconnected from host " + Socket->RemoteHost + ":" + IntToStr(Socket->RemotePort) + " (" + Socket->RemoteAddress + ")");

  if (FOnDisconnected != NULL)
    FOnDisconnected(this);
}
//---------------------------------------------------------------------------
void __fastcall TPop3Agent::EventConnecting(TObject* Sender, TCustomWinSocket* Socket)
{
  if (FLoggingLevel > P3A_LOG_NONE)
    LogEvent("[POP3 AGNT] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [CONNECTING] Connecting to host...");
}
//---------------------------------------------------------------------------
void __fastcall TPop3Agent::EventError(TObject* Sender, TCustomWinSocket* Socket, TErrorEvent ErrorEvent, int &ErrorCode)
{
  switch (ErrorEvent) {
    case eeGeneral:
      LogEvent("[POP3 AGNT] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [ERROR] General - " + IntToStr(ErrorCode));
      break;

    case eeSend:
      LogEvent("[POP3 AGNT] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [ERROR] On Write - " + IntToStr(ErrorCode));
      break;

    case eeReceive:
      LogEvent("[POP3 AGNT] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [ERROR] On Read - " + IntToStr(ErrorCode));
      break;

    case eeConnect:
      LogEvent("[POP3 AGNT] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [ERROR] On Connect - " + IntToStr(ErrorCode));
      break;

    case eeDisconnect:
      LogEvent("[POP3 AGNT] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [ERROR] On Disconnect - " + IntToStr(ErrorCode));
      break;

    case eeAccept:
      LogEvent("[POP3 AGNT] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [ERROR] On Accept - " + IntToStr(ErrorCode));
      break;

    default:
      LogEvent("[POP3 AGNT] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [ERROR] Unknown - " + IntToStr(ErrorCode));
  }

  ErrorCode = 0;
  FActive = FMailSocket->Socket->Connected;
}
//---------------------------------------------------------------------------
void __fastcall TPop3Agent::EventRead(TObject* Sender, TCustomWinSocket* Socket)
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
            LogEvent("[POP3 AGNT] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [READ] [" + Socket->RemoteAddress + ":" + IntToStr(Socket->RemotePort) + "] Data size=" + IntToStr(s));
          } else {
            if (UpperCase(RecvData->SubString(1, 4)) == "PASS")
              LogEvent("[POP3 AGNT] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [READ] [" + Socket->RemoteAddress + ":" + IntToStr(Socket->RemotePort) + "] " + RecvData->SubString(1, 5) + AnsiString::StringOfChar('*', RecvData->Length() - 5));
            else
              LogEvent("[POP3 AGNT] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [READ] [" + Socket->RemoteAddress + ":" + IntToStr(Socket->RemotePort) + "] " + *RecvData);
          }
        }

        ProcessInput(Socket);
      }
  }
}
//---------------------------------------------------------------------------
void __fastcall TPop3Agent::EventWrite(TObject* Sender, TCustomWinSocket* Socket)
{
}
//---------------------------------------------------------------------------
void __fastcall TPop3Agent::GenerateFilename(AnsiString *Newname)
{
  TSearchRec sr;
  AnsiString *tn = new AnsiString();

  do {
    TTimeStamp t = DateTimeToTimeStamp(Now());
    *tn = *FMailDir + "\\" + *FMailFilePrefix + IntToHex(t.Date, 1) + IntToHex(t.Time, 1) + "." + *FMailFileExt;
  } while (FindFirst(*tn, faAnyFile, sr) == 0);

  FindClose(sr);
  *Newname = *tn;

  delete tn;
}
//---------------------------------------------------------------------------
bool __fastcall TPop3Agent::MakeInfoList(AnsiString *text)
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
bool __fastcall TPop3Agent::Successful()
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
bool __fastcall TPop3Agent::ListEnd(TMemoryStream *ms)
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
void __fastcall TPop3Agent::SaveMessage(TMemoryStream *ms, AnsiString filename)
{
  TSearchRec sr;
  TStringList *msg = new TStringList();
  AnsiString *mailfile = new AnsiString();

  *mailfile = ExtractFileName(filename);
  int i = mailfile->AnsiPos(".");
  if (i > 1) {
    *mailfile = mailfile->SubString(1, i - 1);
    *mailfile = *FMailDir + "\\" + *mailfile + "." + *FMailFileExt;
  } else
    *mailfile = *FMailDir + "\\" + *mailfile;


  sr.ExcludeAttr = faDirectory;
  bool Exists = (FindFirst(*mailfile, faAnyFile, sr) == 0);
  FindClose(sr);
  if (Exists) {
    if (FOverwriteExisting)
      DeleteFile(*mailfile);
    else
      throw Exception("Message filename already exists");
  } else {
    ms->Position = 0;
    msg->LoadFromStream(ms);

    while (UpperCase(msg->Strings[0].SubString(1, 3)) != "+OK")
      msg->Delete(0);

    if (UpperCase(msg->Strings[0].SubString(1, 3)) == "+OK")
      msg->Delete(0);  // Delete the +OK line, if present...

    if (msg->Strings[msg->Count - 1] == ".")
      msg->Delete(msg->Count - 1);

    msg->SaveToFile(*mailfile);

    int attr = FileGetAttr(*mailfile);
    attr |= faHidden;
    FileSetAttr(*mailfile, attr);
  }

  delete msg;
}
//---------------------------------------------------------------------------
void __fastcall TPop3Agent::ProcessInput(TCustomWinSocket* Socket)
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

          Step++;
          if (FNumberOfMessages > 0) {
            FCurrMsg = 0;
            FCurrSize = 0;
            Proceed = false;
            *SendData = "RETR " + IntToStr(MsgInfo[FCurrMsg].Number) + "\r\n";
            Expect = EXP_MSG;

            if (FOnMessageStart != NULL)
              FOnMessageStart(this, MsgInfo[FCurrMsg].Number, MsgInfo[FCurrMsg].Size);
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

          if (FOnMessageChunk != NULL) {
            double perc;
            double size;
            if (MsgInfo[FCurrMsg].Size > 0)
              perc = (FCurrSize * 100.0) / MsgInfo[FCurrMsg].Size;
            else
              perc = 0;

            if (perc > 100)
              perc = 100;

            size = FCurrSize;

            FOnMessageChunk(this, MsgInfo[FCurrMsg].Number, size, perc);
          }

          Proceed = (FCurrSize >= MsgInfo[FCurrMsg].Size);
          if (!Proceed)
            Proceed = ListEnd(MailMsg);

        }

        if (Proceed) {
          MsgInfo[FCurrMsg].Retrieved = true;

          if (FOnMessageEnd != NULL)
            FOnMessageEnd(this, MsgInfo[FCurrMsg].Number, FCurrSize);

          bool Cancel = false;
          int MsgID = MsgInfo[FCurrMsg].Number;
          AnsiString *NewFilename = new AnsiString();
          GenerateFilename(NewFilename);

          if (FOnMessageBeforeSave != NULL)
            FOnMessageBeforeSave(this, MsgID, FCurrSize, *NewFilename, Cancel);

          if (!Cancel) {
            if (FOnMessageSave != NULL)
              FOnMessageSave(this, MsgID, FCurrSize, *NewFilename, MailMsg);

            if (!FAlternateMailstore)
              SaveMessage(MailMsg, *NewFilename);
          }

          delete NewFilename;

          if (FOnMessageAfterSave != NULL)
            FOnMessageAfterSave(this, MsgID, FCurrSize);

          if (FCurrMsg < (FNumberOfMessages - 1)) {
            FCurrSize = 0;
            FCurrMsg++;
            Proceed = false;
            MailMsg->Clear();
            *SendData = "RETR " + IntToStr(MsgInfo[FCurrMsg].Number) + "\r\n";
            Expect = EXP_MSG;

            if (FOnMessageStart != NULL)
              FOnMessageStart(this, MsgInfo[FCurrMsg].Number, MsgInfo[FCurrMsg].Size);
          } else {
            Step++;
            CurrDelete = 0;
            *SendData = "NOOP\r\n";
            Expect = EXP_LINE;

            if (FOnRetrievalEnd != NULL)
              FOnRetrievalEnd(this);
          }
        }

        break;

      case 6:
        if (FDeleteOnRetrieve) {
          while (CurrDelete < FNumberOfMessages)
            if (!MsgInfo[CurrDelete].Deleted) {
              if (MsgInfo[CurrDelete].Retrieved)
                break;
              else
                CurrDelete++;
            } else
              CurrDelete++;


          if (CurrDelete < FNumberOfMessages) {
            if (MsgInfo[CurrDelete].Retrieved) {
              *SendData = "DELE " + IntToStr(MsgInfo[CurrDelete].Number) + "\r\n";
              Expect = EXP_LINE;
              MsgInfo[CurrDelete].Deleted = true;
              break;
            } else
              Step++;   // Fall through to QUIT section...
          } else
            Step++;  // Fall through to QUIT section...
        }
        else
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

        ShowHiddenFiles();

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
          LogEvent("[POP3 AGNT] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [WRITE] [" + Socket->RemoteAddress + ":" + Socket->RemotePort + "] Data size=" + IntToStr(SendData->Length()));
      } else {
        if (UpperCase(SendData->SubString(1, 5)) == "PASS ")
          LogEvent("[POP3 AGNT] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [WRITE] [" + Socket->RemoteAddress + ":" + Socket->RemotePort + "] " + SendData->SubString(1, 5) + AnsiString::StringOfChar('*', SendData->Length() - 7) + "\r\n");
        else
          LogEvent("[POP3 AGNT] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [WRITE] [" + Socket->RemoteAddress + ":" + Socket->RemotePort + "] " + *SendData);
      }
    }

    *RecvData = "";
    Socket->SendText(*SendData);
  }
}
//---------------------------------------------------------------------------
void __fastcall TPop3Agent::ShowHiddenFiles()
{
  TSearchRec sr;

  if (FindFirst(*FMailDir + "\\*.*", faHidden, sr) == 0) {
    if ((sr.Attr & faHidden) == faHidden)
      FileSetAttr(*FMailDir + "\\" + sr.Name, sr.Attr & (!faHidden));

    while (FindNext(sr) == 0)
      if ((sr.Attr & faHidden) == faHidden)
        FileSetAttr(*FMailDir + "\\" + sr.Name, sr.Attr & (!faHidden));

    FindClose(sr);
  }
}
//---------------------------------------------------------------------------

