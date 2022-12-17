//---------------------------------------------------------------------------
#ifndef StatusAreaIconH
#define StatusAreaIconH
//---------------------------------------------------------------------------
#include <stdlib.h>
#include <SysUtils.hpp>
#include <Controls.hpp>
#include <Classes.hpp>
#include <Forms.hpp>
#include <ShellAPI.hpp>
//---------------------------------------------------------------------------
#define WM_SYSTEM_TRAY_NOTIFY (WM_USER + 255)
//---------------------------------------------------------------------------
#define NIN_SELECT           (WM_USER + 0)
#define NINF_KEY             0x1
#define NIN_KEYSELECT        (NIN_SELECT | NINF_KEY)

#define NIN_BALLOONSHOW      (WM_USER + 2)
#define NIN_BALLOONHIDE      (WM_USER + 3)
#define NIN_BALLOONTIMEOUT   (WM_USER + 4)
#define NIN_BALLOONUSERCLICK (WM_USER + 5)

#define NIM_SETFOCUS    0x00000003
#define NIM_SETVERSION  0x00000004

#define NIF_MESSAGE     0x00000001
#define NIF_ICON        0x00000002
#define NIF_TIP         0x00000004
#define NIF_STATE       0x00000008
#define NIF_INFO        0x00000010
#define NIF_GUID        0x00000020

#define NIS_HIDDEN      0x00000001
#define NIS_SHAREDICON  0x00000002
#define NIIF_NONE       0x00000000
#define NIIF_INFO       0x00000001
#define NIIF_WARNING    0x00000002
#define NIIF_ERROR      0x00000003
#define NIIF_ICON_MASK  0x0000000F
#define NIIF_NOSOUND    0x00000010

#define NOTIFYICON_VERSION 3
//---------------------------------------------------------------------------
typedef struct _MYNOTIFYICONDATA {
    DWORD cbSize;
    HWND hWnd;
    UINT uID;
    UINT uFlags;
    UINT uCallbackMessage;
    HICON hIcon;
    TCHAR szTip[64];
    DWORD dwState;
    DWORD dwStateMask;
    TCHAR szInfo[256];
    union {
        UINT uTimeout;
        UINT uVersion;
    };
    TCHAR szInfoTitle[64];
    DWORD dwInfoFlags;
    GUID guidItem;
} MYNOTIFYICONDATA, *PMYNOTIFYICONDATA;
//---------------------------------------------------------------------------
typedef struct _DllVersionInfo {
    DWORD cbSize;
    DWORD dwMajorVersion;
    DWORD dwMinorVersion;
    DWORD dwBuildNumber;
    DWORD dwPlatformID;
} DLLVERSIONINFO, *PDLLVERSIONINFO;
//---------------------------------------------------------------------------
typedef int __stdcall (*TWinCall)(PDLLVERSIONINFO ptr);
//---------------------------------------------------------------------------
enum TTooltipType {Standard, Balloon};
enum TBalloonType {None, Info, Warning, Error};
//---------------------------------------------------------------------------
class PACKAGE TStatusAreaIcon : public TComponent
{
private:
        TPopupMenu *FPopupMenu;
        TImageList *FIconList;
        TTooltipType FTooltipType;
        AnsiString *FText;
        AnsiString *FTitleText;
        TBalloonType FBalloonType;
        bool FVisible;
        int FIconID;
        int FShellVersion;
        int FIconIndex;

        TIcon *CurrIcon;
        PMYNOTIFYICONDATA IconData;
        HINSTANCE Shell32Lib;

        void __fastcall SetTooltipText(AnsiString ss);
        void __fastcall SetIcon();
        void __fastcall SetVersion();
        void __fastcall ShowMenu();

protected:
        virtual void __fastcall SetTooltipType(TTooltipType t);
        virtual void __fastcall SetIconIndex(int i);
        virtual AnsiString __fastcall GetText();
        virtual void __fastcall SetText(AnsiString t);
        virtual AnsiString __fastcall GetTitleText();
        virtual void __fastcall SetTitleText(AnsiString t);
        virtual void __fastcall SetPopupMenu(TPopupMenu *m);

public:
        __fastcall TStatusAreaIcon(TComponent* Owner);
        __fastcall ~TStatusAreaIcon();

        void __fastcall Show();
        void __fastcall Update();
        void __fastcall Hide();
        void __fastcall MessageHandler(tagMSG &Message, bool &Handled);

__published:
        __property TImageList *IconList = {read=FIconList, write=FIconList, nodefault};
        __property TPopupMenu *PopupMenu = {read=FPopupMenu, write=SetPopupMenu, nodefault};
        __property int IconIndex = {read=FIconIndex, write=SetIconIndex, default=0};
        __property int ShellVersion = {read=FShellVersion, default=0};
        __property int IconID = {read=FIconID, write=FIconID, default=1};
        __property TTooltipType TooltipType = {read=FTooltipType, write=SetTooltipType, default=Standard};
        __property AnsiString Text = {read=GetText, write=SetText, nodefault};
        __property AnsiString TitleText = {read=GetTitleText, write=SetTitleText, nodefault};
        __property TBalloonType BalloonType = {read=FBalloonType, write=FBalloonType, default=None};
        __property bool Visible = {read=FVisible, default=false};
};
//---------------------------------------------------------------------------
#endif
