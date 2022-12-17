//---------------------------------------------------------------------------
#ifndef MailUserH
#define MailUserH
#include <stdlib.h>
//---------------------------------------------------------------------------
class PACKAGE TMailUser : public TCollectionItem
{
private:
          AnsiString *Name;
          TStringList *Email;
          int AddressCount;

          AnsiString *ValidNameChars;
          AnsiString *ValidEmailChars;

          bool __fastcall SelectEmailChar(AnsiString c);
          bool __fastcall SelectNameChar(AnsiString c);
          void __fastcall ExtractAddress(AnsiString s, AnsiString *addr);
          void __fastcall ExtractName(AnsiString s, AnsiString *n);

public:
          __fastcall TMailUser(TCollection* Collection);
          __fastcall ~TMailUser();

          void __fastcall Clear();
          void __fastcall SetName(AnsiString name);
          void __fastcall AddAddress(AnsiString addr);
          int __fastcall GetAddressCount();
          AnsiString __fastcall GetAddress(int Num);
          AnsiString __fastcall GetName();
          AnsiString __fastcall GetValidEmailChars();
          void __fastcall SetValidEmailChars(AnsiString chars);
          AnsiString __fastcall GetValidNameChars();
          void __fastcall SetValidNameChars(AnsiString chars);
};
//---------------------------------------------------------------------------
#endif
