//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include <Registry.hpp>
#include <winsock2.h>

#include "AgentInfo.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
__fastcall TAgentInfo::TAgentInfo(TComponent *Owner)
        : TComponent(Owner)
{
  FLoadedFromRegistry = false;
  WinSockStarted = false;

  FHostName = new AnsiString();
  FMachineName = new AnsiString();
  FMyName = new AnsiString();
  FRegistryKey = new AnsiString();

  FHostPort = 0;

  GetWinSockInfo();
  LoadInfo();
  if (FMyName->IsEmpty())
    GenerateAgentName();

  SetDirty(false);
}
//---------------------------------------------------------------------------
__fastcall TAgentInfo::~TAgentInfo()
{
  if (WinSockStarted)
    WSACleanup();

  SaveInfo();

  delete FHostName;
  delete FMachineName;
  delete FMyName;
  delete FRegistryKey;
}
//---------------------------------------------------------------------------
namespace Agentinfo
{
        void __fastcall PACKAGE Register()
        {
                 TComponentClass classes[1] = {__classid(TAgentInfo)};
                 RegisterComponents("Celestial", classes, 0);
        }
}
//---------------------------------------------------------------------------
bool __fastcall TAgentInfo::IsDirty()
{
  return Dirty;
}
//---------------------------------------------------------------------------
void __fastcall TAgentInfo::SetDirty(bool dirt)
{
  Dirty = dirt;
}
//---------------------------------------------------------------------------
bool __fastcall TAgentInfo::GetLoadedFromRegistry()
{
  return FLoadedFromRegistry;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TAgentInfo::GetMyName()
{
  return *FMyName;
}
//---------------------------------------------------------------------------
void __fastcall TAgentInfo::SetMyName(const AnsiString n)
{
  SetDirty(true);
  *FMyName = n;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TAgentInfo::GetHostName()
{
  return *FHostName;
}
//---------------------------------------------------------------------------
void __fastcall TAgentInfo::SetHostName(const AnsiString n)
{
  SetDirty(true);
  *FHostName = n;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TAgentInfo::GetMachineName()
{
  return *FMachineName;
}
//---------------------------------------------------------------------------
void __fastcall TAgentInfo::SetMachineName(const AnsiString n)
{
  SetDirty(true);
  *FMachineName = n;
}
//---------------------------------------------------------------------------
void __fastcall TAgentInfo::SetHostPort(int p)
{
  SetDirty(true);
  FHostPort = p;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TAgentInfo::GetRegistryKey()
{
  return *FRegistryKey;
}
//---------------------------------------------------------------------------
void __fastcall TAgentInfo::SetRegistryKey(const AnsiString r)
{
  SetDirty(true);
  *FRegistryKey = r;
  LoadInfo();
}
//---------------------------------------------------------------------------
void __fastcall TAgentInfo::LoadInfo()
{
  if (!ComponentState.Contains(csDesigning)) {
    if (!FRegistryKey->IsEmpty()) {
      TRegistry *Reg = new TRegistry();

      Reg->RootKey = HKEY_LOCAL_MACHINE;

      if (Reg->OpenKey(*FRegistryKey, false)) {
        try {
          *FHostName = Reg->ReadString("DVHost");
          FLoadedFromRegistry = true;
        } catch (Exception &e) {
          *FHostName = "";
        }

        try {
          FHostPort = Reg->ReadInteger("DVPort");
          FLoadedFromRegistry = true;
        } catch (Exception &e) {
          FHostPort = 0;
        }

        try {
          *FMyName = Reg->ReadString("DVAgent");
          FLoadedFromRegistry = true;
        } catch (Exception &e) {
          *FMyName = "";
        }

        Reg->CloseKey();
      }

      delete Reg;
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TAgentInfo::SaveInfo()
{
  if (!ComponentState.Contains(csDesigning)) {
    if (IsDirty()) {
      TRegistry *Reg = new TRegistry();

      Reg->RootKey = HKEY_LOCAL_MACHINE;

      if (Reg->OpenKey(*FRegistryKey, false)) {
        Reg->WriteString("DVHost", *FHostName);
        Reg->WriteInteger("DVPort", FHostPort);
        Reg->WriteString("DVAgent", *FMyName);

        Reg->CloseKey();
      }

      delete Reg;

      SetDirty(false);
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TAgentInfo::GetWinSockInfo()
{
  char *hostname;
  WSADATA wsaData;

  if (!WinSockStarted)
    WinSockStarted = (WSAStartup(WINSOCK_VER_REQD, &wsaData) == 0);

  if (WinSockStarted) {
    hostname = (char *)malloc(HOSTNAME_BUF);
    if (hostname != NULL) {
      if (gethostname(hostname, HOSTNAME_BUF) != SOCKET_ERROR)
        *FMachineName = hostname;
      else
        *FMachineName = "localhost";

      free(hostname);
    } else
      *FMachineName = "localhost";
  }
}
//---------------------------------------------------------------------------
void __fastcall TAgentInfo::SetAgentName(AnsiString n)
{
  *FMyName = n;
}
//---------------------------------------------------------------------------
void __fastcall TAgentInfo::GenerateAgentName()
{
  *FMyName = GetMachineName() + "$" + IntToStr((__int64)Application->Handle);
}
//---------------------------------------------------------------------------

