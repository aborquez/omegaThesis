/*****************************************/
/*  FilterNCombine.cxx                   */
/*                                       */
/*  Author: Andrés Bórquez               */
/*                                       */
/*****************************************/

// UPDATE:
// - added proper photons energy corrections

#include "analysisConfig.h"

/*** Constants ***/

Float_t kEbeam = 5.014;

/*** Global variables ***/

// options
TString targetOption;
Int_t   simFlag = 0;
TString setOption;
TString NjlabDir;
TString rnOption;
TString analyserOption; // new, for photon's energy correction

TString treeName;

TString outDir; // depends on data type
TString outFile;

// input
TString inputFile;

// There are 23 in total
Float_t tTargType;
Float_t tQ2, tNu, tXb, tW;
Float_t tSector;
Float_t tPhiPQ, tThetaPQ;
Float_t tE;
Float_t tEvent;
Float_t tPx, tPy, tPz;
Float_t tXec, tYec, tZec;
Float_t tECX, tECY, tECZ;
Float_t tPex, tPey, tPez;
Float_t tPid;

// added 23 more!
Float_t tmc_TargType;
Float_t tmc_Q2, tmc_Nu, tmc_Xb, tmc_W;
Float_t tmc_Sector;
Float_t tmc_PhiPQ, tmc_ThetaPQ;
Float_t tmc_E;
Float_t tmc_Event;
Float_t tmc_Px, tmc_Py, tmc_Pz;
Float_t tmc_Xec, tmc_Yec, tmc_Zec;
Float_t tmc_ECX, tmc_ECY, tmc_ECZ;
Float_t tmc_Pex, tmc_Pey, tmc_Pez;
Float_t tmc_Pid;

// new
TString eventBranchName;

/*** Declaration of functions ***/

inline int parseCommandLine(int argc, char* argv[]);
void printUsage();
void assignOptions();
void printOptions();

Float_t CorrectGammaEnergy(Float_t fE); // it also depends on a global variable called "analyserOption"
Float_t CorrectGammaMomentum(Int_t i); // i stands for component
Float_t PhiPQ(Float_t fPx, Float_t fPy, Float_t fPz);
Float_t ThetaPQ(Float_t fPx, Float_t fPy, Float_t fPz);

