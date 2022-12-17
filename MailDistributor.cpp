//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include <FileCtrl.hpp>
#include "MailDistributor.h"
#include "Tokenizer.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//

static inline void ValidCtrCheck(TMailDistributor *)
{
        new TMailDistributor(NULL);
}
//---------------------------------------------------------------------------
__fastcall TMailDistributor::TMailDistributor(TComponent* Owner)
        : TComponent(Owner)
{
  FActive = false;
  FParseOnly = true;
  FAllowEmptyUID = false;
  FArchiveAsDelete = false;

  FSourceDir = new AnsiString();
  FDestinationDir = new AnsiString();
  FArchiveDir = new AnsiString();
  FDeadLetterDir = new AnsiString();
  FMailFileExt = new AnsiString();
  FMailHeaders = new TMailHeaders(this);

  FileList = new TStringList();

  AddrFrom = new TMailIdentity(this);
  AddrDeliveredTo = new TMailIdentity(this);
  AddrTo = new TAddressBook(this);
  AddrCc = new TAddressBook(this);
  AddrBcc = new TAddressBook(this);

  FNumberOfMessages = 0;
}
//---------------------------------------------------------------------------
__fastcall TMailDistributor::~TMailDistributor()
{
  delete FSourceDir;
  delete FDestinationDir;
  delete FArchiveDir;
  delete FDeadLetterDir;
  delete FMailFileExt;
  delete FMailHeaders;

  delete AddrFrom;
  delete AddrTo;
  delete AddrCc;
  delete AddrBcc;
  delete AddrDeliveredTo;

  delete FileList;
}
//---------------------------------------------------------------------------
namespace Maildistributor
{
        void __fastcall PACKAGE Register()
        {
                 TComponentClass classes[1] = {__classid(TMailDistributor)};
                 RegisterComponents("LMTS", classes, 0);
        }
}
//---------------------------------------------------------------------------
void __fastcall TMailDistributor::SetParseOnly(bool po)
{
  FParseOnly = po;
}
//---------------------------------------------------------------------------
void __fastcall TMailDistributor::SetAllowEmptyUID(bool eu)
{
  FAllowEmptyUID = eu;
}
//---------------------------------------------------------------------------
void __fastcall TMailDistributor::SetArchiveAsDelete(bool aad)
{
  FArchiveAsDelete = aad;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TMailDistributor::GetSourceDir()
{
  return *FSourceDir;
}
//---------------------------------------------------------------------------
void __fastcall TMailDistributor::SetSourceDir(AnsiString sf)
{
  if (sf.SubString(sf.Length(), 1) == "\\")
    *FSourceDir = sf.SubString(1, sf.Length() - 1);
  else
    *FSourceDir = sf;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TMailDistributor::GetDestinationDir()
{
  return *FDestinationDir;
}
//---------------------------------------------------------------------------
void __fastcall TMailDistributor::SetDestinationDir(AnsiString df)
{
  if (df.SubString(df.Length(), 1) == "\\")
    *FDestinationDir = df.SubString(1, df.Length() - 1);
  else
    *FDestinationDir = df;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TMailDistributor::GetArchiveDir()
{
  return *FArchiveDir;
}
//---------------------------------------------------------------------------
void __fastcall TMailDistributor::SetArchiveDir(AnsiString ad)
{
  if (ad.SubString(ad.Length(), 1) == "\\")
    *FArchiveDir = ad.SubString(1, ad.Length() - 1);
  else
    *FArchiveDir = ad;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TMailDistributor::GetDeadLetterDir()
{
  return *FDeadLetterDir;
}
//---------------------------------------------------------------------------
void __fastcall TMailDistributor::SetDeadLetterDir(AnsiString dld)
{
  if (dld.SubString(dld.Length(), 1) == "\\")
    *FDeadLetterDir = dld.SubString(1, dld.Length() - 1);
  else
    *FDeadLetterDir = dld;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TMailDistributor::GetMailFileExt()
{
  return *FMailFileExt;
}
//---------------------------------------------------------------------------
void __fastcall TMailDistributor::SetMailFileExt(AnsiString me)
{
  *FMailFileExt = me;

  while (FMailFileExt->SubString(1, 1) == ".")
    FMailFileExt->Delete(1, 1);
}
//---------------------------------------------------------------------------
void __fastcall TMailDistributor::GetMailFiles()
{
  AnsiString *FileMask = new AnsiString();
  TSearchRec sr;

  FileList->Clear();
  FNumberOfMessages = 0;
  sr.ExcludeAttr = (faHidden | faDirectory);

  *FileMask = *FSourceDir + "\\*";
  if (*FMailFileExt != "")
    *FileMask = *FileMask + "." + *FMailFileExt;

  if (FindFirst(*FileMask, faAnyFile, sr) == 0) {
    FileList->Add(sr.Name);

    while (FindNext(sr) == 0)
      FileList->Add(sr.Name);

    FindClose(sr);
  }

  FNumberOfMessages = FileList->Count;

  delete FileMask;
}
//---------------------------------------------------------------------------
void __fastcall TMailDistributor::Clear()
{
  FMailHeaders->Clear();
  AddrFrom->Clear();
  AddrDeliveredTo->Clear();
  AddrTo->Clear();
  AddrCc->Clear();
  AddrBcc->Clear();
}
//---------------------------------------------------------------------------
void __fastcall TMailDistributor::Distribute()
{
  TSearchRec sr;

  if (!FActive) {
    FActive = true;

    if (!FArchiveDir->IsEmpty())
      if (!DirectoryExists(*FArchiveDir))
        ForceDirectories(*FArchiveDir);

    if (!FDeadLetterDir->IsEmpty())
      if (!DirectoryExists(*FDeadLetterDir))
        ForceDirectories(*FDeadLetterDir);

    Clear();
    GetMailFiles();

    for (int i = 0; i < FileList->Count; i++) {
      DistributeMessage(FileList->Strings[i]);

      if (FOnMessageDistributed != NULL)
        FOnMessageDistributed(this, (i + 1));
    }

    FActive = false;
  }
}
//---------------------------------------------------------------------------
void __fastcall TMailDistributor::AssignToUser(AnsiString MsgFile, AnsiString uid)
{
  AnsiString *oldname = new AnsiString();
  AnsiString *newname = new AnsiString();
  char Buf[80];

  memset(Buf, 0, 80);
  
  if (!uid.IsEmpty())
    *newname = *FDestinationDir + "\\" + Trim(uid) + "-" + MsgFile;
  else
    *newname = *FDestinationDir + "\\" + MsgFile;

  *oldname = *FSourceDir + "\\" + MsgFile;
  if (!CopyFile(oldname->c_str(), newname->c_str(), true))
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), 0, Buf, 79, NULL);

  delete newname;
  delete oldname;
}
//---------------------------------------------------------------------------
void __fastcall TMailDistributor::DistributeMessage(AnsiString oldname)
{
  TSearchRec sr;
  AnsiString *uid = new AnsiString();
  AnsiString *newname = new AnsiString();
  TStringList *MailMsg = new TStringList();
  TStringList *HdrList = new TStringList();
  int ValidRecipients = 0;

  AddrFrom->Clear();
  AddrFrom->RequireDispName = true;
  AddrDeliveredTo->Clear();
  AddrDeliveredTo->RequireDispName = true;
  AddrTo->Clear();
  AddrCc->Clear();
  AddrBcc->Clear();

  MailMsg->LoadFromFile(*FSourceDir + "\\" + oldname);
  FMailHeaders->SetHeaders(MailMsg);

  HdrList->Clear();
  FMailHeaders->GetHeaderEx("Delivered-To:", HdrList);
  AddrDeliveredTo->AddAddress(HdrList->Text);

  HdrList->Clear();
  FMailHeaders->GetHeaderEx("From:", HdrList);
  AddrFrom->AddAddress(HdrList->Text);

  HdrList->Clear();
  FMailHeaders->GetHeaderEx("To:", HdrList);
  AddrTo->Add(HdrList->Text);

  HdrList->Clear();
  FMailHeaders->GetHeaderEx("Cc:", HdrList);
  AddrCc->Add(HdrList->Text);

  long MsgLen = MailMsg->Text.Length();
  if (FOnMessageLoaded != NULL)
    FOnMessageLoaded(this, MsgLen);


  TAddressBook *AllAddr = new TAddressBook(this);
  AllAddr->Append(AddrTo);
  AllAddr->Append(AddrCc);
  AllAddr->Append(AddrBcc);
//  if (AllAddr->Count == 0)
//    AllAddr->Append(AddrDeliveredTo);


  if (!FParseOnly) {
    int CopyCount = 0;
    AnsiString *Recipient = new AnsiString();

    for (int m = 0; m < AllAddr->Count; m++) {
      if (m == 0)
        *Recipient = AllAddr->GetFirstAddress();
      else
        *Recipient = AllAddr->GetNextAddress();

      bool Valid = true;
      if (FOnMessageValidateRecipient != NULL)
        FOnMessageValidateRecipient(this, *Recipient, uid, Valid);

      if (!FAllowEmptyUID)
        if (Valid)
          Valid = (!uid->IsEmpty());

      if (Valid) {
        ValidRecipients++;

        if (!uid->IsEmpty())
          *newname = *FDestinationDir + "\\" + Trim(*uid) + "-" + oldname;
        else
          *newname = *FDestinationDir + "\\" + oldname;

        bool Save = true;
        if (FOnMessageBeforeSave != NULL)
          FOnMessageBeforeSave(this, *Recipient, newname, Save);

        if (Save) {
          if (newname->AnsiCompareIC(oldname) != 0) {
            while (FindFirst(*newname, faAnyFile, sr) == 0) {
              if (FOnMessageFileExists == NULL) {
                LogEvent("Mail message filename already exists (" + *newname + ")");
                break;
              } else
                FOnMessageFileExists(this, newname);
            }

            FindClose(sr);

            AnsiString *tn = new AnsiString();
            *tn = *FSourceDir + "\\" + oldname;

            if (CopyFile(tn->c_str(), newname->c_str(), true) != 0) // If success...
              CopyCount++;

            delete tn;
          }
        }
      }
    }

    delete Recipient;

    if (CopyCount > 0)
      DeleteMsgFile(*FSourceDir + "\\" + oldname);

    if (ValidRecipients == 0) {
      LogEvent("Mail message has no valid local recipients (" + oldname + ")");

      AnsiString *src = new AnsiString();
      AnsiString *tgt = new AnsiString();

      *src = *FSourceDir + "\\" + oldname;
      *tgt = *FDeadLetterDir + "\\" + oldname;

      if (CopyFile(src->c_str(), tgt->c_str(), false))
        DeleteFile(*src);

      delete tgt;
      delete src;

      if (FOnDeadLetter != NULL)
        FOnDeadLetter(this, oldname);
    }
  }

  delete AllAddr;


  delete HdrList;
  delete MailMsg;
  delete newname;
  delete uid;
}
//---------------------------------------------------------------------------
void __fastcall TMailDistributor::DeleteMsgFile(AnsiString fname)
{
  if (FArchiveAsDelete) {
    AnsiString *aname = new AnsiString();
    AnsiString *sd = new AnsiString();
    AnsiString *dd = new AnsiString();

    *aname = *FArchiveDir + "\\" + ExtractFileName(fname);
    if (aname->SubString(2, 1) == ":")
      *dd = aname->SubString(1, 1);
    if (fname.SubString(2, 1) == ":")
      *sd = fname.SubString(1, 1);

    if (*sd != *dd) {
      CopyFile(fname.c_str(), aname->c_str(), false);
      DeleteFile(fname);
    } else
      MoveFile(fname.c_str(), aname->c_str());

    delete sd;
    delete dd;
    delete aname;
  }
}
//---------------------------------------------------------------------------
void __fastcall TMailDistributor::LogEvent(AnsiString le)
{
  if (FOnLogEvent != NULL)
    FOnLogEvent(this, "[MAIL DIST] [" + FormatDateTime("yyyy-mm-dd hh:nn:ssam/pm", Now()) + "] " + le);
}
//---------------------------------------------------------------------------

