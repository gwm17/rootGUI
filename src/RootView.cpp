/*RootView.cpp
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

#include "RootView.h"
#include "OlineSrcFrame.h"
#include "FileSrcFrame.h"
#include "FileConfigFrame.h"
#include "DivisionFrame.h"
#include "SpectraFrame.h"
#include "CutFrame.h"
#include "CombineCutFrame.h"
#include "SpectraListFrame.h"
#include "CutListFrame.h"
#include "ApplyCutFrame.h"
#include <TGClient.h>
#include <TF1.h>
#include <TRandom.h>
#include <TGMenu.h>
#include <TApplication.h>
#include <stdexcept>
#include <fstream>
#include <TClass.h>

using namespace std;

//ID's for menu choices
enum rvID {
  M_SOURCE_FILE,
  M_SOURCE_ONLINE,
  M_VIEW_DIVIDE,
  M_CREATE_SPECTRUM,
  M_LIST_SPECTRA,
  M_LIST_CUTS
};

/*MainFrame()
 *Constructor. Must initialize the structure of the GUI here, and then display the initial
 *state.
 */
MyMainFrame::MyMainFrame(const TGWindow *p, UInt_t w, UInt_t h) {
  //Init Main Frame along with global params
  fMain = new TGMainFrame(p,w,h);
  fMain->SetCleanup(kDeepCleanup);
  MAIN_H = h;
  MAIN_W = w;
  take_me_home = new converter();
  rows = 1;
  columns = 1;
  nPads = 1;

  //Init single canvas
  TGHorizontalFrame *hcanv = new TGHorizontalFrame(fMain, w,(h*0.75));

  TGVerticalFrame *v3 = new TGVerticalFrame(hcanv, w*0.5, h*0.75);
  fECanvas = new TRootEmbeddedCanvas("Ecanvas",v3,w*0.5,(h*0.75));
  fC1 = fECanvas->GetCanvas();
  fC1->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)","MyMainFrame",this,
               "HandleCanvas(Int_t,Int_t,Int_t,TObject*)");
  status = new TGStatusBar(v3, w*0.5, h*0.1, kHorizontalFrame);
  Int_t parts[] = {45,15,10,30};
  status->SetParts(parts, 4);
  status->Draw3DCorner(kFALSE);
  v3->AddFrame(fECanvas, new TGLayoutHints(kLHintsExpandX|kLHintsExpandY,5,5,5,5));
  v3->AddFrame(status, new TGLayoutHints(kLHintsExpandX|kLHintsBottom,5,5,5,5));

  //Vertical frame for plot options
  //Spacers are left,right,top,bottom
  TGGroupFrame *hspec = new TGGroupFrame(hcanv,"Spectra",kHorizontalFrame);
  TGVerticalFrame *v1 = new TGVerticalFrame(hspec,0.25*w,0.75*h); 
  TGVerticalFrame *v2 = new TGVerticalFrame(hspec,0.25*w,0.75*h);
  TGLayoutHints *entry_hints = new TGLayoutHints(kLHintsCenterX|kLHintsCenterY,5,5,5,5);
  TGLayoutHints *label_hints = new TGLayoutHints(kLHintsLeft|kLHintsCenterY,5,5,5,5);
  //Spectrum boxes
  for(int i=0; i<16; i++) {
    if(i<8) {
      spb_labels[i] = new TGLabel(v1, Form("Spectrum %i",i+1));
      specboxes[i] = new TGComboBox(v1, i+1);
      specboxes[i]->Resize(150,20);
      specboxes[i]->Connect("Selected(Int_t,Int_t)","MyMainFrame",this,"SetPad(Int_t,Int_t)");
      v1->AddFrame(spb_labels[i], label_hints);
      v1->AddFrame(specboxes[i], entry_hints);
    } else {
      spb_labels[i] = new TGLabel(v2, Form("Spectrum %i",i+1));
      specboxes[i] = new TGComboBox(v2, i+1);
      specboxes[i]->Resize(150,20);
      specboxes[i]->Connect("Selected(Int_t,Int_t)","MyMainFrame",this,"SetPad(Int_t,Int_t)");
      v2->AddFrame(spb_labels[i], label_hints);
      v2->AddFrame(specboxes[i], entry_hints);
    }
    if(i != 0) specboxes[i]->SetEnabled(false);    
  }
  //Division buttons
  TGButtonGroup *r = new TGButtonGroup(v1, "Rows", kVerticalFrame);
  r1[0] = new TGRadioButton(r, "1", 1);
  r1[1] = new TGRadioButton(r, "2", 2);
  r1[2] = new TGRadioButton(r, "3", 3);
  r1[3] = new TGRadioButton(r, "4", 4);
  r->Show();
  r->SetButton(1);
  r->Connect("Clicked(Int_t)","MyMainFrame",this,"HandleRClick(Int_t)");
  v1->AddFrame(r, entry_hints);
  TGButtonGroup *c = new TGButtonGroup(v2, "Columns", kVerticalFrame);
  r2[0] = new TGRadioButton(c, "1", 1);
  r2[1] = new TGRadioButton(c, "2", 2);
  r2[2] = new TGRadioButton(c, "3", 3);
  r2[3] = new TGRadioButton(c, "4", 4);
  c->Show();
  c->SetButton(1);
  c->Connect("Clicked(Int_t)","MyMainFrame",this,"HandleCClick(Int_t)");
  v2->AddFrame(c, entry_hints);

  //Cut buttons
  TGVerticalFrame *vcut = new TGVerticalFrame(hspec, 0.25*w, 0.25*h);
  Cut2DButton = new TGTextButton(vcut,"2DCut");
  Cut2DButton->Connect("Clicked()","MyMainFrame",this,"Do2DCut()");
  Cut1DButton = new TGTextButton(vcut,"1DCut");
  Cut1DButton->Connect("Clicked()","MyMainFrame",this,"Do1DCut()");
  ComboCutButton = new TGTextButton(vcut, "CombineCuts");
  ComboCutButton->Connect("Clicked()", "MyMainFrame", this, "DoComboCut()");
  ApplyCutButton = new TGTextButton(vcut, "ApplyCut");
  ApplyCutButton->Connect("Clicked()", "MyMainFrame", this, "DoApplyCut()");
  vcut->AddFrame(Cut2DButton,entry_hints);
  vcut->AddFrame(Cut1DButton,entry_hints);
  vcut->AddFrame(ComboCutButton,entry_hints);
  vcut->AddFrame(ApplyCutButton,entry_hints);

  hspec->AddFrame(v1, new TGLayoutHints(kLHintsCenterX|kLHintsBottom,10,5,5,5));
  hspec->AddFrame(v2, new TGLayoutHints(kLHintsCenterX|kLHintsBottom,10,5,5,5));
  hspec->AddFrame(vcut, new TGLayoutHints(kLHintsCenterX|kLHintsBottom,10,5,5,5));
  hcanv->AddFrame(hspec, new TGLayoutHints(kLHintsLeft|kLHintsCenterY,10,10,10,10));
  hcanv->AddFrame(v3, new TGLayoutHints(kLHintsRight|kLHintsCenterY,10,10,10,10));

  //Horizontal Frame with buttons for drawing and exiting
  TGHorizontalFrame *hframe = new TGHorizontalFrame(fMain,w,h*.15);
  TGLayoutHints *bhints = new TGLayoutHints(kLHintsCenterX|kLHintsCenterY,5,5,3,4);

  //GUI action buttons
  draw = new TGTextButton(hframe,"&Draw");
  draw->Connect("Clicked()","MyMainFrame",this,"DoDraw()");
  exit = new TGTextButton(hframe, "&Exit", "gApplication->Terminate()");
  stpstr = new TGTextButton(hframe, "&Start");
  stpstr->Connect("Clicked()","MyMainFrame",this,"StopStart()");
  load = new TGTextButton(hframe, "&Load");
  load->Connect("Clicked()","MyMainFrame",this,"LoadConfig()");
  save = new TGTextButton(hframe, "&Save");
  save->Connect("Clicked()","MyMainFrame",this,"SaveConfig()");
  clear = new TGTextButton(hframe, "&Clear");
  clear->Connect("Clicked()","MyMainFrame",this,"DoClear()");

  hframe->AddFrame(draw, bhints);
  hframe->AddFrame(exit, bhints);
  hframe->AddFrame(stpstr, bhints);
  hframe->AddFrame(load, bhints);
  hframe->AddFrame(save, bhints);
  hframe->AddFrame(clear, bhints);

  //Menu Bar avec menus for init source, saving, histos, etc
  TGMenuBar *fMenuBar = new TGMenuBar(fMain,w,h*0.1, kHorizontalFrame);
  TGLayoutHints *mbItemLayout = new TGLayoutHints(kLHintsTop|kLHintsLeft,0,4,0,0);
  
  //Menus
  TGPopupMenu *sourceMenu = new TGPopupMenu(gClient->GetRoot());
  sourceMenu->AddEntry("&File",M_SOURCE_FILE);
  sourceMenu->AddEntry("&Online",M_SOURCE_ONLINE);
  sourceMenu->Connect("Activated(Int_t)","MyMainFrame",this,"DoMenuPrompt(Int_t)");
  fMenuBar->AddPopup("&Source",sourceMenu,mbItemLayout);

  TGPopupMenu *viewMenu = new TGPopupMenu(gClient->GetRoot());
  viewMenu->AddEntry("&Divide Canvas",M_VIEW_DIVIDE);
  viewMenu->AddEntry("&List Spectra",M_LIST_SPECTRA);
  viewMenu->AddEntry("&List Cuts", M_LIST_CUTS);
  viewMenu->Connect("Activated(Int_t)","MyMainFrame",this,"DoMenuPrompt(Int_t)");
  fMenuBar->AddPopup("&View",viewMenu,mbItemLayout);

  TGPopupMenu *createMenu = new TGPopupMenu(gClient->GetRoot());
  createMenu->AddEntry("&New Spectrum",M_CREATE_SPECTRUM);
  createMenu->Connect("Activated(Int_t)","MyMainFrame",this,"DoMenuPrompt(Int_t)");
  fMenuBar->AddPopup("&Create",createMenu,mbItemLayout);

  //Adding all frames to the Main Frame (order impacts the way they stack)
  //and build main window
  fMain->AddFrame(fMenuBar);
  fMain->AddFrame(hcanv, new TGLayoutHints(kLHintsLeft|kLHintsCenterY,2,2,2,2));
  fMain->AddFrame(hframe, new TGLayoutHints(kLHintsCenterX,2,2,2,2));
  fMain->SetWindowName("ROOTView");
  fMain->MapSubwindows();
  fMain->Resize();
  fMain->MapWindow();

  //create thread and associations
  fThread = new TThread("MyThread", ThreadFunc, (void*) this);
}

