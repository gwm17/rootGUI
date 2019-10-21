/*rootConversion.h
 *Program which takes in a data source and parses data from that source.
 *This is where users must make the most changes (hopefully).
 *
 *Gordon M. Oct 2019
 */
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
    void run(); //REQUIRED
    Bool_t SetSource(string uri); //REQUIRED
    TObject* getHisto(TObject *histo, string name, vector<string> commands); //REQUIRED
    void AddCut(const char *name, TCut cut); //REQUIRED
    void ClearTree(); //REQUIRED

  private:
    string sURI; //REQUIRED
    TTree *tree; //REQUIRED
    Float_t t;  //all variables must be specified here
    unordered_map<string, TCut> storage; //REQUIRED

};

#endif
