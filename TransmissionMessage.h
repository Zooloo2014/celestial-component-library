//---------------------------------------------------------------------------
#ifndef TransmissionMessageH
#define TransmissionMessageH
#include <stdlib.h>
//---------------------------------------------------------------------------
class TTransmissionMessage : public TObject
{
private:
        AnsiString *Msg;
        AnsiString *FromHostAddr;
        AnsiString *FromHostName;

        TDateTime Created;
        TDateTime Modified;

protected:
public:
        __fastcall TTransmissionMessage();
        __fastcall ~TTransmissionMessage();

        void __fastcall SetMessage(TStringList *m);
        void __fastcall SetMessage(AnsiString m);
        AnsiString * __fastcall GetFromHostAddr();
        void __fastcall SetFromHostAddr(AnsiString fh);
        AnsiString * __fastcall GetFromHostName();
        void __fastcall SetFromHostName(AnsiString fn);

        int  __fastcall GetMessageLines();
        int  __fastcall GetMessageSize();
        TDateTime  __fastcall GetMessageCreated();
        TDateTime  __fastcall GetMessageModified();
};
//---------------------------------------------------------------------------
#endif