/*~MyMainFrame()
 *Destructor
 */
MyMainFrame::~MyMainFrame() {
  fMain->Cleanup();//Destroys all associated frames
  delete take_me_home;
  delete fThread;
  delete fMain;
  for(unsigned int i=0; i<cuts.size(); i++) {
    delete clist[cuts[i]];
  }
}

/*CloseWindow()
 *Function called upon atempt to close the window
 */
void MyMainFrame::CloseWindow() {
  if(fThread->GetState() == TThread::kRunningState) {
    fThread->Kill();//If window closes & thread going, kill it (saves from zombies & orphans)
    stpstr->SetText("&Start");
    gClient->NeedRedraw(stpstr);
  }
  gApplication->Terminate();
}

/*ResetContainers()
 *Function which resets all mapsi and vectors, freeing all currently allocated resources.
 *For use with loading configurations.
 */
void MyMainFrame::ResetContainers() {
  for(unsigned int i=0; i<spectra.size(); i++) {
    delete gROOT->FindObject(spectra[i].c_str());
  }
  for(unsigned int i=0; i<cuts.size(); i++) {
    delete clist[cuts[i]];
  }
  spectra.resize(0);
  variables.resize(0);
  var_defs.resize(0);
  cuts.resize(0);
  SetAvailSpectra();
  hmap.clear();
  pmap.clear();
  cmap.clear();
  clist.clear();
}

