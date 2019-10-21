#include "SpectraFrame.h"
#include <TGLabel.h>
#include <TTimer.h>

using namespace std;

enum histos {
  BLANK,
  TYPE_1F,
  TYPE_1I,
  TYPE_2F,
  TYPE_2I,

  VAR1_ID,
  VAR2_ID,
  HTYPE_ID,
  CUTS_ID,
  V1MIN_ID,
  V1MAX_ID,
  V2MIN_ID,
  V2MAX_ID,
  V1BINS_ID,
  V2BINS_ID
};

SpectraFrame::SpectraFrame(const TGWindow *p, const TGWindow *main, UInt_t w, UInt_t h,
                           MyMainFrame *parent, vector<string> vars, vector<string> cuts) {
  fMain = new TGTransientFrame(p,main,w,h);
  fMain->SetCleanup(kDeepCleanup);
  fMain->DontCallClose();

  variables = vars;
  cutgs = cuts;
  htype = BLANK;
  required_signals = 10;
  recieved_signals = 0;

  TGLayoutHints *basic_hints = new TGLayoutHints(kLHintsCenterX|kLHintsCenterY,5,5,5,5);

  TGHorizontalFrame *h1 = new TGHorizontalFrame(fMain, w, h*0.25);
  
  TGVerticalFrame *htypeframe = new TGVerticalFrame(h1,w*0.2, h*0.25);
  TGLabel *htl = new TGLabel(htypeframe, "Histogram Type");
  HistoType = new TGComboBox(htypeframe,HTYPE_ID);
  HistoType->Resize(150,20);
  HistoType->Connect("Selected(Int_t,Int_t)","SpectraFrame",this,"SetOptions(Int_t,Int_t)");
  HistoType->AddEntry("TH1F", TYPE_1F);
  HistoType->AddEntry("TH1I", TYPE_1I);
  HistoType->AddEntry("TH2F", TYPE_2F);
  HistoType->AddEntry("TH2I", TYPE_2I);
  htypeframe->AddFrame(htl, basic_hints);
  htypeframe->AddFrame(HistoType, basic_hints);

  TGVerticalFrame *titleframe = new TGVerticalFrame(h1, w*0.2, h*0.25);
  TGLabel *tl = new TGLabel(titleframe, "Title");
  TGTextBuffer *titleBuf;
  TitleBox = new TGTextEntry(titleframe, titleBuf = new TGTextBuffer(50));
  TitleBox->Resize(w*0.25,TitleBox->GetDefaultHeight());
  titleframe->AddFrame(tl, basic_hints);
  titleframe->AddFrame(TitleBox, basic_hints);

  TGVerticalFrame *var1frame = new TGVerticalFrame(h1, w*0.2, h*0.25);
  TGLabel *v1l = new TGLabel(var1frame, "Variable X");
  Var1 = new TGComboBox(var1frame, VAR1_ID);
  Var1->Resize(150,20);
  for(unsigned int i=0; i<variables.size(); i++) {
    Var1->AddEntry(variables[i].c_str(), i);
  }
  Var1->Connect("Selected(Int_t,Int_t)","SpectraFrame",this,"Signaled()");
  Var1->SetEnabled(false);
  var1frame->AddFrame(v1l, basic_hints);
  var1frame->AddFrame(Var1, basic_hints);

  TGVerticalFrame *var2frame = new TGVerticalFrame(h1, w*0.2, h*0.25);
  TGLabel *v2l = new TGLabel(var2frame, "Variable Y");
  Var2 = new TGComboBox(var2frame, VAR2_ID);
  Var2->Resize(150,20);
  for(unsigned int i=0; i<variables.size(); i++) {
    Var2->AddEntry(variables[i].c_str(), i);
  }
  Var2->Connect("Selected(Int_t,Int_t)","SpectraFrame",this,"Signaled()");
  Var2->SetEnabled(false);
  var2frame->AddFrame(v2l, basic_hints);
  var2frame->AddFrame(Var2, basic_hints);

  TGVerticalFrame *cutframe = new TGVerticalFrame(h1, w*0.2, h*0.25);
  TGLabel *cl = new TGLabel(cutframe, "Cut");
  Cuts = new TGComboBox(cutframe, CUTS_ID);
  Cuts->Resize(150,20);
  Cuts->AddEntry("none",-1);
  for(unsigned int i=0; i<cuts.size(); i++) {
    Cuts->AddEntry(cuts[i].c_str(), i);
  }
  Cuts->Connect("Selected(Int_t,Int_t)","SpectraFrame",this,"Signaled()");
  Cuts->SetEnabled(false);
  cutframe->AddFrame(cl, basic_hints);
  cutframe->AddFrame(Cuts, basic_hints);
 
  h1->AddFrame(htypeframe, basic_hints);
  h1->AddFrame(titleframe, basic_hints);
  h1->AddFrame(var1frame, basic_hints);
  h1->AddFrame(var2frame, basic_hints); 
  h1->AddFrame(cutframe, basic_hints);

  TGHorizontalFrame *h2 = new TGHorizontalFrame(fMain, w, h*0.25);
  
  TGVerticalFrame *v1minframe = new TGVerticalFrame(h2, w*0.33, h*0.25);
  TGLabel *v1minl = new TGLabel(v1minframe, "Var X Min");
  Var1Min = new TGNumberEntry(v1minframe,0,9,V1MIN_ID,TGNumberFormat::kNESReal,
                              TGNumberFormat::kNEAAnyNumber,TGNumberFormat::kNELNoLimits);
  Var1Min->Resize(150,20);
  Var1Min->SetState(false);
  v1minframe->AddFrame(v1minl, basic_hints);
  v1minframe->AddFrame(Var1Min, basic_hints);
  
  TGVerticalFrame *v1maxframe = new TGVerticalFrame(h2, w*0.33, h*0.25);
  TGLabel *v1maxl = new TGLabel(v1maxframe, "Var X Max");
  Var1Max = new TGNumberEntry(v1maxframe,0,9,V1MAX_ID,TGNumberFormat::kNESReal,
                              TGNumberFormat::kNEAAnyNumber,TGNumberFormat::kNELNoLimits);
  Var1Max->Resize(150,20);
  Var1Max->SetState(false);
  v1maxframe->AddFrame(v1maxl, basic_hints);
  v1maxframe->AddFrame(Var1Max, basic_hints);

  TGVerticalFrame *v1binframe = new TGVerticalFrame(h2, w*0.33, h*0.25);
  TGLabel *v1bl = new TGLabel(v1binframe, "Var X Bins");
  Var1Bins = new TGNumberEntry(v1binframe,0,9,V1BINS_ID,TGNumberFormat::kNESInteger,
                               TGNumberFormat::kNEAPositive,TGNumberFormat::kNELNoLimits);
  Var1Bins->Resize(150,20);
  Var1Bins->SetState(false);
  v1binframe->AddFrame(v1bl, basic_hints);
  v1binframe->AddFrame(Var1Bins, basic_hints);

  h2->AddFrame(v1binframe, basic_hints);
  h2->AddFrame(v1minframe, basic_hints);
  h2->AddFrame(v1maxframe, basic_hints);

  TGHorizontalFrame *h3 = new TGHorizontalFrame(fMain, w, h*0.25);
  
  TGVerticalFrame *v2minframe = new TGVerticalFrame(h3, w*0.33, h*0.25);
  TGLabel *v2minl = new TGLabel(v2minframe, "Var Y Min");
  Var2Min = new TGNumberEntry(v2minframe,0,9,V2MIN_ID,TGNumberFormat::kNESReal,
                              TGNumberFormat::kNEAAnyNumber,TGNumberFormat::kNELNoLimits);
  Var2Min->Resize(150,20);
  Var2Min->SetState(false);
  v2minframe->AddFrame(v2minl, basic_hints);
  v2minframe->AddFrame(Var2Min, basic_hints);
  
  TGVerticalFrame *v2maxframe = new TGVerticalFrame(h3, w*0.33, h*0.25);
  TGLabel *v2maxl = new TGLabel(v2maxframe, "Var Y Max");
  Var2Max = new TGNumberEntry(v2maxframe,0,9,V2MAX_ID,TGNumberFormat::kNESReal,
                              TGNumberFormat::kNEAAnyNumber,TGNumberFormat::kNELNoLimits);
  Var2Max->Resize(150,20);
  Var2Max->SetState(false);
  v2maxframe->AddFrame(v2maxl, basic_hints);
  v2maxframe->AddFrame(Var2Max, basic_hints);

  TGVerticalFrame *v2binframe = new TGVerticalFrame(h3, w*0.33, h*0.25);
  TGLabel *v2bl = new TGLabel(v2binframe, "Var Y Bins");
  Var2Bins = new TGNumberEntry(v2binframe,0,9,V2BINS_ID,TGNumberFormat::kNESInteger,
                               TGNumberFormat::kNEAPositive,TGNumberFormat::kNELNoLimits);
  Var2Bins->Resize(150,20);
  Var2Bins->SetState(false);
  v2binframe->AddFrame(v2bl, basic_hints);
  v2binframe->AddFrame(Var2Bins, basic_hints);

  h3->AddFrame(v2binframe, basic_hints);
  h3->AddFrame(v2minframe, basic_hints);
  h3->AddFrame(v2maxframe, basic_hints);

  TGHorizontalFrame *h4 = new TGHorizontalFrame(fMain,w,h*0.25);
  
  OkButton = new TGTextButton(h4, "Ok");
  OkButton->Connect("Clicked()","SpectraFrame",this,"DoOk()");
  OkButton->SetState(kButtonDisabled);
  CancelButton = new TGTextButton(h4, "Cancel");
  CancelButton->Connect("Clicked()","SpectraFrame",this,"DoCancel()");

  h4->AddFrame(OkButton, basic_hints);
  h4->AddFrame(CancelButton, basic_hints);
  
  Connect("Send1DSpectrum(char*,char*,char*,Float_t,Float_t,Int_t,char*)",
          "MyMainFrame",parent,
          "Add1DSpectrum(char*,char*,char*,Float_t,Float_t,Int_t,char*)");
  Connect("Send2DSpectrum(char*,char*,char*,char*,Float_t,Float_t,Float_t,Float_t,Int_t,Int_t,char*)","MyMainFrame",parent,
          "Add2DSpectrum(char*,char*,char*,char*,Float_t,Float_t,Float_t,Float_t,Int_t,Int_t,char*)");
  fMain->AddFrame(h1, basic_hints);
  fMain->AddFrame(h2, basic_hints);
  fMain->AddFrame(h3, basic_hints);
  fMain->AddFrame(h4, basic_hints);
  fMain->SetWindowName("Create New Spectrum");
  fMain->MapSubwindows();
  fMain->Resize();
  fMain->CenterOnParent();
  fMain->MapWindow();
}

