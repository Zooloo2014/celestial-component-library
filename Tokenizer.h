//---------------------------------------------------------------------------
#ifndef TokenizerH
#define TokenizerH
//---------------------------------------------------------------------------
#include <stdlib.h>
#include <SysUtils.hpp>
#include <Controls.hpp>
#include <Classes.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
typedef void __fastcall (__closure *TTokenNotifyEvent)(System::TObject* Sender, AnsiString *Token);
//---------------------------------------------------------------------------
class PACKAGE TTokenizer : public TComponent
{
private:
        TTokenNotifyEvent FOnGotToken;

        AnsiString *FWhitespace;
        AnsiString *FDelimiters;
        AnsiString *FStrDelims;
        AnsiString *FTrimChars;
        AnsiString *FText;
        TStringList *FTokens;
        int FTokenCount;

        bool EndOfList;
        int CurrToken;

        bool __fastcall IsDelimiter(AnsiString s);
        bool __fastcall IsWhitespace(AnsiString s);
        bool __fastcall IsStringDelimiter(AnsiString s);
        void __fastcall ParseText();
        void __fastcall AddFToken(AnsiString *s);

protected:
        virtual AnsiString __fastcall GetWhitespace();
        virtual void __fastcall SetWhitespace(AnsiString ws);
        virtual AnsiString __fastcall GetDelimiters();
        virtual void __fastcall SetDelimiters(AnsiString d);
        virtual AnsiString __fastcall GetStringDelimiters();
        virtual void __fastcall SetStringDelimiters(AnsiString d);
        virtual AnsiString __fastcall GetText();
        virtual void __fastcall SetText(AnsiString t);
        virtual AnsiString __fastcall GetTrimCharacters();
        virtual void __fastcall SetTrimCharacters(AnsiString tc);

public:
        __fastcall TTokenizer(TComponent* Owner);
        __fastcall ~TTokenizer();

        virtual void __fastcall GetTokens(TStringList *sl);
        virtual AnsiString __fastcall GetToken(int Num);
        virtual AnsiString __fastcall GetFirstToken();
        virtual AnsiString __fastcall GetNextToken();
        bool __fastcall TokenIsNumeric(int i);
        bool __fastcall TokenIsAlphabetic(int i);
        bool __fastcall TokenIsAlphanum(int i);
        bool __fastcall EndOfTokens();

__published:
        __property AnsiString Whitespace = {read=GetWhitespace, write=SetWhitespace, nodefault};
        __property AnsiString Delimiters = {read=GetDelimiters, write=SetDelimiters, nodefault};
        __property AnsiString StringDelimiters = {read=GetStringDelimiters, write=SetStringDelimiters, nodefault};
        __property AnsiString TrimCharacters = {read=GetTrimCharacters, write=SetTrimCharacters, nodefault};
        __property AnsiString Text = {read=GetText, write=SetText, nodefault};
        __property int TokenCount = {read=FTokenCount, nodefault};
        __property TTokenNotifyEvent OnGotToken = {read=FOnGotToken, write=FOnGotToken, nodefault};
};
//---------------------------------------------------------------------------
#endif