/*LoadConfig()
 *Brings up window which prompts for a config file name. Links to SetConfig().
 */
void MyMainFrame::LoadConfig() {
  new FileConfigFrame(gClient->GetRoot(),fMain,MAIN_W*0.5, MAIN_H*0.5,this,0);
}

/*SaveConfig()
 *Brings up window which prompts for a config file name. Links to WriteConfig().
 */
void MyMainFrame::SaveConfig() {
  new FileConfigFrame(gClient->GetRoot(),fMain,MAIN_W*0.5,MAIN_H*0.5,this,1);
}

/*DoDraw()
 *Function called upon clicking draw button. Cycles through each pad and feeds
 *information to converter to make histogram, which is returned back to this function.
 *That returned histogram is then drawn, and stored back into the map. If there is a CutG
 *associated with the histogram, it is drawn as well (CutG was DRAWN on this histo). 
 *Oddly, TCut objects can't be identified with IsA(), making them virtually undrawable...
 */
void MyMainFrame::DoDraw() {
  draw->SetState(kButtonDisabled);
  stpstr->SetState(kButtonDisabled);
  load->SetState(kButtonDisabled);
  save->SetState(kButtonDisabled);
  clear->SetState(kButtonDisabled);
  for(int i=1; i<nPads+1; i++) {
    fC1->cd(i);
    try {
      string pname = "Ecanvas_"+to_string(i);
      string spectrum = pmap.at(pname);
      auto iter = hmap.find(spectrum);
      vector<string> commands = iter->second.second;
      iter->second.first = take_me_home->getHisto(iter->second.first,spectrum,commands);
      fC1->cd(i);
      iter->second.first->Draw("colz");
      for(unsigned int i=0; i<cuts.size(); i++) {
        string cname = cuts[i];
        string hname = cmap.at(cname);
        if(hname == spectrum) {
          TObject *obj = clist.at(cname);
          if(obj != NULL && obj->IsA() == TCutG::Class()) {
            TCutG *cut = (TCutG*) obj;
            cut->Draw("same");
          }
        }
      }
      fC1->Modified();
      fC1->Update();
    } catch (out_of_range& oor) {cout<<"blep"<<endl;}
  }
  draw->SetState(kButtonUp);
  stpstr->SetState(kButtonUp);
  load->SetState(kButtonUp);
  save->SetState(kButtonUp);
  clear->SetState(kButtonUp);
}

/*DoClear()
 *Locks the secondary thread, and wipes away the data stored in the 
 *tree.
 */
