//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "Tokenizer.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//

static inline void ValidCtrCheck(TTokenizer *)
{
        new TTokenizer(NULL);
}
//---------------------------------------------------------------------------
__fastcall TTokenizer::TTokenizer(TComponent* Owner)
        : TComponent(Owner)
{
  FOnGotToken = NULL;

  FWhitespace = new AnsiString();
  FDelimiters = new AnsiString();
  FStrDelims = new AnsiString();
  FTrimChars = new AnsiString();
  FText = new AnsiString();

  FTokens = new TStringList();
  FTokenCount = 0;
}
//---------------------------------------------------------------------------
__fastcall TTokenizer::~TTokenizer()
{
  delete FTokens;

  delete FText;
  delete FDelimiters;
  delete FStrDelims;
  delete FWhitespace;
}
//---------------------------------------------------------------------------
namespace Tokenizer
{
        void __fastcall PACKAGE Register()
        {
                 TComponentClass classes[1] = {__classid(TTokenizer)};
                 RegisterComponents("Celestial", classes, 0);
        }
}
//---------------------------------------------------------------------------
AnsiString __fastcall TTokenizer::GetWhitespace()
{
  return *FWhitespace;
}
//---------------------------------------------------------------------------
void __fastcall TTokenizer::SetWhitespace(AnsiString ws)
{
  *FWhitespace = ws;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TTokenizer::GetDelimiters()
{
  return *FDelimiters;
}
//---------------------------------------------------------------------------
void __fastcall TTokenizer::SetDelimiters(AnsiString d)
{
  *FDelimiters = d;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TTokenizer::GetStringDelimiters()
{
  return *FStrDelims;
}
//---------------------------------------------------------------------------
void __fastcall TTokenizer::SetStringDelimiters(AnsiString d)
{
  *FStrDelims = d;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TTokenizer::GetText()
{
  return *FText;
}
//---------------------------------------------------------------------------
void __fastcall TTokenizer::SetText(AnsiString t)
{
  *FText = t;

  ParseText();
}
//---------------------------------------------------------------------------
AnsiString __fastcall TTokenizer::GetTrimCharacters()
{
  return *FTrimChars;
}
//---------------------------------------------------------------------------
void __fastcall TTokenizer::SetTrimCharacters(AnsiString tc)
{
  *FTrimChars = tc;
}
//---------------------------------------------------------------------------
bool __fastcall TTokenizer::IsDelimiter(AnsiString s)
{
  bool rv = false;

  for (int j = 1; j <= FDelimiters->Length(); j++) {
    rv = (FDelimiters->SubString(j, 1) == s.SubString(1, 1));
    if (rv) break;
  }

  return rv;
}
//---------------------------------------------------------------------------
bool __fastcall TTokenizer::IsStringDelimiter(AnsiString s)
{
  bool rv = false;

  for (int j = 1; j <= FStrDelims->Length(); j++) {
    rv = (FStrDelims->SubString(j, 1) == s.SubString(1, 1));
    if (rv) break;
  }

  return rv;
}
//---------------------------------------------------------------------------
bool __fastcall TTokenizer::IsWhitespace(AnsiString s)
{
  bool rv = false;

  for (int j = 1; j <= FWhitespace->Length(); j++) {
    rv = (FWhitespace->SubString(j, 1) == s.SubString(1, 1));
    if (rv) break;
  }

  return rv;
}
//---------------------------------------------------------------------------
void __fastcall TTokenizer::AddFToken(AnsiString *s)
{
  AnsiString *t = new AnsiString();

  *t = *s;
  if (FTrimChars->Pos(t->SubString(1, 1)) > 0)
    t->Delete(1, 1);

  if (FTrimChars->Pos(t->SubString(t->Length(), 1)) > 0)
    t->Delete(t->Length(), 1);

  FTokens->Add(*t);

  delete t;
}
//---------------------------------------------------------------------------
void __fastcall TTokenizer::ParseText()
{
  AnsiString *buf = new AnsiString();
  AnsiString *hist = new AnsiString();
  AnsiString *inch = new AnsiString();
  AnsiString *quoch = new AnsiString();
  bool inquote = false;
  int len;

  FTokens->Clear();
  len = FText->Length();

  for (int i = 1; i <= len; i++) {
    *inch = FText->SubString(i, 1);

    *hist = *hist + *inch;
    if (hist->Length() > 2)
      hist->Delete(1, hist->Length() - 2);

    if (hist->SubString(1, 1) == "\\")
      buf->Delete(buf->Length(), 1);	// Delete last char

    if (IsStringDelimiter(*inch)) {
      *buf = *buf + *inch;

      if (inquote) {
        if (*inch == *quoch) {
          inquote = false;
          if (buf->Length() > 0)
            AddFToken(buf);
          FTokenCount = FTokens->Count;
          *buf = "";
          *quoch = "";
        }
      } else {
        inquote = true;
        *quoch = *inch;
      }

    } else {

      if (!inquote) {
        if (!IsWhitespace(*inch)) {
          if (IsDelimiter(*inch)) {
            if (buf->Length() > 0)
              AddFToken(buf);
            FTokenCount = FTokens->Count;
            *buf = "";
          } else
            *buf = *buf + *inch;
        }
      } else
        *buf = *buf + *inch;

    }

  }


  if (buf->Length() > 0) {
    FTokens->Add(*buf);
    FTokenCount = FTokens->Count;
  }


  delete quoch;
  delete inch;
  delete hist;
  delete buf;
}
//---------------------------------------------------------------------------
void __fastcall TTokenizer::GetTokens(TStringList *sl)
{
  sl->Clear();

  for (int i = 0; i < FTokens->Count; i++)
    sl->Add(FTokens->Strings[i]);
}
//---------------------------------------------------------------------------
AnsiString __fastcall TTokenizer::GetFirstToken()
{
  CurrToken = 0;
  return GetNextToken();
}
//---------------------------------------------------------------------------
AnsiString __fastcall TTokenizer::GetNextToken()
{
  AnsiString s;
  int NextToken = CurrToken + 1;

  EndOfList = (NextToken > FTokens->Count);

  if (EndOfList)
    s = NULL;
  else
    s = FTokens->Strings[CurrToken];

  CurrToken++;
  
  return s;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TTokenizer::GetToken(int Num)
{
  if ((Num >= 0) && (Num < FTokens->Count))
    return FTokens->Strings[Num];
  else
    return (AnsiString)NULL;
}
//---------------------------------------------------------------------------
bool __fastcall TTokenizer::TokenIsNumeric(int i)
{
  bool rv = false;
  AnsiString *s = new AnsiString();

  *s = FTokens->Strings[i];

  for (int j = 1; j <= s->Length(); j++) {
    rv = ((s->SubString(j, 1) >= "0") && (s->SubString(j, 1) <= "9"));
    if (!rv) break;
  }

  delete s;

  return rv;
}
//---------------------------------------------------------------------------
bool __fastcall TTokenizer::TokenIsAlphabetic(int i)
{
  bool rv = false;
  AnsiString *s = new AnsiString();

  *s = UpperCase(FTokens->Strings[i]);

  for (int j = 1; j <= s->Length(); j++) {
    rv = ((s->SubString(j, 1) >= "A") && (s->SubString(j, 1) <= "Z"));
    if (!rv) break;
  }

  delete s;

  return rv;
}
//---------------------------------------------------------------------------
bool __fastcall TTokenizer::TokenIsAlphanum(int i)
{
  bool rv = false;
  AnsiString *s = new AnsiString();

  *s = UpperCase(FTokens->Strings[i]);

  for (int j = 1; j <= s->Length(); j++) {
    rv = (((s->SubString(j, 1) >= "A") && (s->SubString(j, 1) <= "Z")) || ((s->SubString(j, 1) >= "0") && (s->SubString(j, 1) <= "9")));
    if (!rv) break;
  }

  delete s;

  return rv;
}
//---------------------------------------------------------------------------
bool __fastcall TTokenizer::EndOfTokens()
{
  return EndOfList;
}
//---------------------------------------------------------------------------