int main(int argc, char **argv) {

  parseCommandLine(argc, argv);
  assignOptions();
  printOptions();
  
  // dir structure, just in case
  system("mkdir -p " + outDir);

  /*** Init tree ***/
  
  // hadrons (t - tree)
  TChain *t = new TChain();
  t->Add(inputFile + "/" + treeName); // input

  t->SetBranchStatus("*", 0);
  
  t->SetBranchStatus("TargType", 1);
  t->SetBranchStatus("Q2", 1);
  t->SetBranchStatus("Nu", 1);
  t->SetBranchStatus("Xb", 1);
  t->SetBranchStatus("W", 1);
  t->SetBranchStatus("SectorEl", 1);  
  t->SetBranchStatus("PhiPQ", 1);
  t->SetBranchStatus("ThetaPQ", 1);  
  t->SetBranchStatus("E", 1);
  t->SetBranchStatus("evnt", 1);
  t->SetBranchStatus("Px", 1);
  t->SetBranchStatus("Py", 1);
  t->SetBranchStatus("Pz", 1);
  t->SetBranchStatus("Xec", 1);
  t->SetBranchStatus("Yec", 1);
  t->SetBranchStatus("Zec", 1);
  t->SetBranchStatus("ECX", 1);
  t->SetBranchStatus("ECY", 1);
  t->SetBranchStatus("ECZ", 1);
  t->SetBranchStatus("Pex", 1);
  t->SetBranchStatus("Pey", 1);
  t->SetBranchStatus("Pez", 1);
  t->SetBranchStatus("pid", 1);

  t->SetBranchAddress("TargType", &tTargType);
  t->SetBranchAddress("Q2", &tQ2);
  t->SetBranchAddress("Nu", &tNu);
  t->SetBranchAddress("Xb", &tXb);
  t->SetBranchAddress("W", &tW);
  t->SetBranchAddress("SectorEl", &tSector);  
  t->SetBranchAddress("PhiPQ", &tPhiPQ);
  t->SetBranchAddress("ThetaPQ", &tThetaPQ);
  t->SetBranchAddress("E", &tE);
  t->SetBranchAddress("evnt", &tEvent);
  t->SetBranchAddress("Px", &tPx);
  t->SetBranchAddress("Py", &tPy);
  t->SetBranchAddress("Pz", &tPz);
  t->SetBranchAddress("Xec", &tZec);
  t->SetBranchAddress("Yec", &tZec);
  t->SetBranchAddress("Zec", &tZec);
  t->SetBranchAddress("ECX", &tECX);
  t->SetBranchAddress("ECY", &tECY);
  t->SetBranchAddress("ECZ", &tECZ);
  t->SetBranchAddress("Pex", &tPex);
  t->SetBranchAddress("Pey", &tPey);
  t->SetBranchAddress("Pez", &tPez);
  t->SetBranchAddress("pid", &tPid);

  if (simFlag) {
    t->SetBranchStatus("mc_TargType", 1);
    t->SetBranchStatus("mc_Q2", 1);
    t->SetBranchStatus("mc_Nu", 1);
    t->SetBranchStatus("mc_Xb", 1);
    t->SetBranchStatus("mc_W", 1);
    t->SetBranchStatus("mc_SectorEl", 1);  
    t->SetBranchStatus("mc_PhiPQ", 1);
    t->SetBranchStatus("mc_ThetaPQ", 1);  
    t->SetBranchStatus("mc_E", 1);
    t->SetBranchStatus("mc_evnt", 1);
    t->SetBranchStatus("mc_Px", 1);
    t->SetBranchStatus("mc_Py", 1);
    t->SetBranchStatus("mc_Pz", 1);
    t->SetBranchStatus("mc_Xec", 1);
    t->SetBranchStatus("mc_Yec", 1);
    t->SetBranchStatus("mc_Zec", 1);
    t->SetBranchStatus("mc_ECX", 1);
    t->SetBranchStatus("mc_ECY", 1);
    t->SetBranchStatus("mc_ECZ", 1);
    t->SetBranchStatus("mc_Pex", 1);
    t->SetBranchStatus("mc_Pey", 1);
    t->SetBranchStatus("mc_Pez", 1);
    t->SetBranchStatus("mc_pid", 1);

    t->SetBranchAddress("mc_TargType", &tmc_TargType);
    t->SetBranchAddress("mc_Q2",       &tmc_Q2);
    t->SetBranchAddress("mc_Nu",       &tmc_Nu);
    t->SetBranchAddress("mc_Xb",       &tmc_Xb);
    t->SetBranchAddress("mc_W",        &tmc_W);
    t->SetBranchAddress("mc_SectorEl", &tmc_Sector);  
    t->SetBranchAddress("mc_PhiPQ",    &tmc_PhiPQ);
    t->SetBranchAddress("mc_ThetaPQ",  &tmc_ThetaPQ);
    t->SetBranchAddress("mc_E",        &tmc_E);
    t->SetBranchAddress("mc_evnt",     &tmc_Event);
    t->SetBranchAddress("mc_Px",  &tmc_Px);
    t->SetBranchAddress("mc_Py",  &tmc_Py);
    t->SetBranchAddress("mc_Pz",  &tmc_Pz);
    t->SetBranchAddress("mc_Xec", &tmc_Zec);
    t->SetBranchAddress("mc_Yec", &tmc_Zec);
    t->SetBranchAddress("mc_Zec", &tmc_Zec);
    t->SetBranchAddress("mc_ECX", &tmc_ECX);
    t->SetBranchAddress("mc_ECY", &tmc_ECY);
    t->SetBranchAddress("mc_ECZ", &tmc_ECZ);
    t->SetBranchAddress("mc_Pex", &tmc_Pex);
    t->SetBranchAddress("mc_Pey", &tmc_Pey);
    t->SetBranchAddress("mc_Pez", &tmc_Pez);
    t->SetBranchAddress("mc_pid", &tmc_Pid);
  }
    
  /*** Output settings ***/
  
  TFile *rootFile = new TFile(outFile, "RECREATE", "Omega Meson Filtered Combinations"); // output

  // 29 variables for each original detected particle
  Int_t   oTargType;
  Float_t oQ2, oNu, oXb, oYb, oW; // +1
  Float_t oZ, oPt2; // +2, recalculated
  Int_t   oSector;
  Float_t oPhiPQ, oThetaPQ, oCosThetaPQ; // +1
  Float_t oE;
  Int_t   oEvent;
  Float_t oPx, oPy, oPz;
  Float_t oP2; // +1
  Int_t   oPid;
  Float_t oXec, oYec, oZec;
  Float_t oECX, oECY, oECZ;
  Float_t oPex, oPey, oPez;
  Int_t   oEntry; // +1

  // 29 more variables for original generated particles
  Int_t   omc_TargType;
  Float_t omc_Q2, omc_Nu, omc_Xb, omc_Yb, omc_W; // +1
  Float_t omc_Z, omc_Pt2; // +2, recalculated
  Int_t   omc_Sector;
  Float_t omc_PhiPQ, omc_ThetaPQ, omc_CosThetaPQ; // +1
  Float_t omc_E;
  Int_t   omc_Event;
  Float_t omc_Px, omc_Py, omc_Pz;
  Float_t omc_P2; // +1
  Int_t   omc_Pid;
  Float_t omc_Xec, omc_Yec, omc_Zec;
  Float_t omc_ECX, omc_ECY, omc_ECZ;
  Float_t omc_Pex, omc_Pey, omc_Pez;
  Int_t   omc_Entry; // +1

  // 34 variables for mix of particles (6 are vectors)
  Int_t mTargType;
  Float_t mQ2, mNu, mXb, mYb, mW; // +1
  Float_t mZ, mPt2; // +2, recalculated
  Int_t mSector;
  Float_t mPhiPQ, mThetaPQ, mCosThetaPQ; // +1
  Float_t mE[4], mPx[4], mPy[4], mPz[4];
  Int_t mEvent, nPip, nPim, nGamma; // +3
  Int_t mPid[4];
  Float_t mXec, mYec, mZec;
  Float_t mPex, mPey, mPez;
  Int_t mEntry[4]; // +1

  Float_t pipP2, pipM;
  Float_t pimP2, pimM;
  Float_t pippimP, pippimE, pippimM;
  Float_t pi0Px, pi0Py, pi0Pz;
  Float_t pi0P2, pi0CosTheta, pi0E, pi0M;
  
  Float_t wE, wP2;
  Float_t wPx, wPy, wPz;
  Float_t wM, wD;

  // 34 more variables for mix of generated particles (6 are vectors)
  Int_t   mmc_TargType;
  Float_t mmc_Q2, mmc_Nu, mmc_Xb, mmc_Yb, mmc_W; // +1
  Float_t mmc_Z, mmc_Pt2; // +2, recalculated
  Int_t   mmc_Sector;
  Float_t mmc_PhiPQ, mmc_ThetaPQ, mmc_CosThetaPQ; // +1
  Float_t mmc_E[4], mmc_Px[4], mmc_Py[4], mmc_Pz[4];
  Int_t   mmc_Event, nmc_Pip, nmc_Pim, nmc_Gamma; // +3
  Int_t   mmc_Pid[4];
  Float_t mmc_Xec, mmc_Yec, mmc_Zec;
  Float_t mmc_Pex, mmc_Pey, mmc_Pez;
  Int_t   mmc_Entry[4]; // +1

  Float_t mc_pipP2, mc_pipM;
  Float_t mc_pimP2, mc_pimM;
  Float_t mc_pippimP, mc_pippimE, mc_pippimM;
  Float_t mc_pi0Px, mc_pi0Py, mc_pi0Pz;
  Float_t mc_pi0P2, mc_pi0CosTheta, mc_pi0E, mc_pi0M;
  
  Float_t mc_wE, mc_wP2;
  Float_t mc_wPx, mc_wPy, mc_wPz;
  Float_t mc_wM, mc_wD;

  TTree *tOriginal = new TTree("original", "Original particles");
  tOriginal->Branch("TargType", &oTargType);
  tOriginal->Branch("Q2", &oQ2);
  tOriginal->Branch("Nu", &oNu);
  tOriginal->Branch("Xb", &oXb);
  tOriginal->Branch("Yb", &oYb);
  tOriginal->Branch("W", &oW);
  tOriginal->Branch("Z", &oZ);
  tOriginal->Branch("Pt2", &oPt2);
  tOriginal->Branch("Sector", &oSector);
  tOriginal->Branch("PhiPQ", &oPhiPQ);
  tOriginal->Branch("ThetaPQ", &oThetaPQ);
  tOriginal->Branch("CosThetaPQ", &oCosThetaPQ);
  tOriginal->Branch("E", &oE);
  tOriginal->Branch("Event", &oEvent);
  tOriginal->Branch("Px", &oPx);
  tOriginal->Branch("Py", &oPy);
  tOriginal->Branch("Pz", &oPz);
  tOriginal->Branch("pid", &oPid);
  tOriginal->Branch("Xec", &oXec);
  tOriginal->Branch("Yec", &oYec);
  tOriginal->Branch("Zec", &oZec);
  tOriginal->Branch("ECX", &oECX);
  tOriginal->Branch("ECY", &oECY);
  tOriginal->Branch("ECZ", &oECZ);
  tOriginal->Branch("Pex", &oPex);
  tOriginal->Branch("Pey", &oPey);
  tOriginal->Branch("Pez", &oPez);
  tOriginal->Branch("P2", &oP2);
  tOriginal->Branch("entry", &oEntry);
  
  if (simFlag) {
    tOriginal->Branch("mc_TargType", &omc_TargType);
    tOriginal->Branch("mc_Q2",  &omc_Q2);
    tOriginal->Branch("mc_Nu",  &omc_Nu);
    tOriginal->Branch("mc_Xb",  &omc_Xb);
    tOriginal->Branch("mc_Yb",  &omc_Yb);
    tOriginal->Branch("mc_W",   &omc_W);
    tOriginal->Branch("mc_Z",   &omc_Z);
    tOriginal->Branch("mc_Pt2", &omc_Pt2);
    tOriginal->Branch("mc_Sector",     &omc_Sector);
    tOriginal->Branch("mc_PhiPQ",      &omc_PhiPQ);
    tOriginal->Branch("mc_ThetaPQ",    &omc_ThetaPQ);
    tOriginal->Branch("mc_CosThetaPQ", &omc_CosThetaPQ);
    tOriginal->Branch("mc_E",     &omc_E);
    tOriginal->Branch("mc_Event", &omc_Event);
    tOriginal->Branch("mc_Px",    &omc_Px);
    tOriginal->Branch("mc_Py",    &omc_Py);
    tOriginal->Branch("mc_Pz",    &omc_Pz);
    tOriginal->Branch("mc_pid", &omc_Pid);
    tOriginal->Branch("mc_Xec", &omc_Xec);
    tOriginal->Branch("mc_Yec", &omc_Yec);
    tOriginal->Branch("mc_Zec", &omc_Zec);
    tOriginal->Branch("mc_ECX", &omc_ECX);
    tOriginal->Branch("mc_ECY", &omc_ECY);
    tOriginal->Branch("mc_ECZ", &omc_ECZ);
    tOriginal->Branch("mc_Pex", &omc_Pex);
    tOriginal->Branch("mc_Pey", &omc_Pey);
    tOriginal->Branch("mc_Pez", &omc_Pez);
    tOriginal->Branch("mc_P2", &omc_P2);
    tOriginal->Branch("mc_entry", &omc_Entry);
  }

  TTree *tMix = new TTree("mix", "Combination of particles");
  tMix->Branch("TargType", &mTargType);
  tMix->Branch("Q2", &mQ2);
  tMix->Branch("Nu", &mNu);
  tMix->Branch("Xb", &mXb);
  tMix->Branch("Yb", &mYb);
  tMix->Branch("W",  &mW);
  tMix->Branch("Z",  &mZ);
  tMix->Branch("Pt2", &mPt2);
  tMix->Branch("Sector", &mSector);
  tMix->Branch("PhiPQ",      &mPhiPQ);
  tMix->Branch("ThetaPQ",    &mThetaPQ);
  tMix->Branch("CosThetaPQ", &mCosThetaPQ);
  tMix->Branch("E", &mE, "E[4]/F");
  tMix->Branch("Event", &mEvent);
  tMix->Branch("nPip",   &nPip);
  tMix->Branch("nPim",   &nPim);
  tMix->Branch("nGamma", &nGamma);
  tMix->Branch("Px", &mPx, "Px[4]/F");
  tMix->Branch("Py", &mPy, "Py[4]/F");
  tMix->Branch("Pz", &mPz, "Pz[4]/F");
  tMix->Branch("pid", &mPid, "pid[4]/I");
  tMix->Branch("Xec", &mXec);
  tMix->Branch("Yec", &mYec);
  tMix->Branch("Zec", &mZec);
  tMix->Branch("Pex", &mPex);
  tMix->Branch("Pey", &mPey);
  tMix->Branch("Pez", &mPez);
  tMix->Branch("entry", &mEntry, "entry[4]/I");

  // pions variables (4 + 7 + 3)
  tMix->Branch("pipP2", &pipP2);
  tMix->Branch("pipM",  &pipM);
  tMix->Branch("pimP2", &pimP2);
  tMix->Branch("pimM",  &pimM);
  tMix->Branch("pi0Px", &pi0Px);
  tMix->Branch("pi0Py", &pi0Py);
  tMix->Branch("pi0Pz", &pi0Pz);  
  tMix->Branch("pi0P2", &pi0P2);
  tMix->Branch("pi0CosTheta", &pi0CosTheta);  
  tMix->Branch("pi0M", &pi0M);
  tMix->Branch("pi0E", &pi0E);
  tMix->Branch("pippimE", &pippimE);  
  tMix->Branch("pippimP", &pippimP);
  tMix->Branch("pippimM", &pippimM);
  
  // omega variables (7)
  tMix->Branch("wE",  &wE);
  tMix->Branch("wPx", &wPx);
  tMix->Branch("wPy", &wPy);
  tMix->Branch("wPz", &wPz);
  tMix->Branch("wP2", &wP2);
  tMix->Branch("wM",  &wM);
  tMix->Branch("wD",  &wD);

  if (simFlag) {
    tMix->Branch("mc_TargType", &mmc_TargType);
    tMix->Branch("mc_Q2", &mmc_Q2);
    tMix->Branch("mc_Nu", &mmc_Nu);
    tMix->Branch("mc_Xb", &mmc_Xb);
    tMix->Branch("mc_Yb", &mmc_Yb);
    tMix->Branch("mc_W",  &mmc_W);
    tMix->Branch("mc_Z",  &mmc_Z);
    tMix->Branch("mc_Pt2", &mmc_Pt2);
    tMix->Branch("mc_Sector", &mmc_Sector);
    tMix->Branch("mc_PhiPQ",      &mmc_PhiPQ);
    tMix->Branch("mc_ThetaPQ",    &mmc_ThetaPQ);
    tMix->Branch("mc_CosThetaPQ", &mmc_CosThetaPQ);
    tMix->Branch("mc_E", &mmc_E, "E[4]/F");
    tMix->Branch("mc_Event", &mmc_Event);
    tMix->Branch("mc_nPip",   &nmc_Pip);
    tMix->Branch("mc_nPim",   &nmc_Pim);
    tMix->Branch("mc_nGamma", &nmc_Gamma);
    tMix->Branch("mc_Px", &mmc_Px, "Px[4]/F");
    tMix->Branch("mc_Py", &mmc_Py, "Py[4]/F");
    tMix->Branch("mc_Pz", &mmc_Pz, "Pz[4]/F");
    tMix->Branch("mc_pid", &mmc_Pid, "pid[4]/I");
    tMix->Branch("mc_Xec", &mmc_Xec);
    tMix->Branch("mc_Yec", &mmc_Yec);
    tMix->Branch("mc_Zec", &mmc_Zec);
    tMix->Branch("mc_Pex", &mmc_Pex);
    tMix->Branch("mc_Pey", &mmc_Pey);
    tMix->Branch("mc_Pez", &mmc_Pez);
    tMix->Branch("mc_entry", &mmc_Entry, "entry[4]/I");
    
    // pions variables (4 + 7 + 3)
    tMix->Branch("mc_pipP2", &mc_pipP2);
    tMix->Branch("mc_pipM",  &mc_pipM);
    tMix->Branch("mc_pimP2", &mc_pimP2);
    tMix->Branch("mc_pimM",  &mc_pimM);
    tMix->Branch("mc_pi0Px", &mc_pi0Px);
    tMix->Branch("mc_pi0Py", &mc_pi0Py);
    tMix->Branch("mc_pi0Pz", &mc_pi0Pz);  
    tMix->Branch("mc_pi0P2", &mc_pi0P2);
    tMix->Branch("mc_pi0CosTheta", &mc_pi0CosTheta);  
    tMix->Branch("mc_pi0M", &mc_pi0M);
    tMix->Branch("mc_pi0E", &mc_pi0E);
    tMix->Branch("mc_pippimE", &mc_pippimE);  
    tMix->Branch("mc_pippimP", &mc_pippimP);
    tMix->Branch("mc_pippimM", &mc_pippimM);
    
    // omega variables (7)
    tMix->Branch("mc_wE",  &mc_wE);
    tMix->Branch("mc_wPx", &mc_wPx);
    tMix->Branch("mc_wPy", &mc_wPy);
    tMix->Branch("mc_wPz", &mc_wPz);
    tMix->Branch("mc_wP2", &mc_wP2);
    tMix->Branch("mc_wM",  &mc_wM);
    tMix->Branch("mc_wD",  &mc_wD);
  }

  /*** START FILTERING ***/

  // counting variables
  Int_t nPipThisEvent = 0;
  Int_t nPimThisEvent = 0;
  Int_t nGammaThisEvent = 0;
  Int_t nCombThisEvent = 0;

  Int_t nOmega = 0;
  Int_t nAtLeastOmega = 0;

  Int_t nEvents = 0;
  
  // mc counting variables
  Int_t nMCPipThisEvent = 0;
  Int_t nMCPimThisEvent = 0;
  Int_t nMCGammaThisEvent = 0;
  Int_t nMCCombThisEvent = 0;

  Int_t nMCOmega = 0;
  Int_t nAtLeastMCOmega = 0;

  // combination vectors!
  std::vector<std::vector <int>> combVector;
  std::vector<std::vector <int>> mc_combVector;
  
  /*** START ***/
  
  // define entrylist
  TEntryList *l;

  // on the loop
  Int_t start, finish;
  if (!simFlag) {
    start = (Int_t) t->GetMinimum(eventBranchName);
    finish = (Int_t) t->GetMaximum(eventBranchName);
  } else if (simFlag) {
    start = 0;
    finish = (Int_t) t->GetMaximum(eventBranchName);
  }
  
  // loop in events
  // i = event number
  for (Int_t i = start; i <= finish; i++) { // t->GetMinimum(eventBranchName), t->GetMaximum(eventBranchName)
    
    std::cout << "Current event number: " << i << std::endl;
    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
    
    TString theCondition;
    TString listName = Form("elist_%d", i);

    if (simFlag) theCondition = Form("evnt == %d || mc_evnt == %d", i, i);
    else theCondition = Form("evnt == %d", i);

    // selects all entries from the same event
    t->Draw(">>" + listName, theCondition, "entrylist");
    l = (TEntryList*) gDirectory->Get(listName);
    t->SetEntryList(l);
      
    // looks at the entries (particles) of the current event
    // j = iterator, jj = real entry number
    for (Int_t j = 0; j < (Int_t) l->GetN(); j++) {
      Int_t jj = t->GetEntryNumber(j);
      t->GetEntry(jj);
      std::cout << "  Entry number: " << jj << std::endl;
      std::cout << "  pid:          " << tPid << std::endl;
      std::cout << "  mc_pid:       " << tmc_Pid << std::endl;
      std::cout << "  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl;
      // count the data/simrec particles
      if (tPid == (Float_t) 211) nPipThisEvent++;
      if (tPid == (Float_t) -211) nPimThisEvent++;
      if (tPid == (Float_t) 22) nGammaThisEvent++;
      // count the gsim particles
      if (tmc_Pid == (Float_t) 211) nMCPipThisEvent++;
      if (tmc_Pid == (Float_t) -211) nMCPimThisEvent++;
      if (tmc_Pid == (Float_t) 22) nMCGammaThisEvent++;
    }
    
    // show counts
    std::cout << "  nPip     = " << nPipThisEvent << std::endl;
    std::cout << "  nPim     = " << nPimThisEvent << std::endl;
    std::cout << "  nGamma   = " << nGammaThisEvent << std::endl;
    std::cout << "  nMCPip   = " << nMCPipThisEvent << std::endl;
    std::cout << "  nMCPim   = " << nMCPimThisEvent << std::endl;
    std::cout << "  nMCGamma = " << nMCGammaThisEvent << std::endl;
    std::cout << "  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl;

    // (as you can see, these conditions are not exclusive)
    // candidate appeared for data/simrec
    if (nPipThisEvent >= 1 && nPimThisEvent >= 1 && nGammaThisEvent >= 2) {
      nAtLeastOmega++;
      nCombThisEvent = TMath::Binomial(nPipThisEvent, 1)*TMath::Binomial(nPimThisEvent, 1)*TMath::Binomial(nGammaThisEvent, 2);
      nOmega += nCombThisEvent;
    }

    // candidate appeared for gsim
    if (nMCPipThisEvent >= 1 && nMCPimThisEvent >= 1 && nMCGammaThisEvent >= 2) {
      nAtLeastMCOmega++;
      nMCCombThisEvent = TMath::Binomial(nMCPipThisEvent, 1)*TMath::Binomial(nMCPimThisEvent, 1)*TMath::Binomial(nMCGammaThisEvent, 2);
      nMCOmega += nMCCombThisEvent;
    }

    std::cout << "  There are " << nCombThisEvent << " omegas!" << std::endl;
    std::cout << "  There are " << nCombThisEvent << " reconstructed omegas!" << std::endl;
    std::cout << "  There are " << nMCCombThisEvent << " generated omegas!" << std::endl;
    std::cout << "  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl;

    /*** ORIGINAL ***/

    // saves all information from the original particles from an approved event, with no mixing
    // warning: all the hadronic variables correspond to the respective hadron, nothing more
    
    if (nCombThisEvent > 0 || nMCCombThisEvent > 0) {

      // looks at the entries (particles) of the current event
      // j = iterator, jj = real entry number
      for (Int_t j = 0; j < (Int_t) l->GetN(); j++) {
	Int_t jj = t->GetEntryNumber(j);
	t->GetEntry(jj);

	// assigning energy
	// for simrec
	if (tPid == 22) oE = CorrectGammaEnergy(tE);
	else if (tPid == 211 || tPid == -211) oE = TMath::Sqrt(tPx*tPx + tPy*tPy + tPz*tPz + kMpi*kMpi);
	// for gsim
	if (tmc_Pid == 22 || tmc_Pid == 211 || tmc_Pid == -211) oE = tmc_E;

	// assigning momentum
	// for simrec
	if (tPid == 22) {
	  oPx = CorrectGammaMomentum(0);
	  oPy = CorrectGammaMomentum(1);
	  oPz = CorrectGammaMomentum(2);
	} else if (tPid == 211 || tPid == -211) {
	  oPx = tPx; oPy = tPy; oPz = tPz;
	}
	// for gsim
        if (tmc_Pid == 22 || tmc_Pid == 211 || tmc_Pid == -211) {
	  oPx = tmc_Px; oPy = tmc_Py; oPz = tmc_Pz;
	}

	// all the other variables...
	// for simrec
	oTargType = (Int_t) tTargType;
	oQ2 = tQ2;
	oNu = tNu;
	oXb = tXb;
	oYb = oNu/kEbeam;                   // added
	oW = tW;
	oSector = (Int_t) tSector;
	oZ = oE/oNu;                        // recalculated
	oPhiPQ = PhiPQ(oPx, oPy, oPz);      // updated!
	oThetaPQ = ThetaPQ(oPx, oPy, oPz);  // updated!
	oXec = tXec;
	oYec = tYec;
	oZec = tZec;
	oECX = tECX;
	oECY = tECY;
	oECZ = tECZ;
	oPex = tPex;
	oPey = tPey;
	oPez = tPez;
	oP2 = oPx*oPx + oPy*oPy + oPz*oPz;  // new!
	oCosThetaPQ = ((kEbeam - oPez)*oPz - oPex*oPx - oPey*oPy)/(TMath::Sqrt(oP2*(oQ2 + oNu*oNu))); // new!
	oPt2 = oP2*(1 - oCosThetaPQ*oCosThetaPQ); // recalculated
	oPid = (Int_t) tPid;
	oEntry = jj;
	oEvent = (Int_t) tEvent;

	// all the other variables...
	// for gsim
	omc_TargType = (Int_t) tmc_TargType;
	omc_Q2 = tmc_Q2;
	omc_Nu = tmc_Nu;
	omc_Xb = tmc_Xb;
	omc_Yb = omc_Nu/kEbeam;
	omc_W = tmc_W;
	omc_Sector = (Int_t) tmc_Sector;
	omc_Z = omc_E/omc_Nu;
	omc_PhiPQ = PhiPQ(omc_Px, omc_Py, omc_Pz);
	omc_ThetaPQ = ThetaPQ(omc_Px, omc_Py, omc_Pz);
	omc_Xec = tmc_Xec;
	omc_Yec = tmc_Yec;
	omc_Zec = tmc_Zec;
	omc_ECX = tmc_ECX;
	omc_ECY = tmc_ECY;
	omc_ECZ = tmc_ECZ;
	omc_Pex = tmc_Pex;
	omc_Pey = tmc_Pey;
	omc_Pez = tmc_Pez;
	omc_P2 = omc_Px*omc_Px + omc_Py*omc_Py + omc_Pz*omc_Pz;
	omc_CosThetaPQ = ((kEbeam - omc_Pez)*omc_Pz - omc_Pex*omc_Px - omc_Pey*omc_Py)/(TMath::Sqrt(omc_P2*(omc_Q2 + omc_Nu*omc_Nu)));
	omc_Pt2 = omc_P2*(1 - omc_CosThetaPQ*omc_CosThetaPQ);
	omc_Pid = (Int_t) tmc_Pid;
	omc_Entry = jj;
	omc_Event = (Int_t) tmc_Event;
	
	// fill
	tOriginal->Fill();
	
      } // end of particles loop
    } // end of candidate condition
      
    /*** THE MIXING ***/

    // PART 1: obtain & keep combinations from simrec
    
    // tag
    Int_t jPip = 0;
    Int_t jPim = 0;
    Int_t jGamma1 = 0;
    Int_t jGamma2 = 0;
    
    Int_t partialComb = 0;
    Int_t partialLim = nGammaThisEvent - 1;
    Int_t partialFlag = 1;
    
    if (nCombThisEvent > 0) {
      for (Int_t iPip = 0; iPip < TMath::Binomial(nPipThisEvent, 1); iPip++) {
	
	// find and tag pip (loop in particles)
	// j = iterator, jj = real entry number
	for (Int_t j = 0; j < (Int_t) l->GetN(); j++) {
	  Int_t jj = t->GetEntryNumber(j);
	  t->GetEntry(jj);
	  if (tPid == (Float_t) 211 && jj > jPip) {
	    jPip = jj;      // tag pip
	    j = (Int_t) l->GetN(); // break search
	  }
	}
	
	// force a new beginning for the other particles
	jPim = 0;
	jGamma1 = 0;
	jGamma2 = 0;
	
	for (Int_t iPim = 0; iPim < TMath::Binomial(nPimThisEvent, 1); iPim++) {
	  
	  // find and tag pim
	  // j = iterator, jj = real entry number
	  for (Int_t j = 0; j < (Int_t) l->GetN(); j++) {
	    Int_t jj = t->GetEntryNumber(j);
	    t->GetEntry(jj);
	    if (tPid == (Float_t) -211 && jj > jPim) {
	      jPim = jj; // tag pim
	      j = (Int_t) l->GetN(); // break search
	    }
	  }
	  
	  // force a new beginning for the other particles
	  jGamma1 = 0;
	  jGamma2 = 0;	  
	  
	  for (Int_t iGamma = 0; iGamma < TMath::Binomial(nGammaThisEvent, 2); iGamma++) {
	    
	    // debug
	    if (iGamma == 0) {
	      partialComb = 0;
	      partialLim = nGammaThisEvent - 1;
	      partialFlag = 1;
	    }
	    
	    // the big condition
	    if (partialFlag) {
	      
	      // find and tag gamma1
	      // jj = real entry number, j = iterator
	      for (Int_t j = 0; j < (Int_t) l->GetN(); j++) {
		Int_t jj = t->GetEntryNumber(j);
		t->GetEntry(jj);
		if (tPid == (Float_t) 22 && jj > jGamma1) { // excludes previous gamma1
		  jGamma1 = jj;  // tag gamma1
		  j = (Int_t) l->GetN(); // break search for j
		}
	      }
	      jGamma2 = 0; // resets gamma2
	      partialFlag = 0;
	    }
	    
	    // find and tag gamma2
	    // jj = real entry number, j = iterator
	    for (Int_t j = 0; j < (Int_t) l->GetN(); j++) {
	      Int_t jj = t->GetEntryNumber(j);
	      t->GetEntry(jj);
	      if (tPid == (Float_t) 22 && jj > jGamma2 && jj > jGamma1) { // excludes gamma1 and previous gamma2
		jGamma2 = jj;  // tag gamma2
		j = (Int_t) l->GetN(); // break search for j
	      }
	    }
	    
	    // fix partial comb
	    partialComb++;
	    if (partialComb == partialLim && partialLim > 1) {
	      partialComb = 0;
	      partialLim--;
	      partialFlag = 1;
	    }

	    // fill vector
	    combVector.push_back({jGamma1, jGamma2, jPip, jPim});

	  } // end of loop in gammas
	  
	} // end of loop in pi-
	
      } // end of loop in pi+

    } // end of at-least-one-omega condition
    
    // PART 2: obtain & keep combinations from gsim

    // tag
    jPip = 0;
    jPim = 0;
    jGamma1 = 0;
    jGamma2 = 0;
    
    partialComb = 0;
    partialLim = nMCGammaThisEvent - 1;
    partialFlag = 1;

    if (simFlag && nMCCombThisEvent > 0) {
      for (Int_t iPip = 0; iPip < TMath::Binomial(nMCPipThisEvent, 1); iPip++) {
	
	// find and tag pip (loop in particles)
	// j = iterator, jj = real entry number
	for (Int_t j = 0; j < (Int_t) l->GetN(); j++) {
	  Int_t jj = t->GetEntryNumber(j);
	  t->GetEntry(jj);
	  if (tmc_Pid == (Float_t) 211 && jj > jPip) {
	    jPip = jj;      // tag pip
	    j = (Int_t) l->GetN(); // break search
	  }
	}
	
	// force a new beginning for the other particles
	jPim = 0;
	jGamma1 = 0;
	jGamma2 = 0;
	
	for (Int_t iPim = 0; iPim < TMath::Binomial(nMCPimThisEvent, 1); iPim++) {
	  
	  // find and tag pim
	  // j = iterator, jj = real entry number
	  for (Int_t j = 0; j < (Int_t) l->GetN(); j++) {
	    Int_t jj = t->GetEntryNumber(j);
	    t->GetEntry(jj);
	    if (tmc_Pid == (Float_t) -211 && jj > jPim) {
	      jPim = jj; // tag pim
	      j = (Int_t) l->GetN(); // break search
	    }
	  }
	  
	  // force a new beginning for the other particles
	  jGamma1 = 0;
	  jGamma2 = 0;	  
	  
	  for (Int_t iGamma = 0; iGamma < TMath::Binomial(nMCGammaThisEvent, 2); iGamma++) {
	    
	    // debug
	    if (iGamma == 0) {
	      partialComb = 0;
	      partialLim = nMCGammaThisEvent - 1;
	      partialFlag = 1;
	    }
	    
	    // the big condition
	    if (partialFlag) {
	      
	      // find and tag gamma1
	      // jj = real entry number, j = iterator
	      for (Int_t j = 0; j < (Int_t) l->GetN(); j++) {
		Int_t jj = t->GetEntryNumber(j);
		t->GetEntry(jj);
		if (tmc_Pid == (Float_t) 22 && jj > jGamma1) { // excludes previous gamma1
		  jGamma1 = jj;  // tag gamma1
		  j = (Int_t) l->GetN(); // break search for j
		}
	      }
	      jGamma2 = 0; // resets gamma2
	      partialFlag = 0;
	    }
	  
	    // find and tag gamma2
	    // jj = real entry number, j = iterator
	    for (Int_t j = 0; j < (Int_t) l->GetN(); j++) {
	      Int_t jj = t->GetEntryNumber(j);
	      t->GetEntry(jj);
	      if (tmc_Pid == (Float_t) 22 && jj > jGamma2 && jj > jGamma1) { // excludes gamma1 and previous gamma2
		jGamma2 = jj;  // tag gamma2
		j = (Int_t) l->GetN(); // break search for j
	      }
	    }
	    
	    // fix partial comb
	    partialComb++;
	    if (partialComb == partialLim && partialLim > 1) {
	      partialComb = 0;
	      partialLim--;
	      partialFlag = 1;
	    }

	    // fill vector
	    mc_combVector.push_back({jGamma1, jGamma2, jPip, jPim});

	  } // end of loop in gammas
	  
	} // end of loop in pi-
	
      } // end of loop in pi+

    } // end of at-least-one-omega condition
      
    // PART 3: fill

    std::cout << "  candidates for data:" << std::endl;
    for (Int_t c = 0; c < nCombThisEvent; c++) std::cout << "  {" << combVector[c][0] << ", " << combVector[c][1] << ", "  << combVector[c][2] << ", " << combVector[c][3] << "}" << std::endl;
    std::cout << "  candidates for simrec:" << std::endl;
    for (Int_t c = 0; c < nCombThisEvent; c++) std::cout << "  {" << combVector[c][0] << ", " << combVector[c][1] << ", "  << combVector[c][2] << ", " << combVector[c][3] << "}" << std::endl;
    std::cout << "  candidates for gsim:" << std::endl;
    for (Int_t c = 0; c < nMCCombThisEvent; c++) std::cout << "  {" << mc_combVector[c][0] << ", " << mc_combVector[c][1] << ", "  << mc_combVector[c][2] << ", " << mc_combVector[c][3] << "}" << std::endl;
    std::cout << "  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl;

    // extract
    for (Int_t cc = 0; cc < TMath::Max(nCombThisEvent, nMCCombThisEvent); cc++) { // loop on combinations
      for (Int_t pp = 0; pp < 4; pp++) { // loop on particles
	
	// simrec
	if (nCombThisEvent == 0 || cc >= nCombThisEvent) { // if simrec = null, set everything to -9999
	  mE[pp] = -9999.;
	  mPx[pp] = -9999.;
	  mPy[pp] = -9999.;
	  mPz[pp] = -9999.;
	  mPid[pp] = (Int_t) -9999.;
	  mEntry[pp] = (Int_t) -9999.;
	  mTargType = (Int_t) -9999.;
	  mQ2 = -9999.;
	  mNu = -9999.;
	  mXb = -9999.;
	  mYb = -9999.;
	  mW = -9999.;
	  mSector = (Int_t) -9999.;
	  mXec = -9999.;
	  mYec = -9999.;
	  mZec = -9999.;
	  mPex = -9999.;
	  mPey = -9999.;
	  mPez = -9999.;
	  pipP2 = -9999.;
	  pipM = -9999.;
	  pimP2 = -9999.;
	  pimM = -9999.;
	  pi0Px = -9999.;
	  pi0Py = -9999.;
	  pi0Pz = -9999.;
	  pi0P2 = -9999.;
	  pi0CosTheta = -9999.;
	  pi0M = -9999.;
	  pi0E = -9999.;
	  pippimP = -9999.;
	  pippimE = -9999.;
	  pippimM = -9999.;
	  wE = -9999.;
	  mZ = -9999.;
	  wPx = -9999.;
	  wPy = -9999.;
	  wPz = -9999.;
	  wP2 = -9999.;
	  mPhiPQ = -9999.;
	  mThetaPQ = -9999.;
	  mCosThetaPQ = -9999.;
	  mPt2 = -9999.;
	  wM = -9999.;
	  wD = -9999.;
	  mEvent = -9999.;
	  nPip = -9999.;
	  nPim =-9999.;
	  nGamma = -9999.;
	} else { // data/simrec not null
	  t->GetEntry(combVector[cc][pp]);
	  if (pp < 2) { // gammas
	    mE[pp] = CorrectGammaEnergy(tE); // correction
	    mPx[pp] = CorrectGammaMomentum(0); // correction
	    mPy[pp] = CorrectGammaMomentum(1); // correction
	    mPz[pp] = CorrectGammaMomentum(2); // correction
	  } else if (pp > 1) { // pions
	    mE[pp] = TMath::Sqrt(tPx*tPx + tPy*tPy + tPz*tPz + kMpi*kMpi); // assigned invariant mass
	    mPx[pp] = tPx; mPy[pp] = tPy; mPz[pp] = tPz;
	  }
	  mPid[pp] = (Int_t) tPid;
	  mEntry[pp] = (Int_t) combVector[cc][pp];
	  mTargType = (Int_t) tTargType;
	  mQ2 = tQ2;
	  mNu = tNu;
	  mXb = tXb;
	  mYb = mNu/kEbeam;
	  mW = tW;
	  mSector = (Int_t) tSector;
	  mXec = tXec;
	  mYec = tYec;
	  mZec = tZec;
	  mPex = tPex;
	  mPey = tPey;
	  mPez = tPez;
	  // define pions variables
	  pipP2 = mPx[2]*mPx[2] + mPy[2]*mPy[2] + mPz[2]*mPz[2];
	  pipM = TMath::Sqrt(mE[2]*mE[2] - pipP2); // = kMpi
	  pimP2 = mPx[3]*mPx[3] + mPy[3]*mPy[3] + mPz[3]*mPz[3];
	  pimM = TMath::Sqrt(mE[3]*mE[3] - pimP2); // = kMpi
	  pi0Px = mPx[0] + mPx[1];
	  pi0Py = mPy[0] + mPy[1];
	  pi0Pz = mPz[0] + mPz[1];
	  pi0P2 = pi0Px*pi0Px + pi0Py*pi0Py + pi0Pz*pi0Pz;
	  pi0CosTheta = (mPx[0]*mPx[1] + mPy[0]*mPy[1] + mPz[0]*mPz[1])/(mE[0]*mE[1]);
	  pi0M = TMath::Sqrt(2*mE[0]*mE[1]*(1 - pi0CosTheta));
	  pi0E = TMath::Sqrt(pi0M*pi0M + pi0P2);
	  // crossed terms
	  pippimP = mPx[2]*mPx[3] + mPy[2]*mPy[3] + mPz[2]*mPz[3];
	  pippimE = mE[2]*mE[3];
	  pippimM = TMath::Sqrt(pipM*pipM + pimM*pimM + 2*(pippimE - pippimP));
	  // define omega energy and Z
	  wE = mE[0] + mE[1] + mE[2] + mE[3];
	  mZ = wE/mNu;
	  // define omega momentum and Pt2
	  wPx = mPx[0] + mPx[1] + mPx[2] + mPx[3];
	  wPy = mPy[0] + mPy[1] + mPy[2] + mPy[3];
	  wPz = mPz[0] + mPz[1] + mPz[2] + mPz[3];
	  wP2 = wPx*wPx + wPy*wPy + wPz*wPz;
	  // define omega PhiPQ
	  mPhiPQ = PhiPQ(wPx, wPy, wPz);
	  mThetaPQ = ThetaPQ(wPx, wPy, wPz);
	  mCosThetaPQ = ((kEbeam - mPez)*wPz - mPex*wPx - mPey*wPy)/(TMath::Sqrt(wP2*(mQ2 + mNu*mNu)));
	  // define omega Pt2
	  mPt2 = wP2*(1 - mCosThetaPQ*mCosThetaPQ);
	  // define omega mass
	  wM = TMath::Sqrt(wE*wE - wP2);
	  wD = wM - pipM - pimM - pi0M;
	  mEvent = (Int_t) tEvent;
	  nPip = nPipThisEvent;
	  nPim = nPimThisEvent;
	  nGamma = nGammaThisEvent;
	} // end of simrec
	
	// gsim
	if (simFlag && cc >= nMCCombThisEvent) { // if gsim = null
	  mmc_E[pp] = -9999.;
	  mmc_Px[pp] = -9999.;
	  mmc_Py[pp] = -9999.;
	  mmc_Pz[pp] = -9999.;
	  mmc_Pid[pp] = (Int_t) -9999.;
	  mmc_Entry[pp] = (Int_t) -9999.;
	  mmc_TargType = (Int_t) -9999.;
	  mmc_Q2 = -9999.;
	  mmc_Nu = -9999.;
	  mmc_Xb = -9999.;
	  mmc_Yb = -9999.;
	  mmc_W = -9999.;
	  mmc_Sector = (Int_t) -9999.;
	  mmc_Xec = -9999.;
	  mmc_Yec = -9999.;
	  mmc_Zec = -9999.;
	  mmc_Pex = -9999.;
	  mmc_Pey = -9999.;
	  mmc_Pez = -9999.;
	  mc_pipP2 = -9999.;
	  mc_pipM = -9999.;
	  mc_pimP2 = -9999.;
	  mc_pimM = -9999.;
	  mc_pi0Px = -9999.;
	  mc_pi0Py = -9999.;
	  mc_pi0Pz = -9999.;
	  mc_pi0P2 = -9999.;
	  mc_pi0CosTheta = -9999.;
	  mc_pi0M = -9999.;
	  mc_pi0E = -9999.;
	  mc_pippimP = -9999.;
	  mc_pippimE = -9999.;
	  mc_pippimM = -9999.;
	  mc_wE = -9999.;
	  mmc_Z = -9999.;
	  mc_wPx = -9999.;
	  mc_wPy = -9999.;
	  mc_wPz = -9999.;
	  mc_wP2 = -9999.;
	  mmc_PhiPQ = -9999.;
	  mmc_ThetaPQ = -9999.;
	  mmc_CosThetaPQ = -9999.;
	  mmc_Pt2 = -9999.;
	  mc_wM = -9999.;
	  mc_wD = -9999.;
	  mmc_Event = -9999.;
	  nmc_Pip = -9999.;
	  nmc_Pim =-9999.;
	  nmc_Gamma = -9999.;
	} else if (simFlag && cc < nMCCombThisEvent) {
	  t->GetEntry(mc_combVector[cc][pp]);
	  mmc_E[pp] = tmc_E;
	  mmc_Px[pp] = tmc_Px;
	  mmc_Py[pp] = tmc_Py;
	  mmc_Pz[pp] = tmc_Pz;
	  mmc_Pid[pp] = (Int_t) tmc_Pid;
	  mmc_Entry[pp] = (Int_t) mc_combVector[cc][pp];
	  mmc_TargType = (Int_t) tmc_TargType;
	  mmc_Q2 = tmc_Q2;
	  mmc_Nu = tmc_Nu;
	  mmc_Xb = tmc_Xb;
	  mmc_Yb = mmc_Nu/kEbeam;
	  mmc_W = tmc_W;
	  mmc_Sector = (Int_t) tmc_Sector;
	  mmc_Xec = tmc_Xec;
	  mmc_Yec = tmc_Yec;
	  mmc_Zec = tmc_Zec;
	  mmc_Pex = tmc_Pex;
	  mmc_Pey = tmc_Pey;
	  mmc_Pez = tmc_Pez;
	  // define pions variables
	  mc_pipP2 = mmc_Px[2]*mmc_Px[2] + mmc_Py[2]*mmc_Py[2] + mmc_Pz[2]*mmc_Pz[2];
	  mc_pipM = TMath::Sqrt(mmc_E[2]*mmc_E[2] - mc_pipP2); // = kMpi
	  mc_pimP2 = mmc_Px[3]*mmc_Px[3] + mmc_Py[3]*mmc_Py[3] + mmc_Pz[3]*mmc_Pz[3];
	  mc_pimM = TMath::Sqrt(mmc_E[3]*mmc_E[3] - mc_pimP2); // = kMpi
	  mc_pi0Px = mmc_Px[0] + mmc_Px[1];
	  mc_pi0Py = mmc_Py[0] + mmc_Py[1];
	  mc_pi0Pz = mmc_Pz[0] + mmc_Pz[1];
	  mc_pi0P2 = mc_pi0Px*mc_pi0Px + mc_pi0Py*mc_pi0Py + mc_pi0Pz*mc_pi0Pz;
	  mc_pi0CosTheta = (mmc_Px[0]*mmc_Px[1] + mmc_Py[0]*mmc_Py[1] + mmc_Pz[0]*mmc_Pz[1])/(mmc_E[0]*mmc_E[1]);
	  mc_pi0M = TMath::Sqrt(2*mmc_E[0]*mmc_E[1]*(1 - mc_pi0CosTheta));
	  mc_pi0E = TMath::Sqrt(mc_pi0M*mc_pi0M + mc_pi0P2);
	  // crossed terms
	  mc_pippimP = mmc_Px[2]*mmc_Px[3] + mmc_Py[2]*mmc_Py[3] + mmc_Pz[2]*mmc_Pz[3];
	  mc_pippimE = mmc_E[2]*mmc_E[3];
	  mc_pippimM = TMath::Sqrt(mc_pipM*mc_pipM + mc_pimM*mc_pimM + 2*(mc_pippimE - mc_pippimP));
	  // define omega energy and Z
	  mc_wE = mmc_E[0] + mmc_E[1] + mmc_E[2] + mmc_E[3];
	  mmc_Z = mc_wE/mmc_Nu;
	  // define omega momentum and Pt2
	  mc_wPx = mmc_Px[0] + mmc_Px[1] + mmc_Px[2] + mmc_Px[3];
	  mc_wPy = mmc_Py[0] + mmc_Py[1] + mmc_Py[2] + mmc_Py[3];
	  mc_wPz = mmc_Pz[0] + mmc_Pz[1] + mmc_Pz[2] + mmc_Pz[3];
	  mc_wP2 = mc_wPx*mc_wPx + mc_wPy*mc_wPy + mc_wPz*mc_wPz;
	  // define omega PhiPQ
	  mmc_PhiPQ = PhiPQ(mc_wPx, mc_wPy, mc_wPz);
	  mmc_ThetaPQ = ThetaPQ(mc_wPx, mc_wPy, mc_wPz);
	  mmc_CosThetaPQ = ((kEbeam - mmc_Pez)*mc_wPz - mmc_Pex*mc_wPx - mmc_Pey*mc_wPy)/(TMath::Sqrt(mc_wP2*(mmc_Q2 + mmc_Nu*mmc_Nu)));
	  // define omega Pt2
	  mmc_Pt2 = mc_wP2*(1 - mmc_CosThetaPQ*mmc_CosThetaPQ);
	  // define omega mass
	  mc_wM = TMath::Sqrt(mc_wE*mc_wE - mc_wP2);
	  mc_wD = mc_wM - mc_pipM - mc_pimM - mc_pi0M;
	  mmc_Event = (Int_t) tmc_Event;
	  nmc_Pip = nMCPipThisEvent;
	  nmc_Pim = nMCPimThisEvent;
	  nmc_Gamma = nMCGammaThisEvent;
	} // end of gsim

      } // end of loop in particles

      // and fill
      tMix->Fill();
      
    } // end of loop in combinations

    // reset count variables
    nPipThisEvent = 0;
    nPimThisEvent = 0;
    nGammaThisEvent = 0;
    nCombThisEvent = 0;
    
    nMCPipThisEvent = 0;
    nMCPimThisEvent = 0;
    nMCGammaThisEvent = 0;
    nMCCombThisEvent = 0;

    // update event counter
    nEvents++;
    
    // reset vectors!
    combVector.clear();
    mc_combVector.clear();

    // for data
    Int_t ii;
    if (!simFlag) {
      ii = t->GetEntryNumber(l->GetN() - 1); // look last entry from event list
    }
    
    // set tree back to original tree
    t->SetEntryList(0);

    // for data
    if (!simFlag) {
      if (i == (Int_t) t->GetMaximum(eventBranchName)) break; // break infinite loop
      t->GetEntry(ii+1); // stand in first entry of the next event
      i = (Int_t) tEvent - 1; // jump to next event number
    }
    
    // clean some memory
    gDirectory->Delete(listName + ";1");
    rootFile->Delete(listName);
    
    std::cout << std::endl;
  } // end of loop in events

  /*** Writing tree ***/

  if (simFlag) {
    std::cout << "From a total of " << nEvents << " generated events." << std::endl;
    std::cout << "There are at least " << nAtLeastMCOmega << " generated events with at least one omega particle," << std::endl;
    std::cout << "being in total this amount of generated omega candidates: " << nMCOmega << "."  << std::endl;
    std::cout << "Also, there are at least " << nAtLeastOmega << " reconstructed events with at least one omega particle," << std::endl;
    std::cout << "being in total this amount of reconstructed omega candidates: " << nOmega << "."  << std::endl;
    std::cout << std::endl;
  } else {
    std::cout << "From a total of " << nEvents << " events." << std::endl;
    std::cout << "There are at least " << nAtLeastOmega << " events with at least one omega particle," << std::endl;
    std::cout << "being in total this amount of omega candidates: " << nOmega << "."  << std::endl;
    std::cout << std::endl;
  }
  
  rootFile->Write();
  rootFile->Close();

  std::cout << "File " << outFile << " has been created!" << std::endl;
  
  return 0;
}

