#include "CutFrame.h"
#include <TTimer.h>

using namespace std;

CutFrame::CutFrame(const TGWindow *p, const TGWindow *main, UInt_t w, UInt_t h,
                   MyMainFrame *parent, Int_t np, Int_t dF) {
  fMain = new TGTransientFrame(p,main,w,h);
  fMain->SetCleanup(kDeepCleanup);
  fMain->DontCallClose();
  nPads = np;
  pad = 0;
  dimFlag = dF;
 
  TGVerticalFrame *v1 = new TGVerticalFrame(fMain, w, h*0.5);

  TGLabel *cnl = new TGLabel(v1, "Cut Name:");
  v1->AddFrame(cnl, new TGLayoutHints(kLHintsLeft|kLHintsCenterY,10,10,5,2));
  TGTextBuffer *cnBuf = new TGTextBuffer(50);
  CutName = new TGTextEntry(v1, cnBuf);
  cnBuf->AddText(0,"MyCut");
  CutName->Resize(w*0.5,CutName->GetDefaultHeight());
  v1->AddFrame(CutName, new TGLayoutHints(kLHintsCenterX|kLHintsCenterY,10,10,0,5));

  TGLabel *psl = new TGLabel(v1, "Spectrum Number: ");
  v1->AddFrame(psl, new TGLayoutHints(kLHintsLeft|kLHintsCenterY,10,10,5,2));
  padSelect = new TGComboBox(v1, 0);
  padSelect->Resize(50,20);
  for(int i=1; i<nPads+1; i++) {
    padSelect->AddEntry(Form("%i",i),i);
  }
  padSelect->Connect("Selected(Int_t,Int_t)","CutFrame",this,"SetPad(Int_t,Int_t)");
  v1->AddFrame(padSelect,new TGLayoutHints(kLHintsCenterX|kLHintsCenterY,10,10,0,5));
  
  TGHorizontalFrame *h1 = new TGHorizontalFrame(fMain,w,h*0.5);
  OkButton = new TGTextButton(h1, "Ok");
  OkButton->Connect("Clicked()","CutFrame",this,"DoOk()");
  OkButton->SetState(kButtonDisabled);
  h1->AddFrame(OkButton, new TGLayoutHints(kLHintsCenterX|kLHintsCenterY,10,10,5,5));
  CancelButton = new TGTextButton(h1, "Cancel");
  CancelButton->Connect("Clicked()","CutFrame",this,"DoCancel()");
  h1->AddFrame(CancelButton, new TGLayoutHints(kLHintsCenterX|kLHintsCenterY,10,10,5,5));

  if(dimFlag == 1) {
    Connect("SendCut(char*,Int_t)","MyMainFrame",parent,"Get2DCut(char*,Int_t)");
  } else {
    Connect("SendCut(char*,Int_t)","MyMainFrame",parent,"Get1DCut(char*,Int_t)");
  }

  fMain->AddFrame(v1, new TGLayoutHints(kLHintsCenterX|kLHintsCenterY,5,5,5,5));
  fMain->AddFrame(h1, new TGLayoutHints(kLHintsCenterX|kLHintsCenterY,5,5,5,5));
  fMain->SetWindowName("Set Cut Name");
  fMain->MapSubwindows();
  fMain->Resize();
  fMain->CenterOnParent();
  fMain->MapWindow();
}

CutFrame::~CutFrame() {
  fMain->Cleanup();
  fMain->DeleteWindow();
}

void CutFrame::CloseWindow() {
  delete this;
}

void CutFrame::DoOk() {
  CancelButton->SetState(kButtonDisabled);
  OkButton->SetState(kButtonDisabled);
  const char *name = CutName->GetText();
  char n[50];
  strcpy(n, name);
  SendCut(n, pad);
  TTimer::SingleShot(150,"CutFrame",this,"CloseWindow()");
}

void CutFrame::DoCancel() {
  CancelButton->SetState(kButtonDisabled);
  OkButton->SetState(kButtonDisabled);
  TTimer::SingleShot(150,"CutFrame",this,"CloseWindow()");
}

void CutFrame::SetPad(Int_t box, Int_t entry) {
  if(box == 0) {
    pad = entry;
    OkButton->SetState(kButtonUp);
  }
}

void CutFrame::SendCut(char* text, Int_t p) {
  EmitVA<char*,Int_t>("SendCut(char*,Int_t)",2,text,p);
}
