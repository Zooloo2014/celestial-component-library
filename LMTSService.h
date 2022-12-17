//---------------------------------------------------------------------------
#ifndef LMTSServiceH
#define LMTSServiceH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Classes.hpp>
#include <SvcMgr.hpp>
#include <registry.hpp>
#include <ScktComp.hpp>
#include "MailIdentity.h"
#include "HostAccountInfo.h"
#include "HostList.h"
//---------------------------------------------------------------------------
#define MIN_WINSOCK             2
#define BUF_SIZE                (8 * 1024)
#define PATH_SIZE               (MAX_PATH * 2)
//---------------------------------------------------------------------------
class TLMTSService : public TComponent
{
private:
        TNotifyEvent FOnLMTSServiceStarted;
        TNotifyEvent FOnLMTSServiceStopped;
        bool FStarted;
        bool FSaveSettingsOnStop;
        bool FDeleteOnRetr;
        int FPollDelay;
        int FValidateCount;
        int FAuthenticateCount;

        TService   *FMyService;
        AnsiString *FRegistryKey;

        AnsiString *FHostName;
        AnsiString *FMailHome;
        AnsiString *FMailDir;
        AnsiString *FDownloadDir;
        AnsiString *FQueueDir;
        AnsiString *FDeadLetterDir;
        AnsiString *FTrashDir;

        THostList *PopServers;
        THostList *SmtpServers;
        AnsiString *FPopHosts;
        AnsiString *FSmtpHosts;
        AnsiString *RecUser;
        AnsiString *RecPass;

        THostAccountInfo *LMTSUsers;
        TStringList *Mappings;

protected:
        AnsiString __fastcall GetHostName();
        AnsiString __fastcall GetMailDir();
        void __fastcall SetMailDir(AnsiString md);
        AnsiString __fastcall GetDownloadDir();
        void __fastcall SetDownloadDir(AnsiString dd);
        AnsiString __fastcall GetQueueDir();
        void __fastcall SetQueueDir(AnsiString qd);
        AnsiString __fastcall GetDeadLetterDir();
        void __fastcall SetDeadLetterDir(AnsiString dld);
        AnsiString __fastcall GetTrashDir();
        void __fastcall SetTrashDir(AnsiString td);

        AnsiString __fastcall GetRegistryKey();
        void __fastcall SetRegistryKey(AnsiString rk);
        int  __fastcall GetUserCount();
        AnsiString __fastcall GetAcctPop3();
        void __fastcall SetAcctPop3(AnsiString ap3);
        AnsiString __fastcall GetPwrdPop3();
        void __fastcall SetPwrdPop3(AnsiString pp3);
        AnsiString __fastcall GetAcctSmtp();
        void __fastcall SetAcctSmtp(AnsiString as);
        int __fastcall GetPortPop3();
        int __fastcall GetPortSmtp();
        void __fastcall SetSaveSettingsOnStop(bool sos);
        AnsiString __fastcall GetHostPop3();
        AnsiString __fastcall GetHostSmtp();
        int  __fastcall GetPollDelay();
        void __fastcall SetPollDelay(int pd);
        void __fastcall SetDeleteOnRetr(bool dor);

        void __fastcall LookupHostname();
        void __fastcall GetMailHome();
        void __fastcall ClearUsernames();

public:
	__fastcall TLMTSService(TComponent *Owner);
        __fastcall ~TLMTSService();

        void __fastcall Start();
        void __fastcall Stop();
        void __fastcall LogInfo(AnsiString Msg);
        void __fastcall LogError(AnsiString Msg);
        void __fastcall SetService(TService *Serv);
        void __fastcall SaveInfoToRegistry();
        void __fastcall LoadInfoFromRegistry();
        void __fastcall AddUser(AnsiString uname);
        void __fastcall SetFullName(AnsiString uname, AnsiString fname);
        void __fastcall AddEmailAddr(AnsiString uname, AnsiString email);
        void __fastcall ClearEmails(AnsiString uname);
        void __fastcall GetEmails(AnsiString uname, TStringList *emails);
        void __fastcall SetEmails(AnsiString uname, TStringList *emails);
        void __fastcall SetUserPassword(AnsiString uname, AnsiString pwrd);
        void __fastcall DeleteUser(AnsiString uname);
        bool __fastcall ValidateUsername(AnsiString uname);
        bool __fastcall AuthenticateUsername(AnsiString uname, AnsiString pword);
        void __fastcall AddPopHost(AnsiString hname, int port);
        void __fastcall AddPopAccount(AnsiString host, AnsiString uid, AnsiString pwd);
        void __fastcall AddSmtpAccount(AnsiString host, AnsiString uid, AnsiString pwd);
        void __fastcall AddSmtpHost(AnsiString hname, int port);
        void __fastcall ShowPop3HostInfoDialog();
        void __fastcall ShowSmtpHostInfoDialog();
        THostList * _fastcall GetPopList();
        THostList * _fastcall GetSmtpList();
        THostAccountInfo * _fastcall GetUserList();
        TStringList * _fastcall GetMappingList();
        void __fastcall UnameFromEmail(AnsiString email, AnsiString *uname);
        void __fastcall GenerateStampString(AnsiString *StampString);

__published:
        __property bool Started = {read=FStarted, default=false};
        __property int UserCount = {read=GetUserCount};
        __property int ValidateCount = {read=FValidateCount};
        __property int AuthenticateCount = {read=FAuthenticateCount};
        __property AnsiString HostName = {read=GetHostName};
        __property AnsiString MailDir = {read=GetMailDir, write=SetMailDir};
        __property AnsiString DownloadDir = {read=GetDownloadDir, write=SetDownloadDir};
        __property AnsiString QueueDir = {read=GetQueueDir, write=SetQueueDir};
        __property AnsiString DeadLetterDir = {read=GetDeadLetterDir, write=SetDeadLetterDir};
        __property AnsiString TrashDir = {read=GetTrashDir, write=SetTrashDir};
        __property AnsiString RegistryKey = {read=GetRegistryKey, write=SetRegistryKey};
        __property AnsiString AcctPop3 = {read=GetAcctPop3};
        __property AnsiString PwrdPop3 = {read=GetPwrdPop3};
        __property AnsiString HostPop3 = {read=GetHostPop3};
        __property AnsiString HostSmtp = {read=GetHostSmtp};
        __property AnsiString AcctSmtp = {read=GetAcctSmtp};
        __property int PortPop3 = {read=GetPortPop3};
        __property int PortSmtp = {read=GetPortSmtp};
        __property int PollDelay = {read=GetPollDelay, write=SetPollDelay};
        __property bool DeleteOnRetr = {read=FDeleteOnRetr, write=SetDeleteOnRetr, default=false};
        __property bool SaveSettingsOnStop = {read=FSaveSettingsOnStop, write=SetSaveSettingsOnStop, default=false};

        __property TNotifyEvent OnLMTSServiceStarted = {read=FOnLMTSServiceStarted, write=FOnLMTSServiceStarted};
        __property TNotifyEvent OnLMTSServiceStopped = {read=FOnLMTSServiceStopped, write=FOnLMTSServiceStopped};
};
//---------------------------------------------------------------------------
#endif
