//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "StatusAreaIcon.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//

static inline void ValidCtrCheck(TStatusAreaIcon *)
{
        new TStatusAreaIcon(NULL);
}
//---------------------------------------------------------------------------
__fastcall TStatusAreaIcon::TStatusAreaIcon(TComponent* Owner)
        : TComponent(Owner)
{
  FShellVersion = 0;
  FVisible = false;

  PDLLVERSIONINFO Vers = (PDLLVERSIONINFO)malloc(sizeof(DLLVERSIONINFO));
  if (Vers != NULL) {
    Vers->cbSize = sizeof(DLLVERSIONINFO);

    INITCOMMONCONTROLSEX a;
    a.dwSize = sizeof(INITCOMMONCONTROLSEX);
    a.dwICC = ICC_TAB_CLASSES;
    InitCommonControlsEx(&a);

    Shell32Lib = LoadLibrary("shell32.dll");

    if (Shell32Lib != NULL) {
      TWinCall GetVers = (TWinCall)GetProcAddress(Shell32Lib, "DllGetVersion");
      if (GetVers != NULL)
        GetVers(Vers);
    }

    FShellVersion = Vers->dwMajorVersion;

    free(Vers);
  }

  FIconID = 1;
  FIconIndex = 0;

  CurrIcon = new TIcon();

  FText = new AnsiString();
  FTitleText = new AnsiString();

  IconData = (PMYNOTIFYICONDATA)malloc(sizeof(MYNOTIFYICONDATA));
  if (IconData != NULL)
    IconData->cbSize = sizeof(MYNOTIFYICONDATA);

  SetTooltipType(Standard);
}
//---------------------------------------------------------------------------
__fastcall TStatusAreaIcon::~TStatusAreaIcon() {

  if (Shell32Lib != NULL)
    FreeLibrary(Shell32Lib);

  if (IconData != NULL)
    free(IconData);

  delete FTitleText;
  delete FText;

  delete CurrIcon;
}
//---------------------------------------------------------------------------
namespace Statusareaicon
{
        void __fastcall PACKAGE Register()
        {
                 TComponentClass classes[1] = {__classid(TStatusAreaIcon)};
                 RegisterComponents("Celestial", classes, 0);
        }
}
//---------------------------------------------------------------------------
void __fastcall TStatusAreaIcon::MessageHandler(tagMSG &Message, bool &Handled) {
   switch(Message.message)
   {
      case WM_SYSTEM_TRAY_NOTIFY:
         switch(Message.lParam)
         {
            case WM_LBUTTONDOWN:
              break;

            case WM_LBUTTONUP:
              ShowMenu();
              Handled = true;
              break;

            case WM_LBUTTONDBLCLK:
              break;

            case WM_RBUTTONDOWN:
              break;

            case WM_RBUTTONUP:
            case NIN_KEYSELECT:
            case WM_CONTEXTMENU:
              ShowMenu();
              Handled = true;
              break;

            case WM_RBUTTONDBLCLK:
              break;

            case NIN_BALLOONSHOW:
              break;

            case NIN_BALLOONHIDE:
              break;

            case NIN_BALLOONTIMEOUT:
              break;

            case NIN_BALLOONUSERCLICK:
              break;
         }
   }

   TComponent::Dispatch(&Message);
}
//---------------------------------------------------------------------------
void __fastcall TStatusAreaIcon::SetTooltipType(TTooltipType t) {

  if (FShellVersion < 6)
    FTooltipType = Standard;
  else
    FTooltipType = t;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TStatusAreaIcon::GetText() {
  return *FText;
}
//---------------------------------------------------------------------------
void __fastcall TStatusAreaIcon::SetText(AnsiString t) {
  *FText = t;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TStatusAreaIcon::GetTitleText() {
  return *FTitleText;
}
//---------------------------------------------------------------------------
void __fastcall TStatusAreaIcon::SetTitleText(AnsiString t) {
  *FTitleText = t;
}
//---------------------------------------------------------------------------
void __fastcall TStatusAreaIcon::SetIconIndex(int i) {
  if (FIconList != NULL)
    if ((i >= 0) && (i < FIconList->Count))
      FIconIndex = i;
}
//---------------------------------------------------------------------------
void __fastcall TStatusAreaIcon::SetPopupMenu(TPopupMenu *m) {
  FPopupMenu = m;
  FPopupMenu->Alignment = paRight;
}
//---------------------------------------------------------------------------
void __fastcall TStatusAreaIcon::SetTooltipText(AnsiString ss) {
  AnsiString *s = new AnsiString();
  AnsiString *ts = new AnsiString();

  *s = ss;

  if (FTooltipType == Standard) {
    if (FShellVersion >= 5)
      *s = FText->SubString(1, 127);
    else
      *s = FText->SubString(1, 63);

    strcpy(IconData->szTip, s->c_str());
    strcpy(IconData->szInfo, "");
    IconData->uFlags &= (!NIF_INFO);
    IconData->uFlags |= NIF_TIP;
  }
  else {
    *s = FText->SubString(1, 254);
    *ts = FTitleText->SubString(1, 63);

    strcpy(IconData->szTip, "");
    strcpy(IconData->szInfo, s->c_str());
    strcpy(IconData->szInfoTitle, ts->c_str());

    IconData->uTimeout = (5 * 1000); // Millisecs - max=30

    switch (FBalloonType) {
      case Info:
        IconData->dwInfoFlags = NIIF_INFO;
        break;
      case Warning:
        IconData->dwInfoFlags = NIIF_WARNING;
        break;
      case Error:
        IconData->dwInfoFlags = NIIF_ERROR;
        break;
      case None:
      default:
        IconData->dwInfoFlags = NIIF_NONE;
    }

    IconData->uFlags &= (!NIF_TIP);
    IconData->uFlags |= NIF_INFO;
  }

  delete ts;
  delete s;
}
//---------------------------------------------------------------------------
void __fastcall TStatusAreaIcon::SetIcon() {
  if (FIconList != NULL) {
    if (FIconList->Count > 0) {
      IconData->uID = FIconID;
      FIconList->GetIcon(FIconIndex, CurrIcon);
      IconData->hIcon = CurrIcon->Handle;
      IconData->uFlags |= NIF_ICON;
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TStatusAreaIcon::Show() {

  IconData->uFlags = 0;

  SetTooltipText("");
  SetIcon();

  IconData->hWnd = Application->Handle;
  IconData->uCallbackMessage = WM_SYSTEM_TRAY_NOTIFY;
  IconData->uFlags |= NIF_MESSAGE;

  if (!FVisible) {
    FVisible = Shell_NotifyIcon(NIM_ADD, (PNOTIFYICONDATA)IconData);

    if ((FVisible) && (FShellVersion >= 6))
      SetVersion();
  }
}
//---------------------------------------------------------------------------
void __fastcall TStatusAreaIcon::Hide() {

  IconData->hWnd = Application->Handle;
  IconData->uID = FIconID;

  if (FVisible)
    FVisible = !Shell_NotifyIcon(NIM_DELETE, (PNOTIFYICONDATA)IconData);
}
//---------------------------------------------------------------------------
void __fastcall TStatusAreaIcon::SetVersion() {
  IconData->hWnd = Application->Handle;
  IconData->uVersion = NOTIFYICON_VERSION;

  Shell_NotifyIcon(NIM_SETVERSION, (PNOTIFYICONDATA)IconData);
}
//---------------------------------------------------------------------------
void __fastcall TStatusAreaIcon::Update() {

  if (FVisible) {
    IconData->uFlags = 0;

    SetTooltipText(*FText);
    SetIcon();

    IconData->hWnd = Application->Handle;

    Shell_NotifyIcon(NIM_MODIFY, (PNOTIFYICONDATA)IconData);
  }
  else
    Show();
}
//---------------------------------------------------------------------------
void __fastcall TStatusAreaIcon::ShowMenu() {
  if (FVisible) {
    TPoint *Point = new TPoint();

    if (FPopupMenu != NULL) {
      SetForegroundWindow(Application->Handle);
      if (GetCursorPos((tagPOINT *)Point))
        FPopupMenu->Popup(Point->x, Point->y);
      else
        throw Exception("Menu pop-up operation failed");
    }

    delete Point;
  }
}
//---------------------------------------------------------------------------

