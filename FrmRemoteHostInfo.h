//---------------------------------------------------------------------------
#ifndef FrmRemoteHostInfoH
#define FrmRemoteHostInfoH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <CheckLst.hpp>
#include <ExtCtrls.hpp>
#include "HostAccountInfo.h"
#include "RemoteHostInfo.h"
//---------------------------------------------------------------------------
class TFormRemoteHostInfo : public TForm
{
__published:	// IDE-managed Components
        TLabel *Label1;
        TEdit *EditService;
        TCheckListBox *CheckListBox;
        TLabel *Label2;
        TLabel *Label3;
        TLabel *LabelHost;
        TEdit *EditPort;
        TLabel *Label4;
        TBevel *Bevel1;
        TButton *BtnEdit;
        TButton *BtnApply;
        TLabel *LabelCount;
        TButton *BtnCancel;
        void __fastcall BtnApplyClick(TObject *Sender);
        void __fastcall BtnEditClick(TObject *Sender);
        void __fastcall BtnCancelClick(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall CheckListBoxMouseUp(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
        void __fastcall CheckListBoxClickCheck(TObject *Sender);
private:	// User declarations
        TRemoteHostInfo *MyHost;
        THostAccountInfo *MyAccounts;
        int LastClick;

        void __fastcall UpdateAccounts();

public:		// User declarations
        __fastcall TFormRemoteHostInfo(TComponent* Owner);

        void __fastcall SetMyHost(TRemoteHostInfo *host);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormRemoteHostInfo *FormRemoteHostInfo;
//---------------------------------------------------------------------------
#endif
