/**************************************/
/* Acceptance.cxx                     */
/*                                    */
/* Author: Andrés Bórquez             */
/*                                    */
/**************************************/

// hola

#include "analysisConfig.h"

/*** Global variables ***/

TString outDir = proDir + "/out/Acceptance";
TString inputFile1 = simDir + "/old/C/comb_oldC_0001.root"; // hardcoded

// options
TString targetOption;
TString kinvarOption;

// cuts
TCut cutSIMREC;
TCut cutGSIM;
TCut cutTargType_SIMREC;
TCut cutTargType_GSIM;

TCut     cutProjection;
TString  titleProjection;
Double_t normProjection[5];

Double_t kinvarEdges[6];
TString  kinvarSufix;
Int_t    kinvarConstant = 0; // default for Q2, Pt2 and Nu

TString XAxis;
TString YAxis;
TString XHistProp;
TString YHistProp;
TString histProperties;

// name scheme
TString titleDraw;

TString outPrefix;

TString plotFile1;
TString plotFile2;

/*** Declaration of functions ***/

inline int parseCommandLine(int argc, char* argv[]);
void printUsage();
void assignOptions();
void printOptions();

int main(int argc, char **argv) {

  parseCommandLine(argc, argv);
  assignOptions();
  printOptions();

  // dir structure, just in case
  system("mkdir -p " + outDir);
  
  TChain *treeExtracted = new TChain();
  treeExtracted->Add(inputFile1 + "/mix");
  
  /*** 1D: projections ***/

  TH1F *theHist0;
  TCanvas *c0;
  
  for (Int_t i = 0; i < 5; i++) {

    titleProjection = Form(" (%.02f < mc_", kinvarEdges[i]) + kinvarOption + Form(" < %.02f)", kinvarEdges[i+1]);;
    cutProjection = Form("%f < mc_", kinvarEdges[i]) + kinvarOption + " && mc_" + kinvarOption + Form(" < %f", kinvarEdges[i+1]);;
    
    treeExtracted->Draw(kinvarOption + Form(">>h%d", i), cutProjection, "goff"); // add cuts!
    theHist0 = (TH1F *)gROOT->FindObject(Form("h%d", i));
    
    c0 = new TCanvas("c0", "c0", 1366, 768);
    
    theHist0->SetTitle(kinvarOption + " for " + targetOption + " in " + titleProjection);
    theHist0->Draw("E");
    
    c0->Print(outDir + "/acc0-" + targetOption + kinvarSufix + Form("%d", i+kinvarConstant) + ".png"); // output file

    // keep number of entries
    normProjection[i] = (Double_t) theHist0->GetEntries();
  }
  
  /*** 1D: mc_kinvar ***/

  titleProjection = Form(" (%.02f < mc_", kinvarEdges[0]) + kinvarOption + Form(" < %.02f)", kinvarEdges[5]);;
  cutProjection = Form("%f < mc_", kinvarEdges[0]) + kinvarOption + " && mc_" + kinvarOption + Form(" < %f", kinvarEdges[5]);;
  
  TH1F *theHist1;
  treeExtracted->Draw("mc_" + kinvarOption + ">>hx1", cutProjection, "goff"); // add cuts!
  theHist1 = (TH1F *)gROOT->FindObject("hx1");
    
  TCanvas *c1 = new TCanvas("c1", "c1", 1366, 768);

  theHist1->SetTitle("mc_" + kinvarOption + " for " + targetOption + " in " + titleProjection);
  theHist1->Draw("E");
    
  c1->Print(plotFile1); // output file

  /*** 2D ***/

  TH2F *AccHist = new TH2F("AccHist", "Acceptance(" + kinvarOption + ") for " + targetOption, 5, kinvarEdges[0], kinvarEdges[5], 5, kinvarEdges[0], kinvarEdges[5]);
  
  TH2F *theHist2;
  TCanvas *c2;
  
  for (Int_t j = 0; j < 5; j++) {

    titleProjection = Form(" (%.02f < mc_", kinvarEdges[j]) + kinvarOption + Form(" < %.02f)", kinvarEdges[j+1]);;
    cutProjection = Form("%f < mc_", kinvarEdges[j]) + kinvarOption + " && mc_" + kinvarOption + Form(" < %f", kinvarEdges[j+1]);;
    
    treeExtracted->Draw(kinvarOption + ":mc_" + kinvarOption + Form(">>acc%d",j) + histProperties, cutProjection, "goff"); // add cuts!
    theHist2 = (TH2F *)gROOT->FindObject(Form("acc%d",j));
    
    c2 = new TCanvas("c2", "c2", 1366, 1366);

    c2->SetTickx(1);
    c2->SetTicky(1);
    gStyle->SetOptStat(0);
    gStyle->SetPalette(1, 0);

    // normalization (very important!)
    theHist2->Scale(1./normProjection[j]);
    
    theHist2->SetTitle(kinvarOption + ":mc_" + kinvarOption + " for " + targetOption + " in " + titleProjection);
    theHist2->SetContour(99);
    theHist2->Draw("COLZ TEXT");
    
    c2->Print(outDir + "/acc2-" + targetOption + kinvarSufix + Form("%d", j+kinvarConstant) + ".png"); // output file

    // add to final hist!
    AccHist->Add(theHist2);
  }
  
  /*** 2D ***/

  TCanvas *c3 = new TCanvas("c3", "c3", 1366, 1366);
  c3->SetTickx(1);
  c3->SetTicky(1);
  gStyle->SetOptStat(0);
  gStyle->SetPalette(1, 0);

  AccHist->SetContour(99);
  AccHist->Draw("COLZ TEXT");

  c3->Print(plotFile2); // output file

  return 0;
}

