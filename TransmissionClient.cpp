//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "TransmissionClient.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//

static inline void ValidCtrCheck(TTransmissionClient *)
{
        new TTransmissionClient(NULL);
}
//---------------------------------------------------------------------------
__fastcall TTransmissionClient::TTransmissionClient(TComponent* Owner)
        : TComponent(Owner)
{
  FActive = false;
  
  FMessage = new AnsiString();
  FResponse = new AnsiString();

  FHost = new AnsiString();
  FPort = 0;

  RecvData = new AnsiString();
  SendData = new AnsiString();

  FLastMsgSize = 0;
  FTotalTransmitted = 0;

  MySocket = new TClientSocket(this);
  MySocket->ClientType = ctNonBlocking;
  MySocket->Socket->ASyncStyles << asWrite << asRead << asConnect << asClose;
  MySocket->OnLookup = Lookup;
  MySocket->OnConnecting = Connecting;
  MySocket->OnConnect = Connect;
  MySocket->OnDisconnect = Disconnect;
  MySocket->OnRead = Read;
  MySocket->OnWrite = Write;
  MySocket->OnError = Error;
}
//---------------------------------------------------------------------------
__fastcall TTransmissionClient::~TTransmissionClient()
{
//  SetActive(false);
  Abort();

  delete MySocket;

  delete FMessage;
  delete FResponse;
  delete FHost;
  delete RecvData;
  delete SendData;
}
//---------------------------------------------------------------------------
namespace Transmissionclient
{
        void __fastcall PACKAGE Register()
        {
                 TComponentClass classes[1] = {__classid(TTransmissionClient)};
                 RegisterComponents("Celestial", classes, 0);
        }
}
//---------------------------------------------------------------------------
void __fastcall TTransmissionClient::SetActive(bool act)
{
  if (act) {
    if (!MySocket->Socket->Connected) {
      FLastMsgSize = 0;
      MySocket->Host = *FHost;
      MySocket->Port = FPort;
      MySocket->Socket->Lock();
      MySocket->Open();
      MySocket->Socket->Unlock();
      FActive = true;
    } else
      LogLine("[ERROR] Socket is active");
  } else {
    MySocket->Socket->Lock();
    MySocket->Close();
    MySocket->Socket->Unlock();
    FActive = false;
  }
}
//---------------------------------------------------------------------------
void __fastcall TTransmissionClient::SetEnableLogging(bool en)
{
  if (!FActive)
    FEnableLogging = en;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TTransmissionClient::GetHost()
{
  return *FHost;
}
//---------------------------------------------------------------------------
void __fastcall TTransmissionClient::SetHost(AnsiString h)
{
  if (!FActive)
    *FHost = h;
}
//---------------------------------------------------------------------------
void __fastcall TTransmissionClient::SetPort(int p)
{
  if (!FActive)
    FPort = p;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TTransmissionClient::GetMessage()
{
  return *FMessage;
}
//---------------------------------------------------------------------------
void __fastcall TTransmissionClient::SetMessage(AnsiString h)
{
  if (!FActive)
    *FMessage = h;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TTransmissionClient::GetResponse()
{
  return *FResponse;
}
//---------------------------------------------------------------------------
void __fastcall TTransmissionClient::Lookup(TObject* Sender, TCustomWinSocket* Socket)
{
  LogLine("LOCATE");
}
//---------------------------------------------------------------------------
void __fastcall TTransmissionClient::Connecting(TObject* Sender, TCustomWinSocket* Socket)
{
  LogLine("CONNECTING");
}
//---------------------------------------------------------------------------
void __fastcall TTransmissionClient::Connect(TObject* Sender, TCustomWinSocket* Socket)
{
  *RecvData = "";
  *SendData = "";
  LogLine("CONNECT");

  Read(this, MySocket->Socket);
}
//---------------------------------------------------------------------------
void __fastcall TTransmissionClient::Disconnect(TObject* Sender, TCustomWinSocket* Socket)
{
  LogLine("DISCONNECT");
}
//---------------------------------------------------------------------------
void __fastcall TTransmissionClient::Read(TObject* Sender, TCustomWinSocket* Socket)
{
  if (Socket->Connected) {
    *RecvData = Socket->ReceiveText();
    LogLine("[READ] " + *RecvData);

    if (!RecvData->IsEmpty())
      ProcessInput();
  }
}
//---------------------------------------------------------------------------
void __fastcall TTransmissionClient::Write(TObject* Sender, TCustomWinSocket* Socket)
{
}
//---------------------------------------------------------------------------
void __fastcall TTransmissionClient::Error(TObject* Sender, TCustomWinSocket* Socket, TErrorEvent ErrorEvent, int &ErrorCode)
{
  LogLine("[ERROR] " + IntToStr(ErrorCode));
  ErrorCode = 0;
}
//---------------------------------------------------------------------------
void __fastcall TTransmissionClient::DeliverMessage()
{
  if ((!FActive) && (!MySocket->Active))
    SetActive(true);
}
//---------------------------------------------------------------------------
void __fastcall TTransmissionClient::ProcessInput()
{
  if (RecvData->Pos(TRANS_STRING_SUCCESS) == 1) {

    if (RecvData->Pos(TRANS_STRING_GREETING_MSG) > 0) {
      *SendData = *FMessage;

      FLastMsgSize = SendData->Length();
      FTotalTransmitted += FLastMsgSize;
      FNumberOfMsgSent++;
    }

    if (RecvData->Pos(TRANS_STRING_SUCCESS_MSG) > 0) {
      *SendData = "";

      if (FOnMessageSent != NULL)
        FOnMessageSent(this);

      SetActive(false);
    }

  } else {
    LogLine("[ERROR] Protocol broken");
    *SendData = "";

    if (OnMessageProtocolError != NULL)
      OnMessageProtocolError(this, *RecvData);

    SetActive(false);
  }


  if (MySocket->Socket->Connected) {
    if (!SendData->IsEmpty()) {
      MySocket->Socket->SendText(*SendData);
      LogLine("[WRITE] " + *SendData);
    } else
      LogLine("[PROCESS] No data to send");
  }
}
//---------------------------------------------------------------------------
void __fastcall TTransmissionClient::LogLine(AnsiString le)
{
  if (FEnableLogging)
    if (FOnLogEvent != NULL)
      FOnLogEvent(this, le);
}
//---------------------------------------------------------------------------
void __fastcall TTransmissionClient::Abort()
{
  if (MySocket->Socket->Connected) {
    MySocket->Close();

    while (MySocket->Socket->Connected)
      FActive = MySocket->Socket->Connected;

    FActive = MySocket->Socket->Connected;
  }
}
//---------------------------------------------------------------------------

