//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "ServiceController.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//

static inline void ValidCtrCheck(TServiceController *)
{
        new TServiceController(NULL);
}
//---------------------------------------------------------------------------
__fastcall TServiceController::TServiceController(TComponent *Owner)
        : TComponent(Owner)
{
  Status = (LPSERVICE_STATUS)malloc(sizeof(SERVICE_STATUS));

  FServiceName = new AnsiString();
  FStatusString = new AnsiString();
  FStatus = None;

  ManagerOpen = false;
  ManHandle = NULL;
  SvcHandle = NULL;
}
//---------------------------------------------------------------------------
__fastcall TServiceController::~TServiceController()
{
  if (SvcHandle != NULL)
    CloseServiceHandle(SvcHandle);

  if (ManagerOpen)
    CloseManager();

  delete FStatusString;
  delete FServiceName;

  if (Status != NULL)
    free(Status);
}
//---------------------------------------------------------------------------
namespace Servicecontroller
{
        void __fastcall PACKAGE Register()
        {
                 TComponentClass classes[1] = {__classid(TServiceController)};
                 RegisterComponents("Celestial", classes, 0);
        }
}
//---------------------------------------------------------------------------
void __fastcall TServiceController::OpenManager(bool InstallAccess) {

  if (!InstallAccess)
    ManHandle = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT & GENERIC_READ & GENERIC_WRITE & GENERIC_EXECUTE);
  else
    ManHandle = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

  if (ManHandle != NULL) {
    ManagerOpen = true;
    CanInstall = InstallAccess;
  }
  else {
    DWORD res = GetLastError();
    ThrowException(res);
  }
}
//---------------------------------------------------------------------------
void __fastcall TServiceController::CloseManager() {

  if (CloseServiceHandle(ManHandle) == ERROR_INVALID_HANDLE)
    throw Exception("Invalid Service Control Manager handle");

  ManHandle = NULL;
  ManagerOpen = false;
}
//---------------------------------------------------------------------------
void __fastcall TServiceController::OpenServ(AnsiString ServName) {

  if (!ManagerOpen)
    OpenManager(false);

  if (SvcHandle != NULL)
    CloseServ();

  SvcHandle = OpenService(ManHandle, ServName.c_str(),
    SERVICE_ALL_ACCESS);

  if (SvcHandle == NULL) {
    DWORD res = GetLastError();
    ThrowException(res);
  }
  else
    *FServiceName = ServName;
}
//---------------------------------------------------------------------------
void __fastcall TServiceController::CloseServ() {

  if (CloseServiceHandle(SvcHandle) == ERROR_INVALID_HANDLE)
    throw Exception("Invalid Service Control handle");

  SvcHandle = NULL;
  *FServiceName = "";
}
//---------------------------------------------------------------------------
void __fastcall TServiceController::Start() {

  if (!StartService(SvcHandle, 0, NULL)) {
    DWORD res = GetLastError();
    ThrowException(res);
  }
  else {
    QueryStatus();
    FStatus = MapState(Status->dwCurrentState);
  }
}
//---------------------------------------------------------------------------
void __fastcall TServiceController::Stop() {

  if (!ControlService(SvcHandle, SERVICE_CONTROL_STOP, Status)) {
    DWORD res = GetLastError();
    ThrowException(res);
  }
  else
    FStatus = MapState(Status->dwCurrentState);

}
//---------------------------------------------------------------------------
void __fastcall TServiceController::Pause() {

  if (!ControlService(SvcHandle, SERVICE_CONTROL_PAUSE, Status)) {
    DWORD res = GetLastError();
    ThrowException(res);
  }
  else
    FStatus = MapState(Status->dwCurrentState);
}
//---------------------------------------------------------------------------
void __fastcall TServiceController::Continue() {

  if (!ControlService(SvcHandle, SERVICE_CONTROL_CONTINUE, Status)) {
    DWORD res = GetLastError();
    ThrowException(res);
  }
  else
    FStatus = MapState(Status->dwCurrentState);
}
//---------------------------------------------------------------------------
void __fastcall TServiceController::ThrowException(int i) {
  switch (i) {
    case 0:
      break;
    case ERROR_FILE_NOT_FOUND:
      throw Exception("File not found");
      break;
    case ERROR_ACCESS_DENIED:
      throw Exception("Access denied");
      break;
    case ERROR_INVALID_HANDLE:
      throw Exception("Invalid handle");
      break;
    case ERROR_INVALID_NAME:
      throw Exception("Invalid name");
      break;
    case ERROR_SERVICE_DOES_NOT_EXIST:
      throw Exception("Service does not exist");
      break;
    case ERROR_PATH_NOT_FOUND:
      throw Exception("Path not found");
      break;
    case ERROR_INVALID_PARAMETER:
      throw Exception("Invalid parameter");
      break;
    case ERROR_SERVICE_ALREADY_RUNNING:
      throw Exception("Service already running");
      break;
    case ERROR_SERVICE_DATABASE_LOCKED:
      throw Exception("Service database locked");
      break;
    case ERROR_SERVICE_DEPENDENCY_DELETED:
      throw Exception("Dependency deleted");
      break;
    case ERROR_SERVICE_DEPENDENCY_FAIL:
      throw Exception("Dependency failed");
      break;
    case ERROR_SERVICE_DISABLED:
      throw Exception("Service disabled");
      break;
    case ERROR_SERVICE_LOGON_FAILED:
      throw Exception("Service logon failed");
      break;
    case ERROR_SERVICE_MARKED_FOR_DELETE:
      throw Exception("Service marked for deletion");
      break;
    case ERROR_SERVICE_NO_THREAD:
      throw Exception("No thread for service");
      break;
    case ERROR_DEPENDENT_SERVICES_RUNNING:
      throw Exception("Dependent services running");
      break;
    case ERROR_INVALID_SERVICE_CONTROL:
      throw Exception("Invalid service control");
      break;
    case ERROR_SERVICE_CANNOT_ACCEPT_CTRL:
      throw Exception("Not ready for service control");
      break;
    case ERROR_SERVICE_NOT_ACTIVE:
      throw Exception("Service not active");
      break;
    case ERROR_SERVICE_REQUEST_TIMEOUT:
      throw Exception("Service request timed out");
      break;
    default:
      throw Exception("Unexpected service error: " + IntToStr(i));
  }
}
//---------------------------------------------------------------------------
void __fastcall TServiceController::QueryStatus() {

  if (!QueryServiceStatus(SvcHandle, Status)) {
    DWORD res = GetLastError();
    ThrowException(res);
  }
  else
    FStatus = MapState(Status->dwCurrentState);
}
//---------------------------------------------------------------------------
AnsiString __fastcall TServiceController::GetServiceName() {
  return *FServiceName;
}
//---------------------------------------------------------------------------
void __fastcall TServiceController::SetServiceName(AnsiString sn) {
  *FServiceName = sn;
}
//---------------------------------------------------------------------------
ServiceState __fastcall TServiceController::GetStatus() {
  QueryStatus();
  
  return FStatus;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TServiceController::GetStatusString() {
  QueryStatus();

  return *FStatusString;
}
//---------------------------------------------------------------------------
ServiceState __fastcall TServiceController::MapState(DWORD i) {
  ServiceState r;

  switch (i) {
    case SERVICE_STOPPED:
      r = Stopped;
      break;
    case SERVICE_START_PENDING:
      r = Starting;
      break;
    case SERVICE_STOP_PENDING:
      r = Stopping;
      break;
    case SERVICE_RUNNING:
      r = Running;
      break;
    case SERVICE_CONTINUE_PENDING:
      r = Continuing;
      break;
    case SERVICE_PAUSE_PENDING:
      r = Pausing;
      break;
    case SERVICE_PAUSED:
      r = Paused;
      break;
    default:
      r = None;
  }


  switch (r) {
    case Stopped:
      *FStatusString = "Stopped";
      break;
    case Starting:
      *FStatusString = "Starting";
      break;
    case Stopping:
      *FStatusString = "Stopping";
      break;
    case Running:
      *FStatusString = "Running";
      break;
    case Continuing:
      *FStatusString = "Continuing";
      break;
    case Pausing:
      *FStatusString = "Pausing";
      break;
    case Paused:
      *FStatusString = "Paused";
      break;
    default:
      *FStatusString = "None";
  }

  return r;
}
//---------------------------------------------------------------------------
void __fastcall TServiceController::Install(AnsiString SvcName, AnsiString DispName, StartType st, AnsiString FilePath, bool Interactive) {
  SC_HANDLE hand;

  if (!CanInstall) {
    if (SvcHandle != NULL)
      CloseServ();

    CloseManager();
    OpenManager(true);
  }
  else {
    if (SvcHandle != NULL)
      CloseServ();
  }


  DWORD desAccess = SERVICE_WIN32_OWN_PROCESS;
  if (Interactive)
    desAccess |= SERVICE_INTERACTIVE_PROCESS;

  hand = CreateService(ManHandle, SvcName.c_str(), DispName.c_str(),
    SERVICE_ALL_ACCESS & SERVICE_QUERY_STATUS & DELETE & GENERIC_READ & GENERIC_WRITE & GENERIC_EXECUTE,
    desAccess, st, SERVICE_ERROR_IGNORE,
    FilePath.c_str(), NULL, NULL, NULL, NULL, NULL);


  if (hand != NULL) {
    if (SvcHandle != NULL)
      CloseServ();

    SvcHandle = hand;

    CloseManager();
    OpenManager(false);

    OpenServ(SvcName);
  }
  else {
    DWORD res = GetLastError();
    CloseManager();
    ThrowException(res);
  }
}
//---------------------------------------------------------------------------
void __fastcall TServiceController::Uninstall() {
  AnsiString *OldService = new AnsiString();

  if (!CanInstall) {
    if (SvcHandle != NULL) {
      *OldService = *FServiceName;
      CloseServ();
    }

    CloseManager();
    OpenManager(true);

    OpenServ(*OldService);
    if (MapState(Status->dwCurrentState) != Stopped)
      Stop();
  }


  if (SvcHandle != NULL) {
    if (!DeleteService(SvcHandle)) {
      DWORD res = GetLastError();
      ThrowException(res);
    }
    else
      CloseServ();
  }

  delete OldService;
}
//---------------------------------------------------------------------------

