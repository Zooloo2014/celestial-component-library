//---------------------------------------------------------------------------
#ifndef DotMatrixH
#define DotMatrixH
//---------------------------------------------------------------------------
#define BIN2VAL(a, b, c, d, e, f, g, h) (h + (g * 2) + (f * 4) + (e * 8) + (d * 16) + (c * 32) + (b * 64) + (a * 128))
//---------------------------------------------------------------------------
#include <stdlib.h>
#include <SysUtils.hpp>
#include <Controls.hpp>
#include <Classes.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class PACKAGE TDotMatrix : public TGraphicControl
{
private:
  TColor FColor;
  TColor FElementColor;
  TColor FBackgroundColor;
  int FCharIndex;    // First character from FText to show
  int FDisplayWidth;   // In characters
  AnsiString FText;

  bool FStretch;
  bool FSmoothScrolling;
  int FElementWidth;
  int FElementHeight;
  int FHorizontalGap;
  int FVerticalGap;

  TPicture *Buffer;
  int FScrollOffset;
  int FSmallScroll;
  int FMatrixWidth;    // Width of matrix in blocks
  int FMatrixHeight;   // Height of matrix in blocks
  unsigned char Lookup[256][7];

  void __fastcall Paint(void);
protected:
  TColor __fastcall GetColor();
  void __fastcall SetColor(TColor AColor);
  TColor __fastcall GetElementColor();
  void __fastcall SetElementColor(TColor AColor);
  TColor __fastcall GetBackgroundColor();
  void __fastcall SetBackgroundColor(TColor AColor);
  AnsiString __fastcall GetText();
  void __fastcall SetText(AnsiString AValue);
  int __fastcall GetDisplayWidth();
  void __fastcall SetDisplayWidth(int AValue);
  int __fastcall GetElementWidth();
  void __fastcall SetElementWidth(int AValue);
  int __fastcall GetElementHeight();
  void __fastcall SetElementHeight(int AValue);
  int __fastcall GetHorizontalGap();
  void __fastcall SetHorizontalGap(int AValue);
  int __fastcall GetVerticalGap();
  void __fastcall SetVerticalGap(int AValue);
  bool __fastcall GetStretch();
  void __fastcall SetStretch(bool AValue);
  bool __fastcall GetSmoothScrolling();
  void __fastcall SetSmoothScrolling(bool AValue);
public:
        __fastcall TDotMatrix(TComponent* Owner);
        __fastcall ~TDotMatrix(void);
  void __fastcall Scroll(void);
  void __fastcall ScrollReset(void);
  void __fastcall AppendChar(unsigned char c);
  int __fastcall GetStartingIndex(void);
__published:
  __property TColor Color = {read=GetColor, write=SetColor, default=clRed};
  __property TColor ElementColor = {read=GetElementColor, write=SetElementColor, default=clGray};
  __property TColor BackgroundColor = {read=GetBackgroundColor, write=SetBackgroundColor, default=clBlack};
  __property int DisplayWidth = {read=GetDisplayWidth, write=SetDisplayWidth, default=1};
  __property int ElementWidth = {read=GetElementWidth, write=SetElementWidth, default=4};
  __property int ElementHeight = {read=GetElementHeight, write=SetElementHeight, default=4};
  __property int HorizontalGap = {read=GetHorizontalGap, write=SetHorizontalGap, default=1};
  __property int VerticalGap = {read=GetVerticalGap, write=SetVerticalGap, default=1};
  __property bool Stretch = {read=GetStretch, write=SetStretch, default=False};
  __property bool SmoothScrolling = {read=GetSmoothScrolling, write=SetSmoothScrolling, default=False};
  __property AnsiString Text = {read=GetText, write=SetText};
};
//---------------------------------------------------------------------------
#endif