/*** Input-related functions ***/

inline int parseCommandLine(int argc, char* argv[]) {
  Int_t c;
  if (argc == 1) {
    std::cerr << "Empty command line. Execute ./FilterNCombine -h to print help." << std::endl;
    exit(0);
  }
  while ((c = getopt(argc, argv, "ht:dS:n:r:")) != -1)
    switch (c) {
    case 'h': printUsage(); exit(0); break;
    case 't': targetOption = optarg; break;
    case 'd': simFlag = 0; break;
    case 'S': simFlag = 1; setOption = optarg; break;
    case 'n': NjlabDir = optarg; break;
    case 'r': rnOption = optarg; break;
    default:
      std::cerr << "Unrecognized argument. Execute ./FilterNCombine -h to print help." << std::endl;
      exit(0);
      break;
    }
}

void assignOptions() {
  // for data type
  if (!simFlag) {
    // set
    setOption = "data";
    // ntuple name
    treeName = "ntuple_data";
    eventBranchName = "evnt";
    // input
    inputFile = proDir + "/out/prunedData/" + targetOption + "/pruned_data_" + rnOption + ".root"; // new
    // out
    outDir = dataDir + "/" + targetOption;
    // new!
    analyserOption = targetOption;
  } else if (simFlag) {
    // ntuple name
    treeName = "ntuple_sim";
    eventBranchName = "mc_evnt";
    // input
    inputFile = proDir + "/out/prunedSim/" + setOption + "/" + targetOption + "/pruned_sim" + targetOption + "_" + rnOption + ".root"; // new
    if (setOption == "jlab") inputFile = proDir + "/out/prunedSim/" + setOption + "/" + targetOption + "/" + NjlabDir + "/pruned_sim" + targetOption + "_" + rnOption + ".root"; // new (for jlab)
    // out
    outDir = simDir + "/" + setOption + "/" + targetOption;
    if (setOption == "jlab") outDir += "/" + NjlabDir;
    // new!
    analyserOption = "Sim";
  }
  // regardless of the data type
  outFile = outDir + "/comb_" + setOption + targetOption + "_" + rnOption + ".root";
}