SpectraFrame::~SpectraFrame() {
  fMain->Cleanup();
  fMain->DeleteWindow();
}

void SpectraFrame::CloseWindow() {
  delete this;
}

void SpectraFrame::DoOk() {
  OkButton->SetState(kButtonDisabled);
  CancelButton->SetState(kButtonDisabled);
  if(htype == TYPE_1F || htype == TYPE_1I) {
    const char *n = TitleBox->GetText();
    const char *t = HistoType->GetSelectedEntry()->GetTitle();
    const char *vx = Var1->GetSelectedEntry()->GetTitle();
    char name[50], type[50], varx[50], cut[50];
    strcpy(name, n); strcpy(type, t); strcpy(varx, vx);
    Float_t minx = Var1Min->GetNumber();
    Float_t maxx = Var1Max->GetNumber();
    Int_t binsx = Var1Bins->GetNumber();
    const char *c = Cuts->GetSelectedEntry()->GetTitle();
    strcpy(cut, c);
    Send1DSpectrum(type,name,varx,minx,maxx,binsx,cut);
  } else if (htype == TYPE_2F || htype == TYPE_2I) {
    const char* n = TitleBox->GetText();
    const char* t = HistoType->GetSelectedEntry()->GetTitle();
    const char* vx = Var1->GetSelectedEntry()->GetTitle();
    const char* vy = Var2->GetSelectedEntry()->GetTitle();
    char name[50], type[50], varx[50], vary[50], cut[50];
    strcpy(name, n); strcpy(type, t); strcpy(varx, vx); strcpy(vary, vy);
    Float_t minx = Var1Min->GetNumber();
    Float_t maxx = Var1Max->GetNumber();
    Int_t binsx = Var1Bins->GetNumber();
    Float_t miny = Var2Min->GetNumber();
    Float_t maxy = Var2Max->GetNumber();
    Int_t binsy = Var2Bins->GetNumber();
    const char* c = Cuts->GetSelectedEntry()->GetTitle();
    strcpy(cut, c);
    Send2DSpectrum(type,name,varx,vary,minx,miny,maxx,maxy,binsx,binsy,cut);
  }
  TTimer::SingleShot(150,"SpectraFrame",this,"CloseWindow()");
}

