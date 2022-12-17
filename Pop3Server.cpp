//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "Pop3Server.h"
#include <FileCtrl.hpp>
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//

static inline void ValidCtrCheck(TPop3Server *)
{
        new TPop3Server(NULL);
}
//---------------------------------------------------------------------------
__fastcall TPop3Server::TPop3Server(TComponent* Owner)
        : TComponent(Owner)
{
  FActive = false;
  FArchiveOnDelete = false;
  FPort = 0;
  FLoggingLevel = POP3_LOG_CONNECTS;

  FServerSocket = new TServerSocket(this);
  FServerSocket->ServerType = stNonBlocking;
  FServerSocket->ThreadCacheSize = 0;
  FServerSocket->OnClientConnect = EventClientConnect;
  FServerSocket->OnClientDisconnect = EventClientDisconnect;
  FServerSocket->OnClientRead = EventClientRead;
  FServerSocket->OnClientWrite = EventClientWrite;

  FOnClientConnected = NULL;
  FOnClientDisconnected = NULL;
  FOnUserValidationRequired = NULL;
  FOnPassValidationRequired = NULL;

  FHostName = new AnsiString();
  FMailDir = new AnsiString();
  FMailFileExt = new AnsiString();
  FMailFileArchive = new AnsiString();
  RecvData = new AnsiString();
  SendData = new AnsiString();
  Login = new AnsiString();
  Password = new AnsiString();

  ServeMail = new TStringList();
  DeleteMail = new TStringList();
  Tokens = new TTokenizer(this);
  Command = new AnsiString();
  Parm1 = new AnsiString();
  Parm2 = new AnsiString();
  MsgHeaders = new TStringList();
  MailHeaders = new TMailHeaders(this);

  UserOK = false;
  PassOK = false;

  NumMsg = 0;
  ByteSize = 0;

  Statistics = new TCmdStatsInfo();
}
//---------------------------------------------------------------------------
__fastcall TPop3Server::~TPop3Server()
{
  if (FServerSocket->Active)
    FServerSocket->Close();

  delete FHostName;
  delete FMailDir;
  delete FMailFileExt;
  delete FMailFileArchive;
  delete RecvData;
  delete SendData;
  delete Login;
  delete Password;
  delete ServeMail;
  delete DeleteMail;
  delete Tokens;
  delete Command;
  delete Parm1;
  delete Parm2;
  delete MsgHeaders;
  delete MailHeaders;
  delete Statistics;
}
//---------------------------------------------------------------------------
namespace Pop3server
{
        void __fastcall PACKAGE Register()
        {
                 TComponentClass classes[1] = {__classid(TPop3Server)};
                 RegisterComponents("Celestial", classes, 0);
        }
}
//---------------------------------------------------------------------------
void __fastcall TPop3Server::SetActive(bool act)
{
  if (act) {
    if (FHostName->IsEmpty())
      *FHostName = "localhost";

    FServerSocket->Port = FPort;
    FServerSocket->Open();
  } else
    FServerSocket->Close();

  FActive = FServerSocket->Active;
}
//---------------------------------------------------------------------------
void __fastcall TPop3Server::SetArchiveOnDelete(bool aod)
{
  FArchiveOnDelete = aod;
}
//---------------------------------------------------------------------------
void __fastcall TPop3Server::SetPortA(int p)
{
  if (!FActive)
    FPort = p;
}
//---------------------------------------------------------------------------
void __fastcall TPop3Server::SetLoggingLevel(int i)
{
  if ((i >= POP3_LOG_NONE) && (i <= POP3_LOG_DETAIL))
    FLoggingLevel = i;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TPop3Server::GetHostName()
{
  return *FHostName;
}
//---------------------------------------------------------------------------
void __fastcall TPop3Server::SetHostName(AnsiString hn)
{
  if (!FActive)
    *FHostName = Trim(hn);
}
//---------------------------------------------------------------------------
AnsiString __fastcall TPop3Server::GetMailDir()
{
  return *FMailDir;
}
//---------------------------------------------------------------------------
void __fastcall TPop3Server::SetMailDir(AnsiString md)
{
  if (!FActive) {
    *FMailDir = Trim(md);

    if (FMailDir->SubString(FMailDir->Length(), 1) == "\\")
      FMailDir->Delete(FMailDir->Length(), 1);
  }
}
//---------------------------------------------------------------------------
AnsiString __fastcall TPop3Server::GetMailFileExt()
{
  return *FMailFileExt;
}
//---------------------------------------------------------------------------
void __fastcall TPop3Server::SetMailFileExt(AnsiString mfe)
{
  if (!FActive) {
    *FMailFileExt = Trim(mfe);

    while (FMailFileExt->SubString(1, 1) == ".")
      FMailFileExt->Delete(1, 1);
  }
}
//---------------------------------------------------------------------------
AnsiString __fastcall TPop3Server::GetMailFileArchive()
{
  return *FMailFileArchive;
}
//---------------------------------------------------------------------------
void __fastcall TPop3Server::SetMailFileArchive(AnsiString mfa)
{
  if (!FActive) {
    *FMailFileArchive = Trim(mfa);

    if (FMailFileArchive->SubString(FMailFileArchive->Length(), 1) == "\\")
      FMailFileArchive->Delete(FMailFileArchive->Length(), 1);
  }
}
//---------------------------------------------------------------------------
void __fastcall TPop3Server::LogEvent(const AnsiString le)
{
  if (FOnLogEvent != NULL)
    FOnLogEvent(this, le);
}
//---------------------------------------------------------------------------
void __fastcall TPop3Server::EventClientConnect(TObject* Sender, TCustomWinSocket* Socket)
{
  ResetUser();
  ServerState = P3SS_AUTH;

  *RecvData = "";
  *SendData = "+OK " + *FHostName + " (" + Trim(UpperCase(Socket->LocalHost)) + ") POP3 server ready\r\n";

  if (FLoggingLevel > POP3_LOG_NONE)
    LogEvent("[POP3 SRVR] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [" + *FHostName + "] [CONNECT] Client connect from " + Socket->RemoteHost + ":" + IntToStr(Socket->RemotePort) + " (" + Socket->RemoteAddress + ")");

  if (OnClientConnected != NULL)
    OnClientConnected(this, Socket->RemoteHost, Socket->RemoteAddress, Socket->RemotePort);

  Socket->SendText(*SendData);
}
//---------------------------------------------------------------------------
void __fastcall TPop3Server::EventClientDisconnect(TObject* Sender, TCustomWinSocket* Socket)
{
  *RecvData = "";
  *SendData = "";

  if (FLoggingLevel > POP3_LOG_NONE)
    LogEvent("[POP3 SRVR] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [" + *FHostName + "] [DISCONNECT] Client disconnect from " + Socket->RemoteHost + ":" + IntToStr(Socket->RemotePort) + " (" + Socket->RemoteAddress + ")");

  if (OnClientDisconnected != NULL)
    OnClientDisconnected(this, Socket->RemoteHost, Socket->RemoteAddress, Socket->RemotePort);

  ResetUser();
}
//---------------------------------------------------------------------------
void __fastcall TPop3Server::EventClientRead(TObject* Sender, TCustomWinSocket* Socket)
{
  *SendData = "";
  *RecvData = Socket->ReceiveText();

  if (FLoggingLevel > POP3_LOG_CONNECTS)
    if (RecvData->Length() > MAX_LOG_DETAIL_SIZE) {
        LogEvent("[POP3 SRVR] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [" + *FHostName + "] [READ] [" + Socket->RemoteAddress + ":" + Socket->RemotePort + "] Data size=" + IntToStr(RecvData->Length()));
    } else {
      if (UpperCase(RecvData->SubString(1, 5)) == "PASS ")
        LogEvent("[POP3 SRVR] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [" + *FHostName + "] [READ] [" + Socket->RemoteAddress + ":" + Socket->RemotePort + "] " + RecvData->SubString(1, 5) + AnsiString::StringOfChar('*', RecvData->Length() - 7) + "\r\n");
      else
        LogEvent("[POP3 SRVR] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [" + *FHostName + "] [READ] [" + Socket->RemoteAddress + ":" + Socket->RemotePort + "] " + *RecvData);
    }

  if (!RecvData->IsEmpty())
    ProcessInput(Socket);
}
//---------------------------------------------------------------------------
void __fastcall TPop3Server::EventClientWrite(TObject* Sender, TCustomWinSocket* Socket)
{
}
//---------------------------------------------------------------------------
void __fastcall TPop3Server::ResetUser()
{
  UserOK = false;
  PassOK = false;

  if (!Login->IsEmpty())
    UnlockMailbox(*Login);

  *Login = "";
  *Password = "";

  ServeMail->Clear();
  DeleteMail->Clear();
}
//---------------------------------------------------------------------------
void __fastcall TPop3Server::Start()
{
  if (!FActive) {
    InitStatistics();

    if (!FMailDir->IsEmpty())
      if (!DirectoryExists(*FMailDir))
        ForceDirectories(*FMailDir);

    if (!FMailFileArchive->IsEmpty())
      if (!DirectoryExists(*FMailFileArchive))
        ForceDirectories(*FMailFileArchive);

    UnlockAllMailboxes();

    SetActive(true);

    if (FOnServerStarted != NULL)
      FOnServerStarted(this);
  }
}
//---------------------------------------------------------------------------
void __fastcall TPop3Server::Stop()
{
  if (FActive) {
    SetActive(false);

    if (FOnServerStopped != NULL)
      FOnServerStopped(this);
  }
}
//---------------------------------------------------------------------------
void __fastcall TPop3Server::ProcessInput(TCustomWinSocket* Socket)
{
  *SendData = "";
  Tokens->Delimiters = " \r\n";
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


//  if (ServerState == P3SS_UPDATE) {
//  }


  if (ServerState == P3SS_AUTH) {
    if (UserOK) {
      if (Command->AnsiCompareIC("PASS") == 0) {
        PassOK = false;

        if (!Parm1->IsEmpty()) {
          *Password = *Parm1;
          if (FOnPassValidationRequired != NULL)
            FOnPassValidationRequired(this, *Login, *Password, PassOK);

          if (PassOK) {
            ServerState = P3SS_TRANS;
            *SendData = "+OK Logged in\r\n";
            LockMailbox(*Login);
            GetMailboxStats();
          } else
            *SendData = "-ERR Access denied\r\n";
        } else
          *SendData = "-ERR Please supply a password\r\n";

        Statistics->Update("PASS", 4, SendData->Length());
      }
    }


    if (Command->AnsiCompareIC("USER") == 0) {
      ResetUser();

      if (!Parm1->IsEmpty()) {
        if (FOnUserValidationRequired != NULL)
          FOnUserValidationRequired(this, *Parm1, UserOK);

        if (UserOK) {
          if (!MailboxIsLocked(*Parm1)) {
            *Login = *Parm1;
            *SendData = "+OK Username okay\r\n";
          } else {
            UserOK = false;
            *SendData = "-ERR Mailbox is locked. Try later.\r\n";
          }
        } else
          *SendData = "-ERR Username unknown\r\n";
      } else
        *SendData = "-ERR Please supply a mailbox or username\r\n";

      Statistics->Update("USER", 4, SendData->Length());
    }


    if (Command->AnsiCompareIC("QUIT") == 0) {
      ServerState = P3SS_FINAL;
      *SendData = "+OK Bye\r\n";
      Statistics->Update("QUIT", 4, SendData->Length());
    }
  }


  if (ServerState == P3SS_TRANS) {
    if (Command->AnsiCompareIC("QUIT") == 0) {
      ServerState = P3SS_UPDATE;
      *SendData = "+OK Bye\r\n";
      Statistics->Update("QUIT", 4, SendData->Length());

      UpdateMailbox();
      ResetUser();
    }


    if (Command->AnsiCompareIC("NOOP") == 0) {
      *SendData = "+OK No operation successful\r\n";
      Statistics->Update("NOOP", 4, SendData->Length());
    }


    if (Command->AnsiCompareIC("RSET") == 0) {
      DeleteMail->Clear();
      *SendData = "+OK RESET successful\r\n";
      Statistics->Update("RSET", 4, SendData->Length());
    }


    if (Command->AnsiCompareIC("STAT") == 0) {
      *SendData = "+OK " + IntToStr(NumMsg) + " " + IntToStr((__int64)ByteSize) + "\r\n";
      Statistics->Update("STAT", 4, SendData->Length());
    }


    if (Command->AnsiCompareIC("LIST") == 0) {
      if (!Parm1->IsEmpty()) {
        int l = StrToInt(*Parm1);
        if ((l > 0) && (l <= ServeMail->Count)) {
          l -= 1;
          if (!ServeMail->Strings[l].AnsiCompare("DELETED") == 0) {
            int s = (int)ServeMail->Objects[l];
            *SendData = "+OK " + IntToStr(l + 1) + " " + IntToStr(s) + "\r\n";
          } else
            *SendData = "-ERR Message deleted\r\n";
        } else
          *SendData = "-ERR Invalid message number\r\n";
      } else {
        *SendData = "+OK " + IntToStr(NumMsg) + " " + IntToStr((__int64)ByteSize) + "\r\n";

        for (int j = 0; j < ServeMail->Count; j++)
          *SendData += IntToStr(j + 1) + " " + IntToStr((int)ServeMail->Objects[j]) + "\r\n";

        *SendData += ".\r\n";
      }

      Statistics->Update("LIST", 4, SendData->Length());
    }


    if (Command->AnsiCompareIC("RETR") == 0) {
      if (!Parm1->IsEmpty()) {
        int r = StrToInt(*Parm1);
        if ((r > 0) && (r <= ServeMail->Count)) {
          r -= 1;
          TStringList *TransFile = new TStringList();
          TransFile->LoadFromFile(ServeMail->Strings[r]);
          *SendData = TransFile->Text;
          delete TransFile;

          *SendData += "\r\n.\r\n";
          *SendData = "+OK Message size " + IntToStr((int)ServeMail->Objects[r]) + " bytes\r\n" + *SendData;
        } else
          *SendData = "-ERR Invalid message number\r\n";
      } else
        *SendData = "-ERR No message number supplied\r\n";

      Statistics->Update("RETR", 4, SendData->Length());
    }


    if (Command->AnsiCompareIC("DELE") == 0) {
      if (!Parm1->IsEmpty()) {
        int d = StrToInt(*Parm1);
        if ((d > 0) && (d <= ServeMail->Count)) {
          d -= 1;
          if (!ServeMail->Strings[d].AnsiCompare("DELETED") == 0) {
            DeleteMail->Add(ServeMail->Strings[d]);
            ServeMail->Strings[d] = "DELETED";
            *SendData = "+OK Message " + IntToStr(d + 1) + " deleted\r\n";
          } else
            *SendData = "-ERR Message already deleted\r\n";
        } else
          *SendData = "-ERR Invalid message number\r\n";
      } else
        *SendData = "-ERR No message number supplied\r\n";

      Statistics->Update("DELE", 4, SendData->Length());
    }


    if (Command->AnsiCompareIC("TOP") == 0) {
      if (!Parm1->IsEmpty()) {
        int t = StrToInt(*Parm1);
        if (t > 0) {
          t -= 1;
          if (!ServeMail->Strings[t].AnsiCompare("DELETED") == 0) {
            LoadHeaders(ServeMail->Strings[t]);

            *SendData = "+OK Here come headers\r\n";
            int hc = MsgHeaders->Count;
            if (!Parm2->IsEmpty()) {
              hc = StrToInt(*Parm2);
              if (hc == 0)
                hc = MsgHeaders->Count;
            }

            for (int j = 0; j < hc; j++)
              *SendData += MsgHeaders->Strings[j] + "\r\n";

            *SendData += ".\r\n";
          } else
            *SendData = "-ERR Message deleted\r\n";
        } else
          *SendData = "-ERR No such message\r\n";
      } else
        *SendData = "-ERR No message number supplied\r\n";

      Statistics->Update("TOP", 3, SendData->Length());
    }


    if (Command->AnsiCompareIC("UIDL") == 0) {
      int t = -1;

      if (!Parm1->IsEmpty()) {
        t = StrToInt(*Parm1);
        if ((t > 0) && (t <= ServeMail->Count)) {
          t -= 1;
          if (ServeMail->Strings[t].AnsiCompare("DELETED") == 0)
            *SendData = "-ERR Message deleted\r\n";
        } else
          *SendData = "-ERR No such message\r\n";
      }

      if (t != -1) {
        LoadHeaders(ServeMail->Strings[t]);
        AnsiString *mid = new AnsiString();

        GetMsgIdentifier(mid);
        *SendData = "+OK " + IntToStr(t + 1) + " " + *mid + "\r\n";

        delete mid;
      } else {
        AnsiString *mid = new AnsiString();

        *SendData = "+OK\r\n";

        for (t = 0; t < ServeMail->Count; t++) {
          LoadHeaders(ServeMail->Strings[t]);
          GetMsgIdentifier(mid);
          *SendData += IntToStr(t + 1) + " " + *mid + "\r\n";
        }

        *SendData += ".\r\n";

        delete mid;
      }

      Statistics->Update("UIDL", 4, SendData->Length());
    }
//    if (Command->AnsiCompareIC("APOP") == 0) ;
  }


  if ((ServerState == P3SS_AUTH) || (ServerState == P3SS_TRANS)) {
    Socket->SendText(*SendData);
    if (FLoggingLevel > POP3_LOG_CONNECTS)
      if (SendData->Length() > MAX_LOG_DETAIL_SIZE)
        LogEvent("[POP3 SRVR] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [" + *FHostName + "] [WRITE] [" + Socket->RemoteAddress + ":" + Socket->RemotePort + "] Data size=" + IntToStr(SendData->Length()));
      else
        LogEvent("[POP3 SRVR] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] [" + *FHostName + "] [WRITE] [" + Socket->RemoteAddress + ":" + Socket->RemotePort + "] " + *SendData);
  } else {
    if ((ServerState == P3SS_UPDATE) || (ServerState == P3SS_FINAL)) {
      Socket->Close();
      ServerState = P3SS_AUTH;
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TPop3Server::LoadHeaders(AnsiString filename)
{
  MailHeaders->SetHeaders(filename);
  MsgHeaders->Clear();
  MailHeaders->GetHeaders(MsgHeaders);
}
//---------------------------------------------------------------------------
void __fastcall TPop3Server::GetMsgIdentifier(AnsiString *mid)
{
  bool Found;

  *mid = "";

  for (int i = 0; i < MsgHeaders->Count; i++) {
    Found = (LowerCase(MsgHeaders->Strings[i].SubString(1, 11)) == "message-id:");

    if (Found) {
      TTokenizer *Tok = new TTokenizer(this);
      Tok->Delimiters = " ";

      Tok->Text = MsgHeaders->Strings[i];
      *mid = Tok->GetToken(1);

      delete Tok;

      break;
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TPop3Server::InitStatistics()
{
  Statistics->ResetStats();

  Statistics->AddCommand("USER");
  Statistics->AddCommand("PASS");
  Statistics->AddCommand("QUIT");
  Statistics->AddCommand("NOOP");
  Statistics->AddCommand("RSET");
  Statistics->AddCommand("STAT");
  Statistics->AddCommand("LIST");
  Statistics->AddCommand("RETR");
  Statistics->AddCommand("DELE");
  Statistics->AddCommand("TOP");
  Statistics->AddCommand("UIDL");
}
//---------------------------------------------------------------------------
void __fastcall TPop3Server::GetMailboxStats()
{
  TSearchRec sr;
  sr.ExcludeAttr = (faVolumeID | faDirectory | faHidden);

  if ((UserOK) && (PassOK)) {
    ServeMail->Clear();
    DeleteMail->Clear();

    NumMsg = 0;
    ByteSize = 0;

    if (FindFirst(*FMailDir + "\\" + *Login + "-*." + *FMailFileExt, faAnyFile, sr) == 0) {
      if ((sr.Attr & faHidden) == 0) {
        ServeMail->AddObject(*FMailDir + "\\" + sr.Name, (TObject *)sr.Size);
        ByteSize += sr.Size;
        NumMsg++;
      }

      while (FindNext(sr) == 0) {
        if ((sr.Attr & faHidden) == 0) {
          ServeMail->AddObject(*FMailDir + "\\" + sr.Name, (TObject *)sr.Size);
          ByteSize += sr.Size;
          NumMsg++;
        }
      }
    }

    FindClose(sr);
  }
}
//---------------------------------------------------------------------------
void __fastcall TPop3Server::UpdateMailbox()
{
  for (int i = 0; i < DeleteMail->Count; i++)
    if (!FArchiveOnDelete)
      DeleteFile(DeleteMail->Strings[i]);
    else
      ArchiveFile(DeleteMail->Strings[i]);

  NumMsg = 0;
  ByteSize = 0;
  ServeMail->Clear();
  DeleteMail->Clear();
}
//---------------------------------------------------------------------------
void __fastcall TPop3Server::ArchiveFile(AnsiString fil)
{
  AnsiString *nn;

  nn = new AnsiString();
  *nn = *FMailFileArchive + "\\" + ExtractFileName(fil);

  if (CopyFile(fil.c_str(), nn->c_str(), true) != 0)
    DeleteFile(fil);

  delete nn;
}
//---------------------------------------------------------------------------
bool __fastcall TPop3Server::MailboxIsLocked(AnsiString uid)
{
  bool rc;
  TSearchRec sr;

  rc = (FindFirst(*FMailDir + "\\" + uid + ".lck", faAnyFile, sr) == 0);
  FindClose(sr);

  return rc;
}
//---------------------------------------------------------------------------
void __fastcall TPop3Server::LockMailbox(AnsiString uid)
{
  int hnd;

  if (!MailboxIsLocked(uid)) {
    hnd = FileCreate(*FMailDir + "\\" + uid + ".lck");
    if (hnd != -1)
      FileClose(hnd);
  }
}
//---------------------------------------------------------------------------
void __fastcall TPop3Server::UnlockMailbox(AnsiString uid)
{
  if (MailboxIsLocked(uid))
    DeleteFile(*FMailDir + "\\" + uid + ".lck");
}
//---------------------------------------------------------------------------
void __fastcall TPop3Server::UnlockAllMailboxes()
{
  TSearchRec sr;
  TStringList *fnames;
  AnsiString *nam;

  fnames = new TStringList();
  nam = new AnsiString();

  if (FindFirst(*FMailDir + "\\*.lck", faAnyFile, sr) == 0) {
    if ((sr.Attr & faDirectory) == 0) {
      *nam = sr.Name;
      fnames->Add(nam->SubString(1, nam->Pos(".") - 1));
    }

    while (FindNext(sr) == 0) {
      if ((sr.Attr & faDirectory) == 0) {
        *nam = sr.Name;
        fnames->Add(nam->SubString(1, nam->Pos(".") - 1));
      }
    }
  }

  FindClose(sr);


  for (int i = 0; i < fnames->Count; i++)
    UnlockMailbox(fnames->Strings[i]);


  delete nam;
  delete fnames;
}
//---------------------------------------------------------------------------
void __fastcall TPop3Server::ResetServerStats()
{
  Statistics->ResetStats();
}
//---------------------------------------------------------------------------
void __fastcall TPop3Server::WriteServerStats(AnsiString fn)
{
  TStringList *ss;
  AnsiString *line;
  long Count;
  long CSize;
  long RSize;

  ss = new TStringList();
  line = new AnsiString();

  *line = "LMTS Pop3 Server Statistics";
  ss->Add(*line);

  *line = "Host=" + *FHostName;
  ss->Add(*line);

  *line = FormatDateTime("yyyy-mm-dd hh:ii:ss", Now());
  ss->Add(*line);

  ss->Add("");

  *line = "+" + AnsiString::StringOfChar('-', 48) + "+";
  ss->Add(*line);
  *line = "|Cmd Count     |Cmd Data        |Reply Data      |";
  ss->Add(*line);
  *line = "+" + AnsiString::StringOfChar('-', 48) + "+";
  ss->Add(*line);

  Count = Statistics->GetCount("USER");
  CSize = Statistics->GetCmdSize("USER");
  RSize = Statistics->GetReplySize("USER");
  *line = "|" + Format("%14d", ARRAYOFCONST(((int)Count))) + "|";
  *line = *line + Format("%16d", ARRAYOFCONST(((int)CSize))) + "|";
  *line = *line + Format("%16d", ARRAYOFCONST(((int)RSize))) + "|";
  ss->Add(*line);

  Count = Statistics->GetCount("PASS");
  CSize = Statistics->GetCmdSize("PASS");
  RSize = Statistics->GetReplySize("PASS");
  *line = "|" + Format("%14d", ARRAYOFCONST(((int)Count))) + "|";
  *line = *line + Format("%16d", ARRAYOFCONST(((int)CSize))) + "|";
  *line = *line + Format("%16d", ARRAYOFCONST(((int)RSize))) + "|";
  ss->Add(*line);

  Count = Statistics->GetCount("QUIT");
  CSize = Statistics->GetCmdSize("QUIT");
  RSize = Statistics->GetReplySize("QUIT");
  *line = "|" + Format("%14d", ARRAYOFCONST(((int)Count))) + "|";
  *line = *line + Format("%16d", ARRAYOFCONST(((int)CSize))) + "|";
  *line = *line + Format("%16d", ARRAYOFCONST(((int)RSize))) + "|";
  ss->Add(*line);

  Count = Statistics->GetCount("NOOP");
  CSize = Statistics->GetCmdSize("NOOP");
  RSize = Statistics->GetReplySize("NOOP");
  *line = "|" + Format("%14d", ARRAYOFCONST(((int)Count))) + "|";
  *line = *line + Format("%16d", ARRAYOFCONST(((int)CSize))) + "|";
  *line = *line + Format("%16d", ARRAYOFCONST(((int)RSize))) + "|";
  ss->Add(*line);

  Count = Statistics->GetCount("RSET");
  CSize = Statistics->GetCmdSize("RSET");
  RSize = Statistics->GetReplySize("RSET");
  *line = "|" + Format("%14d", ARRAYOFCONST(((int)Count))) + "|";
  *line = *line + Format("%16d", ARRAYOFCONST(((int)CSize))) + "|";
  *line = *line + Format("%16d", ARRAYOFCONST(((int)RSize))) + "|";
  ss->Add(*line);

  Count = Statistics->GetCount("STAT");
  CSize = Statistics->GetCmdSize("STAT");
  RSize = Statistics->GetReplySize("STAT");
  *line = "|" + Format("%14d", ARRAYOFCONST(((int)Count))) + "|";
  *line = *line + Format("%16d", ARRAYOFCONST(((int)CSize))) + "|";
  *line = *line + Format("%16d", ARRAYOFCONST(((int)RSize))) + "|";
  ss->Add(*line);

  Count = Statistics->GetCount("LIST");
  CSize = Statistics->GetCmdSize("LIST");
  RSize = Statistics->GetReplySize("LIST");
  *line = "|" + Format("%14d", ARRAYOFCONST(((int)Count))) + "|";
  *line = *line + Format("%16d", ARRAYOFCONST(((int)CSize))) + "|";
  *line = *line + Format("%16d", ARRAYOFCONST(((int)RSize))) + "|";
  ss->Add(*line);

  Count = Statistics->GetCount("RETR");
  CSize = Statistics->GetCmdSize("RETR");
  RSize = Statistics->GetReplySize("RETR");
  *line = "|" + Format("%14d", ARRAYOFCONST(((int)Count))) + "|";
  *line = *line + Format("%16d", ARRAYOFCONST(((int)CSize))) + "|";
  *line = *line + Format("%16d", ARRAYOFCONST(((int)RSize))) + "|";
  ss->Add(*line);

  Count = Statistics->GetCount("DELE");
  CSize = Statistics->GetCmdSize("DELE");
  RSize = Statistics->GetReplySize("DELE");
  *line = "|" + Format("%14d", ARRAYOFCONST(((int)Count))) + "|";
  *line = *line + Format("%16d", ARRAYOFCONST(((int)CSize))) + "|";
  *line = *line + Format("%16d", ARRAYOFCONST(((int)RSize))) + "|";
  ss->Add(*line);

  Count = Statistics->GetCount("TOP");
  CSize = Statistics->GetCmdSize("TOP");
  RSize = Statistics->GetReplySize("TOP");
  *line = "|" + Format("%14d", ARRAYOFCONST(((int)Count))) + "|";
  *line = *line + Format("%16d", ARRAYOFCONST(((int)CSize))) + "|";
  *line = *line + Format("%16d", ARRAYOFCONST(((int)RSize))) + "|";
  ss->Add(*line);

  Count = Statistics->GetCount("UIDL");
  CSize = Statistics->GetCmdSize("UIDL");
  RSize = Statistics->GetReplySize("UIDL");
  *line = "|" + Format("%14d", ARRAYOFCONST(((int)Count))) + "|";
  *line = *line + Format("%16d", ARRAYOFCONST(((int)CSize))) + "|";
  *line = *line + Format("%16d", ARRAYOFCONST(((int)RSize))) + "|";
  ss->Add(*line);

  *line = "+" + AnsiString::StringOfChar('-', 48) + "+";
  ss->Add(*line);

  ss->SaveToFile(fn);

  delete line;
  delete ss;
}
//---------------------------------------------------------------------------

