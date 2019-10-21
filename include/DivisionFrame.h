/*DivisonFrame.h
 *
 *RELIC
 *
 *Gordon M. Oct 2019
 */
#ifndef DIVISION_FRAME_H
#define DIVISION_FRAME_H

#include <TROOT.h>
#include <TGFrame.h>
#include <TQObject.h>
#include <RQ_OBJECT.h>
#include <TGButton.h>
#include <TGButtonGroup.h>
#include "RootView.h"

using namespace std;

class DivisionFrame {
  RQ_OBJECT("DivisionFrame"); 
  public:
    DivisionFrame(const TGWindow *p, const TGWindow *main, UInt_t w, UInt_t h, 
                  MyMainFrame *parent);
    virtual ~DivisionFrame();
    void DoOk();
    void DoCancel();
    void CloseWindow();
    void DoRowsClicked(Int_t id);
    void DoColumnsClicked(Int_t id);
    void SendValues(Int_t rows, Int_t columns); // *SIGNAL*
    ClassDef(DivisionFrame,0);

  private:
    TGTransientFrame *fMain;
    TGButtonGroup *rows;
    TGButtonGroup *columns;
    TGTextButton *fOkButton;
    TGTextButton *fCancelButton;
    TGRadioButton *r1[4], *r2[4];
    Int_t r;
    Int_t c;
};
#endif
