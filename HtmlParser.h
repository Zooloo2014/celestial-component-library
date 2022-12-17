//---------------------------------------------------------------------------
#ifndef HtmlParserH
#define HtmlParserH
//---------------------------------------------------------------------------
#include <stdlib.h>
#include <SysUtils.hpp>
#include <Controls.hpp>
#include <Classes.hpp>
#include <Forms.hpp>
#include "Tokenizer.h"
#include "MarkupParser.h"
//---------------------------------------------------------------------------
typedef void __fastcall (__closure *THtmlTagEvent)(System::TObject* Sender, AnsiString Content, int Row, int Col);
typedef void __fastcall (__closure *THtmlAttrEvent)(System::TObject* Sender, AnsiString Tag, AnsiString Attr, AnsiString Value, int Row, int Col);
//---------------------------------------------------------------------------
class PACKAGE THtmlParser : public TMarkupParser
{
private:
        THtmlTagEvent FOnRawContent;
        THtmlTagEvent FOnStartTag;
        THtmlTagEvent FOnStopTag;
        THtmlTagEvent FOnScript;
        THtmlTagEvent FOnComment;
        THtmlTagEvent FOnTagText;
        THtmlAttrEvent FOnTagAttribute;

        bool FDiscardWhitespace;
        bool FWantEndTags;

        AnsiString *FWhitespace;
        AnsiString *Cont;
        AnsiString *TagAttr;
        AnsiString *TagName;
        AnsiString *AttrName;
        AnsiString *AttrVal;
        TTokenizer *Tokens;
        TTokenizer *Attribute;

        bool DoSkip;

        void __fastcall HtmlMarkupElement(TObject *Sender, TokType Ttype, AnsiString *Content, int Row, int Col);
        void __fastcall StripLeading(AnsiString *s);

        __property OnMarkupElement;

protected:
        void __fastcall SetDiscardWhitespace(bool dws);
        void __fastcall SetWantEndTags(bool wet);
        AnsiString __fastcall GetWhitespace();
        void __fastcall SetWhitespace(AnsiString ws);

public:
        __fastcall THtmlParser(TComponent* Owner);

__published:
        __property bool DiscardWhitespace = {read=FDiscardWhitespace, write=SetDiscardWhitespace, default=true};
        __property bool WantEndTags = {read=FWantEndTags, write=SetWantEndTags, default=true};
        __property AnsiString Whitespace = {read=GetWhitespace, write=SetWhitespace};

        __property THtmlTagEvent OnRawContent = {read=FOnRawContent, write=FOnRawContent};
        __property THtmlTagEvent OnStartTag = {read=FOnStartTag, write=FOnStartTag};
        __property THtmlTagEvent OnStopTag = {read=FOnStopTag, write=FOnStopTag};
        __property THtmlTagEvent OnTagText = {read=FOnTagText, write=FOnTagText};
        __property THtmlTagEvent OnScript = {read=FOnScript, write=FOnScript};
        __property THtmlTagEvent OnComment = {read=FOnComment, write=FOnComment};
        __property THtmlAttrEvent OnTagAttribute = {read=FOnTagAttribute, write=FOnTagAttribute};
};
//---------------------------------------------------------------------------
#endif
