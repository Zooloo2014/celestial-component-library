//---------------------------------------------------------------------------
#ifndef TransmissionStringsH
#define TransmissionStringsH
//---------------------------------------------------------------------------
#define TRANS_STRING_SUCCESS            "#OK"
#define TRANS_STRING_FAILURE            "#ER"
#define TRANS_STRING_GREETING_MSG       "#OK Transmission Server Ready (v1.0)"
#define TRANS_STRING_BLOCKED_MSG        "#ER Host is blocked"
#define TRANS_STRING_SUCCESS_MSG        "#OK Thanks"
#define TRANS_STRING_FAILURE_MSG        "#ER Error"
//---------------------------------------------------------------------------
typedef void __fastcall (__closure *TLogNotifyEvent)(System::TObject* Sender, const AnsiString le);
typedef void __fastcall (__closure *TProtocolNotifyEvent)(System::TObject* Sender, const AnsiString pr);
//---------------------------------------------------------------------------
#endif