void printUsage() {
  std::cout << "FilterNCombine program." << std::endl;
  std::cout << "The input file should have this name scheme: " << std::endl;
  std::cout << "    for data     = out/prunedData/[target]/pruned_data_[rn].root" << std::endl;
  std::cout << "    for old/usm  = out/prunedSim/[set]/[target]/pruned_sim[target]_[rn].root" << std::endl;
  std::cout << "    for jlab     = out/prunedSim/jlab/[target]/[ndir]/pruned_sim[target]_[rn].root" << std::endl;
  std::cout << "Usage is:" << std::endl;
  std::cout << std::endl;
  std::cout << "./FilterNCombine -h" << std::endl;
  std::cout << "    prints this message and exits program" << std::endl;
  std::cout << std::endl;
  std::cout << "./FilterNCombine -t[D, C, Fe, Pb]" << std::endl;
  std::cout << "    filters the respective target" << std::endl;
  std::cout << "    IMPORTANT: D option is only for simulations" << std::endl;
  std::cout << std::endl;
  std::cout << "./FilterNCombine -d" << std::endl;
  std::cout << "    filters data" << std::endl;
  std::cout << std::endl;
  std::cout << "./FilterNCombine -S[old, usm, jlab]" << std::endl;
  std::cout << "    filters sim for chosen set" << std::endl;
  std::cout << std::endl;
  std::cout << "./GetSimpleTuple -n[00,01,02,...]" << std::endl;
  std::cout << "    selects N dir (exclusive and mandatory for -Sjlab option)" << std::endl;
  std::cout << "    (please, maintain numbering scheme!)" << std::endl;
  std::cout << std::endl;
  std::cout << "./GetSimpleTuple -r[0001,...,9999]" << std::endl;
  std::cout << "    selects run number (mandatory for all)" << std::endl;
  std::cout << "    (please, maintain numbering scheme!)" << std::endl;
  std::cout << std::endl;
}

