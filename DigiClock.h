//---------------------------------------------------------------------------
#ifndef DigiClockH
#define DigiClockH
//---------------------------------------------------------------------------
#define BIN2VAL(a, b, c, d, e, f, g, h) (h + (g * 2) + (f * 4) + (e * 8) + (d * 16) + (c * 32) + (b * 64) + (a * 128))
#define TIMEGAP 24
//---------------------------------------------------------------------------
#include <stdlib.h>
#include <SysUtils.hpp>
#include <Controls.hpp>
#include <Classes.hpp>
#include <Forms.hpp>
#include "Lcd.h"
//---------------------------------------------------------------------------
class PACKAGE TDigiClock : public TLcd
{
private:
  TPicture *ClockBuf;
  TDateTime FDateTime;
  bool FShowSeconds;
  bool FSignalOnly;
  bool FInternational;
  bool FCountDown;
  bool FEnabled;
  bool FBlinkSeparators;
  bool FBlink;
  bool FBlinkOn;
  bool FDotBlink;

  unsigned int FCountDownHours;
  unsigned int FCountDownMinutes;
  unsigned int FCountDownSeconds;

  int FStageHours;
  int FStageMins;
  int FStageSecs;

  int FDispHours;
  int FDispMins;
  int FDispSecs;
  int FDispMsec;

  void __fastcall Paint(void);
  void __fastcall DrawClock(void);
  void __fastcall DrawDots(int X, int Y, TColor c);

  TDateTime __fastcall GetStageTime();
  void __fastcall SetStageTime(int Hours, int Mins, int Secs);
  TDateTime __fastcall GetDisplayTime();
  void __fastcall SetDisplayTime(void);
protected:
  TDateTime __fastcall GetDate();
  void __fastcall SetDate(TDateTime ADate);
  TTime __fastcall GetTime();
  void __fastcall SetTime(TTime ATime);
  bool __fastcall GetShowSeconds();
  void __fastcall SetShowSeconds(bool AValue);
  bool __fastcall GetInternational();
  void __fastcall SetInternational(bool AValue);
  bool __fastcall GetCountDown();
  void __fastcall SetCountDown(bool AValue);
  bool __fastcall GetSignalOnly();
  void __fastcall SetSignalOnly(bool AValue);
  bool __fastcall GetEnabled();
  void __fastcall SetEnabled(bool AValue);
  bool __fastcall GetBlink();
  void __fastcall SetBlink(bool AValue);

  unsigned int __fastcall GetCountDownHours(void);
  void __fastcall SetCountDownHours(unsigned int AValue);
  unsigned int __fastcall GetCountDownMinutes(void);
  void __fastcall SetCountDownMinutes(unsigned int AValue);
  unsigned int __fastcall GetCountDownSeconds(void);
  void __fastcall SetCountDownSeconds(unsigned int AValue);
  bool __fastcall GetBlinkSeparators(void);
  void __fastcall SetBlinkSeparators(bool AValue);

        void __fastcall SyncTime(void);
public:
        __fastcall TDigiClock(TComponent* Owner);
        __fastcall ~TDigiClock(void);

        void __fastcall Update(void);
__published:
  __property TDateTime Date = {read=GetDate, write=SetDate};
  __property TTime Time = {read=GetTime, write=SetTime};
  __property bool ShowSeconds = {read=GetShowSeconds, write=SetShowSeconds, default=true, stored=true};
  __property bool International = {read=GetInternational, write=SetInternational, default=true, stored=true};
  __property bool CountDown = {read=GetCountDown, write=SetCountDown, default=false, stored=true};
  __property bool SignalOnly = {read=GetSignalOnly, write=SetSignalOnly, default=true, stored=true};
  __property bool Enabled = {read=GetEnabled, write=SetEnabled, default=true, stored=true};
  __property bool Blink = {read=GetBlink, write=SetBlink, default=false, stored=true};
  __property unsigned int CountDownHours = {read=GetCountDownHours, write=SetCountDownHours, default=0, stored=true};
  __property unsigned int CountDownMinutes = {read=GetCountDownMinutes, write=SetCountDownMinutes, default=0, stored=true};
  __property unsigned int CountDownSeconds = {read=GetCountDownSeconds, write=SetCountDownSeconds, default=0, stored=true};
  __property bool BlinkSeparators = {read=GetBlinkSeparators, write=SetBlinkSeparators, default=false, stored=true};
};
//---------------------------------------------------------------------------
#endif
