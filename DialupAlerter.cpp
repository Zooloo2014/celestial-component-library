//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "DialupAlerter.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//

static inline void ValidCtrCheck(TDialupAlerter *)
{
        new TDialupAlerter(NULL);
}
//---------------------------------------------------------------------------
__fastcall TDialupAlerter::TDialupAlerter(TComponent* Owner)
        : TComponent(Owner)
{
  FOnNoLineAvailable = NULL;
  FOnEventNotification = NULL;

  FActive = false;
  FConnected = false;
  FLineDeviceID = -1;

  APIVersion = TAPI_CURRENT_VERSION;
  LinkCount = 0;

  LastErrorMsg = new AnsiString();

  MyTAPIHandle = (LPHLINEAPP)malloc(sizeof(HLINEAPP));
  LineMsg = (LPLINEMESSAGE)malloc(sizeof(LINEMESSAGE));

  InitParms = (LPLINEINITIALIZEEXPARAMS)malloc(sizeof(LINEINITIALIZEEXPARAMS));
  InitParms->dwTotalSize = sizeof(LINEINITIALIZEEXPARAMS);
  InitParms->dwOptions = LINEINITIALIZEEXOPTION_USEEVENT;

  CallInfo = (LPLINECALLINFO)malloc(sizeof(LINECALLINFO));
  CallInfo->dwTotalSize = sizeof(LINECALLINFO);
}
//---------------------------------------------------------------------------
__fastcall TDialupAlerter::~TDialupAlerter()
{
  Shutdown();

  if (CallInfo != NULL)
    free(CallInfo);

  if (InitParms != NULL)
    free(InitParms);

  if (LineMsg != NULL)
    free(LineMsg);

  if (MyTAPIHandle != NULL)
    free(MyTAPIHandle);

  delete LastErrorMsg;
}
//---------------------------------------------------------------------------
namespace Dialupalerter
{
        void __fastcall PACKAGE Register()
        {
                 TComponentClass classes[1] = {__classid(TDialupAlerter)};
                 RegisterComponents("Celestial", classes, 0);
        }
}
//---------------------------------------------------------------------------
void __fastcall TDialupAlerter::GetErrorMessage(AnsiString Tag, int res) {
  *LastErrorMsg = Tag;
  if (res == 0)
    *LastErrorMsg = *LastErrorMsg + ": ";
  else
    *LastErrorMsg = *LastErrorMsg + ": TAPI ERROR: ";

  switch (res) {
    case 0:
      *LastErrorMsg = *LastErrorMsg + "SUCCESS";
      break;
    case LINEERR_INVALAPPNAME:
      *LastErrorMsg = *LastErrorMsg + "Invalid AppName";
      break;
    case LINEERR_OPERATIONFAILED:
      *LastErrorMsg = *LastErrorMsg + "Operation failed";
      break;
    case LINEERR_INIFILECORRUPT:
      *LastErrorMsg = *LastErrorMsg + "INI file corrupt";
      break;
    case LINEERR_INVALPOINTER:
      *LastErrorMsg = *LastErrorMsg + "Invalid pointer";
      break;
    case LINEERR_REINIT:
      *LastErrorMsg = *LastErrorMsg + "Re-initiallized";
      break;
    case LINEERR_NOMEM:
      *LastErrorMsg = *LastErrorMsg + "Out of memory";
      break;
    case LINEERR_INVALPARAM:
      *LastErrorMsg = *LastErrorMsg + "Invalid parameter";
      break;
    case LINEERR_ALLOCATED:
      *LastErrorMsg = *LastErrorMsg + "Allocated";
      break;
    case LINEERR_LINEMAPPERFAILED:
      *LastErrorMsg = *LastErrorMsg + "LineMapper failed";
      break;
    case LINEERR_BADDEVICEID:
      *LastErrorMsg = *LastErrorMsg + "Bad device ID";
      break;
    case LINEERR_NODRIVER:
      *LastErrorMsg = *LastErrorMsg + "No driver";
      break;
    case LINEERR_INCOMPATIBLEAPIVERSION:
      *LastErrorMsg = *LastErrorMsg + "Incompatible API version";
      break;
    case LINEERR_INCOMPATIBLEEXTVERSION:
      *LastErrorMsg = *LastErrorMsg + "Incompatible extension version";
      break;
    case LINEERR_RESOURCEUNAVAIL:
      *LastErrorMsg = *LastErrorMsg + "Resource unavailable";
      break;
    case LINEERR_INVALMEDIAMODE:
      *LastErrorMsg = *LastErrorMsg + "Invalid media mode";
      break;
    case LINEERR_STRUCTURETOOSMALL:
      *LastErrorMsg = *LastErrorMsg + "Structure too small";
      break;
    case LINEERR_UNINITIALIZED:
      *LastErrorMsg = *LastErrorMsg + "Uninitialized";
      break;
    case LINEERR_INVALPRIVSELECT:
      *LastErrorMsg = *LastErrorMsg + "Invalid privilege selection";
      break;
    case LINEERR_NODEVICE:
      *LastErrorMsg = *LastErrorMsg + "No device";
      break;
    case LINEERR_OPERATIONUNAVAIL:
      *LastErrorMsg = *LastErrorMsg + "Operation unavailable";
      break;
    default:
      *LastErrorMsg = *LastErrorMsg + "Unknown error";
  }
}
//---------------------------------------------------------------------------
void __fastcall TDialupAlerter::Startup() {

  LastErrorCode = lineInitializeEx(MyTAPIHandle, NULL,
    NULL, Application->Title.c_str(), &NumDevices, &APIVersion, InitParms);
  GetErrorMessage("INIT", LastErrorCode);

  if (LastErrorCode == 0) {
    EventHandle = InitParms->Handles.hEvent;
    LinkCount++;

    if (FLineDeviceID != -1) {
      for (unsigned long i = 0; i < NumDevices; i++) {
        OpenLine(i);

        if (FLineDeviceID != -1) {
          SetStatusChangeRequests(LineHandle);
          if (LastErrorCode != 0)
            CloseLine(LineHandle);
          else
            break;
        }
      }
    }
  }
  else
    GetErrorMessage("INIT", LastErrorCode);

  FActive = (FLineDeviceID != -1);
}
//---------------------------------------------------------------------------
void __fastcall TDialupAlerter::Shutdown() {
  CloseLine(LineHandle);

  do {

    LastErrorCode = lineShutdown(*MyTAPIHandle);
    GetErrorMessage("SHUT", LastErrorCode);

    LinkCount--;

  } while (LinkCount > 0);
}
//---------------------------------------------------------------------------
void __fastcall TDialupAlerter::OpenLine(int Dev) {
  int id = -1;
  HLINE linehandle;

  LastErrorCode = lineOpen(*MyTAPIHandle, Dev, &linehandle,
    APIVersion, 0, NULL, LINECALLPRIVILEGE_MONITOR, 0, NULL);
  GetErrorMessage("OPEN" + IntToStr(Dev), LastErrorCode);

  if (LastErrorCode == 0) {
    id = Dev;
    LineHandle = linehandle;
  }

  FLineDeviceID = id;
}
//---------------------------------------------------------------------------
void __fastcall TDialupAlerter::CloseLine(HLINE handle) {
  if (FLineDeviceID != -1) {
    LastErrorCode = lineClose(handle);
    GetErrorMessage("CLOS", LastErrorCode);
    FLineDeviceID = -1;
  }
}
//---------------------------------------------------------------------------
void __fastcall TDialupAlerter::SetStatusChangeRequests(HLINE l) {
  LastErrorCode = lineSetStatusMessages(l,
    LINEDEVSTATE_CONNECTED &
    LINEDEVSTATE_DISCONNECTED &
    LINEDEVSTATE_INSERVICE &
    LINEDEVSTATE_OUTOFSERVICE &
    LINEDEVSTATE_OPEN &
    LINEDEVSTATE_CLOSE,
    0);

  GetErrorMessage("SETS", LastErrorCode);
}
//---------------------------------------------------------------------------
void __fastcall TDialupAlerter::CheckMessages() {
  AnsiString *tx = new AnsiString();

  LastErrorCode = lineGetMessage(*MyTAPIHandle, LineMsg, 0);
  if (LastErrorCode == 0) {
    switch (LineMsg->dwMessageID) {
      case LINEDEVSTATE_CONNECTED:
        *tx = "Connected";
        FConnected = true;
        break;
      case LINEDEVSTATE_DISCONNECTED:
        *tx = "Disconnected";
        FConnected = false;
        break;
      case LINEDEVSTATE_INSERVICE:
        *tx = "In-service";
        break;
      case LINEDEVSTATE_OUTOFSERVICE:
        *tx = "Out-of-service";
        break;
      case LINEDEVSTATE_OPEN:
        *tx = "Line open";
        break;
      case LINEDEVSTATE_CLOSE:
        *tx = "Line closed";
        break;
      default:
        *tx = "Unknown";
    }

    if (FOnEventNotification != NULL)
      FOnEventNotification(this, tx);
  }

  delete tx;
}
//---------------------------------------------------------------------------
void __fastcall TDialupAlerter::SetActive(bool act) {
  if (act) {
    Startup();

    if (FLineDeviceID == -1)
      if (FOnNoLineAvailable != NULL)
        FOnNoLineAvailable(this);
  }
  else
    Shutdown();
}
//---------------------------------------------------------------------------
void __fastcall TDialupAlerter::SetLineDeviceID(int lid) {
  if (FLineDeviceID != lid) {
    if (FLineDeviceID != -1)
      CloseLine(LineHandle);

    OpenLine(lid);
  }
}
//---------------------------------------------------------------------------

