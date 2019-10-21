#include "CutListFrame.h"
#include <TTimer.h>
#include <stdexcept>

using namespace std;

CutListFrame::CutListFrame(const TGWindow *p, const TGWindow *main, UInt_t w, UInt_t h,
                           MyMainFrame *parent, vector<string> cuts, 
                           unordered_map<string, string> cmap) {
  fMain = new TGTransientFrame(p,main,w,h);
  fMain->SetCleanup(kDeepCleanup);
  fMain->DontCallClose();
  fCutIcon = gClient->GetPicture("cut_t.xpm");
  fHistIcon = gClient->GetPicture("h1_s.xpm");
  local_map = cmap;
  clist = cuts;
 
  TGLayoutHints *bhints = new TGLayoutHints(kLHintsCenterX|kLHintsCenterY,5,5,5,5);
  TGLayoutHints *chints = new TGLayoutHints(kLHintsExpandX|kLHintsExpandY,5,5,5,5);

  TGVerticalFrame *v1 = new TGVerticalFrame(fMain,w,h*0.75);
  fCanvas = new TGCanvas(v1, w*0.5, h*0.65);
  fTree = new TGListTree(fCanvas, kVerticalFrame);
  fTree->Associate(v1);
  for(unsigned int i=0; i<clist.size(); i++) {
    fTree->AddItem(0, clist[i].c_str(), fCutIcon, fCutIcon);
  }
  fTree->Connect("DoubleClicked(TGListTreeItem*,Int_t)","CutListFrame",this,
                 "HandleDoubleClick(TGListTreeItem*,Int_t)");
  v1->AddFrame(fCanvas, chints);

  TGHorizontalFrame *h1 = new TGHorizontalFrame(fMain, w, h*0.25);
  OkButton = new TGTextButton(h1, "Ok");
  OkButton->Connect("Clicked()","CutListFrame",this,"DoOk()");
  CancelButton = new TGTextButton(h1, "Cancel");
  CancelButton->Connect("Clicked()","CutListFrame",this,"DoCancel()");
  h1->AddFrame(OkButton, bhints);
  h1->AddFrame(CancelButton, bhints);

  fMain->AddFrame(v1, bhints);
  fMain->AddFrame(h1, bhints);
  fMain->SetWindowName("Cut List");
  fMain->MapSubwindows();
  fMain->Resize();
  fMain->CenterOnParent();
  fMain->MapWindow();

}

CutListFrame::~CutListFrame() {
  fMain->Cleanup();
  fMain->DeleteWindow();
}

void CutListFrame::CloseWindow() {
  delete this;
}

void CutListFrame::DoOk() {
  CancelButton->SetState(kButtonDisabled);
  OkButton->SetState(kButtonDisabled);
  TTimer::SingleShot(150,"CutListFrame",this,"CloseWindow()");
}

void CutListFrame::DoCancel() {
  CancelButton->SetState(kButtonDisabled);
  OkButton->SetState(kButtonDisabled);
  TTimer::SingleShot(150,"CutListFrame",this,"CloseWindow()");
}

void CutListFrame::HandleDoubleClick(TGListTreeItem *item, Int_t btn) {
  if(btn != kButton1 || !item || (Bool_t)item->GetUserData()) return;
  try {
    item->SetUserData((void*)1);
    string s(item->GetText());
    string h = local_map.at(s);
    fTree->AddItem(item,h.c_str(),fHistIcon,fHistIcon);
  } catch(out_of_range& oor) {}
}
