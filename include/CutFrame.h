/*CutFrame.h
 *Frame which prompts user for information necessary to make a cut.
 *Used for both 1D (TCut) and 2D (TCutG) cuts. Overwrites allowed
 *
 *Gordon M. Oct 2019
 */
#ifndef CUTFRAME_H
#define CUTFRAME_H

#include <TROOT.h>
#include <TQObject.h>
#include <RQ_OBJECT.h>
#include <TGWindow.h>
#include <TGFrame.h>
#include <TGButton.h>
#include <TGTextEntry.h>
#include <TGTextBuffer.h>
#include <TGComboBox.h>
#include <string>
#include "RootView.h"
#include <TGLabel.h>

using namespace std;

class CutFrame {
  
  RQ_OBJECT("CutFrame");
  public:
    CutFrame(const TGWindow *p, const TGWindow *main, UInt_t w, UInt_t h,
             MyMainFrame *parent, Int_t np, Int_t dF);
    virtual ~CutFrame();
    void DoOk();
    void DoCancel();
    void CloseWindow();
    void SetPad(Int_t box,Int_t entry);
    void SendCut(char* text, Int_t p); // *SIGNAL*
    ClassDef(CutFrame, 0);

  private:
    Int_t pad, nPads, dimFlag;
    TGTransientFrame *fMain;
    TGTextButton *OkButton, *CancelButton;
    TGTextEntry *CutName;
    TGComboBox *padSelect;
};

#endif
