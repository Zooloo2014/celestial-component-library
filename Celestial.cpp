//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
USEPACKAGE("Vcl40.bpi");
USEPACKAGE("Vclx40.bpi");
USEUNIT("TransmissionServer.cpp");
USEUNIT("AddressBook.cpp");
USEUNIT("AgentInfo.cpp");
USEUNIT("CmdStatsInfo.cpp");
USEUNIT("DialupAlerter.cpp");
USEUNIT("DigiClock.cpp");
USEUNIT("DiskMonitor.cpp");
USEUNIT("DiskMonitorFileInfo.cpp");
USEUNIT("DiskMonitorWaitThread.cpp");
USEUNIT("DotMatrix.cpp");
USEUNIT("HistoryComboBox.cpp");
USEUNIT("Lcd.cpp");
USEUNIT("MailHeaders.cpp");
USEUNIT("MailIdentity.cpp");
USEUNIT("MailUser.cpp");
USEUNIT("NewMailNotifier.cpp");
USEUNIT("Pop3Agent.cpp");
USEUNIT("Pop3Server.cpp");
USEUNIT("ServiceController.cpp");
USEUNIT("SmtpAgent.cpp");
USEUNIT("SmtpServer.cpp");
USEUNIT("StatusAreaIcon.cpp");
USEUNIT("Tokenizer.cpp");
USEUNIT("TransmissionClient.cpp");
USEUNIT("TransmissionMessage.cpp");
USEUNIT("DataSize.cpp");
USEUNIT("PopPeeker.cpp");
USEUNIT("CascadedList.cpp");
USEUNIT("MarkupParser.cpp");
USEUNIT("HtmlParser.cpp");
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
