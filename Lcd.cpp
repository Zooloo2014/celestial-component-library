//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "Lcd.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//

static inline void ValidCtrCheck(TLcd *)
{
        new TLcd(NULL);
}
//---------------------------------------------------------------------------
__fastcall TLcd::TLcd(TComponent* Owner)
        : TGraphicControl(Owner)
{
  Visible = false;

  Buffer = new TPicture();
  Buffer->Bitmap = new Graphics::TBitmap();

  FDigitScale = 1.0;
  FShowLeadingZeroes = false;

  FBackgroundColor = clBlack;
  FOffColor = clDkGray;
  FColor = clRed;

  Height = MAX_HEIGHT;
  MaxDigits = GetNumDigits(FValue);

  SegLit[0] = BIN2VAL(1,1,1,1,1,1,0,0);
  SegLit[1] = BIN2VAL(0,0,0,0,1,1,0,0);
  SegLit[2] = BIN2VAL(1,0,1,1,0,1,1,0);
  SegLit[3] = BIN2VAL(1,0,0,1,1,1,1,0);
  SegLit[4] = BIN2VAL(0,1,0,0,1,1,1,0);
  SegLit[5] = BIN2VAL(1,1,0,1,1,0,1,0);
  SegLit[6] = BIN2VAL(1,1,1,1,1,0,1,0);
  SegLit[7] = BIN2VAL(1,0,0,0,1,1,0,0);
  SegLit[8] = BIN2VAL(1,1,1,1,1,1,1,0);
  SegLit[9] = BIN2VAL(1,1,0,1,1,1,1,0);

  Visible = true;
}
//---------------------------------------------------------------------------
__fastcall TLcd::~TLcd(void) {
  delete Buffer;
}
//---------------------------------------------------------------------------
//namespace Lcd
//{
//        void __fastcall PACKAGE Register()
//        {
//                 TComponentClass classes[1] = {__classid(TLcd)};
//                 RegisterComponents("Celestial", classes, 0);
//        }
//}
//---------------------------------------------------------------------------
TColor __fastcall TLcd::GetColor() {
  return FColor;
}
//---------------------------------------------------------------------------
void __fastcall TLcd::SetColor(TColor AColor) {
  FColor = AColor;
  if (Visible) Update();
}
//---------------------------------------------------------------------------
TColor __fastcall TLcd::GetOffColor() {
  return FOffColor;
}
//---------------------------------------------------------------------------
void __fastcall TLcd::SetOffColor(TColor AColor) {
  FOffColor = AColor;
  if (Visible) Update();
}
//---------------------------------------------------------------------------
TColor __fastcall TLcd::GetBackgroundColor() {
  return FBackgroundColor;
}
//---------------------------------------------------------------------------
void __fastcall TLcd::SetBackgroundColor(TColor AColor) {
  FBackgroundColor = AColor;
  if (Visible) Update();
}
//---------------------------------------------------------------------------
int __fastcall TLcd::GetValue() {
  return FValue;
}
//---------------------------------------------------------------------------
void __fastcall TLcd::SetValue(int AValue) {
  if (AValue > -1) {
    FValue = AValue;
    if (Visible) Update();
  } else
    throw "Error: Value must be greater than 0!";
}
//---------------------------------------------------------------------------
int __fastcall TLcd::GetMaxDigits() {
  return FMaxDigits;
}
//---------------------------------------------------------------------------
void __fastcall TLcd::SetMaxDigits(int AValue) {
  FMaxDigits = AValue;
//  Width = (MAX_WIDTH * FMaxDigits) + 3;
  if (Visible) Update();
}
//---------------------------------------------------------------------------
bool __fastcall TLcd::GetShowLeadingZeroes() {
  return FShowLeadingZeroes;
}
//---------------------------------------------------------------------------
void __fastcall TLcd::SetShowLeadingZeroes(bool AValue) {
  FShowLeadingZeroes = AValue;
  if (Visible) Update();
}
//---------------------------------------------------------------------------
double __fastcall TLcd::GetDigitScale() {
  return FDigitScale;
}
//---------------------------------------------------------------------------
void __fastcall TLcd::SetDigitScale(double AValue) {
  FDigitScale = AValue;
  if (Visible) Update();
}
//---------------------------------------------------------------------------
void __fastcall TLcd::DrawDisplay(TCanvas *Canv, TRect CArea, unsigned int Num, bool On) {
  if (Visible) {
    FArea = CArea;

    Buffer->Bitmap->Width = CArea.Right - CArea.Left;
    Buffer->Bitmap->Height = CArea.Bottom - CArea.Top;

    Buffer->Bitmap->Canvas->Brush->Color = FBackgroundColor;
    Buffer->Bitmap->Canvas->FillRect(FArea);

    FString = Num;
    int si = FString.Length();
    if (FShowLeadingZeroes) {
      if (si < FMaxDigits) {
        FString = AnsiString::StringOfChar('0', FMaxDigits - si) + FString;
        si = FString.Length();
      }
    }

    int HGap = MAX_WIDTH;
//    int VGap = MAX_HEIGHT;
    int AtX = (int)(CArea.Left + (FMaxDigits - 1) * HGap * FDigitScale);
    int AtY = 1;
//    if ((CArea.Bottom - CArea.Top) <= VGap)
//      AtY = 1;
//    else
//      AtY = ((CArea.Bottom - CArea.Top) - VGap) >> 1;

    for (int j = 0; j < FMaxDigits; j++) {
      char t;
      if (si > 0)
        t = FString[si];
      else
        t = 0;

      unsigned char k = SegLit[t - 48];
      unsigned char n = 128;

      for (int i = 1; i <= 7; i++) {
        if (((k & n) == n) && (On)) {
          Buffer->Bitmap->Canvas->Brush->Color = FColor;
          Buffer->Bitmap->Canvas->Pen->Color = FColor;
          FDrawColor = FColor;
        } else {
          Buffer->Bitmap->Canvas->Brush->Color = FOffColor;
          Buffer->Bitmap->Canvas->Pen->Color = FOffColor;
          FDrawColor = FOffColor;
        }

        DrawSegment(AtX, AtY, i);
        n >>= 1;
      }

      AtX -= (int)(HGap * FDigitScale);
      if (si > 0)
        si--;
    }

    Canv->CopyRect(FArea, Buffer->Bitmap->Canvas, FArea);
  }
}
//---------------------------------------------------------------------------
void __fastcall TLcd::DrawSegment(int X, int Y, int Seg) {
  TPoint p[7];
  int StartX = X;
  int StartY = Y;


  // Draw top segment

  if (Seg == 1) {
    StartX += (int)(FDigitScale * 8);
    StartY += (int)(FDigitScale * 6);

    p[0].x = StartX;
    p[0].y = StartY;      
    p[1].x = p[0].x + (int)(FDigitScale * 31);
    p[1].y = p[0].y + 0;
    p[2].x = p[1].x - (int)(FDigitScale * 9);
    p[2].y = p[1].y + (int)(FDigitScale * 9);
    p[3].x = p[2].x - (int)(FDigitScale * 13);
    p[3].y = p[2].y + 0;
    p[4].x = StartX;
    p[4].y = StartY;

    Buffer->Bitmap->Canvas->Polygon(p, 3);
  }


  // Draw upper-left segment

  if (Seg == 2) {
    StartX += (int)(FDigitScale * 6);
    StartY += (int)(FDigitScale * 8);

    p[0].x = StartX;
    p[0].y = StartY;
    p[1].x = p[0].x + 0;
    p[1].y = p[0].y + (int)(FDigitScale * 23);
    p[2].x = p[1].x + (int)(FDigitScale * 9);
    p[2].y = p[1].y - (int)(FDigitScale * 6);
    p[3].x = p[2].x + 0;
    p[3].y = p[2].y - (int)(FDigitScale * 8);
    p[4].x = StartX;
    p[4].y = StartY;

    Buffer->Bitmap->Canvas->Polygon(p, 3);
  }


  // Draw lower-left segment

  if (Seg == 3) {
    StartX += (int)(FDigitScale * 6);
    StartY += (int)(FDigitScale * 34);

    p[0].x = StartX;
    p[0].y = StartY;
    p[1].x = p[0].x + 0;
    p[1].y = p[0].y + (int)(FDigitScale * 20);
    p[2].x = p[1].x + (int)(FDigitScale * 9);
    p[2].y = p[1].y - (int)(FDigitScale * 9);
    p[3].x = p[2].x + 0;
    p[3].y = p[2].y - (int)(FDigitScale * 8);
    p[4].x = StartX;
    p[4].y = StartY;

    Buffer->Bitmap->Canvas->Polygon(p, 3);
  }


  // Draw bottom segment

  if (Seg == 4) {
    StartX += (int)(FDigitScale * 8);
    StartY += (int)(FDigitScale * 56);

    p[0].x = StartX;
    p[0].y = StartY;
    p[1].x = p[0].x + (int)(FDigitScale * 30);
    p[1].y = p[0].y + 0;
    p[2].x = p[1].x - (int)(FDigitScale * 9);
    p[2].y = p[1].y - (int)(FDigitScale * 9);
    p[3].x = p[2].x - (int)(FDigitScale * 14);
    p[3].y = p[2].y + 0;
    p[4].x = StartX;
    p[4].y = StartY;

    Buffer->Bitmap->Canvas->Polygon(p, 3);
  }


  // Draw lower-right segment

  if (Seg == 5) {
    StartX += (int)(FDigitScale * 41);
    StartY += (int)(FDigitScale * 54);

    p[0].x = StartX;
    p[0].y = StartY;
    p[1].x = p[0].x + 0;
    p[1].y = p[0].y - (int)(FDigitScale * 22);
    p[2].x = p[1].x - (int)(FDigitScale * 9);
    p[2].y = p[1].y + (int)(FDigitScale * 6);
    p[3].x = p[2].x + 0;
    p[3].y = p[2].y + (int)(FDigitScale * 8);
    p[4].x = StartX;
    p[4].y = StartY;

    Buffer->Bitmap->Canvas->Polygon(p, 3);
  }


  // Draw upper-right segment

  if (Seg == 6) {
    StartX += (int)(FDigitScale * 41);
    StartY += (int)(FDigitScale * 29);

    p[0].x = StartX;
    p[0].y = StartY;
    p[1].x = p[0].x + 0;
    p[1].y = p[0].y - (int)(FDigitScale * 22);
    p[2].x = p[1].x - (int)(FDigitScale * 9);
    p[2].y = p[1].y + (int)(FDigitScale * 9);
    p[3].x = p[2].x + 0;
    p[3].y = p[2].y + (int)(FDigitScale * 10);
    p[4].x = StartX;
    p[4].y = StartY;

    Buffer->Bitmap->Canvas->Polygon(p, 3);
  }


  // Draw middle segment

  if (Seg == 7) {
    StartX += (int)(FDigitScale * 17);
    StartY += (int)(FDigitScale * 27);

    p[0].x = StartX;
    p[0].y = StartY;
    p[1].x = p[0].x + (int)(FDigitScale * 11);
    p[1].y = p[0].y + 0;
    p[2].x = p[1].x + (int)(FDigitScale * 10);
    p[2].y = p[1].y + (int)(FDigitScale * 4);
    p[3].x = p[2].x - (int)(FDigitScale * 7);
    p[3].y = p[2].y + (int)(FDigitScale * 5);
    p[4].x = p[3].x - (int)(FDigitScale * 14);
    p[4].y = p[3].y + 0;
    p[5].x = p[4].x - (int)(FDigitScale * 8);
    p[5].y = p[4].y - (int)(FDigitScale * 4);
    p[6].x = StartX;
    p[6].y = StartY;

    Buffer->Bitmap->Canvas->Polygon(p, 5);
  }
}
//---------------------------------------------------------------------------
void __fastcall TLcd::Paint(void) {
  if (Visible)
    DrawDisplay(Canvas, GetClientRect(), FValue, true);
}
//---------------------------------------------------------------------------
void __fastcall TLcd::Update(void) {
  Paint();
}
//---------------------------------------------------------------------------
int __fastcall TLcd::GetNumDigits(int num) {
  AnsiString i = num;
  return i.Length();
}
//---------------------------------------------------------------------------

