//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "CmdStatsInfo.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
__fastcall TCmdStatsInfo::TCmdStatsInfo()
{
  Commands = new TStringList();
}
//---------------------------------------------------------------------------
__fastcall TCmdStatsInfo::~TCmdStatsInfo()
{
  Clear();

  delete Commands;
}
//---------------------------------------------------------------------------
void __fastcall TCmdStatsInfo::AddCommand(AnsiString Cmd)
{
  int i = GetCmdIndex(Cmd);
  if (i == -1) {
    TCmdInfo *ptr = (TCmdInfo *)malloc(sizeof(TCmdInfo));
    if (ptr != NULL)
      Commands->AddObject(Cmd, (TObject *)ptr);
    else
      throw Exception("Couldn\'t allocate memory!");
  } else
    ResetStats(i);
}
//---------------------------------------------------------------------------
void __fastcall TCmdStatsInfo::RemoveCommand(AnsiString Cmd)
{
  int i = GetCmdIndex(Cmd);
  if (i > -1) {
    TCmdInfo *ptr = (TCmdInfo *)Commands->Objects[i];
    if (ptr != NULL) {
      free(ptr);
      Commands->Objects[i] = NULL;
    }

    Commands->Delete(i);
  } else
    throw Exception("Command not in list!");
}
//---------------------------------------------------------------------------
void __fastcall TCmdStatsInfo::Update(AnsiString Cmd, long CmdBytes, long RepBytes)
{
  int i = Commands->IndexOf(Cmd);
  if (i > -1) {
    TCmdInfo *ptr = (TCmdInfo *)Commands->Objects[i];
    if (ptr != NULL) {
      ptr->Count++;
      ptr->CommandBytes += CmdBytes;
      ptr->ReplyBytes += RepBytes;
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TCmdStatsInfo::ResetStats()
{
  TCmdInfo *ptr;

  for (int i = 0; i < Commands->Count; i++) {
    ptr = (TCmdInfo *)Commands->Objects[i];
    if (ptr != NULL) {
      ptr->Count = 0;
      ptr->CommandBytes = 0;
      ptr->ReplyBytes = 0;
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TCmdStatsInfo::ResetStats(int i)
{
  if (i < Commands->Count) {
    TCmdInfo *ptr = (TCmdInfo *)Commands->Objects[i];
    if (ptr != NULL) {
      ptr->Count = 0;
      ptr->CommandBytes = 0;
      ptr->ReplyBytes = 0;
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TCmdStatsInfo::ResetStats(AnsiString Cmd)
{
  ResetStats(GetCmdIndex(Cmd));
}
//---------------------------------------------------------------------------
void __fastcall TCmdStatsInfo::Clear()
{
  TCmdInfo *ptr;

  for (int i = 0; i < Commands->Count; i++) {
    ptr = (TCmdInfo *)Commands->Objects[i];
    if (ptr != NULL)
      free(ptr);
  }

  Commands->Clear();
}
//---------------------------------------------------------------------------
int __fastcall TCmdStatsInfo::GetCmdIndex(AnsiString Cmd)
{
  return Commands->IndexOf(Cmd);
}
//---------------------------------------------------------------------------
long __fastcall TCmdStatsInfo::GetCount(AnsiString Cmd)
{
  long rc;

  int i = GetCmdIndex(Cmd);
  if (i > -1) {
    TCmdInfo *ptr = (TCmdInfo *)Commands->Objects[i];
    if (ptr != NULL)
      rc = ptr->Count;
    else
      rc = -1;
  } else
    rc = -1;

  return rc;
}
//---------------------------------------------------------------------------
long __fastcall TCmdStatsInfo::GetCmdSize(AnsiString Cmd)
{
  long rc;

  int i = GetCmdIndex(Cmd);
  if (i > -1) {
    TCmdInfo *ptr = (TCmdInfo *)Commands->Objects[i];
    if (ptr != NULL)
      rc = ptr->CommandBytes;
    else
      rc = -1;
  } else
    rc = -1;

  return rc;
}
//---------------------------------------------------------------------------
long __fastcall TCmdStatsInfo::GetReplySize(AnsiString Cmd)
{
  long rc;

  int i = GetCmdIndex(Cmd);
  if (i > -1) {
    TCmdInfo *ptr = (TCmdInfo *)Commands->Objects[i];
    if (ptr != NULL)
      rc = ptr->ReplyBytes;
    else
      rc = -1;
  } else
    rc = -1;

  return rc;
}
//---------------------------------------------------------------------------