void printOptions() {
  std::cout << "Executing FilterNCombine program. The chosen parameters are: " << std::endl;
  std::cout << "  targetOption   = " << targetOption << std::endl;
  std::cout << "  simFlag        = " << simFlag << std::endl;
  std::cout << "  setOption      = " << setOption << std::endl;
  std::cout << "  NjlabDir       = " << NjlabDir << std::endl;
  std::cout << "  rnOption       = " << rnOption << std::endl;
  std::cout << "  inputFile      = " << inputFile << std::endl;
  std::cout << "  setOption      = " << setOption << std::endl;
  std::cout << std::endl;
}

/*** Mathematical functions ***/

Float_t CorrectGammaEnergy(Float_t fE) {
  // returns the respective corrected energy
  // thanks to (Taisiya Mineeva - CLAS Analysis Note)

  Float_t corrFactor;
  if (analyserOption == "C" || analyserOption == "Pb") {
    fE = fE/0.272; // primitive sampling fraction
    corrFactor = 1.129 - 0.05793/fE - 1.0773e-12/(fE*fE);
  } else if (analyserOption == "Fe") {
    fE = fE/0.272; // primitive sampling fraction
    corrFactor = 1.116 - 0.09213/fE + 0.01007/(fE*fE);
  } else if (analyserOption == "Sim") {
    fE = fE/0.2806; // primitive sampling fraction for sim
    corrFactor = 1.000 + 0.005/fE - 0.0052/(fE*fE);
  }

  return fE/corrFactor;
}

