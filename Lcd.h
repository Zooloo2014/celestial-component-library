//---------------------------------------------------------------------------
#ifndef LcdH
#define LcdH
//---------------------------------------------------------------------------
#define BIN2VAL(a, b, c, d, e, f, g, h) (h + (g * 2) + (f * 4) + (e * 8) + (d * 16) + (c * 32) + (b * 64) + (a * 128))
#define MAX_WIDTH 44
#define MAX_HEIGHT 65
//---------------------------------------------------------------------------
#include <stdlib.h>
#include <SysUtils.hpp>
#include <Controls.hpp>
#include <Classes.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class PACKAGE TLcd : public TGraphicControl
{
private:
  TRect FArea;
  AnsiString FString;
  TPicture *Buffer;
  unsigned char SegLit[10];
  TColor FDrawColor;

  bool FShowLeadingZeroes;
  TColor FBackgroundColor;
  TColor FOffColor;
  TColor FColor;
  int FValue;
  int FMaxDigits;
  double FDigitScale;

  void __fastcall DrawSegment(int X, int Y, int Seg);
protected:
  TColor __fastcall GetColor();
  void __fastcall SetColor(TColor AColor);
  TColor __fastcall GetOffColor();
  void __fastcall SetOffColor(TColor AColor);
  TColor __fastcall GetBackgroundColor();
  void __fastcall SetBackgroundColor(TColor AColor);
  int __fastcall GetValue();
  void __fastcall SetValue(int AValue);
  int __fastcall GetMaxDigits();
  void __fastcall SetMaxDigits(int AValue);
  bool __fastcall GetShowLeadingZeroes();
  void __fastcall SetShowLeadingZeroes(bool AValue);
  double __fastcall GetDigitScale();
  void __fastcall SetDigitScale(double AValue);

  void __fastcall DrawDisplay(TCanvas *Canv, TRect CArea, unsigned int Num, bool On);
  void __fastcall Paint(void);
  int __fastcall GetNumDigits(int num);
public:
        __fastcall TLcd(TComponent* Owner);
        __fastcall ~TLcd(void);

  void __fastcall Update(void);

__published:
  __property TColor Color = {read=GetColor, write=SetColor, default=clRed};
  __property TColor OffColor = {read=GetOffColor, write=SetOffColor, default=clDkGray};
  __property TColor BackgroundColor = {read=GetBackgroundColor, write=SetBackgroundColor, default=clBlack};
  __property int Value = {read=GetValue, write=SetValue, default=0};
  __property int MaxDigits = {read=GetMaxDigits, write=SetMaxDigits, default=1};
  __property bool ShowLeadingZeroes = {read=GetShowLeadingZeroes, write=SetShowLeadingZeroes, default=false};
  __property double DigitScale = {read=GetDigitScale, write=SetDigitScale, default=1};
};
//---------------------------------------------------------------------------
#endif
