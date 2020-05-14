#include "analysisConfig.h"

#include "TClasTool.h"
#include "TIdentificator.h"

#include <algorithm>
#include "ROOT/RDataFrame.hxx"
#include "ROOT/RVec.hxx"

int main() {

  /*** 1 ***/
  /*
  std::cout << "PRODIR=" << proDir << std::endl;

  for (Int_t i = 0; i < 6; i++) {
    std::cout << "z" << i << " - " << edgesZ[i] << std::endl;
  }
  std::cout << std::endl;
  */
  
  /*** 2 ***/
  /*
  // init ClasTool
  TClasTool *input = new TClasTool();
  input->InitDSTReader("ROOTDSTR");
  
  TString currentFile = "/home/borquez/Downloads/recsisC_0028.root";
  input->Add(currentFile);

  Double_t nEntries = (Double_t) input->GetEntries();
  std::cout << "nEntries = " << nEntries << std::endl;
  */

  /*** 3 ***/
  /*
  // define TIdentificator and start!
  TIdentificator *t = new TIdentificator(input);
  input->Next();
    
  // loop in evnt
  for (Int_t k = 0; k < 10; k++) { // should be nEntries

    Int_t nRows = input->GetNRows("EVNT");
    Int_t nRowsG = input->GetNRows("GSIM");
    std::cout << "(nRows, nRowsG) = (" << nRows << ", " << nRowsG << ")" << std::endl;
    std::cout << std::endl;

    std::cout << "Event " << k << std::endl;

    // reset electron values
    Int_t    eflag = 0;
    
    Int_t    mc_eid = -9999;
    Double_t mc_epx = -9999.;
    Double_t mc_epy = -9999.;
    Double_t mc_epz = -9999.;
    
    TString  eid = "none";
    Double_t epx = -9999.;
    Double_t epy = -9999.;
    Double_t epz = -9999.;
    
    // found electron in gsim!
    if (t->Id(0, 1) == 11) {
      
      mc_eid = t->Id(0,1);
      mc_epx = t->Px(0,1);
      mc_epy = t->Py(0,1);
      mc_epz = t->Pz(0,1);

      // found electron in simrec!
      if ((t->GetCategorization(0, "Sim", true)) == "electron") {	
	eid = t->GetCategorization(0,"Sim", true);
	epx = t->Px(0);
	epy = t->Py(0);
	epz = t->Pz(0);
	eflag = 1;
      }

      // fill electron tree
      std::cout << "0 : (" << mc_eid << ", " << mc_epx << ", " << mc_epy << ", " << mc_epz << ", "
		<< eid << ", " << epx << ", " << epy << ", " << epz << ")" << std::endl;
      
      // loop in the rest of particles
      for (Int_t i = 1; i < input->GetNRows("GSIM"); i++) {

	// reset values
	Int_t    mc_id = -9999;
	Double_t mc_px = -9999.;
	Double_t mc_py = -9999.;
	Double_t mc_pz = -9999.;
	
	TString  id = "none";
	Double_t px = -9999.;
	Double_t py = -9999.;
	Double_t pz = -9999.;

	Int_t gflag = 0;
	Int_t rflag = 0;
	
      	if (t->Id(i,1) == 22 || t->Id(i,1) == -211 || t->Id(i,1) == 211) {
	  mc_id = t->Id(i,1);
	  mc_px = t->Px(i,1);
	  mc_py = t->Py(i,1);
	  mc_pz = t->Pz(i,1);
	  gflag = 1;
	}

	if (eflag && i < input->GetNRows("EVNT")) {
	  TString category = t->GetCategorization(i, "Sim");
	  
	  if (category == "gamma" || category == "pi-" || category == "high energy pion +" || category == "low energy pion +" || category == "s_electron" || category == "positron") {
	    id = t->GetCategorization(i, "Sim");
	    px = t->Px(i);
	    py = t->Py(i);
	    pz = t->Pz(i);
	    rflag = 1;
	  }
	}
	
	// fill hadron tree
	if (gflag || rflag) {
	  std::cout << i << " : (" << mc_id << ", " << mc_px << ", " << mc_py << ", " << mc_pz << ", "
		    << id << ", " << px << ", " << py << ", " << pz << ")" << std::endl;
	}

      } // end of loop in the rest of particles
      
      std::cout << std::endl;
    } // end of electron at gsim condition
    
    input->Next();
  } // end of loop in event
  */
  /*** 4 ***/

  // go parallel
  // ROOT::EnableImplicitMT();

  // open tree
  /*
  ROOT::RDataFrame d("ntuple_sim", "/home/borquez/omegaThesis/out/prunedSim/old/C/pruned_out.root");

  auto numero = d.Count();
  std::cout << *numero << std::endl;
  numero = d.Filter("mc_evnt != -9999").Count();
  std::cout << *numero << std::endl;
  numero = d.Filter("mc_evnt == -9999").Count();
  std::cout << *numero << std::endl;
  numero = d.Filter("evnt == -9999").Count();
  std::cout << *numero << std::endl;
  numero = d.Filter("evnt == 25 || mc_evnt == 25").Count();
  std::cout << *numero << std::endl;
  std::cout << *d.Filter("evnt == 25 || mc_evnt == 25").Count() << std::endl;
  std::cout << std::endl;

  auto d2 = d.Filter("evnt == 15 || mc_evnt == 15")
    .Define("P2", "Px*Px + Py*Py + Pz*Pz")
    .Define("mc_P2", "mc_Px*mc_Px + mc_Py*mc_Py + mc_Pz*mc_Pz");
  
  d2.Display({"evnt", "pid", "P2", "mc_evnt", "mc_pid", "mc_P2"}, 12)->Print();
  std::cout << std::endl;
  
  d2.Foreach([](Float_t i){std::cout << i << std::endl;}, {"P2"});
  std::cout << std::endl;
  
  auto tomado = d2.Take<float>("mc_pid");
  Int_t peo = (Int_t) tomado->size();
  std::cout << "size:" << peo << std::endl;
  std::cout << "content:" << std::endl;
  for (Int_t i : *tomado) std::cout << "  " << i << std::endl;
  bool poto = (std::find(tomado->begin(), tomado->end(), -211) != tomado->end() && std::find(tomado->begin(), tomado->end(), 211) != tomado->end() && std::find(tomado->begin(), tomado->end(), 22) != tomado->end());
  bool poto2 = (std::count(tomado->begin(), tomado->end(), -211) >= 1 && std::count(tomado->begin(), tomado->end(), 211) >= 1 && std::count(tomado->begin(), tomado->end(), 22) >= 2);

  auto tomado3 = d2.Take<float>("pid");
  bool poto3 = (std::count(tomado3->begin(), tomado3->end(), -211) >= 1 && std::count(tomado3->begin(), tomado3->end(), 211) >= 1 && std::count(tomado3->begin(), tomado3->end(), 22) >= 2);
  
  std::cout << "has omega in gsim: " << poto2 << std::endl;
  std::cout << "has omega in simrec: " << poto3 << std::endl;
  */
  /*** 5 ***/
  // color stuff for 2D plots (from smoran)
  const Int_t NRGBs = 5;
  const Int_t NCont = 255;
  
  Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
  Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
  Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
  Double_t blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
  
  TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
  gStyle->SetNumberContours(NCont);
}
