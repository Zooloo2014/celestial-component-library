//---------------------------------------------------------------------------
#ifndef MailIdentityH
#define MailIdentityH
#include <stdlib.h>
#include <SysUtils.hpp>
#include <Controls.hpp>
#include <Classes.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class PACKAGE TMailIdentity : public TComponent
{
private:
          bool FRequireDispName;
          AnsiString *FDispName;
          TStringList *FEmails;

          AnsiString *FValidNameChars;
          AnsiString *FValidEmailChars;

protected:
          bool __fastcall SelectEmailChar(AnsiString c);
          bool __fastcall SelectNameChar(AnsiString c);
          void __fastcall ExtractAddress(AnsiString str, AnsiString *addr);
          void __fastcall ExtractName(AnsiString str, AnsiString *name);

          void __fastcall SetRequireDispName(bool req);
          AnsiString __fastcall GetDispName();
          void __fastcall SetDispName(AnsiString name);
          AnsiString __fastcall GetValidEmailChars();
          void __fastcall SetValidEmailChars(AnsiString chars);
          AnsiString __fastcall GetValidNameChars();
          void __fastcall SetValidNameChars(AnsiString chars);

public:
          __fastcall TMailIdentity(TComponent *Owner);
          __fastcall TMailIdentity(TComponent *Owner, bool DispNameReqd);
          __fastcall ~TMailIdentity();
          void __fastcall Construct();

          void __fastcall Clear();
          void __fastcall ClearAddresses();
          int __fastcall GetAddressCount();
          AnsiString __fastcall GetAddress(int Num);
          void __fastcall AddAddress(AnsiString addr);
          void __fastcall AddAddresses(TStringList *addrlist);
          int  __fastcall ContainsAddress(AnsiString addr);
          void __fastcall Export(AnsiString fname);

__published:
          __property bool RequireDispName = {read=FRequireDispName, write=SetRequireDispName, default=false};
          __property AnsiString DispName = {read=GetDispName, write=SetDispName};
          __property AnsiString ValidNameChars = {read=GetValidNameChars, write=SetValidNameChars};
          __property AnsiString ValidEmailChars = {read=GetValidEmailChars, write=SetValidEmailChars};
};
//---------------------------------------------------------------------------
#endif
