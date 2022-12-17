#ifndef LMTSTypesH
#define LMTSTypesH
#include "MailHeaders.h"
//---------------------------------------------------------------------------
#define BUF_HEADER              (8 * 1024)
#define PATH_BUFSIZE            (MAX_PATH * 2)
#define MAX_LOG_DETAIL_SIZE     512
//---------------------------------------------------------------------------
#define P3A_LOG_NONE            0
#define P3A_LOG_CONNECTS        1
#define P3A_LOG_DETAIL          2
//---------------------------------------------------------------------------
#define POP3_LOG_NONE           0
#define POP3_LOG_CONNECTS       1
#define POP3_LOG_DETAIL         2
//---------------------------------------------------------------------------
#define SMTP_LOG_NONE           0
#define SMTP_LOG_CONNECTS       1
#define SMTP_LOG_DETAIL         2
//---------------------------------------------------------------------------
#define SMA_LOG_NONE            0
#define SMA_LOG_CONNECTS        1
#define SMA_LOG_DETAIL          2
//---------------------------------------------------------------------------
#define P3RO_DEFAULT            0
#define P3RO_SMALLEST_FIRST     1
//---------------------------------------------------------------------------
typedef void __fastcall (__closure *TMessageNotifyEvent)(System::TObject* Sender, int MessageID, long MessageSize);
typedef void __fastcall (__closure *TPeekerHeaderEvent)(System::TObject* Sender, int MessageID, TMailHeaders *Headers, bool &DelMsg);
typedef void __fastcall (__closure *TPeekerEmailEvent)(System::TObject* Sender, int MessageID, AnsiString Name, AnsiString Email, bool &DelMsg);
typedef void __fastcall (__closure *TPeekerSizeEvent)(System::TObject* Sender, int MessageID, long MessageSize, bool &DelMsg);
typedef void __fastcall (__closure *TPeekerDateEvent)(System::TObject* Sender, int MessageID, TDateTime MessageDate, bool &DelMsg);
typedef void __fastcall (__closure *TMessageChunkNotifyEvent)(System::TObject* Sender, int MessageID, double Bytes, double Percent);
typedef void __fastcall (__closure *TMessageNameNotifyEvent)(System::TObject* Sender, int MessageID, long MessageSize, AnsiString MsgName, bool &DontSave);
typedef void __fastcall (__closure *TMessageSaveNotifyEvent)(System::TObject* Sender, int MessageID, long MessageSize, AnsiString MsgName, TMemoryStream *Message);
typedef void __fastcall (__closure *TLogNotifyEvent)(System::TObject* Sender, const AnsiString le);
typedef void __fastcall (__closure *TClientNotifyEvent)(System::TObject* Sender, AnsiString ClientName, AnsiString ClientAddr, int ClientPort);
typedef void __fastcall (__closure *TUserValidationEvent)(System::TObject* Sender, AnsiString UID, bool &Valid);
typedef void __fastcall (__closure *TPassValidationEvent)(System::TObject* Sender, AnsiString UID, AnsiString Password, bool &Valid);
typedef void __fastcall (__closure *THostValidationEvent)(System::TObject* Sender, AnsiString Host, bool &Valid);
typedef void __fastcall (__closure *TMessageBeforeSaveEvent)(System::TObject* Sender, AnsiString Filename, long SpaceReqd, bool &Cancel);
typedef void __fastcall (__closure *TMessageAfterSaveEvent)(System::TObject* Sender, const AnsiString Filename);
typedef void __fastcall (__closure *TValidateAddressNotifyEvent)(System::TObject* Sender, AnsiString EmailAddr, bool &Valid);
typedef void __fastcall (__closure *TValidateMaxRecipients)(System::TObject* Sender, long Recipients, bool &Valid);
typedef void __fastcall (__closure *TValidateMaxMsgSize)(System::TObject* Sender, long MsgSize, bool &Valid);
//---------------------------------------------------------------------------
enum TPop3ServerState {P3SS_AUTH, P3SS_TRANS, P3SS_UPDATE, P3SS_FINAL};
//---------------------------------------------------------------------------
enum TSmtpServerState {SMSS_INIT, SMSS_ROUTE, SMSS_TRANS, SMSS_FINAL};
//---------------------------------------------------------------------------
enum __TExpectType {EXP_LINE, EXP_LIST, EXP_MSG};
typedef enum __TExpectType TExpectType;
//---------------------------------------------------------------------------
struct TMsgInfo {
  int Number;
  long Size;    // in Bytes...
  bool Retrieved;
  bool Deleted;
};
//---------------------------------------------------------------------------
#endif

