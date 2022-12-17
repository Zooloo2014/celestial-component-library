//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include <math.h>
#include "DataSize.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//

static inline void ValidCtrCheck(TDataSize *)
{
        new TDataSize(NULL);
}
//---------------------------------------------------------------------------
__fastcall TDataSize::TDataSize(TComponent* Owner)
        : TComponent(Owner)
{
  FValue = 0;
  FShowLabels = true;
  FAutoScale = false;
  FTrailingZeroes = false;
  FDelimitThousands = true;
  FScale = ST_BYTE;
  FPrecision = 1;
  FLabelType = LT_SHORT;

  FText = new AnsiString();
  Label = new AnsiString();
  TxVal = new AnsiString();
}
//---------------------------------------------------------------------------
__fastcall TDataSize::~TDataSize()
{
  delete FText;
  delete Label;
  delete TxVal;
}
//---------------------------------------------------------------------------
namespace Datasize
{
        void __fastcall PACKAGE Register()
        {
                 TComponentClass classes[1] = {__classid(TDataSize)};
                 RegisterComponents("Celestial", classes, 0);
        }
}
//---------------------------------------------------------------------------
void __fastcall TDataSize::SetValue(double val)
{
  Clear();
  FValue = val;
  Convert();
}
//---------------------------------------------------------------------------
AnsiString __fastcall TDataSize::GetText()
{
  return *FText;
}
//---------------------------------------------------------------------------
void __fastcall TDataSize::SetShowLabels(bool l)
{
  FShowLabels = l;
}
//---------------------------------------------------------------------------
void __fastcall TDataSize::SetPrecision(int p)
{
  if (p > -1)
    FPrecision = p;
}
//---------------------------------------------------------------------------
void __fastcall TDataSize::SetScale(TScaleTypes s)
{
  if ((s >= ST_BYTE) && (s <= ST_TERA))
    FScale = s;
  else
    throw Exception("Invalid value for Scale property");
}
//---------------------------------------------------------------------------
void __fastcall TDataSize::Clear()
{
  FValue = 0;
  *FText = "";
}
//---------------------------------------------------------------------------
void __fastcall TDataSize::SetAutoScale(bool asu)
{
  FAutoScale = asu;
}
//---------------------------------------------------------------------------
void __fastcall TDataSize::SetTrailingZeroes(bool tz)
{
  FTrailingZeroes = tz;
}
//---------------------------------------------------------------------------
void __fastcall TDataSize::SetLabelType(TLabelTypes lt)
{
  FLabelType = lt;
}
//---------------------------------------------------------------------------
void __fastcall TDataSize::Convert()
{
  AnsiString *fmt = new AnsiString();
  AnsiString *lab = new AnsiString();

  ValBytes = FValue;
  ValKilos = (ValBytes + 1) / 1024;
  ValMegs = ValKilos / 1024;
  ValGigs = ValMegs / 1024;
  ValTeras = ValGigs / 1024;

  if (AutoScale) {
    if (ValBytes > 0)
      FScale = ST_BYTE;
    if (ValKilos > 1)
      FScale = ST_KILO;
    if (ValMegs > 1)
      FScale = ST_MEGA;
    if (ValGigs > 1)
      FScale = ST_GIGA;
    if (ValTeras > 1)
      FScale = ST_TERA;
  }


  if (FDelimitThousands)
    *fmt = "%14." + IntToStr(FPrecision) + "n";
  else
    *fmt = "%14." + IntToStr(FPrecision) + "f";


  if (FScale == ST_BYTE)
    *TxVal = Format(*fmt, ARRAYOFCONST(((double)ValBytes)));
  if (FScale == ST_KILO)
    *TxVal = Format(*fmt, ARRAYOFCONST(((double)ValKilos)));
  if (FScale == ST_MEGA)
    *TxVal = Format(*fmt, ARRAYOFCONST(((double)ValMegs)));
  if (FScale == ST_GIGA)
    *TxVal = Format(*fmt, ARRAYOFCONST(((double)ValGigs)));
  if (FScale == ST_TERA)
    *TxVal = Format(*fmt, ARRAYOFCONST(((double)ValTeras)));


  if (!FTrailingZeroes) {
    int i;
    for (i = TxVal->Length(); i > 0; i--)
      if (TxVal->SubString(i, 1) != "0")
        break;

    if (TxVal->SubString(i, 1) == ".") i--;
    TxVal->SetLength(i);
  }


  if (FLabelType == LT_SHORT) {
    if (FScale == ST_BYTE)
      *lab = "B";
    else
    if (FScale == ST_KILO)
      *lab = "Kb";
    else
    if (FScale == ST_MEGA)
       *lab = "Mb";
    else
    if (FScale == ST_GIGA)
      *lab = "Gb";
    else
    if (FScale == ST_TERA)
      *lab = "Tb";
    else
      *lab = "B";
  }

  if (FLabelType == LT_LONG) {
    if (FScale == ST_BYTE)
      *lab = "Bytes";
    else
    if (FScale == ST_KILO)
      *lab = "Kilobytes";
    else
    if (FScale == ST_MEGA)
       *lab = "Megabytes";
    else
    if (FScale == ST_GIGA)
      *lab = "Gigabytes";
    else
    if (FScale == ST_TERA)
      *lab = "Terabytes";
    else
      *lab = "Bytes";
  }


  if (FShowLabels)
    *FText = TxVal->Trim() + " " + *lab;
  else
    *FText = TxVal->Trim();

  delete lab;
  delete fmt;
}
//---------------------------------------------------------------------------

