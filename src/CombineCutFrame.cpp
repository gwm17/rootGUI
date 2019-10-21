#include "CombineCutFrame.h"
#include <TTimer.h>
#include <TGTextBuffer.h>
#include <TGLabel.h>
#include <iostream>
#include <TCut.h>
#include <TCutG.h>

using namespace std;

CombineCutFrame::CombineCutFrame(const TGWindow *p, const TGWindow *main, UInt_t w, UInt_t h,
                                 MyMainFrame *parent, vector<string> cuts) {
  fMain = new TGTransientFrame(p,main,w,h);
  fMain->SetCleanup(kDeepCleanup);
  fMain->DontCallClose();
  string dummy_name = "MyCut"+to_string(cuts.size()+1);
  type_str = "";

  TGLayoutHints *bhints = new TGLayoutHints(kLHintsCenterX|kLHintsCenterY,5,5,5,5);
  TGLayoutHints *lhints = new TGLayoutHints(kLHintsLeft|kLHintsCenterY,10,10,5,0);
  fIcon = gClient->GetPicture("cut_t.xpm");

  TGVerticalFrame *v1 = new TGVerticalFrame(fMain, w, h*0.75);
  TGLabel *cnl = new TGLabel(v1, "Cut Name:");
  TGTextBuffer *cnBuf = new TGTextBuffer(50);
  CutName = new TGTextEntry(v1, cnBuf);
  cnBuf->AddText(0,dummy_name.c_str());
  CutName->Resize(w, CutName->GetDefaultHeight());
  TGHorizontalFrame *h1 = new TGHorizontalFrame(v1, w, h*0.5);
  fCanvas = new TGCanvas(h1, w*0.5, h*0.5);
  fTree = new TGListTree(fCanvas, kVerticalFrame);
  fTree->Associate(h1);
  for(unsigned int i=0; i<cuts.size(); i++) {
    TObject *obj = parent->GetCut(cuts[i].c_str());
    if(obj->IsA() == TCutG::Class()) {
      fTree->AddItem(0,cuts[i].c_str(), fIcon, fIcon);
    } else if (obj->IsA() == TCut::Class()) {
      TCut *cut = (TCut*) obj;
      fTree->AddItem(0,cut->GetTitle(), fIcon, fIcon);
    }
  }
  fTree->Connect("DoubleClicked(TGListTreeItem*, Int_t)","CombineCutFrame",this,
                 "AddCut(TGListTreeItem*, Int_t)");
  fOptions = new TGButtonGroup(h1, "Options", kVerticalFrame);
  opts[0] = new TGRadioButton(fOptions, "And", AND);
  opts[1] = new TGRadioButton(fOptions, "Or", OR);
  opts[2] = new TGRadioButton(fOptions, "Not", NOT);
  fOptions->Connect("Clicked(Int_t)","CombineCutFrame",this,"SetType(Int_t)");
  fOptions->Show();
  h1->AddFrame(fOptions, bhints);
  h1->AddFrame(fCanvas, bhints);
  TGLabel *cil = new TGLabel(v1, "Cut Information:");
  TGTextBuffer *ciBuf = new TGTextBuffer(200);
  CutInfo = new TGTextEntry(v1, ciBuf);
  CutInfo->Resize(w, CutInfo->GetDefaultHeight());
  v1->AddFrame(cnl, lhints);
  v1->AddFrame(CutName, bhints);
  v1->AddFrame(h1, bhints);
  v1->AddFrame(cil, lhints);
  v1->AddFrame(CutInfo, bhints);
  
  TGHorizontalFrame *h2 = new TGHorizontalFrame(fMain, w, h*0.25);
  OkButton = new TGTextButton(h2, "Ok");
  OkButton->Connect("Clicked()","CombineCutFrame",this,"DoOk()");
  CancelButton = new TGTextButton(h2, "Cancel");
  CancelButton->Connect("Clicked()","CombineCutFrame",this,"DoCancel()");
  h2->AddFrame(OkButton, bhints);
  h2->AddFrame(CancelButton, bhints);

  Connect("SendCut(char*,char*)","MyMainFrame",parent,"GetComboCut(char*,char*)");
 
  fMain->AddFrame(v1, bhints); 
  fMain->AddFrame(h2,bhints);
  fMain->SetWindowName("Combine Cuts");
  fMain->MapSubwindows();
  fMain->Resize();
  fMain->CenterOnParent();
  fMain->MapWindow();
}

CombineCutFrame::~CombineCutFrame() {
  fMain->Cleanup();
  fMain->DeleteWindow();
}

void CombineCutFrame::CloseWindow() {
  delete this;
}

void CombineCutFrame::DoOk() {
  CancelButton->SetState(kButtonDisabled);
  OkButton->SetState(kButtonDisabled);
  char n[50];
  strcpy(n, CutName->GetText());
  char i[200];
  strcpy(i, CutInfo->GetText());
  SendCut(n, i);
  TTimer::SingleShot(150,"CombineCutFrame",this,"CloseWindow()");
}

void CombineCutFrame::DoCancel() {
  CancelButton->SetState(kButtonDisabled);
  OkButton->SetState(kButtonDisabled);
  TTimer::SingleShot(150,"CombineCutFrame",this,"CloseWindow()");
}

void CombineCutFrame::AddCut(TGListTreeItem *item, Int_t btn) {
  string tc(item->GetText());
  selectedCuts.push_back(tc);
  string new_info = "";
  if(type_str == "") return;
  for(unsigned int i=0; i<selectedCuts.size(); i++) {
    if(i == selectedCuts.size()-1) {
      if(type_str == "!") {
        new_info += type_str + "("+selectedCuts[i]+")";
      } else {
        new_info += "("+selectedCuts[i]+")";
      }
    } else {
      if(type_str == "!") {
        new_info += type_str+"("+selectedCuts[i]+")&&";
      } else {
        new_info += "("+selectedCuts[i]+")"+type_str;
      }
    }
  }
  CutInfo->SetText(new_info.c_str());
}

void CombineCutFrame::RemoveCut() {
  vector<string> new_list;
  string new_info = "";
  for(unsigned int i=0; i<selectedCuts.size()-1; i++) {
    if(i == selectedCuts.size()-2) {
      if(type_str == "!") {
        new_info += type_str + "("+selectedCuts[i]+")";
      } else {
        new_info += "("+selectedCuts[i]+")";
      }
    } else {
      if(type_str == "!") {
        new_info += type_str+"("+selectedCuts[i]+")&&";
      } else {
        new_info += "("+selectedCuts[i]+")"+type_str;
      }
    }
    new_list.push_back(selectedCuts[i]);
  }
  selectedCuts = new_list;
  CutInfo->SetText(new_info.c_str());
}

void CombineCutFrame::SetType(Int_t id) {
  switch(id) {
    case AND:
      type_str = "&&";
      break;
    case OR:
      type_str = "||";
      break;
    case NOT:
      type_str = "!";
      break;
  }
}

void CombineCutFrame::SendCut(char *name, char *cut) {
  EmitVA<char*, char*>("SendCut(char*,char*)",2,name,cut);
}