/*** Functions ***/

inline int parseCommandLine(int argc, char* argv[]) {
  Int_t c;
  if (argc == 1) {
    std::cerr << "Empty command line. Execute ./Acceptance -h to print usage." << std::endl;
    exit(0);
  }
  while ((c = getopt(argc, argv, "ht:k:")) != -1)
    switch (c) {
    case 'h': printUsage(); exit(0); break;
    case 't': targetOption = optarg; break;
    case 'k': kinvarOption = optarg; break;
    default:
      std::cerr << "Unrecognized argument. Execute ./Acceptance -h to print usage." << std::endl;
      exit(0);
      break;
    }
}

void printUsage() {
  std::cout << "Acceptance program. Usage is:" << std::endl;
  std::cout << std::endl;
  std::cout << "./Acceptance -h" << std::endl;
  std::cout << "  prints help and exits program" << std::endl;
  std::cout << std::endl;
  std::cout << "./Acceptance -t[target]" << std::endl;
  std::cout << "  selects target: D, C, Fe, Pb" << std::endl;
  std::cout << std::endl;  
  std::cout << "./Acceptance -k[kinvar]" << std::endl;
  std::cout << "  selects kinvar: Q2, Nu, Z, Pt2" << std::endl;
  std::cout << std::endl;
}

void printOptions() {
  std::cout << "Executing Acceptance program. The chosen parameters are:" << std::endl;
  std::cout << "  targetOption = " << targetOption << std::endl;
  std::cout << "  kinvarOption = " << kinvarOption << std::endl;
  std::cout << std::endl;
}

void assignOptions() {
  // for targets
  if (targetOption == "D") {
    cutTargType_SIMREC = "TargType == 1";
    cutTargType_GSIM = "mc_TargType == 1";
  } else if (targetOption == "C") {
    cutTargType_SIMREC = "TargType == 2";
    cutTargType_GSIM = "mc_TargType == 2";
  } else if (targetOption == "Fe") {
    cutTargType_SIMREC = "TargType == 2";
    cutTargType_GSIM = "mc_TargType == 2";
  } else if (targetOption == "Pb") {
    cutTargType_SIMREC = "TargType == 2";
    cutTargType_GSIM = "mc_TargType == 2";
  }
  // kinvar
  if (kinvarOption == "Q2") {
    histProperties = "(5, 1., 4., 5, 1., 4.)";
    kinvarSufix = "-q";
    for (Int_t i = 0; i < 6; i++) kinvarEdges[i] = edgesQ2[i];
  } else if (kinvarOption == "Nu") {
    histProperties = "(5, 2.2, 4.2, 5, 2.2, 4.2)";
    kinvarSufix = "-n";
    for (Int_t i = 0; i < 6; i++) kinvarEdges[i] = edgesNu[i];
  } else if (kinvarOption == "Z") {
    kinvarConstant = 3;
    kinvarSufix = "-z";
    histProperties = "(5, 0.5, 1.0, 5, 0.5, 1.0)";
    for (Int_t i = 0; i < 6; i++) kinvarEdges[i] = edgesZ[0] + i*(edgesZ[5]-edgesZ[0])/5.;
  } else if (kinvarOption == "Pt2") {
    histProperties = "(5, 0., 1.5, 5, 0., 1.5)";
    kinvarSufix = "-p";
    for (Int_t i = 0; i < 6; i++) kinvarEdges[i] = edgesPt2[i];
  }
  // names
  plotFile1 = outDir + "/acc1-" + targetOption + "-" + kinvarOption + ".png";
  plotFile2 = outDir + "/accf-" + targetOption + "-" + kinvarOption + ".png";
}
