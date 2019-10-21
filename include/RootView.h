/*RootView.h
 *Makes a main frame for the RootView GUI called MyMainFrame
 *Does a whole bunch of fun stuff, most of which is outlined in
 *https://root.cern.ch/root/htmldoc/guides/users-guide/WritingGUI.html and
 *https://root.cern/doc/master/guitest_8C.html and
 *https://pep-root6.github.io/docs/analysis/parallell/root.html (for threading).
 *Still work in progress, let me know at gmccann@fsu.edu if you find any room for improvement
 *(there definitely is)
 *
 *Gordon M. Oct 2019
 */

#ifndef ROOTVIEW_H
#define ROOTVIEW_H

#include <TGWindow.h>
#include <TQObject.h>
#include <RQ_OBJECT.h>
#include <TGButton.h>
#include <TGFrame.h>
#include <TRootEmbeddedCanvas.h>
#include <TGComboBox.h>
#include <TGStatusBar.h>
#include <TGButtonGroup.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>
#include <TCutG.h>
#include <TCut.h>
#include <TLine.h>
#include <THashTable.h>
#include <string>
#include <TThread.h>
#include <TString.h>
#include <TGLabel.h>
#include <unordered_map>
#include "rootConversion.h"

using namespace std;

class MyMainFrame {

  /*Add to list of graphical objects*/  
  RQ_OBJECT("MyMainFrame");

  public:
    /*Basic class operations*/
    MyMainFrame(const TGWindow *p, UInt_t w, UInt_t h);
    virtual ~MyMainFrame();
    void CloseWindow();

    /*Functions of config, draw, reset, etc*/
    void DoDraw();
    void DoClear();
    void ResetContainers();
    void LoadConfig();
    void SaveConfig();
    void DoMenuPrompt(Int_t id);
    void SetURI(char* input_uri);
    void HandleRClick(Int_t id) { rows = id; SetCanvasDiv(rows, columns);}
    void HandleCClick(Int_t id) { columns = id; SetCanvasDiv(rows, columns);}
    void SetCanvasDiv(Int_t r, Int_t c);
    void SetConfig(char* config);
    void WriteConfig(char* config);
    void SetAvailSpectra();
    void Do2DCut();
    void Do1DCut();
    void DoComboCut();
    void DoApplyCut();
    void Get2DCut(char *name, Int_t pd);
    void Get1DCut(char *name, Int_t pd);
    void GetComboCut(char *name, char *title); 
    void ApplyCut(char *sname, char *cname);
    void Add1DSpectrum(char*,char*,char*,Float_t,Float_t,Int_t,char*);
    void Add2DSpectrum(char*,char*,char*,char*,Float_t,Float_t,Float_t,Float_t,Int_t,
                       Int_t,char*);
    void SetPad(Int_t spb_id, Int_t spec_id);
    void StopStart();
    void HandleCanvas(Int_t event, Int_t x, Int_t y, TObject* selected);

    /*Threading function*/
    static void* ThreadFunc(void *ptr); //Must be of this format

    /*Access functions*/
    TThread* GetThread() {return fThread;};
    TCanvas* GetCanvas() {return fC1;};
    converter* GetConverter() {return take_me_home;};
    TObject* GetCut(const char *name) {string n(name); return clist[n];};

    /*Define class for ROOT*/
    ClassDef(MyMainFrame, 0);

  private:
    /*ROOT secondary thread*/
    TThread *fThread; //If only to be used offline, not needed

    /*ROOT Objects*/
    TRootEmbeddedCanvas *fECanvas;
    TCanvas *fC1;
    TGMainFrame *fMain;
    TGTextButton *stpstr, *draw, *exit, *load, *save, *clear;
    TGTextButton *Cut2DButton, *Cut1DButton, *ComboCutButton, *ApplyCutButton;
    TGComboBox *specboxes[16];
    TGLabel *spb_labels[16];
    TGStatusBar* status;
    TGRadioButton *r1[4], *r2[4];

    /*Converter*/
    converter *take_me_home; //VITAL

    /*Storage*/
    vector<string> spectra; //spectrum names
    vector<string> variables; //variable names
    vector<pair<string,string>> var_defs; //variable types with variable names
    vector<string> cuts; //cut names
    unordered_map<string, pair<TObject*,vector<string>>> hmap; //name to histo info
    unordered_map<string, string> pmap; //pad name to histogram name
    unordered_map<string, string> cmap; //cut name to histogram name
    unordered_map<string, TObject*> clist; //cut name to cut object

    /*class wide flags and variables*/
    string fURI; //name with protocol for data input
    string fConfig; //name of configuration fle
    UInt_t MAIN_H; //height of main frame
    UInt_t MAIN_W; //width of main frame
    Bool_t kActive; //flags state of secondary thread
    Int_t nPads; //global number of pads
    Int_t clickedX, clickedY; //global pad location
    Int_t rows, columns; //canvas divisions

};

#endif
