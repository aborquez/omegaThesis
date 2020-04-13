/**************************************/
/* analysisConfig.h                   */
/*                                    */
/* Created by Andrés Bórquez, CCTVAL  */
/*                                    */
/**************************************/

#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <iomanip>

#include "TROOT.h"
#include "TString.h"
#include "TCut.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "TTree.h"
#include "TChain.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TLine.h"
#include "TPaveText.h"
#include "TLegend.h"
#include "TLatex.h"
#include "TGaxis.h"
#include "TAxis.h"
#include "TMath.h"
#include "TPad.h"
#include "TObjArray.h"

/*** Global variables ***/

// TString proDir    = "/home/borquez/omegaThesis";
TString proDir    = getenv("PRODIR");
TString dataDir   = proDir + "/out/filterData";
TString gsimDir   = proDir + "/out/filterSim/gsim";
TString simrecDir = proDir + "/out/filterSim/simrec";

TCut cutDIS = "Q2 > 1 && W > 2 && Yb < 0.85";
TCut cutPi0 = "0.059 < pi0M && pi0M < 0.203"; // mean=0.131 & sigma=0.024
TCut cutPipPim = "0.48 > pippimM || 0.51 < pippimM";

// cut for pi0 from MW
TCut cutPi0_MW = "0.059 < pi0M && pi0M < 0.209"; // mean=0.134 & sigma=0.025

// cut for pi0 from GSIM files
TCut cutPi0_sim = "0.132 < mpi0 && mpi0 < 0.138";

// these cuts are for the files filtered with old OS programs
TCut cutDIS_old = "Q2 > 1 && W > 2";
TCut cutPi0_old = "0.059 < mpi0 && mpi0 < 0.209"; // mikewood cut
TCut cutPipPim_old = "0.48 > mpippim || 0.51 < mpippim"; // mikewood cut

// edges obtained with GetQuantiles.cxx
const Double_t edgesQ2[6] = {1., 1.181, 1.364, 1.598, 1.960, 3.970};
const Double_t edgesNu[6] = {2.2, 3.191, 3.504, 3.744, 3.964, 4.2};
const Double_t edgesZ[6] = {0.5, 0.554, 0.611, 0.676, 0.760, 0.9};
const Double_t edgesPt2[6] = {0.0, 0.040, 0.090, 0.159, 0.273, 1.5};

