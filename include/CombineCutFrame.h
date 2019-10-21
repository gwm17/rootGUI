/*CombineCutFrame.h
 *Frame which allows user to combine multiple prexisiting cuts into a single cut
 *with AND, OR, and NOT options.
 *
 *Gordon M. Oct 2019
 */
#ifndef COMBO_CUT_FRAME_H
#define COMBO_CUT_FRAME_H

#include <TROOT.h>
#include <RQ_OBJECT.h>
#include <TGWindow.h>
#include <TQObject.h>
#include <TGFrame.h>
#include <TGCanvas.h>
#include <TGPicture.h>
#include <TGListTree.h>
#include <TGTextEntry.h>
#include <TGButton.h>
#include <TGButtonGroup.h>
#include <vector>
#include <string>
#include "RootView.h"

using namespace std;

class CombineCutFrame {

  RQ_OBJECT("CombineCutFrame");
  public:
    CombineCutFrame(const TGWindow *p, const TGWindow *main, UInt_t w, UInt_t h,
                    MyMainFrame *parent, vector<string> cuts);
    virtual ~CombineCutFrame();
    void CloseWindow();
    void DoOk();
    void DoCancel();
    void SetType(Int_t id);
    void AddCut(TGListTreeItem *item, Int_t btn);
    void RemoveCut();
    void SendCut(char *name, char *cut); // *SIGNAL*
    ClassDef(CombineCutFrame, 0);

  private:
    enum CutTypes {
       AND,
       OR,
       NOT
    };

    TGTransientFrame *fMain;
    TGTextButton *OkButton, *CancelButton;
    TGTextEntry *CutName, *CutInfo;
    TGCanvas *fCanvas;
    TGListTree *fTree;
    const TGPicture *fIcon;
    TGButtonGroup *fOptions;
    TGRadioButton *opts[3];
    vector<string> selectedCuts;
    string type_str; 
   
};

#endif