Float_t CorrectGammaMomentum(Int_t i) {
  // returns the respective corrected momentum component
  // extracted from eta_id.cxx (Orlando Soto)

  Float_t fP[3];
  Float_t fE = tE/0.272; // primitive sampling fraction
  
  Float_t Rt = TMath::Sqrt(tECX*tECX + tECY*tECY);
  Float_t R = TMath::Sqrt(tECX*tECX + tECY*tECY + (tECZ - tZec)*(tECZ - tZec));
  Float_t theta_gam = TMath::ASin(Rt/R);
  Float_t phi_gam = TMath::ATan2(tECY, tECX);

  fP[0] = fE*TMath::Sin(theta_gam)*TMath::Cos(phi_gam);
  fP[1] = fE*TMath::Sin(theta_gam)*TMath::Sin(phi_gam);
  fP[2] = fE*TMath::Cos(theta_gam);

  // (0 -> x, 1 -> y, 2 -> z)
  return fP[i];
}

Float_t PhiPQ(Float_t fPx, Float_t fPy, Float_t fPz) {
  Float_t fPhiPQ;

  // two 3-momentum vectors are defined
  // one for the hadron
  // one for the virtual photon
  TVector3 hadr(fPx, fPy, fPz);
  TVector3 virt(-tPex, -tPey, kEbeam - tPez);
  
  // the function Phi() and Theta() converts the vector into spherical coordinates
  // and the function RotateZ(amount) rotates the vector around z-axis a certain amount
  // now, this paragraph turns y-component of the virtual photon vector into 0 and rotates the hadron vector the same amount
  // to have the hadron vector in function of the virtual photon direction
  // if its not clear, try to draw these two vectors in the xy plane
  Float_t Phi_z = TMath::Pi() - virt.Phi();
  virt.RotateZ(Phi_z);
  hadr.RotateZ(Phi_z);

  // the function Angle() returns the angle between two vectors
  // now, take into account that the virtual photon vector is in the y=0 plane
  // again, this paragraph turns the x-component of the virtual photon vector into 0 and rotates the hadron vector the same amount
  // to have the hadron vector in function of the virtual photon direction
  // if its not clear, try to draw these two vectors in the xz plane
  TVector3 unit(0., 0., 1.);
  Float_t Phi_y = virt.Angle(unit);
  virt.RotateY(Phi_y);
  hadr.RotateY(Phi_y);
  
  // finally, it obtains the phi component (in spherical coordinates) of the hadron vector (now in the virtual photon frame of reference)
  // and converts it from radians to degrees
  fPhiPQ = hadr.Phi() * 180./(TMath::Pi());
  
  return fPhiPQ;
}

Float_t ThetaPQ(Float_t fPx, Float_t fPy, Float_t fPz) {
  Float_t fThetaPQ;
  
  TVector3 hadr(fPx, fPy, fPz);
  TVector3 virt(-tPex, -tPey, kEbeam - tPez);

  // apparently, the angle between these two vectors is the theta angle
  // and this makes perfect sense if one draws it
  fThetaPQ = virt.Angle(hadr) * 180./(TMath::Pi());

  return fThetaPQ;
}
