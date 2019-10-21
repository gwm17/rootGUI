#ifndef FILE_SRC_FRAME_H
#define FILE_SRC_FRAME_H

#include <TROOT.h>
#include <TGFrame.h>
#include <TQObject.h>
#include <RQ_OBJECT.h>
#include <TGButton.h>
#include <TGTextEntry.h>
#include <string>
#include <TGFSContainer.h>
#include <TGListView.h>

#include "RootView.h"

using namespace std;

class FileSrcFrame {
  RQ_OBJECT("FileSrcFrame");
  public:
    FileSrcFrame(const TGWindow *p,const TGWindow *main,UInt_t w,UInt_t h,
                 MyMainFrame *parent);
    virtual ~FileSrcFrame();
    void DoOk();
    void DoCancel();
    void CloseWindow();
    void DoDoubleClick(TGLVEntry *entry, Int_t id);
    void DisplayDir(const TString &name);
    void SendText(char* text); // *SIGNAL*
    ClassDef(FileSrcFrame, 0);

  private:
    TGTransientFrame *fMain;
    TGTextButton *fOkButton, *fCancelButton;
    TGTextEntry *fFNameField;
    TGFileContainer *fContents;
    TGListView *fViewer;
};

#endif
