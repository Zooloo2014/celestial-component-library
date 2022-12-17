//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include <FileCtrl.hpp>
#include "LMTSService.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//

static inline void ValidCtrCheck(TLMTSService *)
{
        new TLMTSService(NULL);
}
//---------------------------------------------------------------------------
__fastcall TLMTSService::TLMTSService(TComponent *Owner)
	: TComponent(Owner)
{
  FSaveSettingsOnStop = false;
  FDeleteOnRetr = false;
  FStarted = false;
  FValidateCount = 0;
  FAuthenticateCount = 0;
  FMyService = NULL;

  FRegistryKey = new AnsiString();
  FHostName = new AnsiString();
  LookupHostname();

  FMailHome = new AnsiString();
  FMailDir = new AnsiString();
  FDownloadDir = new AnsiString();
  FQueueDir = new AnsiString();
  FDeadLetterDir = new AnsiString();
  FTrashDir = new AnsiString();

  GetMailHome();

  FPopHosts = new AnsiString();
  FSmtpHosts = new AnsiString();
  PopServers = new THostList(this, "POP3 Servers");
  SmtpServers = new THostList(this, "SMTP Servers");
  Mappings = new TStringList();
  LMTSUsers = new THostAccountInfo(this);

  RecUser = new AnsiString();
  RecPass = new AnsiString();
}
//---------------------------------------------------------------------------
__fastcall TLMTSService::~TLMTSService()
{
  delete FRegistryKey;
  delete FHostName;
  delete FMailHome;
  delete FMailDir;
  delete FDownloadDir;
  delete FQueueDir;
  delete FDeadLetterDir;
  delete FTrashDir;
  delete FPopHosts;
  delete FSmtpHosts;
  delete PopServers;
  delete SmtpServers;
  delete LMTSUsers;
  delete Mappings;
  delete RecUser;
  delete RecPass;
}
//---------------------------------------------------------------------------
namespace Lmtsservice
{
        void __fastcall PACKAGE Register()
        {
                 TComponentClass classes[1] = {__classid(TLMTSService)};
                 RegisterComponents("LMTS", classes, 0);
        }
}
//---------------------------------------------------------------------------
void __fastcall TLMTSService::SetService(TService *Serv)
{
  FMyService = Serv;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TLMTSService::GetRegistryKey()
{
  return *FRegistryKey;
}
//---------------------------------------------------------------------------
void __fastcall TLMTSService::SetRegistryKey(AnsiString rk)
{
  *FRegistryKey = rk;

  PopServers->RegistryKey = *FRegistryKey;
  SmtpServers->RegistryKey = *FRegistryKey;
  LMTSUsers->RegistryKey = *FRegistryKey + "\\Accounts";
}
//---------------------------------------------------------------------------
AnsiString __fastcall TLMTSService::GetMailDir()
{
  return *FMailDir;
}
//---------------------------------------------------------------------------
void __fastcall TLMTSService::SetMailDir(AnsiString md)
{
  *FMailDir = md;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TLMTSService::GetDownloadDir()
{
  return *FDownloadDir;
}
//---------------------------------------------------------------------------
void __fastcall TLMTSService::SetDownloadDir(AnsiString dd)
{
  *FDownloadDir = dd;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TLMTSService::GetQueueDir()
{
  return *FQueueDir;
}
//---------------------------------------------------------------------------
void __fastcall TLMTSService::SetQueueDir(AnsiString qd)
{
  *FQueueDir = qd;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TLMTSService::GetDeadLetterDir()
{
  return *FDeadLetterDir;
}
//---------------------------------------------------------------------------
void __fastcall TLMTSService::SetDeadLetterDir(AnsiString dld)
{
  *FDeadLetterDir = dld;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TLMTSService::GetTrashDir()
{
  return *FTrashDir;
}
//---------------------------------------------------------------------------
void __fastcall TLMTSService::SetTrashDir(AnsiString td)
{
  *FTrashDir = td;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TLMTSService::GetHostName()
{
  return *FHostName;
}
//---------------------------------------------------------------------------
int __fastcall TLMTSService::GetUserCount()
{
  return LMTSUsers->UserCount;
}
//---------------------------------------------------------------------------
void __fastcall TLMTSService::LogInfo(AnsiString Msg)
{
  if (FMyService != NULL)
    FMyService->LogMessage(Msg, EVENTLOG_INFORMATION_TYPE, 0, 0);
}
//---------------------------------------------------------------------------
void __fastcall TLMTSService::LogError(AnsiString Msg)
{
  if (FMyService != NULL)
    FMyService->LogMessage(Msg, EVENTLOG_ERROR_TYPE, 0, 0);
}
//---------------------------------------------------------------------------
void __fastcall TLMTSService::LookupHostname()
{
  WSADATA wsaData;
  char Hname[BUF_SIZE];

  if (WSAStartup(MIN_WINSOCK, &wsaData) == 0) {
    if (gethostname(Hname, BUF_SIZE) != SOCKET_ERROR)
      *FHostName = Hname;
    else
      *FHostName = "localhost";
  } else
    *FHostName = "localhost";

  WSACleanup();
}
//---------------------------------------------------------------------------
void __fastcall TLMTSService::GetMailHome()
{
  char Buf[PATH_SIZE];

  memset(Buf, 0, PATH_SIZE);
  if (GetSystemDirectory(Buf, MAX_PATH) != 0)
    *FMailHome = Buf;

  *FMailDir = *FMailHome + "\\LMTS\\MAILBOX";
  *FDownloadDir = *FMailHome + "\\LMTS\\DOWNLOAD";
  *FQueueDir = *FMailHome + "\\LMTS\\QUEUE";
  *FDeadLetterDir = *FMailHome + "\\LMTS\\DEADLETTER";
  *FTrashDir = *FMailHome + "\\LMTS\\TRASH";
}
//---------------------------------------------------------------------------
AnsiString __fastcall TLMTSService::GetAcctPop3()
{
  return PopServers->PreferredAccount;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TLMTSService::GetPwrdPop3()
{
  return PopServers->PreferredPassword;
}
//---------------------------------------------------------------------------
int __fastcall TLMTSService::GetPortPop3()
{
  return PopServers->PreferredPort;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TLMTSService::GetAcctSmtp()
{
  return SmtpServers->PreferredAccount;
}
//---------------------------------------------------------------------------
int __fastcall TLMTSService::GetPortSmtp()
{
  return SmtpServers->PreferredPort;
}
//---------------------------------------------------------------------------
void __fastcall TLMTSService::ClearUsernames()
{
  LMTSUsers->ClearAccounts();
}
//---------------------------------------------------------------------------
void __fastcall TLMTSService::LoadInfoFromRegistry()
{
  if (!FRegistryKey->IsEmpty()) {
    PopServers->LoadFromRegistryMachine();
    SmtpServers->LoadFromRegistryMachine();

    
    LMTSUsers->ClearAccounts();
    LMTSUsers->RegistryKey = *FRegistryKey + "\\Accounts";
    LMTSUsers->LoadFromRegistryMachine();


    TRegistry *Reg = new TRegistry();
    AnsiString *rk = new AnsiString();
    AnsiString *email = new AnsiString();
    TStringList *addr = new TStringList();
    TStringList *map = new TStringList();
    AnsiString *mapusr = new AnsiString();
    AnsiString *mapstr = new AnsiString();

    Reg->RootKey = HKEY_LOCAL_MACHINE;

    if (Reg->OpenKey(*FRegistryKey, false)) {
      try {
        *FMailDir = Reg->ReadString("Mailbox");
      } catch (Exception &e) {
      }

      try {
        *FDownloadDir = Reg->ReadString("Download");
      } catch (Exception &e) {
      }

      try {
        *FQueueDir = Reg->ReadString("Queue");
      } catch (Exception &e) {
      }

      try {
        *FDeadLetterDir = Reg->ReadString("DeadLetter");
      } catch (Exception &e) {
      }

      try {
        *FTrashDir = Reg->ReadString("Trash");
      } catch (Exception &e) {
      }

      PollDelay = Reg->ReadInteger("PDelay");
      DeleteOnRetr = Reg->ReadBool("DOR");

      Reg->CloseKey();
    }


    Mappings->Clear();

    for (int i = 0; i < LMTSUsers->UserCount; i++) {
      *rk = *FRegistryKey + "\\Accounts\\" + LMTSUsers->ExtractUserID(LMTSUsers->UserID->Strings[i]);
      *mapusr = LowerCase(LMTSUsers->ExtractUserID(LMTSUsers->UserID->Strings[i]));

      if (Reg->OpenKey(*rk, false)) {
        addr->Clear();
        Reg->GetValueNames(addr);
        addr->Sort();

        for (int j = 0; j < addr->Count; j++) {
          if (addr->Strings[j].AnsiPos("Fname") == 1) {
            *email = Reg->ReadString(addr->Strings[j]);
            SetFullName(*mapusr, *email);
          }
          if (addr->Strings[j].AnsiPos("Email") == 1) {
            *email = Reg->ReadString(addr->Strings[j]);
            AddEmailAddr(*mapusr, *email);
          }
        }

        Reg->CloseKey();


        *rk = *rk + "\\Map";
        if (Reg->OpenKey(*rk, false)) {
          Reg->GetValueNames(map);

          for (int j = 0; j < map->Count; j++)
            Mappings->Add(*mapusr + "=" + Reg->ReadString(map->Strings[j]));

          Reg->CloseKey();
        }
      }
    }

    delete mapstr;
    delete mapusr;
    delete map;
    delete addr;
    delete email;
    delete rk;
    delete Reg;
  } else
    throw Exception("RegistryKey property not set!");
}
//---------------------------------------------------------------------------
void __fastcall TLMTSService::SaveInfoToRegistry()
{
  if (!FRegistryKey->IsEmpty()) {
    PopServers->SaveToRegistryMachine();
    SmtpServers->SaveToRegistryMachine();

    LMTSUsers->SaveToRegistryMachine();


    TRegistry *Reg = new TRegistry();
    AnsiString *rk = new AnsiString();

    Reg->RootKey = HKEY_LOCAL_MACHINE;

    if (Reg->OpenKey(*FRegistryKey, false)) {
      Reg->WriteString("Mailbox", *FMailDir);
      Reg->WriteString("Download", *FDownloadDir);
      Reg->WriteString("Queue", *FQueueDir);
      Reg->WriteString("DeadLetter", *FDeadLetterDir);
      Reg->WriteString("Trash", *FTrashDir);

      Reg->WriteInteger("PDelay", PollDelay);
      Reg->WriteBool("DOR", DeleteOnRetr);

      Reg->CloseKey();
    }

    for (int i = 0; i < LMTSUsers->UserCount; i++) {
      *rk = *FRegistryKey + "\\Accounts\\" + LMTSUsers->ExtractUserID(LMTSUsers->UserID->Strings[i]);
      if (Reg->OpenKey(*rk, false)) {
        TMailIdentity *ptr = (TMailIdentity *)LMTSUsers->GetObject(LMTSUsers->ExtractUserID(LMTSUsers->UserID->Strings[i]));
        if (ptr != NULL) {
          for (int j = 0; j < ptr->GetAddressCount(); j++)
            Reg->WriteString("Email" + IntToStr(j), ptr->GetAddress(j));
        }

        Reg->CloseKey();
      }
    }

    delete rk;
    delete Reg;
  } else
    throw Exception("RegistryKey property not set!");
}
//---------------------------------------------------------------------------
void __fastcall TLMTSService::SetSaveSettingsOnStop(bool sos)
{
  FSaveSettingsOnStop = sos;
}
//---------------------------------------------------------------------------
bool __fastcall TLMTSService::ValidateUsername(AnsiString uname)
{
  FValidateCount++;
  return LMTSUsers->IsUserAccount(uname);
}
//---------------------------------------------------------------------------
bool __fastcall TLMTSService::AuthenticateUsername(AnsiString uname, AnsiString pword)
{
  bool rv = LMTSUsers->IsPasswordValid(uname, pword);

  if (rv) {
    FValidateCount = 0;
    FAuthenticateCount = 0;
  } else
    FAuthenticateCount++;

  return rv;
}
//---------------------------------------------------------------------------
void __fastcall TLMTSService::DeleteUser(AnsiString uname)
{
  if (!FRegistryKey->IsEmpty())
    LMTSUsers->RemoveAccount(uname);
  else
    throw Exception("RegistryKey property not set!");
}
//---------------------------------------------------------------------------
void __fastcall TLMTSService::AddUser(AnsiString uname)
{
  LMTSUsers->AddAccount(uname, LowerCase(uname));
}
//---------------------------------------------------------------------------
void __fastcall TLMTSService::SetFullName(AnsiString uname, AnsiString fname)
{
  if (!LMTSUsers->IsUserAccount(uname))
    AddUser(uname);

  TMailIdentity *ptr = (TMailIdentity *)LMTSUsers->GetObject(uname);
  if (ptr == NULL)
    ptr = new TMailIdentity(this);

  ptr->DispName = fname;
  LMTSUsers->SetObject(uname, ptr);
}
//---------------------------------------------------------------------------
void __fastcall TLMTSService::AddEmailAddr(AnsiString uname, AnsiString email)
{
  if (!LMTSUsers->IsUserAccount(uname))
    AddUser(uname);

  TMailIdentity *ptr = (TMailIdentity *)LMTSUsers->GetObject(uname);
  if (ptr == NULL)
    ptr = new TMailIdentity(this);

  ptr->AddAddress(email);
  LMTSUsers->SetObject(uname, ptr);
}
//---------------------------------------------------------------------------
void __fastcall TLMTSService::ClearEmails(AnsiString uname)
{
  TMailIdentity *ptr = (TMailIdentity *)LMTSUsers->GetObject(uname);
  if (ptr != NULL)
    ptr->ClearAddresses();
}
//---------------------------------------------------------------------------
void __fastcall TLMTSService::GetEmails(AnsiString uname, TStringList *emails)
{
  TMailIdentity *ptr = (TMailIdentity *)LMTSUsers->GetObject(uname);
  emails->Clear();

  if (ptr != NULL)
    for (int i = 0; i < ptr->GetAddressCount(); i++)
      emails->Add(ptr->GetAddress(i));
}
//---------------------------------------------------------------------------
void __fastcall TLMTSService::SetEmails(AnsiString uname, TStringList *emails)
{
  for (int i = 0; i < emails->Count; i++)
    AddEmailAddr(uname, emails->Strings[i]);
}
//---------------------------------------------------------------------------
void __fastcall TLMTSService::SetUserPassword(AnsiString uname, AnsiString pwrd)
{
  LMTSUsers->SetAccountPassword(uname, pwrd);
}
//---------------------------------------------------------------------------
void __fastcall TLMTSService::Start()
{
  if (FStarted)
    Stop();

  LookupHostname();

  if (!FRegistryKey->IsEmpty()) {
    LoadInfoFromRegistry();

    FStarted = true;

    if (FOnLMTSServiceStarted != NULL)
      FOnLMTSServiceStarted(this);
  } else
    throw Exception("Registry key not set");
}
//---------------------------------------------------------------------------
void __fastcall TLMTSService::Stop()
{
  if (FSaveSettingsOnStop)
   SaveInfoToRegistry();

  FStarted = false;

  if (FOnLMTSServiceStopped != NULL)
    FOnLMTSServiceStopped(this);
}
//---------------------------------------------------------------------------
AnsiString __fastcall TLMTSService::GetHostPop3()
{
  *FPopHosts = PopServers->PreferredHost;

  return *FPopHosts;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TLMTSService::GetHostSmtp()
{
  *FSmtpHosts = SmtpServers->PreferredHost;

  return *FSmtpHosts;
}
//---------------------------------------------------------------------------
void __fastcall TLMTSService::ShowPop3HostInfoDialog()
{
  PopServers->ShowDialog();
}
//---------------------------------------------------------------------------
void __fastcall TLMTSService::ShowSmtpHostInfoDialog()
{
  SmtpServers->ShowDialog();
}
//---------------------------------------------------------------------------
THostList * _fastcall TLMTSService::GetPopList()
{
  return PopServers;
}
//---------------------------------------------------------------------------
THostList * _fastcall TLMTSService::GetSmtpList()
{
  return SmtpServers;
}
//---------------------------------------------------------------------------
THostAccountInfo * _fastcall TLMTSService::GetUserList()
{
  return LMTSUsers;
}
//---------------------------------------------------------------------------
void __fastcall TLMTSService::AddPopHost(AnsiString hname, int port)
{
  PopServers->Add(hname);
  TRemoteHostInfo *ptr = PopServers->GetHost(hname);
  ptr->Port = port;
  ptr->Service = "Pop3";
}
//---------------------------------------------------------------------------
void __fastcall TLMTSService::AddSmtpHost(AnsiString hname, int port)
{
  SmtpServers->Add(hname);
  TRemoteHostInfo *ptr = SmtpServers->GetHost(hname);
  ptr->Port = port;
  ptr->Service = "Smtp";
}
//---------------------------------------------------------------------------
void __fastcall TLMTSService::AddPopAccount(AnsiString host, AnsiString uid, AnsiString pwd)
{
  TRemoteHostInfo *ptr = PopServers->GetHost(host);
  if (!ptr->IsUserAccount(uid))
    ptr->AddAccount(uid, pwd);
}
//---------------------------------------------------------------------------
void __fastcall TLMTSService::AddSmtpAccount(AnsiString host, AnsiString uid, AnsiString pwd)
{
  TRemoteHostInfo *ptr = SmtpServers->GetHost(host);
  if (!ptr->IsUserAccount(uid))
    ptr->AddAccount(uid, pwd);
}
//---------------------------------------------------------------------------
int  __fastcall TLMTSService::GetPollDelay()
{
  return FPollDelay;
}
//---------------------------------------------------------------------------
void __fastcall TLMTSService::SetPollDelay(int pd)
{
  FPollDelay = pd;
}
//---------------------------------------------------------------------------
void __fastcall TLMTSService::SetDeleteOnRetr(bool dor)
{
  FDeleteOnRetr = dor;
}
//---------------------------------------------------------------------------
TStringList * _fastcall TLMTSService::GetMappingList()
{
  return Mappings;
}
//---------------------------------------------------------------------------
void __fastcall TLMTSService::UnameFromEmail(AnsiString email, AnsiString *uname)
{
  TStringList *ul = new TStringList();

  *uname = "";

  for (int i = 0; i < LMTSUsers->UserID->Count; i++)
    ul->Add(LMTSUsers->UserID->Strings[i]);

  for (int i = 0; i < ul->Count; i++) {
    TMailIdentity *ptr = (TMailIdentity *)LMTSUsers->GetObject(LMTSUsers->ExtractUserID(ul->Strings[i]));
    if (ptr != NULL) {
      if (ptr->ContainsAddress(email) != -1) {
        *uname = LMTSUsers->ExtractUserID(ul->Strings[i]);
        break;
      }
    }
  }

  delete ul;
}
//---------------------------------------------------------------------------
void __fastcall TLMTSService::GenerateStampString(AnsiString *StampString)
{
  TSearchRec sr;
  AnsiString *tn = new AnsiString();
  TTimeStamp t2;

  TTimeStamp t1 = DateTimeToTimeStamp(Now());

  do
    t2 = DateTimeToTimeStamp(Now());
  while ((t1.Time == t2.Time) && (t1.Date == t2.Date));

  *tn = IntToHex(t2.Date, 1) + IntToHex(t2.Time, 1);
  *StampString = *tn;

  delete tn;
}
//---------------------------------------------------------------------------