void setAlias_old(TTree *treeExtracted) {
  // pip
  treeExtracted->SetAlias("p2pip", "fX[2]*fX[2] + fY[2]*fY[2] + fZ[2]*fZ[2]");
  treeExtracted->SetAlias("E2pip", "p2pip + 0.13957*0.13957");
  treeExtracted->SetAlias("m2pip", "E2pip - p2pip");
  treeExtracted->SetAlias("mpip", "TMath::Sqrt(m2pip)");
  treeExtracted->SetAlias("Epip", "TMath::Sqrt(E2pip)");
  treeExtracted->SetAlias("Ppip", "TMath::Sqrt(p2pip)");
  
  // pim
  treeExtracted->SetAlias("p2pim", "fX[3]*fX[3] + fY[3]*fY[3] + fZ[3]*fZ[3]");
  treeExtracted->SetAlias("E2pim", "p2pim + 0.13957*0.13957");
  treeExtracted->SetAlias("m2pim", "E2pim - p2pim");
  treeExtracted->SetAlias("mpim", "TMath::Sqrt(m2pim)");
  treeExtracted->SetAlias("Epim", "TMath::Sqrt(E2pim)");
  treeExtracted->SetAlias("Ppim", "TMath::Sqrt(p2pim)");
  
  // pi0
  treeExtracted->SetAlias("Pxpi0", "fX[0] + fX[1]");
  treeExtracted->SetAlias("Pypi0", "fY[0] + fY[1]");
  treeExtracted->SetAlias("Pzpi0", "fZ[0] + fZ[1]");
  treeExtracted->SetAlias("p2pi0", "Pxpi0*Pxpi0 + Pypi0*Pypi0 + Pzpi0*Pzpi0");
  treeExtracted->SetAlias("cos_theta", "(fX[0]*fX[1] + fY[0]*fY[1] + fZ[0]*fZ[1])/(fE[0]*fE[1])"); // original
  treeExtracted->SetAlias("m2pi0", "2*fE[0]*fE[1]*(1 - cos_theta)"); // original
  treeExtracted->SetAlias("mpi0", "TMath::Sqrt(m2pi0)");
  treeExtracted->SetAlias("E2pi0", "m2pi0 + p2pi0");
  treeExtracted->SetAlias("Epi0", "TMath::Sqrt(E2pi0)");
  treeExtracted->SetAlias("Ppi0", "TMath::Sqrt(p2pi0)");
  
  // crossed terms
  treeExtracted->SetAlias("p1p2", "fX[2]*fX[3] + fY[2]*fY[3] + fZ[2]*fZ[3]");
  treeExtracted->SetAlias("E1E2", "TMath::Sqrt(E2pip*E2pim)");
  treeExtracted->SetAlias("p2p3", "fX[3]*Pxpi0 + fY[3]*Pypi0 + fZ[3]*Pzpi0");
  treeExtracted->SetAlias("E2E3", "TMath::Sqrt(E2pim*E2pi0)");
  treeExtracted->SetAlias("p1p3", "fX[2]*Pxpi0 + fY[2]*Pypi0 + fZ[2]*Pzpi0");
  treeExtracted->SetAlias("E1E3", "TMath::Sqrt(E2pip*E2pi0)");

  // dalitz plots!
  treeExtracted->SetAlias("m2pippim", "m2pip + m2pim + 2*(E1E2 - p1p2)");
  treeExtracted->SetAlias("m2pimpi0", "m2pim + m2pi0 + 2*(E2E3 - p2p3)");
  treeExtracted->SetAlias("m2pippi0", "m2pip + m2pi0 + 2*(E1E3 - p1p3)");

  // for the cuts
  treeExtracted->SetAlias("mpippim", "TMath::Sqrt(m2pippim)");
  treeExtracted->SetAlias("mpimpi0", "TMath::Sqrt(m2pimpi0)");
  treeExtracted->SetAlias("mpippi0", "TMath::Sqrt(m2pippi0)");

  // omega
  treeExtracted->SetAlias("Eomega", "Epip + Epim + fE[0] + fE[1]");
  treeExtracted->SetAlias("Pxomega", "fX[0] + fX[1] + fX[2] + fX[3]");
  treeExtracted->SetAlias("Pyomega", "fY[0] + fY[1] + fY[2] + fY[3]");
  treeExtracted->SetAlias("Pzomega", "fZ[0] + fZ[1] + fZ[2] + fZ[3]");
  treeExtracted->SetAlias("p2omega", "Pxomega*Pxomega + Pyomega*Pyomega + Pzomega*Pzomega");
  treeExtracted->SetAlias("momega", "TMath::Sqrt(Eomega*Eomega - Pxomega*Pxomega - Pyomega*Pyomega - Pzomega*Pzomega)");
  treeExtracted->SetAlias("deltam", "momega - mpi0 - mpip - mpim");
  treeExtracted->SetAlias("Pomega", "TMath::Sqrt(p2omega)");
}

void drawHorizontalLine(Double_t y) {
  Double_t u;
  gPad->Update(); // necessary
  u = (y - gPad->GetY1())/(gPad->GetY2() - gPad->GetY1());
  // u = (y - y1)/(y2 - y1);
  TLine *liney = new TLine(0.1, u, 0.9, u);
  liney->SetLineWidth(3);
  liney->SetLineColor(kRed);
  liney->SetLineStyle(2);
  liney->SetNDC(kTRUE);
  liney->Draw();
}

