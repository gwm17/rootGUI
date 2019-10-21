#ifndef ROOTVIEW_H
#define ROOTVIEW_H

#include <TGWindow.h>
#include <TQObject.h>
#include <RQ_OBJECT.h>
#include <TGButton.h>
#include <TGFrame.h>
#include <TRootEmbeddedCanvas.h>
#include <TGComboBox.h>
#include <TGStatusBar.h>
#include <TGButtonGroup.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>
#include <TCutG.h>
#include <TCut.h>
#include <TLine.h>
#include <THashTable.h>
#include <string>
#include <TThread.h>
#include <TString.h>
#include <TGLabel.h>
#include <unordered_map>
#include "rootConversion.h"

using namespace std;

class MyMainFrame {
  
  RQ_OBJECT("MyMainFrame");

  public:
    MyMainFrame(const TGWindow *p, UInt_t w, UInt_t h);
    virtual ~MyMainFrame();
    void CloseWindow();
    void DoDraw();
    void DoClear();
    void ResetContainers();
    void LoadConfig();
    void SaveConfig();
    void DoMenuPrompt(Int_t id);
    void SetURI(char* input_uri);
    void HandleRClick(Int_t id) { rows = id; SetCanvasDiv(rows, columns);}
    void HandleCClick(Int_t id) { columns = id; SetCanvasDiv(rows, columns);}
    void SetCanvasDiv(Int_t r, Int_t c);
    void SetConfig(char* config);
    void WriteConfig(char* config);
    void SetAvailSpectra();
    void Do2DCut();
    void Do1DCut();
    void DoComboCut();
    void DoApplyCut();
    void Get2DCut(char *name, Int_t pd);
    void Get1DCut(char *name, Int_t pd);
    void GetComboCut(char *name, char *title); 
    void ApplyCut(char *sname, char *cname);
    void Add1DSpectrum(char*,char*,char*,Float_t,Float_t,Int_t,char*);
    void Add2DSpectrum(char*,char*,char*,char*,Float_t,Float_t,Float_t,Float_t,Int_t,
                       Int_t,char*);
    void SetPad(Int_t spb_id, Int_t spec_id);
    void StopStart();
    void HandleCanvas(Int_t event, Int_t x, Int_t y, TObject* selected);
    static void* ThreadFunc(void *ptr);
    TThread* GetThread() {return fThread;};
    TCanvas* GetCanvas() {return fC1;};
    converter* GetConverter() {return take_me_home;};
    TObject* GetCut(const char *name) {string n(name); return clist[n];};
    ClassDef(MyMainFrame, 0);

  private:
    TRootEmbeddedCanvas *fECanvas;
    TCanvas *fC1;
    TH1F *fh1;
    TH1F *fh2;
    TThread *fThread;
    TGMainFrame *fMain;
    TGTextButton *stpstr;
    vector<string> spectra;
    vector<string> variables;
    vector<pair<string,string>> var_defs;
    vector<string> cuts;
    TGComboBox *specboxes[16];
    TGLabel *spb_labels[16];
    TGStatusBar* status;
    TGRadioButton *r1[4], *r2[4];
    converter *take_me_home;
    unordered_map<string, pair<TObject*,vector<string>>> hmap;
    unordered_map<string, string> pmap;
    unordered_map<string, string> cmap;
    unordered_map<string, TObject*> clist;
    string fURI;
    string fConfig;
    UInt_t MAIN_H;
    UInt_t MAIN_W;
    Bool_t kActive;
    Int_t nPads;
    Int_t clickedX, clickedY;
    Int_t rows, columns;

};

#endif
