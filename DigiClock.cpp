//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "DigiClock.h"
#include <math.h>
#pragma link "Lcd"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//

static inline void ValidCtrCheck(TDigiClock *)
{
        new TDigiClock(NULL);
}
//---------------------------------------------------------------------------
__fastcall TDigiClock::TDigiClock(TComponent* Owner)
        : TLcd(Owner)
{
  Visible = false;

  ClockBuf = new TPicture();
  ClockBuf->Bitmap = new Graphics::TBitmap();

  FBlinkOn = true;
  FDotBlink = false;

  FEnabled = true;
  FBlink = false;
  FInternational = true;
  FShowSeconds = true;
  FSignalOnly = true;
  FCountDown = false;
  FBlinkSeparators = false;

  FCountDownHours = 0;
  FCountDownMinutes = 0;
  FCountDownSeconds = 0;
  SetStageTime(FCountDownHours, FCountDownMinutes, FCountDownSeconds);

  SetShowLeadingZeroes(true);
  SetMaxDigits(2);

  Width = 20 + (MAX_WIDTH * 6) + (TIMEGAP * 2);

  SetDate(Now());
  SetTime(Now());
  SetDisplayTime();

  Visible = true;
}
//---------------------------------------------------------------------------
__fastcall TDigiClock::~TDigiClock(void)
{
  delete ClockBuf;
}
//---------------------------------------------------------------------------
//namespace DigiClock
//{
//        void __fastcall PACKAGE Register()
//        {
//                 TComponentClass classes[1] = {__classid(TDigiClock)};
//                 RegisterComponents("Celestial", classes, 0);
//        }
//}
//---------------------------------------------------------------------------
TDateTime __fastcall TDigiClock::GetDate() {
  return floor(FDateTime);
}
//---------------------------------------------------------------------------
void __fastcall TDigiClock::SetDate(TDateTime ADate) {
  FDateTime = ADate + fmod(FDateTime, floor(FDateTime));
  if (Visible) Paint();
}
//---------------------------------------------------------------------------
TTime __fastcall TDigiClock::GetTime() {
  return (FDateTime - floor(FDateTime));
}
//---------------------------------------------------------------------------
void __fastcall TDigiClock::SetTime(TTime ATime) {
  FDateTime = floor(FDateTime) + (double)(ATime - floor(ATime));
  if (!FCountDown)
    SyncTime();
  if (Visible) Paint();
}
//---------------------------------------------------------------------------
bool __fastcall TDigiClock::GetShowSeconds() {
  return FShowSeconds;
}
//---------------------------------------------------------------------------
void __fastcall TDigiClock::SetShowSeconds(bool AValue) {
  FShowSeconds = AValue;
  if (Visible) Paint();
}
//---------------------------------------------------------------------------
bool __fastcall TDigiClock::GetInternational() {
  return FInternational;
}
//---------------------------------------------------------------------------
void __fastcall TDigiClock::SetInternational(bool AValue) {
  FInternational = AValue;
  if (Visible) Paint();
}
//---------------------------------------------------------------------------
unsigned int __fastcall TDigiClock::GetCountDownHours(void) {
  return FCountDownHours;
}
//---------------------------------------------------------------------------
void __fastcall TDigiClock::SetCountDownHours(unsigned int AValue) {
  FCountDownHours = AValue;
  if (FCountDown)
    SetStageTime(FCountDownHours, FCountDownMinutes, FCountDownSeconds);
  if (Visible) Paint();
}
//---------------------------------------------------------------------------
unsigned int __fastcall TDigiClock::GetCountDownMinutes(void) {
  return FCountDownMinutes;
}
//---------------------------------------------------------------------------
void __fastcall TDigiClock::SetCountDownMinutes(unsigned int AValue) {
  FCountDownMinutes = AValue;
  if (FCountDown)
    SetStageTime(FCountDownHours, FCountDownMinutes, FCountDownSeconds);
  if (Visible) Paint();
}
//---------------------------------------------------------------------------
unsigned int __fastcall TDigiClock::GetCountDownSeconds(void) {
  return FCountDownSeconds;
}
//---------------------------------------------------------------------------
void __fastcall TDigiClock::SetCountDownSeconds(unsigned int AValue) {
  FCountDownSeconds = AValue;
  if (FCountDown)
    SetStageTime(FCountDownHours, FCountDownMinutes, FCountDownSeconds);
  if (Visible) Paint();
}
//---------------------------------------------------------------------------
bool __fastcall TDigiClock::GetCountDown() {
  return FCountDown;
}
//---------------------------------------------------------------------------
void __fastcall TDigiClock::SetCountDown(bool AValue) {
  FCountDown = AValue;
  if (FCountDown)
    SetStageTime(FCountDownHours, FCountDownMinutes, FCountDownSeconds);
  if (Visible) Paint();
}
//---------------------------------------------------------------------------
bool __fastcall TDigiClock::GetSignalOnly() {
  return FSignalOnly;
}
//---------------------------------------------------------------------------
void __fastcall TDigiClock::SetSignalOnly(bool AValue) {
  FSignalOnly = AValue;
  if (FSignalOnly)
    SetTime(Now());
  if (Visible) Paint();
}
//---------------------------------------------------------------------------
bool __fastcall TDigiClock::GetEnabled() {
  return FEnabled;
}
//---------------------------------------------------------------------------
void __fastcall TDigiClock::SetEnabled(bool AValue) {
  FEnabled = AValue;
  if (Visible) Paint();
}
//---------------------------------------------------------------------------
bool __fastcall TDigiClock::GetBlink() {
  return FBlink;
}
//---------------------------------------------------------------------------
void __fastcall TDigiClock::SetBlink(bool AValue) {
  FBlink = AValue;
  FBlinkOn = true;
}
//---------------------------------------------------------------------------
void __fastcall TDigiClock::SyncTime(void) {
  unsigned short Hours = 0;
  unsigned short Mins = 0;
  unsigned short Secs = 0;
  unsigned short Msec = 0;

  DecodeTime(FDateTime, Hours, Mins, Secs, Msec);
  SetStageTime(Hours, Mins, Secs);
}
//---------------------------------------------------------------------------
void __fastcall TDigiClock::Update(void) {
  if (FEnabled) {

    if (FBlinkSeparators)
      FDotBlink = !FDotBlink;
    else
      FDotBlink = true;

    if (FBlink)
      FBlinkOn = !(FBlinkOn);
    else
      FBlinkOn = true;

    if (FCountDown) {

      if ((FStageSecs < 0) || (FStageMins < 0) || (FStageHours < 0)) {
        FStageSecs = 0;
        FStageMins = 0;
        FStageHours = 0;
      }

      if ((FStageSecs == 0) && (FStageMins == 0) && (FStageHours == 0)) {
        FBlink = true;
      } else {
        FStageSecs--;
        if (FStageSecs < 0) {
          if (FStageHours > -1) {
            FStageSecs = 59;
            FStageMins--;
          }
        }

        if (FStageMins < 0) {
          if (FStageHours > -1) {
            FStageMins = 59;
            FStageHours--;
          }
        }
      }

    } else {

      if (FSignalOnly) {

        SetTime(Now());

      } else {

        FStageSecs++;
        if (FStageSecs > 59) {
          FStageSecs = 0;
          FStageMins++;
        }

        if (FStageMins > 59) {
          FStageMins = 0;
          FStageHours++;
        }

        if (FStageHours > 23)
          FStageHours = 0;

        SetTime(GetStageTime());

      }
    }
  }

  Paint();
}
//---------------------------------------------------------------------------
void __fastcall TDigiClock::SetDisplayTime(void) {
  FDispHours = FStageHours;

  if ((!International) && (!CountDown))
    if (FStageHours > 12)
      FDispHours = FStageHours - 12;

  FDispMins = FStageMins;
  FDispSecs = FStageSecs;
  FDispMsec = 0;
}
//---------------------------------------------------------------------------
TDateTime __fastcall TDigiClock::GetDisplayTime() {
  unsigned short Hours = (unsigned short)FDispHours;
  unsigned short Mins = (unsigned short)FDispMins;
  unsigned short Secs = (unsigned short)FDispSecs;

  return (EncodeTime(Hours, Mins, Secs, 0));
}
//---------------------------------------------------------------------------
void __fastcall TDigiClock::SetStageTime(int Hours, int Mins, int Secs) {
  FStageHours = Hours;
  FStageMins = Mins;
  FStageSecs = Secs;
}
//---------------------------------------------------------------------------
TDateTime __fastcall TDigiClock::GetStageTime() {
  unsigned short Hours = (unsigned short)FStageHours;
  unsigned short Mins = (unsigned short)FStageMins;
  unsigned short Secs = (unsigned short)FStageSecs;

  return (EncodeTime(Hours, Mins, Secs, 0));
}
//---------------------------------------------------------------------------
bool __fastcall TDigiClock::GetBlinkSeparators(void) {
  return FBlinkSeparators;
}
//---------------------------------------------------------------------------
void __fastcall TDigiClock::SetBlinkSeparators(bool AValue) {
  FBlinkSeparators = AValue;
}
//---------------------------------------------------------------------------
void __fastcall TDigiClock::DrawClock(void) {
  TRect DrawPos = ClientRect;

  ClockBuf->Bitmap->Width = Width;
  ClockBuf->Bitmap->Height = Height;
  ClockBuf->Bitmap->Canvas->Brush->Color = clBlack;
  ClockBuf->Bitmap->Canvas->FillRect(ClientRect);

  TColor dbc;
  if (FDotBlink)
    dbc = GetColor();
  else
    dbc = GetOffColor();

  DrawPos.Left = 10;

  int GroupWide = (int)((MAX_WIDTH * 2) * GetDigitScale());
  int GroupGap = (int)(TIMEGAP * GetDigitScale());

  DrawDisplay(ClockBuf->Bitmap->Canvas, DrawPos, FDispHours, FBlinkOn);
  DrawPos.Left += (GroupWide + GroupGap);
  DrawPos.Right += (GroupWide + GroupGap);

  double ds = GetDigitScale();
  DrawDots(DrawPos.Left - (GroupGap >> 1), DrawPos.Top + (int)(ds * 20), dbc);
  DrawDots(DrawPos.Left - (GroupGap >> 1), DrawPos.Top + (int)(ds * 40), dbc);
  DrawDisplay(ClockBuf->Bitmap->Canvas, DrawPos, FDispMins, FBlinkOn);

  if (FShowSeconds) {
    DrawPos.Left += (GroupWide + GroupGap);
    DrawPos.Right += (GroupWide + GroupGap);

    DrawDots(DrawPos.Left - (GroupGap >> 1), DrawPos.Top + (int)(ds * 20), dbc);
    DrawDots(DrawPos.Left - (GroupGap >> 1), DrawPos.Top + (int)(ds * 40), dbc);
    DrawDisplay(ClockBuf->Bitmap->Canvas, DrawPos, FDispSecs, FBlinkOn);
  }

  Canvas->CopyRect(ClientRect, ClockBuf->Bitmap->Canvas, ClientRect);
}
//---------------------------------------------------------------------------
void __fastcall TDigiClock::Paint(void) {
  FBlinkOn = ((FEnabled) && (FBlinkOn));

  if (Visible) {
    SetDisplayTime();
    DrawClock();
  }
}
//---------------------------------------------------------------------------
void __fastcall TDigiClock::DrawDots(int X, int Y, TColor c) {
  TPoint p[6];
  double s = GetDigitScale();
  int offs = (int)(s * 6);

  p[0].x = X;
  p[0].y = Y;
  p[1].x = p[0].x - offs;
  p[1].y = p[0].y;
  p[2].x = p[1].x + offs;
  p[2].y = p[1].y + offs;
  p[3].x = p[2].x + offs;
  p[3].y = p[2].y - offs;
  p[4].x = p[3].x - offs;
  p[4].y = p[3].y - offs;
  p[5].x = p[4].x - offs;
  p[5].y = p[4].y + offs;
  ClockBuf->Bitmap->Canvas->Pen->Color = c;
  ClockBuf->Bitmap->Canvas->Brush->Color = c;
  ClockBuf->Bitmap->Canvas->Polyline(p, 5);
  ClockBuf->Bitmap->Canvas->FloodFill(p[0].x + 1, p[0].y, c, fsBorder);
}
//---------------------------------------------------------------------------

