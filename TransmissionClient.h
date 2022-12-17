//---------------------------------------------------------------------------
#ifndef TransmissionClientH
#define TransmissionClientH
//---------------------------------------------------------------------------
#include <stdlib.h>
#include <SysUtils.hpp>
#include <Controls.hpp>
#include <Classes.hpp>
#include <Forms.hpp>
#include <ScktComp.hpp>
#include "TransmissionCommon.h"
//---------------------------------------------------------------------------
class PACKAGE TTransmissionClient : public TComponent
{
private:
        TNotifyEvent FOnMessageSent;
        TLogNotifyEvent FOnLogEvent;
        TProtocolNotifyEvent FOnMessageProtocolError;

        long FLastMsgSize;
        long FTotalTransmitted;
        long FNumberOfMsgSent;
        AnsiString *FMessage;
        AnsiString *FResponse;

        bool FEnableLogging;
        bool FActive;
        AnsiString *FHost;
        int FPort;

        AnsiString *RecvData;
        AnsiString *SendData;
        TClientSocket *MySocket;

protected:
        void __fastcall SetActive(bool act);
        void __fastcall SetEnableLogging(bool en);
        AnsiString __fastcall GetHost();
        void __fastcall SetHost(const AnsiString h);
        void __fastcall SetPort(int p);
        AnsiString __fastcall GetMessage();
        void __fastcall SetMessage(AnsiString h);
        AnsiString __fastcall GetResponse();

        void __fastcall Lookup(TObject* Sender, TCustomWinSocket* Socket);
        void __fastcall Connecting(TObject* Sender, TCustomWinSocket* Socket);
        void __fastcall Connect(TObject* Sender, TCustomWinSocket* Socket);
        void __fastcall Disconnect(TObject* Sender, TCustomWinSocket* Socket);
        void __fastcall Read(TObject* Sender, TCustomWinSocket* Socket);
        void __fastcall Write(TObject* Sender, TCustomWinSocket* Socket);
        void __fastcall Error(TObject* Sender, TCustomWinSocket* Socket, TErrorEvent ErrorEvent, int &ErrorCode);
        void __fastcall ProcessInput();
        void __fastcall LogLine(const AnsiString le);

public:
        __fastcall TTransmissionClient(TComponent* Owner);
        __fastcall ~TTransmissionClient();

        void __fastcall DeliverMessage();
        void __fastcall Abort();

__published:
        __property bool Active = {read=FActive};
        __property AnsiString Host = {read=GetHost, write=SetHost};
        __property int Port = {read=FPort, write=SetPort, default=0};
        __property long LastMsgSize = {read=FLastMsgSize};
        __property long TotalTransmitted = {read=FTotalTransmitted};
        __property long NumberOfMsgSent = {read=FNumberOfMsgSent};
        __property bool EnableLogging = {read=FEnableLogging, write=SetEnableLogging, default=false};

        __property AnsiString Message = {read=GetMessage, write=SetMessage};
        __property AnsiString Response = {read=GetResponse};

        __property TNotifyEvent OnMessageSent = {read=FOnMessageSent, write=FOnMessageSent};
        __property TLogNotifyEvent OnLogEvent = {read=FOnLogEvent, write=FOnLogEvent};
        __property TProtocolNotifyEvent OnMessageProtocolError = {read=FOnMessageProtocolError, write=FOnMessageProtocolError};
};
//---------------------------------------------------------------------------
#endif
