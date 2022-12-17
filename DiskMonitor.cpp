//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "DiskMonitor.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//

static inline void ValidCtrCheck(TDiskMonitor *)
{
        new TDiskMonitor(NULL);
}
//---------------------------------------------------------------------------
__fastcall TDiskMonitor::TDiskMonitor(TComponent* Owner)
        : TComponent(Owner)
{
  FActive = false;
  FExcludeSysFiles = true;

  Suspended = !FActive;
  CleanupProgress = 0;

  Monitors = new TStringList();
  MultiplexFile = new AnsiString();
  AuxiliaryFile = new AnsiString();
}
//---------------------------------------------------------------------------
__fastcall TDiskMonitor::~TDiskMonitor()
{
  if (Monitors->Count > 0)
    CleanupMonitors();

  delete Monitors;
  delete MultiplexFile;
  delete AuxiliaryFile;
}
//---------------------------------------------------------------------------
namespace Diskmonitor
{
        void __fastcall PACKAGE Register()
        {
                 TComponentClass classes[1] = {__classid(TDiskMonitor)};
                 RegisterComponents("Celestial", classes, 0);
        }
}
//---------------------------------------------------------------------------
void __fastcall TDiskMonitor::CleanupMonitors()
{
  TMonitorWaitThread *Thread;

  CleanupProgress = 0;

  for (int i = 0; i < Monitors->Count; i++) {
    Thread = (TMonitorWaitThread *)Monitors->Objects[i];
    if (Thread != NULL) {
      if (!Thread->Terminated) {
        Thread->Terminate();
        Thread->WaitFor();
      }
      delete Thread;
    }

    CleanupProgress = ((i + 1) / Monitors->Count);
  }

  Monitors->Clear();
  ResetCounters();
}
//---------------------------------------------------------------------------
double __fastcall TDiskMonitor::GetCleanupProgress()
{
  return CleanupProgress;
}
//---------------------------------------------------------------------------
//void __fastcall TDiskMonitor::EventMultiplexer(AnsiString Dir, AnsiString Filename, AnsiString Oldname, TMonitorFileInfo *Info, TMonitorFileInfo *Oldinfo, TMonitorChangedItem i)
void __fastcall TDiskMonitor::EventMultiplexer()
{
  TSearchRec sr;
  int attr = 0;
  long size = 0;
  TDateTime fdatetime = 0;
  TMonitorChangedItem Evt;

  if (FActive)
    FReceivedEvents++;

  if ((FActive) && (!Suspended)) {
    FHandledEvents++;

    TMonitorWaitThread *Thread;
    TMonitorFileInfo *Info;

    for (int i = 0; i < Monitors->Count; i++) {
      Thread = (TMonitorWaitThread *)Monitors->Objects[i];
      if (Thread != NULL)
        if (Thread->GetEvent() != MCI_UNKNOWN)
          break;
    }

//    Evt = i;
    Evt = Thread->GetEvent();

//    *MultiplexFile = Dir;
    Info = Thread->GetEventFileInfo();
    *MultiplexFile = Info->GetPathname() + "\\" + Info->GetFilename();

    if (FOnDirEvent != NULL)
      FOnDirEvent(this, MultiplexFile);

/*
    if (!Oldname.IsEmpty()) {   // Create or rename
      *AuxiliaryFile = *MultiplexFile + "\\" + Oldname;  // Old name...
      *MultiplexFile = *MultiplexFile + "\\" + Filename; // New (current) name...
    } else     // Delete
      *MultiplexFile = *MultiplexFile + "\\" + Filename;

    if (Filename.IsEmpty() && Oldname.IsEmpty()) { // Directory event...
      Evt = MCI_UNKNOWN;
    }
*/

    if (Info != NULL) {
      attr = Info->GetAttributes();
      size = Info->GetFileSize();
      fdatetime = Info->GetLastWrite();
    }


    bool Accept;
    if (FExcludeSysFiles) {
      Accept = ((attr & faSysFile) != faSysFile);
    } else
      Accept = true;

    if (Accept) {
      switch (Evt) {
        case MCI_UNKNOWN:
          break;
        case MCI_CREATE:
          if (FOnFileCreated != NULL)
            FOnFileCreated(this, MultiplexFile);
          break;
        case MCI_DELETE:
          if (FOnFileDeleted != NULL)
            FOnFileDeleted(this, MultiplexFile);
          break;
        case MCI_RENAME:
          if (FOnFileRenamed != NULL)
            FOnFileRenamed(this, AuxiliaryFile, MultiplexFile);
          break;
        case MCI_MODIFY:
          if (FOnFileModified != NULL)
            FOnFileModified(this, MultiplexFile, attr, size, fdatetime);
          break;
        case MCI_ATTR:
          if (FOnAttributeChange != NULL)
            FOnAttributeChange(this, MultiplexFile, attr);
          break;
        case MCI_SIZE:
          if (FOnSizeChange != NULL)
            FOnSizeChange(this, MultiplexFile, size);
          break;
        case MCI_DATE:
          if (FOnModifiedTimeChange != NULL)
            FOnModifiedTimeChange(this, MultiplexFile, fdatetime);
          break;
        default: ;
      }
    }
  }
}
//---------------------------------------------------------------------------
bool __fastcall TDiskMonitor::DirExists(AnsiString dir)
{
  bool rc;
  AnsiString *d = new AnsiString();

  *d = GetCurrentDir();
  rc = SetCurrentDir(dir);
  SetCurrentDir(*d);

  return rc;
}
//---------------------------------------------------------------------------
void __fastcall TDiskMonitor::SetActive(bool act)
{
  if (act != FActive) {
    if (act) {
      ResetCounters();
      Resume();
    } else
      CleanupMonitors();

    FActive = act;
  }
}
//---------------------------------------------------------------------------
void __fastcall TDiskMonitor::SetExcludeSysFiles(bool esf)
{
  FExcludeSysFiles = esf;
}
//---------------------------------------------------------------------------
int __fastcall TDiskMonitor::GetMonitorCount()
{
  return Monitors->Count;
}
//---------------------------------------------------------------------------
void __fastcall TDiskMonitor::Open()
{
  if (!Active) {
    TMonitorWaitThread *Thread;

    for (int i = 0; i < Monitors->Count; i++) {
      Thread = (TMonitorWaitThread *)Monitors->Objects[i];
      if (Thread != NULL)
        if (Thread->Suspended)
          Thread->Resume();
    }

    SetActive(true);
  }
}
//---------------------------------------------------------------------------
void __fastcall TDiskMonitor::Close()
{
  if (Active) {
    TMonitorWaitThread *Thread;

    for (int i = 0; i < Monitors->Count; i++) {
      Thread = (TMonitorWaitThread *)Monitors->Objects[i];
      if (Thread != NULL) {
        if (!Thread->Terminated) {
          if (Thread->Suspended)
            Thread->Resume();

          Thread->Terminate();
        }
      }
    }

    SetActive(false);
    CleanupMonitors();
  }
}
//---------------------------------------------------------------------------
void __fastcall TDiskMonitor::Suspend()
{
  if (!Suspended) {
    TMonitorWaitThread *Thread;

    for (int i = 0; i < Monitors->Count; i++) {
      Thread = (TMonitorWaitThread *)Monitors->Objects[i];
      if (Thread != NULL)
        if (!Thread->Suspended)
          Thread->Suspend();
    }

    Suspended = true;
  }
}
//---------------------------------------------------------------------------
void __fastcall TDiskMonitor::Resume()
{
  if (Suspended) {
    TMonitorWaitThread *Thread;

    for (int i = 0; i < Monitors->Count; i++) {
      Thread = (TMonitorWaitThread *)Monitors->Objects[i];
      if (Thread != NULL)
        if (Thread->Suspended)
          Thread->Resume();
    }

    Suspended = false;
  }
}
//---------------------------------------------------------------------------
void __fastcall TDiskMonitor::ResetCounters()
{
  FReceivedEvents = 0;
  FHandledEvents = 0;
}
//---------------------------------------------------------------------------
void __fastcall TDiskMonitor::AddMonitorPath(TFileName Pathname, bool WatchSubTree)
{
  TMonitorWaitThread *Thread;
  AnsiString *fdir = new AnsiString();
  AnsiString *fname = new AnsiString();
  bool IsDir;


  *fdir = Pathname;
  if (fdir->SubString(fdir->Length(), 1) == "\\")
    fdir->Delete(fdir->Length(), 1);


//  IsDir = DirectoryExists(*fdir);
  IsDir = DirExists(*fdir);
  if (!IsDir)
    *fdir = ExtractFilePath(*fdir);


  *fdir = fdir->LowerCase();
  if (fdir->SubString(fdir->Length(), 1) == "\\")
    fdir->Delete(fdir->Length(), 1);


  int j = Monitors->IndexOf(*fdir);
  if (j > -1) {
    Thread = (TMonitorWaitThread *)Monitors->Objects[j];
    if (!Thread->Suspended)
      Thread->Suspend();
  } else
    Thread = new TMonitorWaitThread(*fdir, WatchSubTree, false, &EventMultiplexer);


  if (!IsDir) {
    *fname = ExtractFileName(Pathname);
    *fname = fname->LowerCase();
    Thread->AddFilename(fname);
  }


  if (j == -1)
    Monitors->AddObject(*fdir, Thread);

  if (Thread != NULL)
    if ((FActive) && (!Suspended))
      if (Thread->Suspended)
        Thread->Resume();


  delete fname;
  delete fdir;
}
//---------------------------------------------------------------------------
#include "DiskMonitorWaitThread.cpp"
//---------------------------------------------------------------------------

