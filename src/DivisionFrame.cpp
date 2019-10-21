/*DivisonFrame.cpp
 *
 *RELIC
 *
 *Gordon M. Oct 2019
 */
#include "DivisionFrame.h"
#include <TTimer.h>

using namespace std;

DivisionFrame::DivisionFrame(const TGWindow *p, const TGWindow *main, UInt_t w, UInt_t h,
                             MyMainFrame *parent) {
  fMain = new TGTransientFrame(p,main,w,h);
  fMain->SetCleanup(kDeepCleanup);
  fMain->DontCallClose();

  TGHorizontalFrame *h1 = new TGHorizontalFrame(fMain, w, h*0.75);
  rows = new TGButtonGroup(h1,"Rows",kVerticalFrame);
  r1[0] = new TGRadioButton(rows, "1",0);
  r1[1] = new TGRadioButton(rows, "2",1);
  r1[2] = new TGRadioButton(rows, "3",2);
  r1[3] = new TGRadioButton(rows, "4",3);
  rows->Show();
  rows->Connect("Clicked(Int_t)","DivisionFrame",this,"DoRowsClicked(Int_t)");
  h1->AddFrame(rows, new TGLayoutHints(kLHintsCenterX|kLHintsCenterY,5,5,5,5));
   

  columns = new TGButtonGroup(h1,"Columns",kVerticalFrame);
  r2[0] = new TGRadioButton(columns, "1",4);
  r2[1] = new TGRadioButton(columns, "2",5);
  r2[2] = new TGRadioButton(columns, "3",6);
  r2[3] = new TGRadioButton(columns, "4",7);
  columns->Show();
  columns->Connect("Clicked(Int_t)","DivisionFrame",this,"DoColumnsClicked(Int_t)");
  h1->AddFrame(columns, new TGLayoutHints(kLHintsCenterX|kLHintsCenterY,5,5,5,5));
  
  TGHorizontalFrame *h2 = new TGHorizontalFrame(fMain, w, h*0.25);
  fOkButton = new TGTextButton(h2, "Ok");
  fOkButton->Connect("Clicked()","DivisionFrame",this,"DoOk()");
  fCancelButton = new TGTextButton(h2, "Cancel");
  fCancelButton->Connect("Clicked()","DivisionFrame",this,"DoCancel()");
  h2->AddFrame(fOkButton, new TGLayoutHints(kLHintsCenterX|kLHintsCenterY,10,10,10,10));
  h2->AddFrame(fCancelButton, new TGLayoutHints(kLHintsCenterX|kLHintsCenterY,10,10,10,10));
  
  Connect("SendValues(Int_t,Int_t)","MyMainFrame",parent,"SetCanvasDiv(Int_t,Int_t");

  fMain->AddFrame(h1, new TGLayoutHints(kLHintsCenterX|kLHintsCenterY,5,5,5,5));
  fMain->AddFrame(h2, new TGLayoutHints(kLHintsBottom|kLHintsCenterX,5,5,5,5));
  fMain->SetWindowName("Set Canvas Divisions");
  fMain->MapSubwindows();
  fMain->Resize();
  fMain->CenterOnParent();
  fMain->MapWindow();
}

DivisionFrame::~DivisionFrame() {
  fMain->Cleanup();
  fMain->DeleteWindow();
}

void DivisionFrame::CloseWindow() {
  delete this;
}

void DivisionFrame::DoRowsClicked(Int_t id) {
  switch(id) {
    case 0: r = 1; break;
    case 1: r = 2; break;
    case 2: r = 3; break;
    case 3: r = 4; break;
  }
}

void DivisionFrame::DoColumnsClicked(Int_t id) {
  switch(id) {
    case 4: c = 1; break;
    case 5: c = 2; break;
    case 6: c = 3; break;
    case 7: c = 4; break;
  }
}

void DivisionFrame::DoOk() {
  fCancelButton->SetState(kButtonDisabled);
  fOkButton->SetState(kButtonDisabled);
  SendValues(r,c);
  TTimer::SingleShot(150,"DivisionFrame",this,"CloseWindow()");
}

void DivisionFrame::DoCancel() {
  fCancelButton->SetState(kButtonDisabled);
  fOkButton->SetState(kButtonDisabled);
  TTimer::SingleShot(150,"DivisionFrame",this,"CloseWindow()");
}

void DivisionFrame::SendValues(Int_t rs, Int_t cs) {
  EmitVA<Int_t, Int_t>("SendValues(Int_t,Int_t)",2,rs,cs);
}

