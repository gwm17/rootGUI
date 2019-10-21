#include "FileConfigFrame.h"
#include <TGClient.h>
#include <TGTextBuffer.h>
#include <TGLabel.h>
#include <TTimer.h>

using namespace std;

FileConfigFrame::FileConfigFrame(const TGWindow *p, const TGWindow *main, UInt_t w, UInt_t h, MyMainFrame *parent, Int_t save) {
  
  fMain = new TGTransientFrame(p,main,w,h);
  fMain->SetCleanup(kDeepCleanup);
  fMain->DontCallClose();

  fViewer = new TGListView(fMain, w, h/2);
  Pixel_t white;
  gClient->GetColorByName("white", white);
  fContents = new TGFileContainer(fViewer, kSunkenFrame, white);
  fContents->Connect("DoubleClicked(TGFrame*,Int_t","FileConfigFrame",this,
                     "DoDoubleClick(TGLVEntry*,Int_t)");
  
  TGVerticalFrame *vframe = new TGVerticalFrame(fMain, w, h/4);
  TGLabel *fFNamelabel = new TGLabel(vframe, "File:");
  TGTextBuffer *fBufFName = new TGTextBuffer(50);
  fFNameField = new TGTextEntry(vframe, fBufFName = new TGTextBuffer(50));
  fFNameField->Resize(w*0.5, fFNameField->GetDefaultHeight());
  vframe->AddFrame(fFNamelabel, new TGLayoutHints(kLHintsLeft|kLHintsCenterY,10,10,5,2));
  vframe->AddFrame(fFNameField, new TGLayoutHints(kLHintsCenterX|kLHintsCenterY,10,10,0,0));

  TGHorizontalFrame *hframe = new TGHorizontalFrame(fMain, w, h/4);
  fOkButton = new TGTextButton(hframe, "Ok");
  fCancelButton = new TGTextButton(hframe, "Cancel");
  fOkButton->Connect("Clicked()","FileConfigFrame",this,"DoOk()");
  fCancelButton->Connect("Clicked()","FileConfigFrame",this,"DoCancel()");
  hframe->AddFrame(fOkButton, new TGLayoutHints(kLHintsCenterX|kLHintsCenterY,10,10,10,10));
  hframe->AddFrame(fCancelButton, new TGLayoutHints(kLHintsCenterX|kLHintsCenterY,10,10,10,10));

  fMain->AddFrame(fViewer, new TGLayoutHints(kLHintsCenterX|kLHintsCenterY,10,10,10,10));
  fMain->AddFrame(vframe, new TGLayoutHints(kLHintsCenterX|kLHintsCenterY,10,10,10,10));
  fMain->AddFrame(hframe, new TGLayoutHints(kLHintsCenterX|kLHintsCenterY,10,10,10,10));

  if(save) {
    Connect("SendText(char*)","MyMainFrame",parent,"WriteConfig(char*)");
  } else {
    Connect("SendText(char*)","MyMainFrame",parent,"SetConfig(char*)");
  }
  fMain->SetWindowName("Select Configuration File");
  fMain->MapSubwindows();
  fMain->Resize();
  fMain->CenterOnParent();
  fMain->MapWindow();

  fContents->SetDefaultHeaders();
  fContents->SetFilter("*.txt");
  fContents->DisplayDirectory();
  fContents->AddFile("..");
  fContents->Resize();
  fContents->StopRefreshTimer();
  string dir(fContents->GetDirectory());
  dir += "/"; 
  fFNameField->SetText(dir.c_str());
  
  fMain->Resize();
}

FileConfigFrame::~FileConfigFrame() {
  fMain->Cleanup();
  fMain->DeleteWindow();
}

void FileConfigFrame::CloseWindow() {
  delete this;
}

void FileConfigFrame::DoCancel() {
  fCancelButton->SetState(kButtonDisabled);
  fOkButton->SetState(kButtonDisabled);
  TTimer::SingleShot(150,"FileConfigFrame",this,"CloseWindow()");
}

void FileConfigFrame::DoOk() {
  fOkButton->SetState(kButtonDisabled);
  fCancelButton->SetState(kButtonDisabled);
  const char *filename = fFNameField->GetText();
  string temp(filename);
  char name[temp.size()+1];
  strcpy(name, temp.c_str());
  SendText(name);
  TTimer::SingleShot(150,"FileConfigFrame",this,"CloseWindow()");
}

void FileConfigFrame::DisplayDir(const TString &name) {
  string dir(fContents->GetDirectory());
  dir += "/"; 
  fFNameField->SetText(dir.c_str());
  fContents->SetDefaultHeaders();
  fContents->ChangeDirectory(name);
  fContents->DisplayDirectory();
  fContents->AddFile("..");
  fMain->Resize();
}

void FileConfigFrame::SendText(char* text) {
  Emit("SendText(char*)",text);
}

void FileConfigFrame::DoDoubleClick(TGLVEntry *entry, Int_t id) {
  if(id != kButton1) return;
  
  TString entryname(entry->GetTitle());
  TString dirname(fContents->GetDirectory());
  
  if(entryname.EndsWith(".txt")) {
    TString name = dirname+"/"+entryname;
    const char *evtname = name.Data();
    fFNameField->SetText(evtname);
  } else {
    DisplayDir(entryname);
  }
}