void MyMainFrame::DoClear() {
  TThread::Lock();
  take_me_home->ClearTree();
  TThread::UnLock();
}

/*DoMenuPrompt()
 *Switches menu ids and generates the appropriate window
 */
void MyMainFrame::DoMenuPrompt(Int_t id) {
  switch(id) {
    case(M_SOURCE_ONLINE):
      new OlineSrcFrame(gClient->GetRoot(), fMain, MAIN_W*0.25, MAIN_H*0.25, this);
      break;
    case(M_SOURCE_FILE):
      new FileSrcFrame(gClient->GetRoot(), fMain, MAIN_W*0.5, MAIN_H*0.5, this);
      break;
    case(M_VIEW_DIVIDE):
      new DivisionFrame(gClient->GetRoot(), fMain, MAIN_W*0.75, MAIN_H*0.75, this);
      break;
    case(M_CREATE_SPECTRUM):
      new SpectraFrame(gClient->GetRoot(),fMain,MAIN_W*0.75,MAIN_H*0.75,this,variables,cuts);
      break;
    case(M_LIST_SPECTRA):
      new SpectraListFrame(gClient->GetRoot(),fMain,MAIN_W*0.5,MAIN_H*0.75,this,spectra,hmap);
      break;
    case(M_LIST_CUTS):
      new CutListFrame(gClient->GetRoot(),fMain,MAIN_W*0.5,MAIN_H*0.75,this,cuts,cmap);
      break;
  }
}

/*SetURI()
 *Function linked to signal which gives the name of the data source
 *For NSCLDAQ needs full protocol as well
 */
void MyMainFrame::SetURI(char* input_uri) {
  std::string temp(input_uri);
  fURI = temp;
  cout<<"Attaching: "<<fURI<<" ..."<<endl;

  if(take_me_home->SetSource(fURI)) {
    cout<<"Source "<<fURI<<" set and ready to read"<<endl;
    kActive = true;
  } else {
    cout<<"Unable to set source "<<fURI<<"! Choose a different source"<<endl;
    kActive = false;
  }
}

/*SetCanvasDiv()
 *Divides canvas. Linked to buttons on main frame, along
 *with the division window buttons
 */
void MyMainFrame::SetCanvasDiv(Int_t r, Int_t c) {
  fC1->Clear();
  fC1->Divide(r,c);
  nPads = r*c;
  for(int i=0; i<16; i++) {
    if(i<nPads) {
      specboxes[i]->SetEnabled(true);
    } else {
      specboxes[i]->SetEnabled(false);
    }
    gClient->NeedRedraw(specboxes[i]);
  }
}

/*SetConfig()
 *Linked to load config window. Parses config file and initializes storage
 */
void MyMainFrame::SetConfig(char* config) {
  ResetContainers();//dump anything extant
  std::string temp(config);
  fConfig = temp; //global name 
  ifstream configFile(fConfig);
  if(!configFile.is_open()) {cout<<"Failed to open config file!"<<endl; return;}
  string junk;
  TString var_type;
  string var, vt;
  getline(configFile, junk);
  while(configFile>>var_type) {
    if(var_type.BeginsWith("-")) break;
    else {
      configFile>>var;
      variables.push_back(var);
      vt = var_type.Data();
      var_defs.push_back(make_pair(vt, var));
    }
  }
  getline(configFile, junk);
  getline(configFile, junk);
  TString type;
  string name, varx, vary, cuton, cutby;
  int binsx, binsy;
  float minx,maxx,miny,maxy;
  while(configFile>>type) {
    if(type.BeginsWith("-")) break;
    configFile>>name;
    configFile>>varx;
    configFile>>vary;
    configFile>>binsx;
    configFile>>minx;
    configFile>>maxx;
    configFile>>binsy;
    configFile>>miny;
    configFile>>maxy;
    configFile>>cuton;

    string command1 = "";
    if(type.BeginsWith("TH1")) {
      command1 = varx+">>"+name;
    } else if(type.BeginsWith("TH2")) {
      command1 = vary+":"+varx+">>"+name;
    } else {
      cout<<"Invalid histogram type: "<<type.Data()<<"!!"<<endl;
    }

    string command2("");
    if(cuton != "n/a") {
      command2 += cuton;
    }

    vector<string> cmds;
    cmds.push_back(command1);
    cmds.push_back(command2);
    if(type == "TH1I") {
      spectra.push_back(name);
      TH1I* h = new TH1I(name.c_str(), name.c_str(), binsx, minx, maxx);
      auto set = make_pair(h, cmds);
      hmap.insert(make_pair(name,set));
    } else if (type == "TH1F") {
      spectra.push_back(name);
      TH1F* h = new TH1F(name.c_str(), name.c_str(), binsx, minx, maxx);
      auto set = make_pair(h, cmds);
      hmap.insert(make_pair(name,set));
    } else if (type == "TH2F") {
      spectra.push_back(name);
      TH2F* h = new TH2F(name.c_str(), name.c_str(), binsx, minx, maxx, binsy, miny, maxy);
      auto set = make_pair(h, cmds);
      hmap.insert(make_pair(name,set));
    } else if (type == "TH2I") {
      spectra.push_back(name);
      TH2I* h = new TH2I(name.c_str(), name.c_str(), binsx, minx, maxx, binsy, miny, maxy);
      auto set = make_pair(h, cmds);
      hmap.insert(make_pair(name,set));
    }
  }
  
  string hname; 
  while(configFile>>type) {
    if(type.BeginsWith("CutG")) {
      configFile>>name;
      configFile>>hname;
      configFile>>varx;
      configFile>>vary;
      TCutG *cut = new TCutG(name.c_str());
      vector<Double_t> xpoints, ypoints;
      Double_t x,y;
      configFile>>junk;
      while(junk != "}") {
        configFile>>x;
        configFile>>junk;
        configFile>>y;
        xpoints.push_back(x); ypoints.push_back(y);
        configFile>>junk;
      }
      cut->Set(xpoints.size());
      for(unsigned int i=0; i<xpoints.size(); i++) {
        cut->SetPoint(i, xpoints[i], ypoints[i]);
      }
      cut->SetVarX(varx.c_str());
      cut->SetVarY(vary.c_str());
      cuts.push_back(name);
      clist[name] = (cut);
      cmap[name] = hname;
      TCut c(name.c_str(),name.c_str());
      take_me_home->AddCut(name.c_str(),c);
    } else if (type.BeginsWith("Cut")) {
      configFile>>name;
      configFile>>hname;
      configFile>>junk;
      string action = "";
      while(configFile>>junk) {
        if(junk == "}") break;
        action += junk;
      }
      TCut *cut = new TCut(name.c_str(), action.c_str());
      cuts.push_back(name);
      clist[name] = (cut);
      cmap[name] = hname;
      take_me_home->AddCut(name.c_str(),*cut);
    }
  }
  configFile.close();
  SetAvailSpectra(); //fill all of the spectrum boxes
}

