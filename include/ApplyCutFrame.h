#ifndef APPLY_CUT_FRAME
#define APPLY_CUT_FRAME

#include <TROOT.h>
#include <TGFrame.h>
#include <RQ_OBJECT.h>
#include <TQObject.h>
#include <TGWindow.h>
#include <TGButton.h>
#include <TGComboBox.h>
#include <vector>
#include <string>
#include "RootView.h"

using namespace std;

class ApplyCutFrame {
  
  RQ_OBJECT("ApplyCutFrame");
  public:
    ApplyCutFrame(const TGWindow *p, const TGWindow *main, UInt_t w, UInt_t h, 
                  MyMainFrame *parent, vector<string> cuts, vector<string> spectra);
    virtual ~ApplyCutFrame();
    void CloseWindow();
    void DoOk();
    void DoCancel();
    void Signaled(Int_t box_id, Int_t entry_id);
    void SendName(char *sname, char *cname); // *SIGNAL*    
    ClassDef(ApplyCutFrame, 0);

  private:
    TGTransientFrame *fMain;
    TGTextButton *OkButton, *CancelButton;
    TGComboBox *cbox, *sbox;
    vector<string> slist, clist;
    Int_t s_signal, c_signal;
    const Int_t required_signals = 2;
};

#endif
