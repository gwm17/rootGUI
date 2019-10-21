#ifndef FILE_CONFIG_FRAME_H
#define FILE_CONFIG_FRAME_H

#include <TROOT.h>
#include <TGFrame.h>
#include <TQObject.h>
#include <RQ_OBJECT.h>
#include <TGButton.h>
#include <TGTextEntry.h>
#include <string>
#include <TGFSContainer.h>
#include <TGListView.h>
#include <TString.h>

#include "RootView.h"

using namespace std;

class FileConfigFrame {
  RQ_OBJECT("FileConfigFrame");
  public:
    FileConfigFrame(const TGWindow *p,const TGWindow *main,UInt_t w,UInt_t h,
                 MyMainFrame *parent, Int_t save);
    virtual ~FileConfigFrame();
    void DoOk();
    void DoCancel();
    void CloseWindow();
    void DoDoubleClick(TGLVEntry *entry, Int_t id);
    void DisplayDir(const TString &name);
    void SendText(char* text); // *SIGNAL*
    ClassDef(FileConfigFrame, 0);

  private:
    TGTransientFrame *fMain;
    TGTextButton *fOkButton, *fCancelButton;
    TGTextEntry *fFNameField;
    TGFileContainer *fContents;
    TGListView *fViewer;
};

#endif