void SpectraFrame::DoCancel() {
  OkButton->SetState(kButtonDisabled);
  CancelButton->SetState(kButtonDisabled);
  TTimer::SingleShot(150,"SpectraFrame",this,"CloseWindow()");
}

void SpectraFrame::SetOptions(Int_t box_id, Int_t type) {
  if(box_id != HTYPE_ID) return;
  htype = type;
  if(type == TYPE_1F || type == TYPE_1I) {
    Var1->SetEnabled(true);
    Var2->SetEnabled(false);
    Cuts->SetEnabled(true);
    Var1Min->SetState(true);
    Var1Max->SetState(true);
    Var1Bins->SetState(true);
    Var2Min->SetState(false);
    Var2Max->SetState(false);
    Var2Bins->SetState(false);
    required_signals = 2;
  } else if (type == TYPE_2F || type == TYPE_2I) {
    Var1->SetEnabled(true);
    Var2->SetEnabled(true);
    Cuts->SetEnabled(true);
    Var1Min->SetState(true);
    Var1Max->SetState(true);
    Var1Bins->SetState(true);
    Var2Min->SetState(true);
    Var2Max->SetState(true);
    Var2Bins->SetState(true);
    required_signals = 3;
  }
}

void SpectraFrame::Signaled() {
  recieved_signals++;
  if(recieved_signals>=required_signals) {
    OkButton->SetState(kButtonUp);
  }
}

void SpectraFrame::Send2DSpectrum(char* type, char* name, char* varx, char* vary,
                                  Float_t minx, Float_t miny, Float_t maxx, Float_t maxy,
                                  Int_t binsx, Int_t binsy, char* cut) { 
  EmitVA<char*,char*,char*,char*,Float_t,Float_t,Float_t,Float_t,Int_t,Int_t,char*>
        ("Send2DSpectrum(char*,char*,char*,char*,Float_t,Float_t,Float_t,Float_t,Int_t,Int_t,char*)",11,type,name,varx,vary,minx,miny,maxx,maxy,binsx,binsy,cut);
}

void SpectraFrame::Send1DSpectrum(char* type, char* name, char* varx, Float_t minx,
                                  Float_t maxx, Int_t binsx, char* cut) {
  EmitVA<char*,char*,char*,Float_t,Float_t,Int_t,char*>
        ("Send1DSpectrum(char*,char*,char*,Float_t,Float_t,Int_t,char*)",
         7,type,name,varx,minx,maxx,binsx,cut);
}
