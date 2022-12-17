//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "DiskMonitorFileInfo.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
__fastcall TMonitorFileInfo::TMonitorFileInfo(AnsiString Path, AnsiString Name)
        : TObject()
{
  Pathname = new AnsiString();
  Filename = new AnsiString();
  SetPathname(Path);
  *Filename = Name;

  Construct();
}
//---------------------------------------------------------------------------
__fastcall TMonitorFileInfo::TMonitorFileInfo(AnsiString FullPathname)
{
  Pathname = new AnsiString();
  Filename = new AnsiString();
  SetPathname(FullPathname);

  Construct();
}
//---------------------------------------------------------------------------
void __fastcall TMonitorFileInfo::Construct()
{
  Attributes = 0;
  FileSize = -1;
  LastWrite = 0;
  FileExists = false;

  LoadInfo();
}
//---------------------------------------------------------------------------
__fastcall TMonitorFileInfo::~TMonitorFileInfo()
{
  delete Pathname;
  delete Filename;
}
//---------------------------------------------------------------------------
void __fastcall TMonitorFileInfo::LoadInfo()
{
  TSearchRec sr;

  if (!Filename->IsEmpty()) {
    if (FindFirst(*Pathname + "\\" + *Filename, faAnyFile, sr) == 0) {
      FileExists = true;
      Attributes = sr.Attr;
      FileSize = sr.Size;
      LastWrite = LastWrite.FileDateToDateTime(sr.Time);
    } else
      FileExists = false;

    FindClose(sr);
  }
}
//---------------------------------------------------------------------------
void __fastcall TMonitorFileInfo::ReloadInfo()
{
  LoadInfo();
}
//---------------------------------------------------------------------------
bool __fastcall TMonitorFileInfo::Exists()
{
  ReloadInfo();
  return FileExists;
}
//---------------------------------------------------------------------------
void __fastcall TMonitorFileInfo::SetPathname(AnsiString Path)
{
  if (Path.SubString(Path.Length(), 1) == "\\")
    *Pathname = Path.SubString(1, Path.Length() - 1);
  else
    *Pathname = Path;

  if (!DirExists(*Pathname)) {
    *Filename = ExtractFileName(*Pathname);
    *Pathname = ExtractFilePath(*Pathname);
  } else
    *Filename = "";
}
//---------------------------------------------------------------------------
AnsiString __fastcall TMonitorFileInfo::GetPathname()
{
  return *Pathname;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TMonitorFileInfo::GetFilename()
{
  return *Filename;
}
//---------------------------------------------------------------------------
int __fastcall TMonitorFileInfo::GetAttributes()
{
  return Attributes;
}
//---------------------------------------------------------------------------
long __fastcall TMonitorFileInfo::GetFileSize()
{
  return FileSize;
}
//---------------------------------------------------------------------------
TDateTime __fastcall TMonitorFileInfo::GetLastWrite()
{
  return LastWrite;
}
//---------------------------------------------------------------------------
bool __fastcall TMonitorFileInfo::IsReadOnly()
{
  return ((Attributes & faReadOnly) == faReadOnly);
}
//---------------------------------------------------------------------------
bool __fastcall TMonitorFileInfo::IsHidden()
{
  return ((Attributes & faHidden) == faHidden);
}
//---------------------------------------------------------------------------
bool __fastcall TMonitorFileInfo::IsSystem()
{
  return ((Attributes & faSysFile) == faSysFile);
}
//---------------------------------------------------------------------------
bool __fastcall TMonitorFileInfo::IsVolumeID()
{
  return ((Attributes & faVolumeID) == faVolumeID);
}
//---------------------------------------------------------------------------
bool __fastcall TMonitorFileInfo::IsDirectory()
{
  return ((Attributes & faDirectory) == faDirectory);
}
//---------------------------------------------------------------------------
bool __fastcall TMonitorFileInfo::IsArchive()
{
  return ((Attributes & faArchive) == faArchive);
}
//---------------------------------------------------------------------------
bool __fastcall TMonitorFileInfo::DirExists(AnsiString dir)
{
  bool rc;
  AnsiString *d = new AnsiString();

  *d = GetCurrentDir();
  rc = SetCurrentDir(dir);
  SetCurrentDir(*d);

  return rc;
}
//---------------------------------------------------------------------------

