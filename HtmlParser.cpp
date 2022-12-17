//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "HtmlParser.h"
#pragma link "MarkupParser"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//

static inline void ValidCtrCheck(THtmlParser *)
{
        new THtmlParser(NULL);
}
//---------------------------------------------------------------------------
__fastcall THtmlParser::THtmlParser(TComponent* Owner)
        : TMarkupParser(Owner)
{
  OnMarkupElement = HtmlMarkupElement;
  FDiscardWhitespace = true;
  FWantEndTags = true;
  FWhitespace = new AnsiString();
  *FWhitespace = " ";

  Cont = new AnsiString();
  TagName = new AnsiString();
  TagAttr = new AnsiString();
  AttrName = new AnsiString();
  AttrVal = new AnsiString();
  Tokens = new TTokenizer(this);
  Tokens->Delimiters = " ";
  Tokens->StringDelimiters = "\'\"";
  Attribute = new TTokenizer(this);
  Attribute->Delimiters = "=";
  Attribute->StringDelimiters = "\'\"";
  Attribute->TrimCharacters = "\'\"";

  DoSkip = false;
}
//---------------------------------------------------------------------------
namespace Htmlparser
{
        void __fastcall PACKAGE Register()
        {
                 TComponentClass classes[1] = {__classid(THtmlParser)};
                 RegisterComponents("Celestial", classes, 0);
        }
}
//---------------------------------------------------------------------------
void __fastcall THtmlParser::SetDiscardWhitespace(bool dws)
{
  FDiscardWhitespace = dws;
}
//---------------------------------------------------------------------------
void __fastcall THtmlParser::SetWantEndTags(bool wet)
{
  FWantEndTags = wet;
}
//---------------------------------------------------------------------------
AnsiString __fastcall THtmlParser::GetWhitespace()
{
  return *FWhitespace;
}
//---------------------------------------------------------------------------
void __fastcall THtmlParser::SetWhitespace(AnsiString ws)
{
  *FWhitespace = ws;
}
//---------------------------------------------------------------------------
void __fastcall THtmlParser::StripLeading(AnsiString *s)
{
  for (int j = 1; j < FWhitespace->Length(); j++) {
    if (s->SubString(1, 1) == FWhitespace->SubString(j, 1)) {
      s->Delete(1, 1);
      StripLeading(s);
      break;
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall THtmlParser::HtmlMarkupElement(TObject *Sender, TokType Ttype, AnsiString *Content, int Row, int Col)
{
  bool OpenTag = false;  // Standalone start-tag
  bool EndTag = false;  // Embedded end-tag
  bool CloseTag = false; // Standalone end-tag
  
  *Cont = *Content;

  if (FOnRawContent != NULL)
    FOnRawContent(this, *Cont, Row, Col);


  if (FDiscardWhitespace) {
    while ((FWhitespace->Pos(Cont->SubString(1, 1)) > 0) && (Cont->Length() > 0))
      Cont->Delete(1, 1);
    while ((FWhitespace->Pos(Cont->SubString(Cont->Length(), 1)) > 0) && (Cont->Length() > 0))
      Cont->Delete(Cont->Length(), 1);
  }


  if (*Cont != "") {
    if (Ttype == EL_TAG) {

      if (Cont->SubString(1, 1) == "<") {
        if (Cont->SubString(2, 1) == "/") {
          CloseTag = true;
          Cont->Delete(1, 2);
        } else {
          OpenTag = true;
          Cont->Delete(1, 1);
        }
      }

      if (!CloseTag) {
        if (Cont->SubString(Cont->Length() - 1, 2) == "/>") {
          EndTag = true;
          Cont->Delete(Cont->Length() - 1, 2);
        }
      }

      if (Cont->SubString(Cont->Length(), 1) == ">")
        Cont->Delete(Cont->Length(), 1);


      *Cont = Cont->Trim();
      Tokens->Text = *Cont;
      if (Tokens->TokenCount > 0)
        *TagName = LowerCase(Tokens->GetFirstToken());

      if (*TagName == "script")
        DoSkip = (OpenTag && (!CloseTag));

      if (!DoSkip) {
        if (OpenTag)
          if (FOnStartTag != NULL)
            FOnStartTag(this, *TagName, Row, Col);

        for (int k = 1; k < Tokens->TokenCount; k++) {
          *TagAttr = Tokens->GetNextToken();
          Attribute->Text = *TagAttr;

          if (Attribute->TokenCount > 0) {
            *AttrName = Attribute->GetFirstToken();
            if (Attribute->TokenCount > 1)
              *AttrVal = Attribute->GetNextToken();
            else
              *AttrVal = "";
          }

          if (FOnTagAttribute != NULL)
            FOnTagAttribute(this, *TagName, *AttrName, *AttrVal, Row, Col);
        }

        if (CloseTag || EndTag) {
          if (*TagName != "script") {
            if (FWantEndTags)
              if (FOnStopTag != NULL)
                FOnStopTag(this, *TagName, Row, Col);
          } else
            Ttype = EL_SCR;
        }
      } else
        Ttype = EL_SCR;
    }


    if (Ttype == EL_TEX) {
      if (!DoSkip) {
        if (FOnTagText != NULL)
          FOnTagText(this, *Cont, Row, Col);
      } else
        Ttype = EL_SCR;
    }

    if (Ttype == EL_COM)
      if (!DoSkip) {
        if (FOnComment != NULL)
          FOnComment(this, *Cont, Row, Col);
      } else
        Ttype = EL_SCR;

    if (Ttype == EL_SCR)
      if (FOnScript != NULL)
        FOnScript(this, *Cont, Row, Col);
  }

}
//---------------------------------------------------------------------------