/*WriteConfig()
 *Linked to save config window. Parses through storage and writes to file
 */
void MyMainFrame::WriteConfig(char* config) {
  ofstream outfile(config);
  if(outfile.is_open()) {
    outfile<<"--------------------Variables--------------------"<<endl;
    for(unsigned int i=0; i<var_defs.size(); i++) {
      outfile<<var_defs[i].first<<" "<<var_defs[i].second<<endl;
    }
    outfile<<"--------------------Histograms--------------------"<<endl;
    outfile<<"Type HistoName VarX VarY BinsX MinX MaxX BinsY MinY MaxY Cut"<<endl;
    for(unsigned int i=0; i<spectra.size(); i++) {
      auto entry = hmap.at(spectra[i]);
      TObject *obj = entry.first;
      if(obj->IsA() == TH1F::Class()) {
        TH1F *h = (TH1F*) obj;
        outfile<<"TH1F"<<" ";
        outfile<<spectra[i]<<" ";
        string command = entry.second[0];
        string varx = "";
        string temp = "";
        for(unsigned int j=0; j<command.size(); j++) {
          temp = command[j];
          if(temp == ">") break;
          varx += temp;
        }
        outfile<<varx<<" "<<"n/a"<<" "<<h->GetNbinsX()<<" "<<h->GetXaxis()->GetXmin()<<" "
               <<h->GetXaxis()->GetXmax()<<" "<<"0 "<<"0 "<<"0 ";
        if(entry.second[1] != "") {
          outfile<<entry.second[1]<<" "<<endl;
        } else {
          outfile<<"n/a "<<endl;
        }
      } else if(obj->IsA() == TH1I::Class()) {
        TH1I *h = (TH1I*) obj;
        outfile<<"TH1I"<<" ";
        outfile<<spectra[i]<<" ";
        string command = entry.second[0];
        string varx = "";
        string temp = "";
        for(unsigned int j=0; j<command.size(); j++) {
          temp = command[j];
          if(temp == ">") break;
          varx += temp;
        }
        outfile<<varx<<" "<<"n/a"<<" "<<h->GetNbinsX()<<" "<<h->GetXaxis()->GetXmin()<<" "
               <<h->GetXaxis()->GetXmax()<<" "<<"0 "<<"0 "<<"0 ";
        if(entry.second[1] != "") {
          outfile<<entry.second[1]<<" "<<endl;
        } else {
          outfile<<"n/a "<<endl;
        }
      } else if(obj->IsA() == TH2F::Class()) {
        TH2F *h = (TH2F*) obj;
        outfile<<"TH2F"<<" ";
        outfile<<spectra[i]<<" ";
        string command = entry.second[0];
        string varx, vary;
        string temp = "", c;
        for(unsigned int j=0; j<command.size(); j++) {
          c = command[j];
          if(c == ">") {
            varx = temp;
            break;
          } else if(c == ":") {
            vary = temp;
            temp = "";
          } else {
            temp += c;
          }
        }
        outfile<<varx<<" "<<vary<<" "<<h->GetNbinsX()<<" "<<h->GetXaxis()->GetXmin()<<" "
               <<h->GetXaxis()->GetXmax()<<" "<<h->GetNbinsY()<<" "<<h->GetYaxis()->GetXmin()
               <<" "<<h->GetYaxis()->GetXmax()<<" ";
        if(entry.second[1] != "") {
          outfile<<entry.second[1]<<" "<<endl;
        } else {
          outfile<<"n/a "<<endl;
        }
      } else if(obj->IsA() == TH2I::Class()) {
        TH2I *h = (TH2I*) obj;
        outfile<<"TH2I"<<" ";
        outfile<<spectra[i]<<" ";
        string command = entry.second[0];
        string varx, vary;
        string temp = "", c;
        for(unsigned int j=0; j<command.size(); j++) {
          c = command[j];
          if(c == ">") {
            varx = temp;
            break;
          } else if(c == ":") {
            vary = temp;
            temp = "";
          } else {
            temp += c;
          }
        }
        outfile<<varx<<" "<<vary<<" "<<h->GetNbinsX()<<" "<<h->GetXaxis()->GetXmin()<<" "
               <<h->GetXaxis()->GetXmax()<<" "<<h->GetNbinsY()<<" "<<h->GetYaxis()->GetXmin()
               <<" "<<h->GetYaxis()->GetXmax()<<" ";
        if(entry.second[1] != "") {
          outfile<<entry.second[1]<<" "<<endl;
        } else {
          outfile<<"n/a "<<endl;;
        }
      }
    }
    outfile<<"-----------------------Cuts-----------------------"<<endl;
    for(unsigned int i=0; i<cuts.size(); i++) {
      string cname = cuts[i];
      string hname = cmap[cname];
      TObject *obj = clist[cname];
      if(obj->IsA() == TCutG::Class()) {
        TCutG *cut = (TCutG*) obj;
        outfile<<"CutG "<<cname<<" "<<hname<<" "<<cut->GetVarX()<<" "<<cut->GetVarY()<<" { ";
        vector<Double_t> xarray(cut->GetX(), cut->GetX()+cut->GetN());
        vector<Double_t> yarray(cut->GetY(), cut->GetY()+cut->GetN());
        for (int j=0; j<cut->GetN(); j++) {
          outfile<<xarray[j]<<" , "<<yarray[j]<<" ";
          if(j != cut->GetN()-1) outfile<<"; "; 
        }
        outfile<<"}"<<endl;
      } else if (obj->IsA() == TCut::Class()) {
        TCut *cut = (TCut*) obj;
        outfile<<"Cut "<<cname<<" "<<hname<<" { "<<cut->GetTitle()<<" }"<<endl;
      }
    }
    outfile.close();
  } else {
    cout<<"Unable to open config file when attempting to write config!"<<endl;
  }
}

