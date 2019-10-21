#ifndef ROOT_CONVERSION_H
#define ROOT_CONVERSION_H

#include <TROOT.h>
#include <iostream>
#include <string>
#include <vector>
#include <TObject.h>
#include <TH1.h>
#include <TCut.h>
#include <TTree.h>
#include <THashTable.h>
#include <unordered_map>

using namespace std;

////////TESTING/////////

class converter {

  public:
    converter();
    ~converter();
    void run();
    Bool_t SetSource(string uri);
    TObject* getHisto(TObject *histo, string name, vector<string> commands);
    void AddCut(const char *name, TCut cut);
    void ClearTree();

  private:
    string sURI;
    TTree *tree;
    Float_t t;
    unordered_map<string, TCut> storage;

};

#endif
