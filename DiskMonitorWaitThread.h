//---------------------------------------------------------------------------
#ifndef DiskMonitorWaitThreadH
#define DiskMonitorWaitThreadH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <stdlib.h>
#include "Filectrl.hpp"
#include "DiskMonitorFileInfo.h"
//---------------------------------------------------------------------------
#define MIN_TIMEOUT     5
//---------------------------------------------------------------------------
enum TMonitorChangedItem {MCI_UNKNOWN, MCI_PRIVATE, MCI_CREATE, MCI_DELETE, MCI_MODIFY, MCI_RENAME, MCI_ATTR, MCI_SIZE, MCI_DATE};
//---------------------------------------------------------------------------
//typedef void __fastcall (__closure *TMonitorCallback)(AnsiString Dir, AnsiString Filename, AnsiString Oldname, TMonitorFileInfo *Info, TMonitorFileInfo *Oldinfo, TMonitorChangedItem Item);
typedef void __fastcall (__closure *TMonitorCallback)(void);
//---------------------------------------------------------------------------
class TMonitorWaitThread : public TThread
{
private:
        TMonitorCallback FMultiplexer;
        AnsiString *FMonitorPath;
        long FTimeout;

        TStringList *FileList;
        AnsiString *AffectedFile;
        AnsiString *AuxFile;

        HANDLE MyHandle;
        bool WatchSubtree;
        bool AllFiles;

        FILE_NOTIFY_INFORMATION *NotBuf;
        OVERLAPPED *Overlap;
        HANDLE DirHandle;
        DWORD CompKey;
        DWORD FilterCond;
        DWORD BytesBack;
        unsigned int NotBufSize;

        TMonitorChangedItem FEvent;
        TMonitorFileInfo *EventInfo;
        TMonitorFileInfo *EventHistory;

protected:
        long __fastcall GetTimeout();
        void __fastcall SetTimeout(long sec);
        int  __fastcall GetFileCount();
        AnsiString __fastcall GetMonitorPath();
        void __fastcall SetMonitorPath(AnsiString p);

        void __fastcall Execute();
        void __fastcall SetupMonitoring();
        void __fastcall ShutdownMonitoring();

        bool __fastcall IsSpecificFile(AnsiString fname);
        int  __fastcall GetSpecificFileIndex(AnsiString fname);
        bool __fastcall DirExists(AnsiString dir);
        void __fastcall LoadFileInfo(int i, AnsiString fn);
        void __fastcall ClearFileInfo();
        void __fastcall LoadAllFiles();

public:
        __fastcall TMonitorWaitThread(AnsiString Dir, bool SubTree, bool SpecificFiles, TMonitorCallback multip);
        __fastcall ~TMonitorWaitThread();

        void __fastcall AddFilename(AnsiString *fname);
        TMonitorFileInfo * __fastcall GetEventFileInfo();
        TMonitorFileInfo * __fastcall GetEventFileHistory();
        TMonitorChangedItem __fastcall GetEvent();

        __property Terminated;
        __property int FileCount = {read=GetFileCount};
        __property long Timeout = {read=GetTimeout, write=SetTimeout};
        __property AnsiString MonitorPath = {read=GetMonitorPath, write=SetMonitorPath};
};
//---------------------------------------------------------------------------
#endif
