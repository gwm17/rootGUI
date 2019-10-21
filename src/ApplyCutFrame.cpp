/*ApplyCutFrame.h
 *Frame which allows user to apply a prexisting cut to a prexisting histogram
 *
 *Gordon M. Oct 2019
 */
#include "ApplyCutFrame.h"
#include <TTimer.h>

using namespace std;

enum boxes {
  C,
  S
};

//Constructor
ApplyCutFrame::ApplyCutFrame(const TGWindow *p, const TGWindow *main, UInt_t w, UInt_t h,
                             MyMainFrame *parent, vector<string> cuts, vector<string> spectra){

  fMain = new TGTransientFrame(p, main, w, h);
  fMain->SetCleanup(kDeepCleanup);
  fMain->DontCallClose();
  slist = spectra;
  clist = cuts;
  s_signal = 0;
  c_signal = 0;

  TGLayoutHints *bhints = new TGLayoutHints(kLHintsCenterX|kLHintsCenterY,5,5,5,5);
  
  TGHorizontalFrame *h1 = new TGHorizontalFrame(fMain, w, h*0.5);
  cbox = new TGComboBox(h1, C);
  for(unsigned int i=0; i<clist.size(); i++) {
    cbox->AddEntry(clist[i].c_str(), i);
  }
  cbox->Resize(150,20);
  cbox->Connect("Selected(Int_t,Int_t)","ApplyCutFrame",this,"Signaled(Int_t,Int_t)");
  sbox = new TGComboBox(h1, S);
  for(unsigned int i=0; i<slist.size(); i++) {
    sbox->AddEntry(slist[i].c_str(), i);
  }
  sbox->Resize(150,20);
  sbox->Connect("Selected(Int_t,Int_t)","ApplyCutFrame",this,"Signaled(Int_t,Int_t)");
  h1->AddFrame(sbox, bhints);
  h1->AddFrame(cbox, bhints);

  TGHorizontalFrame *h2 = new TGHorizontalFrame(fMain, w, h*0.5);
  OkButton = new TGTextButton(h2, "Ok");
  OkButton->Connect("Clicked()","ApplyCutFrame",this,"DoOk()");
  OkButton->SetState(kButtonDisabled);
  CancelButton = new TGTextButton(h2, "Cancel");
  CancelButton->Connect("Clicked()","ApplyCutFrame",this,"DoCancel()");
  h2->AddFrame(OkButton, bhints);
  h2->AddFrame(CancelButton, bhints);
  
  Connect("SendName(char*,char*)","MyMainFrame",parent,"ApplyCut(char*,char*)");

  fMain->AddFrame(h1, bhints);
  fMain->AddFrame(h2, bhints);
  fMain->SetWindowName("Apply Cut to Spectrum");
  fMain->MapSubwindows();
  fMain->Resize();
  fMain->CenterOnParent();
  fMain->MapWindow();
}

//Destructor
ApplyCutFrame::~ApplyCutFrame() {
  fMain->Cleanup();
  fMain->DeleteWindow();
}

void ApplyCutFrame::CloseWindow() {
  delete this;
}

void ApplyCutFrame::DoOk() {
  OkButton->SetState(kButtonDisabled);
  CancelButton->SetState(kButtonDisabled);
  const char *sn = sbox->GetSelectedEntry()->GetTitle();
  const char *cn = cbox->GetSelectedEntry()->GetTitle();
  char sname[50], cname[50];
  strcpy(sname, sn); strcpy(cname, cn);
  SendName(sname, cname);
  //Waits and then closes frame
  TTimer::SingleShot(150,"ApplyCutFrame",this,"CloseWindow()");
}

void ApplyCutFrame::DoCancel() {
  OkButton->SetState(kButtonDisabled);
  CancelButton->SetState(kButtonDisabled);
  //Waits and then closes frame
  TTimer::SingleShot(150,"ApplyCutFrame",this,"CloseWindow()");
}

//requires a certain number of signals from user
void ApplyCutFrame::Signaled(Int_t box_id, Int_t entry_id) {
  switch(box_id) {
    case C:
      c_signal = 1;
      break;
    case S:
      s_signal = 1;
      break;
  }
  if((c_signal+s_signal) >= required_signals) {
    OkButton->SetState(kButtonUp);
  }
}

void ApplyCutFrame::SendName(char *sname, char *cname) {
  EmitVA<char*,char*>("SendName(char*,char*)",2,sname,cname); //send it
}
