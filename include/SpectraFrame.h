#ifndef SPECTRAFRAME_H
#define SPECTRAFRAME_H

#include <TGWindow.h>
#include <TQObject.h>
#include <TGFrame.h>
#include <vector>
#include <string>
#include <TString.h>
#include <RQ_OBJECT.h>
#include "RootView.h"
#include <TGComboBox.h>
#include <TGNumberEntry.h>

using namespace std;

class SpectraFrame {

  RQ_OBJECT("SpectraFrame");
  public:
    SpectraFrame(const TGWindow *p, const TGWindow *main, UInt_t w, UInt_t h,
                 MyMainFrame *parent, vector<string> vars, vector<string> cuts);
    virtual ~SpectraFrame();
    void DoOk();
    void DoCancel();
    void CloseWindow();
    void SetOptions(Int_t box_id, Int_t type);
    void Signaled();
    void Send1DSpectrum(char*,char*,char*,Float_t,Float_t,Int_t,char*); // *SIGNAL*
    void Send2DSpectrum(char*,char*,char*,char*,Float_t,Float_t,
                        Float_t,Float_t,Int_t,Int_t,char*); // *SIGNAL*
    ClassDef(SpectraFrame,0);

  private:
    TGTransientFrame *fMain;
    TGTextButton *OkButton;
    TGTextButton *CancelButton;
    TGComboBox *Var1;
    TGComboBox *Var2;
    TGTextEntry *TitleBox;
    TGNumberEntry *Var1Min, *Var2Min, *Var1Max, *Var2Max, *Var1Bins, *Var2Bins;
    TGComboBox *HistoType;
    TGComboBox *Cuts;

    vector<string> variables;
    vector<string> cutgs;
    Int_t htype;
    int required_signals;
    int recieved_signals;
    
};


#endif
