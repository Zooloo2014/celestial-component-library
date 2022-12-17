//---------------------------------------------------------------------------
#ifndef HistoryComboBoxH
#define HistoryComboBoxH
//---------------------------------------------------------------------------
#include <stdlib.h>
#include <SysUtils.hpp>
#include <Controls.hpp>
#include <Classes.hpp>
#include <Forms.hpp>
#include <StdCtrls.hpp>
//---------------------------------------------------------------------------
class PACKAGE THistoryComboBox : public TComboBox
{
private:
  int FHistorySize;

  void __fastcall UpdateHistory(char Key);  // Centralised action...
protected:
  int __fastcall GetHistorySize();         // read method
  void __fastcall SetHistorySize(int ACount);
  void __fastcall KeyPress(char &Key);
public:
       __fastcall THistoryComboBox(TComponent* Owner);
__published:
  __property int HistorySize = {read=GetHistorySize, write=SetHistorySize, default = 8};
};
//---------------------------------------------------------------------------
#endif
