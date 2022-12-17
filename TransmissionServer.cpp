//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "TransmissionServer.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//

static inline void ValidCtrCheck(TTransmissionServer *)
{
        new TTransmissionServer(NULL);
}
//---------------------------------------------------------------------------
__fastcall TTransmissionServer::TTransmissionServer(TComponent* Owner)
        : TComponent(Owner)
{
  FActive = false;
  FEnableBlockList = false;
  FPort = 0;
  FLoggingLevel = LOG_LEVEL_CONNECTS;

  MySocket = new TServerSocket(this);
  MySocket->ServerType = stNonBlocking;
  MySocket->OnClientConnect = ClientConnect;
  MySocket->OnClientDisconnect = ClientDisconnect;
  MySocket->OnClientRead = ClientRead;
  MySocket->OnClientWrite = ClientWrite;
  MySocket->OnClientError = ClientError;
  MySocket->OnAccept = Accept;

  Queue = new TStringList();
  Queue->Sorted = true;

  FBlockedHosts = new TStringList();
  RecvData = new AnsiString();
  SendData = new AnsiString();
}
//---------------------------------------------------------------------------
__fastcall TTransmissionServer::~TTransmissionServer()
{
  if (MySocket->Active)
    SetActive(false);

  delete MySocket;
  delete Queue;
  delete FBlockedHosts;
  delete RecvData;
  delete SendData;
}
//---------------------------------------------------------------------------
namespace Transmissionserver
{
        void __fastcall PACKAGE Register()
        {
                 TComponentClass classes[1] = {__classid(TTransmissionServer)};
                 RegisterComponents("Celestial", classes, 0);
        }
}
//---------------------------------------------------------------------------
void __fastcall TTransmissionServer::SetActive(bool act)
{
  if (act) {
    MySocket->Port = Port;
    MySocket->Open();
  } else {
    MySocket->Close();
    MySocket->Port = 0;
  }

  FActive = act;
}
//---------------------------------------------------------------------------
void __fastcall TTransmissionServer::SetPort(int port)
{
  FPort = port;
}
//---------------------------------------------------------------------------
void __fastcall TTransmissionServer::SetLoggingLevel(int i)
{
  if ((i >= LOG_LEVEL_NONE) && (i <= LOG_LEVEL_CONTENT))
    FLoggingLevel = i;
}
//---------------------------------------------------------------------------
void __fastcall TTransmissionServer::SetEnableBlockList(bool act)
{
  FEnableBlockList = act;
}
//---------------------------------------------------------------------------
TStringList * __fastcall TTransmissionServer::GetBlockedHosts()
{
  return FBlockedHosts;
}
//---------------------------------------------------------------------------
void __fastcall TTransmissionServer::SetBlockedHosts(TStringList *bh)
{
  FBlockedHosts->Clear();
  FBlockedHosts->AddStrings(bh);
}
//---------------------------------------------------------------------------
void __fastcall TTransmissionServer::Start()
{
  if (!FActive)
    SetActive(true);
}
//---------------------------------------------------------------------------
void __fastcall TTransmissionServer::Stop()
{
  if (FActive)
    SetActive(false);
}
//---------------------------------------------------------------------------
void __fastcall TTransmissionServer::AddToQueue(TTransmissionMessage *m)
{
  TDateTime ModTime = m->GetMessageModified();
  Queue->AddObject(FormatDateTime("yyyymmddhhnnss", ModTime), m);
  FQueuedMsgCount = Queue->Count;

  if (FOnMessageQueued != NULL)
    FOnMessageQueued(this);
}
//---------------------------------------------------------------------------
TTransmissionMessage * __fastcall TTransmissionServer::Dequeue()
{
  TTransmissionMessage *rc;

  if (FQueuedMsgCount > 0) {
    rc = (TTransmissionMessage *)Queue->Objects[0];
    Queue->Delete(0);
  } else
    rc = NULL;

  return rc;
}
//---------------------------------------------------------------------------
void __fastcall TTransmissionServer::ClientConnect(TObject* Sender, TCustomWinSocket* Socket)
{
  *RecvData = "";

  FConnectCount++;

  if (FEnableBlockList) {
    int i = FBlockedHosts->IndexOf(Socket->RemoteHost);
    int j = FBlockedHosts->IndexOf(Socket->RemoteAddress);

    if ((i > -1) || (j > -1)) {
      *SendData = TRANS_STRING_BLOCKED_MSG;
      HostIsBlocked = true;
      FBlockedConnectCount++;
    } else {
      *SendData = TRANS_STRING_GREETING_MSG;
      HostIsBlocked = false;
    }
  } else {
    *SendData = TRANS_STRING_GREETING_MSG;
    HostIsBlocked = false;
  }

  Socket->SendText(*SendData);

  if (FLoggingLevel > LOG_LEVEL_NONE)
    LogEvent("Connection by client at " + Socket->RemoteAddress + ":" + IntToStr(Socket->RemotePort));
}
//---------------------------------------------------------------------------
void __fastcall TTransmissionServer::ClientDisconnect(TObject* Sender, TCustomWinSocket* Socket)
{
  if (FLoggingLevel > LOG_LEVEL_NONE)
    if (HostIsBlocked)
      LogEvent("Disconnect from blocked client at " + Socket->RemoteAddress + ":" + IntToStr(Socket->RemotePort));
    else
      LogEvent("Disconnect from client at " + Socket->RemoteAddress + ":" + IntToStr(Socket->RemotePort));
}
//---------------------------------------------------------------------------
void __fastcall TTransmissionServer::ClientRead(TObject* Sender, TCustomWinSocket* Socket)
{
  if (!HostIsBlocked) {
    *RecvData = Socket->ReceiveText();
    if (FLoggingLevel > LOG_LEVEL_CONNECTS)
      LogEvent("[FROM CLIENT] " + *RecvData);
  } else
    *RecvData = "BLOCKED HOST: " + Socket->RemoteHost + "(" + Socket->RemoteAddress + ":" + Socket->RemotePort + ")";

  if (!RecvData->IsEmpty())
    ProcessInput(Socket->RemoteHost, Socket->RemoteAddress);
}
//---------------------------------------------------------------------------
void __fastcall TTransmissionServer::ClientWrite(TObject* Sender, TCustomWinSocket* Socket)
{
  if (FLoggingLevel > LOG_LEVEL_CONNECTS)
    LogEvent("[TO CLIENT] " + *SendData);
}
//---------------------------------------------------------------------------
void __fastcall TTransmissionServer::Accept(TObject* Sender, TCustomWinSocket* Socket)
{
}
//---------------------------------------------------------------------------
void __fastcall TTransmissionServer::LogEvent(const AnsiString le)
{
  if (FOnLogEvent != NULL)
    FOnLogEvent(this, le);
}
//---------------------------------------------------------------------------
void __fastcall TTransmissionServer::ProcessInput(AnsiString remname, AnsiString ipaddr)
{
  TTransmissionMessage *mess = new TTransmissionMessage();
  mess->SetMessage(*RecvData);
  mess->SetFromHostName(remname);
  mess->SetFromHostAddr(ipaddr);

  bool Cancel = false;
  if (FOnNewMessage != NULL)
    FOnNewMessage(this, mess, Cancel);

  if (Cancel) {
    delete mess;
    FDroppedMsgCount++;
  } else
    AddToQueue(mess);

  if (HostIsBlocked) {
    *SendData = TRANS_STRING_SUCCESS_MSG;
    MySocket->Socket->Connections[0]->Close();
  } else {
    *SendData = TRANS_STRING_SUCCESS_MSG;
    MySocket->Socket->Connections[0]->SendText(*SendData);
  }
}
//---------------------------------------------------------------------------
void __fastcall TTransmissionServer::ClientError(TObject* Sender, TCustomWinSocket* Socket, TErrorEvent ErrorEvent, int &ErrorCode)
{
  switch (ErrorEvent) {
    case eeGeneral:
      LogEvent("General Error");
      break;
    case eeSend:
      LogEvent("Send Error");
      break;
    case eeReceive:
      LogEvent("Receive Error");
      break;
    case eeConnect:
      LogEvent("Connect Error");
      break;
    case eeDisconnect:
      LogEvent("Disconnect Error");
      break;
    case eeAccept:
      LogEvent("Accept Error");
      break;
    default:
      LogEvent("Unknown Error");
  }

  ErrorCode = 0;
}
//---------------------------------------------------------------------------
#include "TransmissionMessage.cpp"
//---------------------------------------------------------------------------

