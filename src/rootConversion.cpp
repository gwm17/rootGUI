/*rootConversion.h
 *Program which takes in a data source and parses data from that source.
 *This is where users must make the most changes (hopefully).
 *
 *Gordon M. Oct 2019
 */
#include "rootConversion.h"
#include <TThread.h>

using namespace std;
///////TESTING////////

//Constructor
converter::converter() {
  sURI = "";
  tree =  new TTree("tree","tree");
  //initialize branches here!
  tree->Branch("t",&t,"t/F");
};

//Destructor
converter::~converter() {
};

//Do the work
void converter::run() {
  t = 1;
  TThread::Lock();
  tree->Fill();
  TThread::UnLock();
}

//Take hist info and fill it from tree
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
