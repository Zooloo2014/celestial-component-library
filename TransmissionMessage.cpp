//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "TransmissionMessage.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
__fastcall TTransmissionMessage::TTransmissionMessage()
{
  Created = Now();
  Msg = new AnsiString();
  FromHostAddr = new AnsiString();
  FromHostName = new AnsiString();
}
//---------------------------------------------------------------------------
__fastcall TTransmissionMessage::~TTransmissionMessage()
{
  delete Msg;
  delete FromHostAddr;
  delete FromHostName;
}
//---------------------------------------------------------------------------
void __fastcall TTransmissionMessage::SetMessage(AnsiString m)
{
  *Msg = m;
  Modified = Now();
}
//---------------------------------------------------------------------------
void __fastcall TTransmissionMessage::SetMessage(TStringList *m)
{
  *Msg = m->Text;

  if ((Msg->SubString(Msg->Length(), 1) == "\r") || (Msg->SubString(Msg->Length(), 1) == "\n"))
    Msg->Delete(Msg->Length(), 1);
  if ((Msg->SubString(Msg->Length(), 1) == "\r") || (Msg->SubString(Msg->Length(), 1) == "\n"))
    Msg->Delete(Msg->Length(), 1);

  Modified = Now();
}
//---------------------------------------------------------------------------
AnsiString * __fastcall TTransmissionMessage::GetFromHostAddr()
{
  return FromHostAddr;
}
//---------------------------------------------------------------------------
void __fastcall TTransmissionMessage::SetFromHostAddr(AnsiString fh)
{
  *FromHostAddr = fh;
}
//---------------------------------------------------------------------------
AnsiString * __fastcall TTransmissionMessage::GetFromHostName()
{
  return FromHostName;
}
//---------------------------------------------------------------------------
void __fastcall TTransmissionMessage::SetFromHostName(AnsiString fn)
{
  *FromHostName = fn;
}
//---------------------------------------------------------------------------
int __fastcall TTransmissionMessage::GetMessageLines()
{
  return Msg->Length();
}
//---------------------------------------------------------------------------
int __fastcall TTransmissionMessage::GetMessageSize()
{
  return Msg->Length();
}
//---------------------------------------------------------------------------
TDateTime  __fastcall TTransmissionMessage::GetMessageCreated()
{
  return Created;
}
//---------------------------------------------------------------------------
TDateTime  __fastcall TTransmissionMessage::GetMessageModified()
{
  return Modified;
}
//---------------------------------------------------------------------------
