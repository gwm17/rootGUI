#ifndef SPECTRA_LIST_FRAME_H
#define SPECTRA_LIST_FRAME_H

#include <TROOT.h>
#include <RQ_OBJECT.h>
#include <TQObject.h>
#include <TGFrame.h>
#include <TGCanvas.h>
#include <TGButton.h>
#include <TGListTree.h>
#include <TGPicture.h>
#include <vector>
#include <string>
#include "RootView.h"
#include <unordered_map>

using namespace std;

class SpectraListFrame {
  RQ_OBJECT("SpectraListFrame");
  public:
    SpectraListFrame(const TGWindow *p, const TGWindow *main, UInt_t w, UInt_t h,
                     MyMainFrame *parent, vector<string> spectra,
                     unordered_map<string,pair<TObject*,vector<string>>> hmap);
    virtual ~SpectraListFrame();
    void CloseWindow();
    void DoOk();
    void DoCancel();
    void HandleDoubleClick(TGListTreeItem *item, Int_t btn);
    ClassDef(SpectraListFrame, 0);

  private:
    TGTransientFrame *fMain;
    TGTextButton *OkButton, *CancelButton;
    TGCanvas *fCanvas;
    TGListTree *fTree;
    unordered_map<string, pair<TObject*,vector<string>>> local_map;
    vector<string> slist;
    const TGPicture *fh1Icon;
    const TGPicture *fh2Icon;
    const TGPicture *fxIcon;
    const TGPicture *fyIcon;
    const TGPicture *fcutIcon;

};

#endif
