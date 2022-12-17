//---------------------------------------------------------------------------
#ifndef AddressBookH
#define AddressBookH
//---------------------------------------------------------------------------
#include <stdlib.h>
#include <SysUtils.hpp>
#include <Controls.hpp>
#include <Classes.hpp>
#include <Forms.hpp>
#include "MailIdentity.h"
//---------------------------------------------------------------------------
class PACKAGE TAddressBook : public TComponent
{
private:
        bool FDuplicatesAllowed;
        bool FCaseSensitive;
        AnsiString *FListDelimiters;

        TStringList *Addresses;

        bool __fastcall IsDelimiter(AnsiString s);
        void __fastcall ParseList(AnsiString s);

        TStringList *LastEntry;
        int LastEntryCtr;
        TStringList *LastAddress;
        int LastAddressCtr;

protected:
        AnsiString __fastcall GetListDelimiters();
        void __fastcall SetListDelimiters(AnsiString ld);
        int  __fastcall GetCount();
        void __fastcall SetDuplicatesAllowed(bool dup);
        void __fastcall SetCaseSensitive(bool cs);
        bool __fastcall FindIdentity(TMailIdentity *mid);
        void __fastcall GetAllEntries();
        void __fastcall GetAllAddresses();

public:
        __fastcall TAddressBook(TComponent *Owner);
        __fastcall ~TAddressBook();

        void __fastcall Clear();
        void __fastcall Add(TMailIdentity *mid);
        void __fastcall Add(AnsiString mid);
        TMailIdentity * __fastcall GetEntry(int i);
        TMailIdentity * __fastcall FindEntry(AnsiString str);
        void __fastcall Append(TAddressBook *ab);
        void __fastcall Append(TMailIdentity *mi);
        void __fastcall Export(AnsiString fname);
        AnsiString __fastcall GetFirstEntry();
        AnsiString __fastcall GetNextEntry();
        AnsiString __fastcall GetFirstAddress();
        AnsiString __fastcall GetNextAddress();

__published:
        __property bool DuplicatesAllowed = {read=FDuplicatesAllowed, write=SetDuplicatesAllowed, default=true};
        __property bool CaseSensitive = {read=FCaseSensitive, write=SetCaseSensitive, default=false};
        __property AnsiString ListDelimiters = {read=GetListDelimiters, write=SetListDelimiters};
        __property int Count = {read=GetCount};
};
//---------------------------------------------------------------------------
#endif