/*SetAvailSpectra()
 *Fills up spectrum list boxes
 */
void MyMainFrame::SetAvailSpectra() {
  for(int i=0; i<16; i++) {
    specboxes[i]->RemoveAll();
    for(unsigned int j=0; j<spectra.size(); j++) {
      specboxes[i]->AddEntry(spectra[j].c_str(), j);   
    }
  }
}

/*Do*Cut
 *Linked to appropriate cut button and generates a window
 */
void MyMainFrame::Do2DCut() {
  new CutFrame(gClient->GetRoot(), fMain, MAIN_W*0.25, MAIN_H*0.25, this, nPads,1);
}

void MyMainFrame::Do1DCut() {
  new CutFrame(gClient->GetRoot(), fMain, MAIN_W*0.25, MAIN_H*0.25, this, nPads,0);
}

void MyMainFrame::DoComboCut() {
  new CombineCutFrame(gClient->GetRoot(), fMain, MAIN_W*0.25, MAIN_H*0.25, this, cuts);
}

void MyMainFrame::DoApplyCut() {
  new ApplyCutFrame(gClient->GetRoot(), fMain, MAIN_W*0.5, MAIN_H*0.35, this, cuts, spectra);
}

/*Get2DCut()
 *Takes info from frame and allows user to make a 2D cut in the selected pad
 */
