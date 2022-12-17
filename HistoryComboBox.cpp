//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "DotMatrix.h"
#include "Lcd.h"
#include "DigiClock.h"
#include "HistoryComboBox.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//

static inline void ValidCtrCheck(THistoryComboBox *)
{
        new THistoryComboBox(NULL);
}
//---------------------------------------------------------------------------
__fastcall THistoryComboBox::THistoryComboBox(TComponent* Owner)
        : TComboBox(Owner)
{
  if (HistorySize < DropDownCount)
    HistorySize = DropDownCount;
}
//---------------------------------------------------------------------------
namespace Historycombobox
{
        void __fastcall PACKAGE Register()
        {
                 TComponentClass classes[4] = {{__classid(THistoryComboBox)},
                 {__classid(TDotMatrix)},
                 {__classid(TLcd)},
                 {__classid(TDigiClock)}};
                 RegisterComponents("Celestial", classes, 3);
        }
}
//---------------------------------------------------------------------------
int __fastcall THistoryComboBox::GetHistorySize() {
  return FHistorySize;
}
//---------------------------------------------------------------------------

void __fastcall THistoryComboBox::SetHistorySize(int ACount) {
  FHistorySize = ACount;
}
//---------------------------------------------------------------------------

void __fastcall THistoryComboBox::KeyPress(char &Key) {
  TComboBox::KeyPress(Key);  // perform standard handling, including calling handler

  if (Key != NULL)
    if (Key == 0x0d)        // Carriage return...
      UpdateHistory(Key);
}
//---------------------------------------------------------------------------

void __fastcall THistoryComboBox::UpdateHistory(char Key) {
  if (!Text.IsEmpty()) {
    int i = Items->IndexOf(Text);  // Is string in list?

    if (i != -1)           // If yes, remove it...
      Items->Delete(i);

    if (Items->Count >= HistorySize)  // Trim the list...
      Items->Delete(Items->Count - 1);

    Items->Insert(0, Text);  // Add to top of list...

    SelectAll();    // Select the entire string...
  }
}
//---------------------------------------------------------------------------

