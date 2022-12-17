//---------------------------------------------------------------------------
#ifndef CmdStatsInfoH
#define CmdStatsInfoH
#include <stdlib.h>
//---------------------------------------------------------------------------
struct TCmdInfo {
  long Count;
  long CommandBytes;
  long ReplyBytes;
};
//---------------------------------------------------------------------------
class TCmdStatsInfo : public TObject
{
private:
        TStringList *Commands;

protected:
        void __fastcall Clear();
        void __fastcall ResetStats(int i);
        int __fastcall GetCmdIndex(AnsiString Cmd);

public:
        __fastcall TCmdStatsInfo();
        __fastcall ~TCmdStatsInfo();

        void __fastcall AddCommand(AnsiString Cmd);
        void __fastcall RemoveCommand(AnsiString Cmd);
        void __fastcall Update(AnsiString Cmd, long CmdBytes, long RepBytes);
        void __fastcall ResetStats();
        void __fastcall ResetStats(AnsiString Cmd);

        long __fastcall GetCount(AnsiString Cmd);
        long __fastcall GetCmdSize(AnsiString Cmd);
        long __fastcall GetReplySize(AnsiString Cmd);
};
//---------------------------------------------------------------------------
#endif
