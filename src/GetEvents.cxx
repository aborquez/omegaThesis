/************************************/
/* GetEvents.cxx                    */
/*                                  */
/* Andrés Bórquez                   */
/*                                  */
/************************************/

// USAGE: ./bin/GetEvents textFile.txt

#include "analysisConfig.h"

int main(int argc, char* argv[]) {
  Int_t NEvents = 0;
  // TString textFile = "simulFiles.txt";
  TString textFile = argv[1];
  std::cout << "Reading " << textFile << "..." << std::endl;
  std::ifstream inFile(textFile);
  TString auxLine;
  TString rootFile;
  TFile *File;
  while (inFile >> auxLine) {
    rootFile = auxLine;
    // std::cout << "  rootFile = " << rootFile << std::endl;
    File = TFile::Open(rootFile);
    TTree *treeExtracted = (TTree*)File->Get("CLASEVENT");
    NEvents += treeExtracted->GetEntries();
    // std::cout << "  NEvents = " << NEvents << std::endl;
    File->Close(); // debug
  }
  inFile.close();
  std::cout << "NEvents = " << NEvents << std::endl;

  return 0;
}
