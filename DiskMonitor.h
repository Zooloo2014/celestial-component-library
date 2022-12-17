//---------------------------------------------------------------------------
#ifndef DiskMonitorH
#define DiskMonitorH
//---------------------------------------------------------------------------
#include <stdlib.h>
#include <SysUtils.hpp>
#include <Controls.hpp>
#include <Classes.hpp>
#include <Forms.hpp>
#include <Filectrl.hpp>
#include "DiskMonitorWaitThread.h"
//---------------------------------------------------------------------------
typedef void __fastcall (__closure *TMonitorNotifyDirEvent)(System::TObject* Sender, AnsiString *Pathname);
typedef void __fastcall (__closure *TMonitorNotifyFileEvent)(System::TObject* Sender, AnsiString *Pathname);
typedef void __fastcall (__closure *TMonitorNotifyFileModifyEvent)(System::TObject* Sender, AnsiString *Pathname, int Attributes, long FileSize, TDateTime LastModified);
typedef void __fastcall (__closure *TMonitorNotifyFileRenameEvent)(System::TObject* Sender, AnsiString *OldPathname, AnsiString *NewPathname);
typedef void __fastcall (__closure *TMonitorNotifyAttrEvent)(System::TObject* Sender, AnsiString *Pathname, int Attributes);
typedef void __fastcall (__closure *TMonitorNotifySizeEvent)(System::TObject* Sender, AnsiString *Pathname, long Size);
typedef void __fastcall (__closure *TMonitorNotifyTimeEvent)(System::TObject* Sender, AnsiString *Pathname, TDateTime FileDateTime);
//---------------------------------------------------------------------------
class PACKAGE TDiskMonitor : public TComponent
{
private:
        TMonitorNotifyDirEvent  FOnDirEvent;
        TMonitorNotifyFileEvent FOnFileCreated;
        TMonitorNotifyFileEvent FOnFileDeleted;
        TMonitorNotifyFileModifyEvent FOnFileModified;
        TMonitorNotifyFileRenameEvent FOnFileRenamed;
        TMonitorNotifyAttrEvent FOnAttributeChange;
        TMonitorNotifySizeEvent FOnSizeChange;
        TMonitorNotifyTimeEvent FOnModifiedTimeChange;

        bool FActive;
        bool FExcludeSysFiles;
        long FReceivedEvents;
        long FHandledEvents;

        bool Suspended;
        double CleanupProgress;

        TStringList *Monitors;
        AnsiString *MultiplexFile;
        AnsiString *AuxiliaryFile;

protected:
        void __fastcall SetActive(bool act);
        void __fastcall SetExcludeSysFiles(bool esf);
        int  __fastcall GetMonitorCount();
        void __fastcall CleanupMonitors();
//        void __fastcall EventMultiplexer(AnsiString Dir, AnsiString Filename, AnsiString Oldname, TMonitorFileInfo *Info, TMonitorFileInfo *Oldinfo, TMonitorChangedItem Item);
        void __fastcall EventMultiplexer();
        bool __fastcall DirExists(AnsiString dir);

public:
        __fastcall TDiskMonitor(TComponent* Owner);
        __fastcall ~TDiskMonitor();

        void __fastcall Open();
        void __fastcall Close();
        void __fastcall Suspend();
        void __fastcall Resume();
        void __fastcall AddMonitorPath(TFileName Pathname, bool WatchSubTree);

        void __fastcall ResetCounters();
        double __fastcall GetCleanupProgress();

__published:
        __property bool Active = {read=FActive, write=SetActive, default=false};
        __property bool ExcludeSysFiles = {read=FExcludeSysFiles, write=SetExcludeSysFiles, default=true};
        __property int MonitorCount = {read=GetMonitorCount};
        __property long ReceivedEvents = {read=FReceivedEvents};
        __property long HandledEvents = {read=FHandledEvents};

        __property TMonitorNotifyDirEvent  OnDirEvent = {read=FOnDirEvent, write=FOnDirEvent};
        __property TMonitorNotifyFileEvent OnFileCreated = {read=FOnFileCreated, write=FOnFileCreated};
        __property TMonitorNotifyFileEvent OnFileDeleted = {read=FOnFileDeleted, write=FOnFileDeleted};
        __property TMonitorNotifyFileModifyEvent OnFileModified = {read=FOnFileModified, write=FOnFileModified};
        __property TMonitorNotifyFileRenameEvent OnFileRenamed = {read=FOnFileRenamed, write=FOnFileRenamed};
        __property TMonitorNotifyAttrEvent OnAttributeChange = {read=FOnAttributeChange, write=FOnAttributeChange};
        __property TMonitorNotifySizeEvent OnSizeChange = {read=FOnSizeChange, write=FOnSizeChange};
        __property TMonitorNotifyTimeEvent OnModifiedTimeChange = {read=FOnModifiedTimeChange, write=FOnModifiedTimeChange};
};
//---------------------------------------------------------------------------
#endif

