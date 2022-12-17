//---------------------------------------------------------------------------
#ifndef CascadedListH
#define CascadedListH
//---------------------------------------------------------------------------
#include <stdlib.h>
#include <SysUtils.hpp>
#include <Controls.hpp>
#include <Classes.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
typedef enum {
  ORIENT_VERTICAL=0, ORIENT_HORIZONTAL=1
} TOrientation;
//---------------------------------------------------------------------------
class PACKAGE TCascadedList : public TCustomPanel
{
private:
        TNotifyEvent FOnChange;
        
        AnsiString *FItemCaption;
        AnsiString *FListCaption;
        AnsiString *FItem;
        AnsiString *FListSelection;
        TStringList *FList;
        TOrientation FOrientation;

        TLabel *Name1;
        TLabel *Name2;
        TComboBox *List1;
        TComboBox *List2;

        void __fastcall List1Change(TObject* Sender);
        void __fastcall List2Change(TObject* Sender);

protected:
        virtual void __fastcall Layout(void);
        virtual void __fastcall Paint(void);
        void __fastcall SetOrientation(TOrientation o);
        AnsiString __fastcall GetItemCaption();
        void __fastcall SetItemCaption(AnsiString c);
        AnsiString __fastcall GetListCaption();
        void __fastcall SetListCaption(AnsiString c);
        AnsiString __fastcall GetItem();
        void __fastcall SetItem(AnsiString i);
        TStringList* __fastcall GetList();
        void __fastcall SetList(TStringList *i);
        AnsiString __fastcall GetListSelection();

public:
        __fastcall TCascadedList(TComponent* Owner);

__published:
        __property TOrientation Orientation = {read=FOrientation, write=SetOrientation,default=ORIENT_VERTICAL};
        __property AnsiString ItemCaption = {read=GetItemCaption,write=SetItemCaption};
        __property AnsiString ListCaption = {read=GetListCaption,write=SetListCaption};
        __property AnsiString Item = {read=GetItem,write=SetItem};
        __property AnsiString ListSelection = {read=GetListSelection};
        __property TStringList *List = {read=GetList,write=SetList};
        __property Height;
        __property Width;
        __property Font;
        __property TNotifyEvent OnChange = {read=FOnChange,write=FOnChange};
};
//---------------------------------------------------------------------------
#endif
