#include "FileSrcFrame.h"
#include <TGClient.h>
#include <TGTextBuffer.h>
#include <TGLabel.h>
#include <TTimer.h>

using namespace std;

FileSrcFrame::FileSrcFrame(const TGWindow *p, const TGWindow *main, UInt_t w, UInt_t h, MyMainFrame *parent) {
  
  fMain = new TGTransientFrame(p,main,w,h);
  fMain->SetCleanup(kDeepCleanup);
  fMain->DontCallClose();

  fViewer = new TGListView(fMain, w, h/2);
  Pixel_t white;
  gClient->GetColorByName("white", white);
  fContents = new TGFileContainer(fViewer, kSunkenFrame, white);
  fContents->Connect("DoubleClicked(TGFrame*,Int_t","FileSrcFrame",this,
                     "DoDoubleClick(TGLVEntry*,Int_t)");
  
  TGVerticalFrame *vframe = new TGVerticalFrame(fMain, w, h/4);
  TGLabel *fFNamelabel = new TGLabel(vframe, "File:");
  TGTextBuffer *fBufFName;
  fFNameField = new TGTextEntry(vframe, fBufFName = new TGTextBuffer(50));
  fFNameField->Resize(w*0.5, fFNameField->GetDefaultHeight());
  vframe->AddFrame(fFNamelabel, new TGLayoutHints(kLHintsLeft|kLHintsCenterY,10,10,5,2));
  vframe->AddFrame(fFNameField, new TGLayoutHints(kLHintsCenterX|kLHintsCenterY,10,10,0,0));

  TGHorizontalFrame *hframe = new TGHorizontalFrame(fMain, w, h/4);
  fOkButton = new TGTextButton(hframe, "Ok");
  fCancelButton = new TGTextButton(hframe, "Cancel");
  fOkButton->Connect("Clicked()","FileSrcFrame",this,"DoOk()");
  fCancelButton->Connect("Clicked()","FileSrcFrame",this,"DoCancel()");
  hframe->AddFrame(fOkButton, new TGLayoutHints(kLHintsCenterX|kLHintsCenterY,10,10,10,10));
  hframe->AddFrame(fCancelButton, new TGLayoutHints(kLHintsCenterX|kLHintsCenterY,10,10,10,10));

  fMain->AddFrame(fViewer, new TGLayoutHints(kLHintsCenterX|kLHintsCenterY,10,10,10,10));
  fMain->AddFrame(vframe, new TGLayoutHints(kLHintsCenterX|kLHintsCenterY,10,10,10,10));
  fMain->AddFrame(hframe, new TGLayoutHints(kLHintsCenterX|kLHintsCenterY,10,10,10,10));

  Connect("SendText(char*)","MyMainFrame",parent,"SetURI(char*)");
  fMain->SetWindowName("Set File Data Source");
  fMain->MapSubwindows();
  fMain->Resize();
  fMain->CenterOnParent();
  fMain->MapWindow();

  fContents->SetDefaultHeaders();
  fContents->SetFilter("*.evt");
  fContents->DisplayDirectory();
  fContents->AddFile("..");
  fContents->Resize();
  fContents->StopRefreshTimer();
  
  fMain->Resize();
}

FileSrcFrame::~FileSrcFrame() {
  fMain->Cleanup();
  fMain->DeleteWindow();
}

void FileSrcFrame::CloseWindow() {
  delete this;
}

void FileSrcFrame::DoCancel() {
  fCancelButton->SetState(kButtonDisabled);
  fOkButton->SetState(kButtonDisabled);
  TTimer::SingleShot(150,"FileSrcFrame",this,"CloseWindow()");
}

void FileSrcFrame::DoOk() {
  fOkButton->SetState(kButtonDisabled);
  fCancelButton->SetState(kButtonDisabled);
  const char *filename = fFNameField->GetText();
  string fn(filename);
  string u;
  u = "file://"+fn;
  char uri[u.size()+1];
  strcpy(uri, u.c_str());
  SendText(uri);
  TTimer::SingleShot(150,"FileSrcFrame",this,"CloseWindow()");
}

void FileSrcFrame::DisplayDir(const TString &name) {
  fContents->SetDefaultHeaders();
  fContents->ChangeDirectory(name);
  fContents->DisplayDirectory();
  fContents->AddFile("..");
  fMain->Resize();
}

void FileSrcFrame::SendText(char* text) {
  Emit("SendText(char*)",text);
}

void FileSrcFrame::DoDoubleClick(TGLVEntry *entry, Int_t id) {
  if(id != kButton1) return;
  TString dirname(fContents->GetDirectory()); 
  TString entryname(entry->GetTitle());
  
  if(entryname.EndsWith(".evt")) {
    TString name = dirname+"/"+entryname;
    const char *evtname = name.Data();
    fFNameField->SetText(evtname);
  } else {
    DisplayDir(entryname);
  }
}
