//---------------------------------------------------------------------------
#ifndef MarkupParserH
#define MarkupParserH
//---------------------------------------------------------------------------
#include <stdlib.h>
#include <SysUtils.hpp>
#include <Controls.hpp>
#include <Classes.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
enum _TokType {EL_NON, EL_TEX, EL_TAG, EL_COM, EL_SCR};
typedef _TokType TokType;
//---------------------------------------------------------------------------
typedef void __fastcall (__closure *TMarkupNotifyEvent)(System::TObject* Sender, TokType TT, AnsiString *Content, int Row, int Col);
//---------------------------------------------------------------------------
class PACKAGE TMarkupParser : public TComponent
{
private:
        TMarkupNotifyEvent FOnMarkupElement;

        int FNumberOfElements;
        bool FRemoveCRLF;

protected:
        void __fastcall SetRemoveCRLF(bool rc);

public:
        __fastcall TMarkupParser(TComponent* Owner);

        void __fastcall ParseElements(TMemoryStream *ms);

__published:
        __property int NumberOfElements = {read=FNumberOfElements};
        __property bool RemoveCRLF = {read=FRemoveCRLF, write=SetRemoveCRLF, default=false};

        __property TMarkupNotifyEvent OnMarkupElement = {read=FOnMarkupElement,write=FOnMarkupElement};
};
//---------------------------------------------------------------------------
#endif
