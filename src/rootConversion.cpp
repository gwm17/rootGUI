#include "rootConversion.h"
#include <TThread.h>

using namespace std;
///////TESTING////////

converter::converter() {
  sURI = "";
  tree =  new TTree("tree","tree");
  tree->Branch("t",&t,"t/F");
};

converter::~converter() {
};

void converter::run() {
  t = 1;
  TThread::Lock();
  tree->Fill();
  TThread::UnLock();
}

TObject* converter::getHisto(TObject *histo, string name, vector<string> commands) {
  if(commands[1] != "") {
    TCut cut = storage[commands[1]];
    tree->Draw(commands[0].c_str(),cut,"goff");
  } else {
    tree->Draw(commands[0].c_str(),"","goff");
  }
  histo = gDirectory->Get(name.c_str());
  return histo;
}

Bool_t converter::SetSource(string uri) {
  if(uri != "") {
    sURI = uri;
    return true;
  } else {
    return false;
  }
}

void converter::AddCut(const char *name, TCut cut) {
  string n(name);
  storage[name] = cut;
}

void converter::ClearTree() {
  tree->Reset();
}
