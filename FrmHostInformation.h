//---------------------------------------------------------------------------
#ifndef FrmHostInformationH
#define FrmHostInformationH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>
#include <CheckLst.hpp>
//---------------------------------------------------------------------------
class TFormHostInformation : public TForm
{
__published:	// IDE-managed Components
        TButton *BtnAdd;
        TButton *BtnRemove;
        TButton *BtnClose;
        TCheckListBox *CheckListBox;
        TLabel *Label1;
        TButton *BtnEdit;
        void __fastcall BtnCloseClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TFormHostInformation(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormHostInformation *FormHostInformation;
//---------------------------------------------------------------------------
#endif
