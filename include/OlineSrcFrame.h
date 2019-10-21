/*OlineSrcFrame.h
 *Frame for selecting an online data source, currently of NSCLDAQ origins.
 *Signal sent to MyMainFrame
 *
 *Gordon M. Oct 2019
 */
#ifndef OLINE_SRC_FRAME_H
#define OLINE_SRC_FRAME_H

#include <TGWindow.h>
#include <TQObject.h>
#include <RQ_OBJECT.h>
#include <TGFrame.h>
#include <TGButton.h>
#include <TGTextEntry.h>
#include <string>

#include "RootView.h"

using namespace std;

class OlineSrcFrame {
  RQ_OBJECT("OlineSrcFrame");
  public:
    OlineSrcFrame(const TGWindow *p,const TGWindow *main, UInt_t w, UInt_t h,
                  MyMainFrame *parent);
    virtual ~OlineSrcFrame();
    void DoOk();
    void DoCancel();
    void CloseWindow();
    void SendText(char* text); // *SIGNAL*
    ClassDef(OlineSrcFrame, 0);

  private:
    TGTransientFrame *fMain;
    TGTextButton *fOkButton, *fCancelButton;
    TGTextEntry *fHostField, *fRingField;

};

#endif
