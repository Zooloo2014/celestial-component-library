//---------------------------------------------------------------------------
#ifndef DiskMonitorFileInfoH
#define DiskMonitorFileInfoH
#include <stdlib.h>
//---------------------------------------------------------------------------
class TMonitorFileInfo : public TObject
{
private:
        AnsiString *Pathname;
        AnsiString *Filename;

        int Attributes;
        int FileSize;
        TDateTime LastWrite;

        bool FileExists;

protected:
        bool __fastcall DirExists(AnsiString dir);
        void __fastcall LoadInfo();

public:
        __fastcall TMonitorFileInfo(AnsiString Path, AnsiString Name);
        __fastcall TMonitorFileInfo(AnsiString FullPathname);
        __fastcall ~TMonitorFileInfo();
        void __fastcall Construct();

        bool __fastcall Exists();
        AnsiString __fastcall GetPathname();
        AnsiString __fastcall GetFilename();
        void __fastcall SetPathname(AnsiString Path);

        void __fastcall ReloadInfo();
        int __fastcall GetAttributes();
        long __fastcall GetFileSize();
        TDateTime __fastcall GetLastWrite();
        bool __fastcall IsReadOnly();
        bool __fastcall IsHidden();
        bool __fastcall IsSystem();
        bool __fastcall IsVolumeID();
        bool __fastcall IsDirectory();
        bool __fastcall IsArchive();
};
//---------------------------------------------------------------------------
#endif