void drawBlackHorizontalLine(Double_t y) {
  Double_t u;
  gPad->Update(); // necessary
  u = (y - gPad->GetY1())/(gPad->GetY2() - gPad->GetY1());
  // u = (y - y1)/(y2 - y1);
  TLine *liney = new TLine(0.1, u, 0.9, u);
  liney->SetLineWidth(3);
  liney->SetLineColor(kBlack);
  liney->SetLineStyle(2);
  liney->SetNDC(kTRUE);
  liney->Draw();
}

void drawVerticalLineBlack(Double_t x) {
  Double_t u;
  gPad->Update(); // necessary
  u = (x - gPad->GetX1())/(gPad->GetX2() - gPad->GetX1());
  // u = (x - x1)/(x2 - x1);
  TLine *linex = new TLine(u, 0.1, u, 0.9);
  linex->SetLineWidth(3);
  linex->SetLineColor(kBlack);
  linex->SetLineStyle(2);
  linex->SetNDC(kTRUE);
  linex->Draw();
}

void drawVerticalLine(Double_t x) {
  drawVerticalLineBlack(x);
}

void drawVerticalLineRed(Double_t x) {
  Double_t u;
  gPad->Update(); // necessary
  u = (x - gPad->GetX1())/(gPad->GetX2() - gPad->GetX1());
  // u = (x - x1)/(x2 - x1);
  TLine *linex = new TLine(u, 0.1, u, 0.9);
  linex->SetLineWidth(3);
  linex->SetLineColor(kRed);
  linex->SetLineStyle(2);
  linex->SetNDC(kTRUE);
  linex->Draw();
}

void drawVerticalLineGrayest(Double_t x) {
  Double_t u;
  gPad->Update(); // necessary
  u = (x - gPad->GetX1())/(gPad->GetX2() - gPad->GetX1());
  // u = (x - x1)/(x2 - x1);
  TLine *linex = new TLine(u, 0.1, u, 0.9);
  linex->SetLineWidth(3);
  linex->SetLineColor(kGray+3);
  linex->SetLineStyle(2);
  linex->SetNDC(kTRUE);
  linex->Draw();
}

void drawVerticalLineGrayer(Double_t x) {
  Double_t u;
  gPad->Update(); // necessary
  u = (x - gPad->GetX1())/(gPad->GetX2() - gPad->GetX1());
  // u = (x - x1)/(x2 - x1);
  TLine *linex = new TLine(u, 0.1, u, 0.9);
  linex->SetLineWidth(3);
  linex->SetLineColor(kGray+2);
  linex->SetLineStyle(2);
  linex->SetNDC(kTRUE);
  linex->Draw();
}

void drawVerticalLineGray(Double_t x) {
  Double_t u;
  gPad->Update(); // necessary
  u = (x - gPad->GetX1())/(gPad->GetX2() - gPad->GetX1());
  // u = (x - x1)/(x2 - x1);
  TLine *linex = new TLine(u, 0.1, u, 0.9);
  linex->SetLineWidth(3);
  linex->SetLineColor(kGray+1);
  linex->SetLineStyle(2);
  linex->SetNDC(kTRUE);
  linex->Draw();
}

TString particleName(Int_t particleID) {
  if (particleID == 223) return "omega";
  else if (particleID == 111) return "pi0";
  else if (particleID == 211) return "pip";
  else if (particleID == -211) return "pim";
}













/*
    // my own normalization
    simrecHistA->Draw("HIST");
    c->Update();
    //scale hint1 to the pad coordinates
    Float_t rightmax2 = 1.1*simrecHistC->GetMaximum();
    Float_t scale2 = gPad->GetUymax()/rightmax2;
    simrecHistC->Scale(scale2);
    simrecHistC->Draw("SAME HIST");
    //draw an axis on the right side
    TGaxis *axis2 = new TGaxis(gPad->GetUxmax(), gPad->GetUymin(),
			       gPad->GetUxmax(), gPad->GetUymax(),
			       0, rightmax2, 510, "+L");
    axis2->SetLineColor(kRed);
    axis2->SetLabelColor(kRed);
    axis2->Draw();
*/