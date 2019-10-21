/*SpectraListFrame.cpp
 *Frame which lists all spectra for user and allows user to see spectrum
 *parameters. Currently does nothing else
 *
 *Gordon M. Oct 2019
 */
#include "SpectraListFrame.h"
#include <TTimer.h>
#include <stdexcept>

using namespace std;

//constructor
SpectraListFrame::SpectraListFrame(const TGWindow *p, const TGWindow *main, UInt_t w,
                               UInt_t h, MyMainFrame *parent, vector<string> spectra,
                               unordered_map<string,pair<TObject*,vector<string>>> hmap) {
  fMain = new TGTransientFrame(p,main,w,h);
  fMain->SetCleanup(kDeepCleanup);
  fMain->DontCallClose();
  local_map = hmap;
  slist = spectra;
  fh1Icon = gClient->GetPicture("h1_s.xpm");
  fh2Icon = gClient->GetPicture("h2_s.xpm");
  fxIcon = gClient->GetPicture("x_pic.xpm");
  fyIcon = gClient->GetPicture("y_pic.xpm");
  fcutIcon = gClient->GetPicture("cut_t.xpm");
  
  TGLayoutHints *bhints = new TGLayoutHints(kLHintsCenterX|kLHintsCenterY,5,5,5,5);
  TGLayoutHints *chints = new TGLayoutHints(kLHintsExpandX|kLHintsExpandY,5,5,5,5);

  TGVerticalFrame *v1 = new TGVerticalFrame(fMain,w, h*0.75);
  fCanvas = new TGCanvas(v1, w*0.5, h*0.65);
  fTree = new TGListTree(fCanvas, kVerticalFrame);
  fTree->Associate(v1);
  for(unsigned int  i=0; i<slist.size(); i++) {
    auto item = local_map[slist[i]];
    if(item.first->IsA() == TH1F::Class() || item.first->IsA() == TH1I::Class()) {
      fTree->AddItem(0, slist[i].c_str(), fh1Icon, fh1Icon);
    } else if(item.first->IsA() == TH2F::Class() || item.first->IsA() == TH2I::Class()) {
      fTree->AddItem(0, slist[i].c_str(), fh2Icon, fh2Icon);
    }
  }
  fTree->Connect("DoubleClicked(TGListTreeItem*,Int_t)","SpectraListFrame",this,
                 "HandleDoubleClick(TGListTreeItem*,Int_t)");
  v1->AddFrame(fCanvas, chints);

  TGHorizontalFrame *h1 = new TGHorizontalFrame(fMain,w,h*0.25);
  OkButton = new TGTextButton(h1, "Ok");
  OkButton->Connect("Clicked()","SpectraListFrame",this,"DoOk()");
  CancelButton = new TGTextButton(h1, "Cancel");
  CancelButton->Connect("Clicked()","SpectraListFrame",this,"DoCancel()");
  h1->AddFrame(OkButton, bhints);
  h1->AddFrame(CancelButton, bhints);

  fMain->AddFrame(v1, bhints);
  fMain->AddFrame(h1, bhints);
  fMain->SetWindowName("Spectrum List");
  fMain->MapSubwindows();
  fMain->Resize();
  fMain->CenterOnParent();
  fMain->MapWindow();
}

//Destructor
SpectraListFrame::~SpectraListFrame() {
  fMain->Cleanup();
  fMain->DeleteWindow();
}

void SpectraListFrame::CloseWindow() {
  delete this;
}

void SpectraListFrame::DoOk() {
  CancelButton->SetState(kButtonDisabled);
  OkButton->SetState(kButtonDisabled);
  //waits and then closes
  TTimer::SingleShot(150,"SpectraListFrame",this,"CloseWindow()");
}

void SpectraListFrame::DoCancel() {
  CancelButton->SetState(kButtonDisabled);
  OkButton->SetState(kButtonDisabled);
  //waits and then closes
  TTimer::SingleShot(150,"SpectraListFrame",this,"CloseWindow()");
}

void SpectraListFrame::HandleDoubleClick(TGListTreeItem *item, Int_t btn) {
  if(btn != kButton1 || !item || (Bool_t)item->GetUserData()) return;
  try{
    item->SetUserData((void*)1);
    string s(item->GetText());
    auto info = local_map.at(s);
    TObject *obj = info.first;
    vector<string> cmds = info.second;
    string type = "";
    if(obj->IsA() == TH1F::Class()) type = "TH1F";
    else if(obj->IsA() == TH1I::Class()) type = "TH1I";
    else if(obj->IsA() == TH2F::Class()) type = "TH2F";
    else if(obj->IsA() == TH2I::Class()) type = "TH2I";

    if(type == "TH1I" || type == "TH1F") {
      string temp = "", c;
      string varx;
      TH1 *h = (TH1*) obj;
      for(unsigned int i=0; i<cmds[0].size(); i++) {
        c = cmds[0][i];
        if(c == ">") {
          varx = temp;
          temp = "";
          break;
        } else {
          temp += c;
        }
      }
      string name = "Type: "+type;
      fTree->AddItem(item, name.c_str());
      name = varx+" {"+to_string(h->GetNbinsX())+","+to_string(h->GetXaxis()->GetXmin())+
                      ","+to_string(h->GetXaxis()->GetXmax())+"}";
      fTree->AddItem(item, name.c_str(),fxIcon,fxIcon);
      if(cmds[1] != "") fTree->AddItem(item, cmds[1].c_str(),fcutIcon,fcutIcon);
    } else if (type == "TH2F" || type == "TH1I") {
      string temp = "", c;
      string varx, vary;
      TH2 *h = (TH2*) obj;
      for(unsigned int i=0; i<cmds[0].size(); i++) {
        c = cmds[0][i];
        if(c==">") {
          varx = temp;
          break;
        } else if(c==":") {
          vary = temp;
          temp = "";
        } else {
          temp += c;
        }
      }
      string name = "Type: "+type;
      fTree->AddItem(item, name.c_str());
      name = varx+" {"+to_string(h->GetNbinsX())+","+to_string(h->GetXaxis()->GetXmin())+
                      ","+to_string(h->GetXaxis()->GetXmax())+"}";
      fTree->AddItem(item,name.c_str(),fxIcon,fxIcon);
      name = vary+" {"+to_string(h->GetNbinsY())+","+to_string(h->GetYaxis()->GetXmin())+
                      ","+to_string(h->GetYaxis()->GetXmax())+"}";
      fTree->AddItem(item, name.c_str(), fyIcon, fyIcon);
      if(cmds[1] != "") fTree->AddItem(item, cmds[1].c_str(),fcutIcon,fcutIcon);
    }
  } catch(out_of_range& oor) {}
}