void MyMainFrame::Get2DCut(char* name, Int_t pd) {
  TCutG *cut = 0; //init as NULL
  try {
    fC1->cd(pd);
    const char *p = gPad->GetName();
    //pull histogram info
    string pname(p);
    if(nPads == 1) pname = "Ecanvas_1";
    string spectrum = pmap.at(pname);
    auto hist_info = hmap.at(spectrum);
    string vars = hist_info.second[0];
    if(hist_info.first->IsA() == TH1F::Class() || hist_info.first->IsA() == TH1I::Class()) {
      cout<<"Not a 2D histogram. Ignoring 2D cut command."<<endl;
      return;
    }
    string vx, vy, temp="", c;
    //grab variable info for setting
    for(unsigned int i=0; i<vars.size(); i++) {
      c = vars[i];
      if(c == ":") {
        vx = temp;
        temp = "";
        continue;
      } else if (c == ">") {
        vy = temp;
        break;
      }
      temp += c;
    }
    gROOT->SetEditorMode("CutG");//Enables the drawing of a CutG
    cut = (TCutG*) fC1->WaitPrimitive("CUTG", "CutG"); //Grabs first cut created
    cut->SetName(name);
    cut->SetVarX(vx.c_str());
    cut->SetVarY(vy.c_str());
    cut->SetLineColor(2);
    string n(name);
    //check to see if this is an overwrite. if so, dont add to list of cuts
    int flag = 0;
    for(unsigned int i=0; i<cuts.size(); i++) {
      if(cuts[i] == n) {
        flag = 1;
        break;
      }
    }
    if(!flag) cuts.push_back(n);
    TCut storage(name, name);
    clist[name] = cut;
    take_me_home->AddCut(n.c_str(), storage);
    cmap[n] = spectrum;
  } catch(out_of_range& oor) {
    cout<<"No histogram available on pad. Ignoring 2D cut command."<<endl;
  }
}

/*Get1DCut()
 *Takes in info from cut window, and allows user to draw 1D cut
 */
void MyMainFrame::Get1DCut(char* name, Int_t pd) {
  try {
    fC1->cd(pd);
    Double_t x1, x2, dx;
    //get histo info
    const char *p = gPad->GetName();
    string pname(p);
    if(nPads == 1) pname = "Ecanvas_1";
    string spectrum = pmap.at(pname);
    auto hist_info = hmap.at(spectrum);
    string vars = hist_info.second[0];
    string vx, temp="",c;
    //grab variable
    for(unsigned int i=0; i<vars.size(); i++) {
      c = vars[i];
      if(c == ">") {
        vx = temp;
        break;
      } else {
        temp += c;
      }
    }
    TH1* h = (TH1*) hist_info.first;
    //really imperfect way of drawing a 1D cut... currently not replicable at draw
    Double_t ymax = h->GetYaxis()->GetXmax();
    fC1->WaitPrimitive();
    x1 = clickedX;
    dx = gPad->AbsPixeltoX(x1);
    x1 = gPad->PadtoX(dx);
    TLine *l1 = new TLine(x1,0,x1,ymax);
    l1->SetLineColor(2);
    l1->Draw();
    fC1->WaitPrimitive();
    x2 = clickedX;
    dx = gPad->AbsPixeltoX(x2);
    x2 = gPad->PadtoX(dx);
    TLine *l2 = new TLine(x2,0,x2,ymax);
    l2->SetLineColor(2);
    l2->Draw();
    //make sure ordering is proper
    if(x2<x1) swap(x1, x2);
    string cstr = vx+">"+to_string(x1)+" && "+vx+"<"+to_string(x2);
    TCut *cut = new TCut(name, cstr.c_str());
    string n(name);
    int flag = 0;
    //check if overwrite. if so, dont add to cut list
    for(unsigned int i=0; i<cuts.size(); i++) {
      if(cuts[i] == n) {
        flag = 1;
        break;
      }
    }
    if(!flag) cuts.push_back(n);
    clist[n] = cut;
    take_me_home->AddCut(n.c_str(), *cut);
    cmap[n] = spectrum;
  } catch(out_of_range& oor) {
    cout<<"No histogram available on pad. Ignoring 1D cut command."<<endl;
  }
}

/*GetComboCut
 *Gets cut info from frame, and then makes a new TCut
 */
void MyMainFrame::GetComboCut(char *name, char *title) {
  TCut *cut = new TCut(name, title);
  string n(name);
  int flag = 0;
  //check if overwrite. if so dont add to cut list
  for(unsigned int i=0; i<cuts.size(); i++) {
    if(cuts[i] == n) {
      flag = 1;
      break;
    }
  }
  if(!flag) cuts.push_back(n);
  clist[n] = cut;
  take_me_home->AddCut(n.c_str(), *cut);
  cmap[name] = "n/a";
}

/*ApplyCut()
 *Gets info from frame, and applies cut to the spectrum
 */
void MyMainFrame::ApplyCut(char *sname, char *cname) {
  string spectrum(sname); string cut(cname);
  auto info = hmap[spectrum];
  info.second[1] = cut;
}

/*Add1DSpectrum()
 *Gets histo info from frame and makes a new one. Creates/overwrites
 */
