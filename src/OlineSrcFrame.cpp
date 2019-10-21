#include "OlineSrcFrame.h"
#include <TGClient.h>
#include <TGButton.h>
#include <TGMenu.h>
#include <TGTextEntry.h>
#include <TGTextBuffer.h>
#include <TGLabel.h>
#include <TTimer.h>

using namespace std;

OlineSrcFrame::OlineSrcFrame(const TGWindow *p, const TGWindow *main, UInt_t w, UInt_t h, 
                             MyMainFrame *parent) {
  //Init main frame of the transient
  fMain = new TGTransientFrame(p,main,w,h);
  fMain->SetCleanup(kDeepCleanup);
  fMain->DontCallClose();
  TGVerticalFrame *vframe = new TGVerticalFrame(fMain, w, h/2);
  TGTextBuffer *fBufHost, *fBufRing;
  fHostField = new TGTextEntry(vframe, fBufHost = new TGTextBuffer(50));
  fRingField = new TGTextEntry(vframe, fBufRing = new TGTextBuffer(50));
  TGLabel *fHostlabel = new TGLabel(vframe, "Host:");
  TGLabel *fRinglabel = new TGLabel(vframe, "Ring:");
  fBufHost->AddText(0,"localhost");
  fHostField->Resize(w*0.5, fHostField->GetDefaultHeight());
  fBufRing->AddText(0,"splitpole");
  fRingField->Resize(w*0.5, fRingField->GetDefaultHeight());
  vframe->AddFrame(fHostlabel, new TGLayoutHints(kLHintsLeft|kLHintsCenterY,10,10,5,2));
  vframe->AddFrame(fHostField, new TGLayoutHints(kLHintsCenterX|kLHintsCenterY,10,10,0,0));
  vframe->AddFrame(fRinglabel, new TGLayoutHints(kLHintsLeft|kLHintsCenterY,10,10,5,2));
  vframe->AddFrame(fRingField, new TGLayoutHints(kLHintsCenterX|kLHintsCenterY,10,10,0,5));
  TGHorizontalFrame *hframe = new TGHorizontalFrame(fMain, w, h/2);
  fOkButton = new TGTextButton(hframe, "Ok");
  fCancelButton = new TGTextButton(hframe, "Cancel");
  fOkButton->Connect("Clicked()","OlineSrcFrame", this, "DoOk()");
  fCancelButton->Connect("Clicked()","OlineSrcFrame",this,"DoCancel()");
  hframe->AddFrame(fOkButton,new TGLayoutHints(kLHintsCenterX|kLHintsCenterY,10,10,10,10));
  hframe->AddFrame(fCancelButton,new TGLayoutHints(kLHintsCenterX|kLHintsCenterY,10,10,10,10));
  Connect("SendText(char*)","MyMainFrame",parent,"SetURI(char*)");
  fMain->AddFrame(vframe, new TGLayoutHints(kLHintsTop|kLHintsCenterX,10,10,10,10));
  fMain->AddFrame(hframe, new TGLayoutHints(kLHintsBottom|kLHintsCenterX,10,10,10,10));
  fMain->SetWindowName("Set Online Data Source");
  fMain->MapSubwindows();
  fMain->Resize();
  fMain->CenterOnParent();
  fMain->MapWindow();
  
}

OlineSrcFrame::~OlineSrcFrame() {
  fMain->Cleanup();
  fMain->DeleteWindow();
}

void OlineSrcFrame::CloseWindow() {
  delete this;
}

void OlineSrcFrame::DoCancel() {
  fCancelButton->SetState(kButtonDisabled);
  fOkButton->SetState(kButtonDisabled);
  TTimer::SingleShot(150, "OlineSrcFrame",this,"CloseWindow()");
}

void OlineSrcFrame::DoOk() {
  fOkButton->SetState(kButtonDisabled);
  fCancelButton->SetState(kButtonDisabled);
  const char *host = fHostField->GetText();
  const char *ring = fRingField->GetText();
  string hs(host);
  string rs(ring);
  string u;
  u = "tcp://"+hs+"/"+rs;
  char uri[u.size()+1];
  strcpy(uri, u.c_str());
  SendText(uri);
  TTimer::SingleShot(150,"OlineSrcFrame",this,"CloseWindow()");
}

void OlineSrcFrame::SendText(char* text) {
  Emit("SendText(char*)",text);
}
