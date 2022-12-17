//---------------------------------------------------------------------------
#ifndef DataSizeH
#define DataSizeH
//---------------------------------------------------------------------------
#include <stdlib.h>
#include <SysUtils.hpp>
#include <Controls.hpp>
#include <Classes.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
enum __TLabelTypes {LT_SHORT, LT_LONG};
typedef __TLabelTypes TLabelTypes;
enum __TScaleTypes {ST_BYTE, ST_KILO, ST_MEGA, ST_GIGA, ST_TERA};
typedef __TScaleTypes TScaleTypes;
//---------------------------------------------------------------------------
class PACKAGE TDataSize : public TComponent
{
private:
        double FValue;
        AnsiString *FText;

        bool FShowLabels;
        bool FAutoScale;
        bool FTrailingZeroes;
        bool FDelimitThousands;
        TScaleTypes FScale;
        int FPrecision;
        TLabelTypes FLabelType;

        double ValBytes;
        double ValKilos;
        double ValMegs;
        double ValGigs;
        double ValTeras;
        AnsiString *TxVal;
        AnsiString *Label;

protected:
        void __fastcall SetValue(double val);
        AnsiString __fastcall GetText();
        void __fastcall SetShowLabels(bool l);
        void __fastcall SetScale(TScaleTypes s);
        void __fastcall SetPrecision(int p);
        void __fastcall SetAutoScale(bool asu);
        void __fastcall SetTrailingZeroes(bool tz);
        void __fastcall SetLabelType(TLabelTypes lt);

        void __fastcall Convert();

public:
        __fastcall TDataSize(TComponent* Owner);
        __fastcall TDataSize::~TDataSize();

        void __fastcall Clear();

__published:
        __property double Value = {read=FValue, write=SetValue, default=0};
        __property AnsiString *Text = {read=GetText};

        __property bool ShowLabels = {read=FShowLabels, write=SetShowLabels, default=true};
        __property bool AutoScale = {read=FAutoScale, write=SetAutoScale, default=false};
        __property bool TrailingZeroes = {read=FTrailingZeroes, write=SetTrailingZeroes, default=false};
        __property bool DelimitThousands = {read=FDelimitThousands, write=FDelimitThousands, default=true};
        __property TScaleTypes Scale = {read=FScale, write=SetScale, default=ST_BYTE};
        __property int Precision = {read=FPrecision, write=SetPrecision, default=1};
        __property TLabelTypes LabelType = {read=FLabelType, write=SetLabelType, default=LT_SHORT};
};
//---------------------------------------------------------------------------
#endif
