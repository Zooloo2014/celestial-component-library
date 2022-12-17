//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "DiskMonitorWaitThread.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
//   Important: Methods and properties of objects in VCL can only be
//   used in a method called using Synchronize, for example:
//
//      Synchronize(UpdateCaption);
//
//   where UpdateCaption could look like:
//
//      void __fastcall TMonitorWaitThread::UpdateCaption()
//      {
//        Form1->Caption = "Updated in a thread";
//      }
//---------------------------------------------------------------------------
__fastcall TMonitorWaitThread::TMonitorWaitThread(AnsiString Dir, bool SubTree, bool SpecificFiles, TMonitorCallback multip)
        : TThread(true)
{
  FreeOnTerminate = false;
  WatchSubtree = SubTree;
  AllFiles = !SpecificFiles;

  FMultiplexer = multip;
  MyHandle = NULL;
  EventInfo = NULL;
  EventHistory = NULL;
  FEvent = MCI_UNKNOWN;

  SetTimeout(15);    // 15 seconds default...

  FMonitorPath = new AnsiString();
  SetMonitorPath(Dir.LowerCase());

  FileList = new TStringList();
  FileList->Sorted = true;
  AffectedFile = new AnsiString();
  AuxFile = new AnsiString();

  if (!Suspended)
    Suspend();

  SetupMonitoring();
}
//---------------------------------------------------------------------------
__fastcall TMonitorWaitThread::~TMonitorWaitThread()
{
  ShutdownMonitoring();
  ClearFileInfo();

  delete FMonitorPath;
  delete FileList;
  delete AffectedFile;
  delete AuxFile;
}
//---------------------------------------------------------------------------
long __fastcall TMonitorWaitThread::GetTimeout()
{
  return (FTimeout / 1000);
}
//---------------------------------------------------------------------------
void __fastcall TMonitorWaitThread::SetTimeout(long sec)
{
  if (sec < MIN_TIMEOUT)
    FTimeout = (MIN_TIMEOUT * 1000);
  else
    FTimeout = (sec * 1000);
}
//---------------------------------------------------------------------------
AnsiString __fastcall TMonitorWaitThread::GetMonitorPath()
{
  return *FMonitorPath;
}
//---------------------------------------------------------------------------
void __fastcall TMonitorWaitThread::SetMonitorPath(AnsiString p)
{
  *FMonitorPath = p;

  if (FMonitorPath->SubString(FMonitorPath->Length(), 1) == "\\")
    FMonitorPath->Delete(FMonitorPath->Length(), 1);
}
//---------------------------------------------------------------------------
void __fastcall TMonitorWaitThread::AddFilename(AnsiString *fname)
{
//  if (!DirectoryExists(*fname)) {
  if (!DirExists(*fname)) {
    AnsiString *tn = new AnsiString();
    AnsiString *tf = new AnsiString();

    *tn = ExtractFilePath(fname->LowerCase());
    *tf = ExtractFileName(fname->LowerCase());

    if (tn->SubString(tn->Length(), 1) == "\\")
      tn->Delete(tn->Length(), 1);

    if (tn->AnsiCompareIC(*FMonitorPath) == 0) {
      if (!tf->IsEmpty()) {   // *fname is a directory...
        int i = FileList->Add(*tf);
        LoadFileInfo(i, *tf);
      }
    }

    delete tn;
    delete tf;
  }
}
//---------------------------------------------------------------------------
void __fastcall TMonitorWaitThread::SetupMonitoring()
{
  FilterCond = 0;
  BytesBack = 0;
  NotBufSize = (sizeof(FILE_NOTIFY_INFORMATION) + 4096) * 8;

  MyHandle = FindFirstChangeNotification(FMonitorPath->c_str(), WatchSubtree,
          FILE_NOTIFY_CHANGE_FILE_NAME |
          FILE_NOTIFY_CHANGE_ATTRIBUTES |
          FILE_NOTIFY_CHANGE_SIZE |
          FILE_NOTIFY_CHANGE_LAST_WRITE);

  if (MyHandle == INVALID_HANDLE_VALUE)
    MyHandle = NULL;

  FilterCond |= FILE_NOTIFY_CHANGE_FILE_NAME;
  FilterCond |= FILE_NOTIFY_CHANGE_DIR_NAME;
  FilterCond |= FILE_NOTIFY_CHANGE_ATTRIBUTES;
  FilterCond |= FILE_NOTIFY_CHANGE_SIZE;
  FilterCond |= FILE_NOTIFY_CHANGE_LAST_WRITE;
  FilterCond |= FILE_NOTIFY_CHANGE_LAST_ACCESS;
  FilterCond |= FILE_NOTIFY_CHANGE_CREATION;
  FilterCond |= FILE_NOTIFY_CHANGE_SECURITY;

  DirHandle = CreateFile(FMonitorPath->c_str(),
    FILE_LIST_DIRECTORY,
    FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
    NULL,
    OPEN_EXISTING,
    FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
    NULL);

  if (DirHandle == INVALID_HANDLE_VALUE)
    DirHandle = NULL;

  Overlap = (OVERLAPPED *)malloc(sizeof(OVERLAPPED));
  memset(Overlap, 0x00, sizeof(OVERLAPPED));

  NotBuf = (FILE_NOTIFY_INFORMATION *)malloc(NotBufSize);
  memset(NotBuf, 0x00, NotBufSize);

  if (AllFiles)
    LoadAllFiles();
}
//---------------------------------------------------------------------------
void __fastcall TMonitorWaitThread::ShutdownMonitoring()
{
  if (MyHandle != NULL) {
    FindCloseChangeNotification(MyHandle);
    MyHandle = NULL;
  }

  if (DirHandle != NULL) {
    CloseHandle(DirHandle);
    DirHandle = NULL;
  }

  if (Overlap != NULL)
    free(Overlap);
  if (NotBuf != NULL)
    free(NotBuf);
}
//---------------------------------------------------------------------------
bool __fastcall TMonitorWaitThread::IsSpecificFile(AnsiString fname)
{
  bool rc = false;

  for (int i = 0; i < FileList->Count; i++) {
    rc = (FileList->Strings[i].AnsiCompareIC(fname) == 0);
    if (rc)
      break;
  }

  return rc;
}
//---------------------------------------------------------------------------
int __fastcall TMonitorWaitThread::GetSpecificFileIndex(AnsiString fname)
{
  int rc = -1;

  for (int i = 0; i < FileList->Count; i++) {
    if (FileList->Strings[i].AnsiCompareIC(fname) == 0) {
      rc = i;
      break;
    }
  }

  return rc;
}
//---------------------------------------------------------------------------
bool __fastcall TMonitorWaitThread::DirExists(AnsiString dir)
{
  bool rc;
  AnsiString *d = new AnsiString();

  *d = GetCurrentDir();
  rc = SetCurrentDir(dir);
  SetCurrentDir(*d);

  return rc;
}
//---------------------------------------------------------------------------
void __fastcall TMonitorWaitThread::Execute()
{
  TMonitorChangedItem Change = MCI_UNKNOWN;
  int rc;

  FILE_NOTIFY_INFORMATION *ptr;
  DWORD Ofs;
  int BufSize = 8192;
  char wc_buf[8192];

  if ((MyHandle == NULL) || (DirHandle == NULL))
    Terminate();

  *AffectedFile = "";
  *AuxFile = "";

  while (!Terminated) {
    rc = WaitForSingleObject(MyHandle, FTimeout);

    if (!Terminated) {
      switch (rc) {
        case WAIT_OBJECT_0:
          if (ReadDirectoryChangesW(DirHandle, NotBuf, NotBufSize, WatchSubtree, FilterCond, &BytesBack, Overlap, NULL)) {
            ptr = NotBuf;
            Ofs = ptr->NextEntryOffset;
            do {
              memset(wc_buf, 0x00, BufSize);
              memcpy(wc_buf, ptr->FileName, ptr->FileNameLength);

              if (Change != MCI_PRIVATE) {
                *AffectedFile = "";
                *AuxFile = "";
              }

              switch (ptr->Action) {
                case FILE_ACTION_ADDED:
                  Change = MCI_CREATE;
                  *AffectedFile = WideCharToString((wchar_t *)wc_buf);
                  break;
                case FILE_ACTION_REMOVED:
                  Change = MCI_DELETE;
                  *AffectedFile = WideCharToString((wchar_t *)wc_buf);
                  break;
                case FILE_ACTION_MODIFIED:
                  Change = MCI_MODIFY;
                  *AffectedFile = WideCharToString((wchar_t *)wc_buf);
                  break;
                case FILE_ACTION_RENAMED_OLD_NAME: // Renamed to...
                  Change = MCI_PRIVATE;
                  *AuxFile = WideCharToString((wchar_t *)wc_buf);
                  break;
                case FILE_ACTION_RENAMED_NEW_NAME: // Renamed from...
                  Change = MCI_RENAME;
                  *AffectedFile = WideCharToString((wchar_t *)wc_buf);
                  break;
                default:
                  Change = MCI_UNKNOWN;
              }


              if (Change != MCI_UNKNOWN)
                if (Change != MCI_PRIVATE)
                  if (FMultiplexer != NULL) {
                    int j = GetSpecificFileIndex(*AffectedFile);
                    if (j > -1)
                      EventHistory = (TMonitorFileInfo *)FileList->Objects[j];
                    else
                      EventHistory = NULL;


                    EventInfo = new TMonitorFileInfo(*FMonitorPath, *AffectedFile);

                    if (!AllFiles) {
                      if (EventHistory != NULL) {
//                        FMultiplexer(*MonDirectory, *AffectedFile, *AuxFile, c, p, Change);
                        FEvent = Change;
                        Synchronize((TThreadMethod)FMultiplexer);
                        FEvent = MCI_UNKNOWN;
                      }
                    } else {
//                      FMultiplexer(*MonDirectory, *AffectedFile, *AuxFile, c, p, Change);
                      FEvent = Change;
                      Synchronize((TThreadMethod)FMultiplexer);
                      FEvent = MCI_UNKNOWN;
                    }

                    delete EventInfo;


                    if (EventHistory != NULL)   // Load latest info for file...
                      EventHistory->ReloadInfo();
                  }


              if (Ofs > 0) {
                ptr = (FILE_NOTIFY_INFORMATION *)((DWORD)ptr + Ofs);
                Ofs = ptr->NextEntryOffset;
                continue;
              }
            } while (Ofs > 0);

            FindNextChangeNotification(MyHandle);
          } else
            Terminate();
          break;

        case WAIT_ABANDONED:
          Terminate();
          break;

        case WAIT_TIMEOUT:
          FindNextChangeNotification(MyHandle);
          break;

        default:
          Terminate();
      }
    }
  }

  ShutdownMonitoring();
}
//---------------------------------------------------------------------------
int __fastcall TMonitorWaitThread::GetFileCount()
{
  return FileList->Count;
}
//---------------------------------------------------------------------------
void __fastcall TMonitorWaitThread::ClearFileInfo()
{
  TMonitorFileInfo *p;

  for (int i = 0; i < FileList->Count; i++) {
    p = (TMonitorFileInfo *)FileList->Objects[i];
    delete p;
  }

  FileList->Clear();
}
//---------------------------------------------------------------------------
void __fastcall TMonitorWaitThread::LoadFileInfo(int i, AnsiString fn)
{
  TMonitorFileInfo *p;

  p = (TMonitorFileInfo *)FileList->Objects[i];
  if (p != NULL)
    delete p;

  FileList->Objects[i] = new TMonitorFileInfo(*FMonitorPath, fn);
}
//---------------------------------------------------------------------------
void __fastcall TMonitorWaitThread::LoadAllFiles()
{
  TSearchRec sr;
  int i;

  if (FileCount > 0)
    ClearFileInfo();

  if (FindFirst(*FMonitorPath + "\\*.*", faAnyFile, sr) == 0) {
    i = FileList->Add(sr.Name);
    LoadFileInfo(i, sr.Name);

    while (FindNext(sr) == 0) {
      i = FileList->Add(sr.Name);
      LoadFileInfo(i, sr.Name);
    }
  }

  FindClose(sr);
}
//---------------------------------------------------------------------------
TMonitorFileInfo * __fastcall TMonitorWaitThread::GetEventFileInfo()
{
  return EventInfo;
}
//---------------------------------------------------------------------------
TMonitorFileInfo * __fastcall TMonitorWaitThread::GetEventFileHistory()
{
  return EventHistory;
}
//---------------------------------------------------------------------------
TMonitorChangedItem __fastcall TMonitorWaitThread::GetEvent()
{
  return FEvent;
}
//---------------------------------------------------------------------------
#include "DiskMonitorFileInfo.cpp"
//---------------------------------------------------------------------------

