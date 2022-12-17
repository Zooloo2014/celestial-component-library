//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
USERES("LMTSPack.res");
USEPACKAGE("vcl40.bpi");
USEPACKAGE("Celestial.bpi");
USEUNIT("RemoteHostInfo.cpp");
USEUNIT("HostList.cpp");
USEUNIT("HostAccountInfo.cpp");
USEUNIT("LMTSService.cpp");
USEFORM("FrmRemoteHostInfo.cpp", FormRemoteHostInfo);
USEFORM("FrmHostAccountInfo.cpp", FormHostAccountInfo);
USEFORM("FrmHostList.cpp", FormHostList);
USEUNIT("MailDistributor.cpp");
USEPACKAGE("vclx40.bpi");
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
//   Package source.
//---------------------------------------------------------------------------
int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void*)
{
        return 1;
}
//---------------------------------------------------------------------------
