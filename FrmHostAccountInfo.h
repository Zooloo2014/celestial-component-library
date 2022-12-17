//---------------------------------------------------------------------------
#ifndef FrmHostAccountInfoH
#define FrmHostAccountInfoH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>
//---------------------------------------------------------------------------
class TFormHostAccountInfo : public TForm
{
__published:	// IDE-managed Components
        TButton *BtnOk;
        TButton *BtnCancel;
        TStringGrid *StringGrid;
        TLabel *Label1;
        TLabel *LabelUsrCnt;
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall BtnCancelClick(TObject *Sender);
        void __fastcall BtnOkClick(TObject *Sender);
        void __fastcall StringGridSetEditText(TObject *Sender, int ACol,
          int ARow, const AnsiString Value);
        void __fastcall FormDestroy(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
private:	// User declarations
        TStringList *SortedID;
        TStringList *Pass;
        TStringList *FName;

        int  __fastcall CountUsers();
        void __fastcall SortGridInfo();

public:		// User declarations
        __fastcall TFormHostAccountInfo(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormHostAccountInfo *FormHostAccountInfo;
//---------------------------------------------------------------------------
#endif
