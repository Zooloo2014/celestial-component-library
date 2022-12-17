//---------------------------------------------------------------------------
#ifndef FrmHostListH
#define FrmHostListH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>
#include <CheckLst.hpp>
#include "HostList.h"
//---------------------------------------------------------------------------
class TFormHostList : public TForm
{
__published:	// IDE-managed Components
        TButton *BtnCancel;
        TCheckListBox *CheckListBox;
        TLabel *Label1;
        TButton *BtnEdit;
        TButton *BtnApply;
        TButton *BtnAdd;
        TButton *BtnRemove;
        void __fastcall BtnCancelClick(TObject *Sender);
        void __fastcall BtnApplyClick(TObject *Sender);
        void __fastcall CheckListBoxMouseUp(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
        void __fastcall CheckListBoxClickCheck(TObject *Sender);
        void __fastcall BtnEditClick(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall BtnAddClick(TObject *Sender);
        void __fastcall BtnRemoveClick(TObject *Sender);
private:	// User declarations
        int LastClick;
        int HostCount;
        THostList *Source;
        TRemoteHostInfoPtr MyHostList;

public:		// User declarations
        __fastcall TFormHostList(TComponent* Owner);

        void __fastcall SetMyHostList(THostList *src, TRemoteHostInfoPtr hl, int Count);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormHostList *FormHostList;
//---------------------------------------------------------------------------
#endif
