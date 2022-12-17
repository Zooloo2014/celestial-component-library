//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "MarkupParser.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//

static inline void ValidCtrCheck(TComponent *)
{
        new TMarkupParser(NULL);
}
//---------------------------------------------------------------------------
__fastcall TMarkupParser::TMarkupParser(TComponent* Owner)
        : TComponent(Owner)
{
  FRemoveCRLF = false;
  FNumberOfElements = 0;
}
//---------------------------------------------------------------------------
namespace Markupparser
{
        void __fastcall PACKAGE Register()
        {
                 TComponentClass classes[1] = {__classid(TMarkupParser)};
                 RegisterComponents("Celestial", classes, 0);
        }
}
//---------------------------------------------------------------------------
void __fastcall TMarkupParser::SetRemoveCRLF(bool rc)
{
  FRemoveCRLF = rc;
}
//---------------------------------------------------------------------------
void __fastcall TMarkupParser::ParseElements(TMemoryStream *ms)
{
  AnsiString *el_comment = new AnsiString();
  AnsiString *el_script = new AnsiString();
  AnsiString *el_text = new AnsiString();
  AnsiString *el_tag = new AnsiString();
  AnsiString *buf = new AnsiString();
  AnsiString *hist = new AnsiString();
  bool capture = false;
  bool com_strt;
  bool com_end;
  bool scr_strt;
  bool scr_end;
  TokType lasttok;
  char inch = 0;
  int cRow = 1;
  int cCol = 1;
  int xRow = 1;
  int xCol = 1;
  int yRow = 1;
  int yCol = 1;

  FNumberOfElements = 0;
  lasttok = EL_NON;

  while (ms->Read(&inch, 1) != 0) {
    if (FRemoveCRLF)
      if ((inch == 0x0d) || (inch == 0x0a))
        continue;

    cCol++;
    if (inch == 0x0d) {
      cCol = 0;
      cRow++;
    }

    *hist = *hist + inch;
    if (hist->Length() > 4)
      hist->Delete(1, hist->Length() - 4);

    com_strt = (hist->SubString(1, 4) == "<!--");
    com_end = (hist->SubString(2, 3) == "-->");
    scr_strt = ((hist->SubString(3, 2) == "<?") || (hist->SubString(3, 2) == "<%"));
    scr_end = ((hist->SubString(3, 2) == "?>") || (hist->SubString(3, 2) == "%>"));

    if (com_strt) {
      *el_comment = "";
      capture = false;
      lasttok = EL_COM;
      xRow = cRow;
      xCol = cCol - 3;
    }

    if (com_end) {
      *el_comment = *buf + inch;
      *buf = "";

      if (FOnMarkupElement != NULL)
        FOnMarkupElement(this, EL_COM, el_comment, xRow, xCol);

      lasttok = EL_NON;
      continue;
    }

    if (scr_strt) {
      *el_script = "";
      capture = false;
      lasttok = EL_SCR;
      xRow = cRow;
      xCol = cCol - 1;
    }

    if (scr_end) {
      *el_script = *buf + inch;
      *buf = "";

      if (FOnMarkupElement != NULL)
        FOnMarkupElement(this, EL_SCR, el_script, xRow, xCol);

      lasttok = EL_NON;
      continue;
    }

    if (lasttok == EL_COM) {
      *buf = *buf + inch;       /* Build up comment */
      continue;
    }

    switch (inch) {
      case '<':
        if (!capture) {
          *el_text = *buf;
          lasttok = EL_TEX;
          *buf = inch;		/* Start new token	*/
          capture = true;
          xRow = cRow;
          xCol = cCol;
        } else {
          *buf = *buf + inch;	/* Output char to buffer and fail	*/
          return;
        }
        break;

      case '>':
        if (!capture) {
          *buf = *buf + inch;	/* Output char to buffer - element-text	*/
          lasttok = EL_NON;
        } else {
          *el_tag = *buf + inch;	/* Output char to buffer - token	*/
          *buf = "";
          capture = false;
          lasttok = EL_TAG;
          yRow = cRow;
          yCol = cCol + 1;
        }
        break;

      default:
        *buf = *buf + inch;	/* Output char to buffer - text	*/
        lasttok = EL_NON;
    }


    switch (lasttok) {
      case EL_TEX:
        FNumberOfElements++;

        if (FOnMarkupElement != NULL)
          FOnMarkupElement(this, EL_TEX, el_text, yRow, yCol);

        break;

      case EL_TAG:
        FNumberOfElements++;

        if (FOnMarkupElement != NULL)
          FOnMarkupElement(this, EL_TAG, el_tag, xRow, xCol);

        break;

      default: ;
    }
  }


  delete buf;
  delete el_tag;
  delete el_text;
}
//---------------------------------------------------------------------------

