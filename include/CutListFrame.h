#ifndef CUT_LIST_FRAME_H
#define CUT_LIST_FRAME_H

#include <TROOT.h>
#include <TQObject.h>
#include <RQ_OBJECT.h>
#include <TGButton.h>
#include <TGFrame.h>
#include <TGCanvas.h>
#include <TGPicture.h>
#include <TGListTree.h>
#include <unordered_map>
#include <vector>
#include <string>
#include "RootView.h"

using namespace std;

class CutListFrame {
  
  RQ_OBJECT("CutListFrame");
  public:
    CutListFrame(const TGWindow *p, const TGWindow *main, UInt_t w, UInt_t h, 
                 MyMainFrame *parent, vector<string> cuts, unordered_map<string, string> cmap);
    virtual ~CutListFrame();
    void CloseWindow();
    void DoOk();
    void DoCancel();
    void HandleDoubleClick(TGListTreeItem *item, Int_t btn);
    ClassDef(CutListFrame, 0);

  private:
    TGTransientFrame *fMain;
    TGTextButton *OkButton, *CancelButton;
    TGCanvas *fCanvas;
    TGListTree *fTree;
    unordered_map<string, string> local_map;
    vector<string> clist;
    const TGPicture *fCutIcon;
    const TGPicture *fHistIcon;

};

#endif