void MyMainFrame::Add1DSpectrum(char* type, char* name, char* varx, Float_t minx,
                                Float_t maxx, Int_t binsx, char* cut) {
  string title(name);
  int flag = 0;
  //if overwrite, dont add to histogram list
  for(unsigned int i=0; i<spectra.size(); i++) {
    if(spectra[i] == title) {
      flag = 1;
      break;
    }
  }
  if(!flag) spectra.push_back(title);
  string vx(varx);
  string command1 = vx + ">>" + title;
  string command2(cut);
  if(command2 == "none") command2 = "";
  vector<string> cmds;
  cmds.push_back(command1);
  cmds.push_back(command2);
  string htype(type);
  if(htype == "TH1F") {
    TH1F *h = new TH1F(name, name, binsx, minx, maxx);
    auto set = make_pair(h, cmds);
    hmap[title] = set;
  } else if (htype == "TH1I") {
    TH1I *h = new TH1I(name, name, binsx, minx, maxx);
    auto set = make_pair(h, cmds);
    hmap[title] = set;
  }
  SetAvailSpectra(); //update list boxes
}

/*Add2DSpectrum()
 *Gets histo info from frame and makes a new one. Creates/overwrites
 */
void MyMainFrame::Add2DSpectrum(char* type, char* name, char* varx, char* vary,
                                Float_t minx, Float_t miny, Float_t maxx, Float_t maxy,
                                Int_t binsx, Int_t binsy, char* cut) {
  string title(name);
  int flag = 0;
  //if overwrite, dont add to histogram list
  for(unsigned int i=0; i<spectra.size(); i++) {
    if(spectra[i] == title) {
      flag = 1;
      break;
    }
  }
  if(!flag) spectra.push_back(title);
  string vx(varx); string vy(vary);
  string command1 = vy+":"+vx+">>"+title;
  string command2(cut);
  if(command2 == "none") command2 = "";
  vector<string> cmds;
  cmds.push_back(command1); cmds.push_back(command2);
  string htype(type);
  if(htype == "TH2F") {
    TH2F *h = new TH2F(name, name, binsx, minx, maxx, binsy, miny, maxy);
    auto set = make_pair(h, cmds);
    hmap[title] = set;
  } else if (htype == "TH2I") {
    TH2I *h = new TH2I(name, name, binsx, minx, maxx, binsy, miny, maxy);
    auto set = make_pair(h, cmds);
    hmap[title] = set;
  }
  SetAvailSpectra(); //update list boxes
}

/*SetPad()
 *maps pads to spectra
 */
void MyMainFrame::SetPad(Int_t spb_id, Int_t spec_id) {
  string spectrum = spectra[spec_id];
  string pname = "Ecanvas_"+to_string(spb_id);
  pmap[pname] = spectrum;
}

/*StopStart()
 *Function that initializes conversion... currently buttoned. Connect differently later
 */
void MyMainFrame::StopStart() {
  if(fThread->GetState() == TThread::kRunningState) {
    fThread->Kill(); //if stop and its running, kill it
    stpstr->SetText("&Start");
  } else {
    fThread->Run(this); //if start, run it in this frame
    fThread->SetCancelAsynchronous(); // Good luck
    stpstr->SetText("&Stop");
  }
  gClient->NeedRedraw(stpstr);
}

/*ThreadFunc()
 *Function acted by the secondary thread
 *This is where users will potentially need to make changes
 */
void *MyMainFrame::ThreadFunc(void *ptr) {
  MyMainFrame *main = (MyMainFrame*) ptr;
  if(!main) return 0;  //Yikes
  TThread *thread = main->GetThread();  //grab thread
  converter *conv = main->GetConverter(); //grab converter
  if(!thread || !conv) return 0;
  //this loop probably needs condition where converter gives feedback to if data stream is
  //closed
  while(true) { //inf data stream loop
    //if state change, switch it
    if(thread && thread->GetState() != TThread::kRunningState) break;
    if(conv) {
      //do this
      conv->run();
    }
    gSystem->Sleep(100); //I think only necessary for testing where data rate is real low
  }
  return ptr;
}

/*HandleCanvas()
 *Does heavy lifting of canvas info bar
 */
void MyMainFrame::HandleCanvas(Int_t event, Int_t x, Int_t y, TObject* selected) {
  const char *text1, *text2, *text4;
  char text3[50];
  text1 = selected->GetTitle();
  status->SetText(text1,0);
  text2 = selected->GetName();
  status->SetText(text2,1);
  if(event==kKeyPress) {
    sprintf(text3, "%c", (char) x);
  } else if (event == kButton1Double) {
    sprintf(text3, "clicked");
    clickedX = x;
    clickedY = y;
  } else {
    sprintf(text3, "%d,%d",x,y);
  }
  status->SetText(text3,2);
  text4 = selected->GetObjectInfo(x,y);
  status->SetText(text4,3);
}

/*example()
 *Generates main frame
 */
void example() {
  UInt_t width = 2000;
  UInt_t height = 1000;
  new MyMainFrame(gClient->GetRoot(),width,height);
}

/*main()
 *run it
 */
int main(int argc, char **argv) {
  TApplication app("app", &argc, argv);
  example();
  app.Run();
  return 0;
}
