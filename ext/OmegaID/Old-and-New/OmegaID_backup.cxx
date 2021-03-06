/*************************************************/
/*  eta_id.cxx                                   */
/*                                               */
/*  Created by Orlando Soto, CCTVAL              */
/*  Modified for omega by Andrés Bórquez, CCTVAL */
/*                                               */
/*************************************************/

#include <vector>
#include <sys/stat.h>
#include <cstdarg>
#include <algorithm>
#include <map>
#include <string.h>

#include "TSpectrum.h"
#include "Riostream.h"
#include "TApplication.h"
#include "TROOT.h"
#include "TFile.h"
#include "TNtuple.h"
#include "TClasTool.h"
#include "TIdentificator.h"
#include "TMath.h"
#include "TBenchmark.h"
#include "TLorentzVector.h"
#include "TCanvas.h"
#include "TH2F.h"
#include "TLatex.h"
#include "TDatabasePDG.h"
#include "TParticlePDG.h"
#include "TMatrixD.h"
#include "TClonesArray.h"

#include "do_pid.h"

// Global Variables
bool DEBUG = false;

TH1F *hW;
TH1F *hW2;
TH1F *hWmb;
TH1F *hW2mb;
TH1F *hT;
TH1F *hcfm;
TH2F *hEpi0_th;
Float_t kMPi0 = 1.33196e-1;// Got from sim
Float_t kSPi0 = 1.94034e-2;// Got from sim
//Float_t kMPi0=5.39609e-01;
//Float_t kSPi0=5.98542e-02;
bool GSIM = false;
int data_type=0;
Float_t kPt2, kEvent; 
TNtuple *tuple;
//TNtuple *tuple_sim;
TNtuple *tuplemb;
TNtuple *tuplePi0_gamma, *tupleGamma;
Float_t kEbeam=5.014,E,Ee,Ee_prev,Ep,P,Px,Py,Pz,evnt,evnt_prev,Zec,Zec_prev,Yec,Yec_prev,Xec,Xec_prev,TEc,Q2,Q2_prev,W,W_prev,Nu,Nu_prev,Pex,Pex_prev,Pey,Pey_prev,Pez,Pez_prev,TargType,TargType_prev,TargTypeO=0,TargTypeO_prev=0,pid,vx,vy,vz,ECX,ECY,ECZ;
long Ne = -1;
char st[3]= "C"; // solid target: C | Fe | Pb // important line 1
char tt[3] = "C"; // cut on solid target or Deuterium : (st) or D. // important line 2

Float_t kMprt = 0.938272, kMntr = 0.939565;
TClonesArray *P4Arr;

class Particle: public TLorentzVector
{
public:
  Float_t vx,vy,vz,pid,time;
  //TParticlePDG *info;
  Particle() : TLorentzVector(), vx(0),vy(0),vz(0),pid(0),time(0){}
  Particle(Float_t px,Float_t py, Float_t pz, Float_t e, Float_t x, Float_t y, Float_t z, Float_t pid=0, Float_t t=0): TLorentzVector(px,py,pz,e),vx(x),vy(y),vz(z),pid(pid),time(t){}
  Particle(TLorentzVector lv, Float_t x=0, Float_t y=0, Float_t z=0, Float_t pid=0, Float_t t=0): TLorentzVector(lv),vx(x),vy(y),vz(z),pid(pid),time(time){}
  Particle(Particle &p):vx(p.vx),vy(p.vy),vz(p.vz),pid(p.pid),time(p.time) {SetVect(p.Vect()); SetT(p.T());}
  inline Double_t P2() const {return P()*P();}
  inline Particle operator + (const Particle & q) const //const: the object that owns the method will not be modified by this method
  {
    Particle p;
    p.SetVect(Vect()+q.Vect());
    p.SetT(E()+q.T());
    return p;
  }

  inline Bool_t checkFiducial() {
    // Converts x,y,z EC hit in CLAS coordinate system
    // into u,v,w distances of the EC hit, and then test the fiducial cut. (Phonetic names)
    Bool_t test = kFALSE;

    Float_t ex = 0., wy = 0., zd = 0., yu = 0., ve = 0.,  wu = 0., xi = 0., yi = 0., zi = 0., ec_phy = 0., phy = 0., rot[3][3];

    // Parameters
    Float_t ec_the = 0.4363323;
    Float_t ylow = -182.974;
    Float_t yhi = 189.956;
    Float_t tgrho = 1.95325; 
    Float_t sinrho = 0.8901256; 
    Float_t cosrho = 0.455715;
    
    // Variables
    ex = vx;
    wy = vy;
    zd = vz;
    
    phy = TMath::ATan2(wy,ex)*57.29578;
    if (phy < 0.) phy = phy + 360;
    phy = phy+30.;
    if (phy > 360.) phy = phy - 360.;
    
    ec_phy = ((Int_t) (phy/60.))*1.0471975;
    
    rot[0][0] = TMath::Cos(ec_the)*TMath::Cos(ec_phy);
    rot[0][1] = -TMath::Sin(ec_phy);
    rot[0][2] = TMath::Sin(ec_the)*TMath::Cos(ec_phy);
    rot[1][0] = TMath::Cos(ec_the)*TMath::Sin(ec_phy);
    rot[1][1] = TMath::Cos(ec_phy);
    rot[1][2] = TMath::Sin(ec_the)*TMath::Sin(ec_phy);
    rot[2][0] = -TMath::Sin(ec_the);
    rot[2][1] = 0.;
    rot[2][2] = TMath::Cos(ec_the);
    
    yi = ex*rot[0][0]+wy*rot[1][0]+zd*rot[2][0];
    xi = ex*rot[0][1]+wy*rot[1][1]+zd*rot[2][1];
    zi = ex*rot[0][2]+wy*rot[1][2]+zd*rot[2][2];
    zi = zi-510.32 ;
    
    yu = (yi-ylow)/sinrho;
    ve = (yhi-ylow)/tgrho - xi + (yhi-yi)/tgrho;
    wu = ((yhi-ylow)/tgrho + xi + (yhi-yi)/tgrho)/2./cosrho;

    //U in ]40, 410[ , V in [0,370[ and W in [0,410[.)
    if ((40<yu&&yu<410) && (ve<370)  && (wu<410))
      test=kTRUE;
    //    TVector3 * result3= new TVector3(yu,ve,wu);
  
    return test;
  }

  inline Particle operator += (const Particle & q) {
    SetVect(Vect()+q.Vect());
    SetT(E()+q.T());
    return *this;
  }
  const char *getName() {
    return ((TDatabasePDG::Instance())->GetParticle(pid))->GetName(); 
  }
};

class Combo
{
public:
  std::vector<Particle*> kParticles;
  static constexpr Float_t kQ2Tol=0.3,kNuTol=0.3;
  int Npart;
  Float_t lastEvent,kQ2,kNu;
  TLorentzVector *q4; // virtual photon 4th vector.
  Combo(): Npart(0),lastEvent(0),kQ2(0),kNu(0),q4(0){}
  
  Combo(Combo &c)
  {
    Npart=0;
    for (int k=0;k<c.Npart;k++)
    {
      addParticle(c.kParticles[k]);
    }
    lastEvent=c.lastEvent;
    kQ2=c.kQ2;
    kNu=c.kNu;
  }
  inline  Double_t Px(){return getSum().Px();}
  inline  Double_t Py(){return getSum().Py();}
  inline  Double_t Pz(){return getSum().Pz();}
  inline  Double_t P2(){return getSum().P2();}
  inline  Double_t P(){return getSum().P();}
  inline  Double_t E(){return getSum().E();}
  inline  Double_t M(){return getSum().M();}
  inline  Double_t M2(){return getSum().M2();}



  //  Combo(Particle *&p): {}
  ~Combo()
  {
    //    std::cout<<"Combo destructor called: "<<kParticles.size()<<"\n";
    //for (int k=0;k< kParticles.size();k++)
    // delete kParticles[k];
    delete q4;
    clear();
  }
  void clear(){ kParticles.clear();Npart=0;}

  void boost() //Go to CM frame.
  {
    //Particle *p=&getSum();
    for (int k =0;k<Npart;k++)
      kParticles[k]->Boost(-getSum().BoostVector());
  }

  int addParticle(Particle *p,Float_t ev=0,Bool_t rotfirst=kFALSE, Bool_t fid=kFALSE)
  {
    lastEvent=ev;
    if (Npart==0)
    {
      kQ2= Q2; // using global variable, must be changed!.
      kNu= Nu; // using global variable, must be changed!.
      q4 = new TLorentzVector(-Pex,-Pey,kEbeam-Pez,kEbeam-Ee);
    }
    else if (rotfirst)
    {
      TLorentzVector *q4n = new TLorentzVector(-Pex,-Pey,kEbeam-Pez,kEbeam-Ee);
      // Double_t Dth = q4->Theta() - q4n->Theta(); //no theta rotation, beam direction.
      Double_t Dphi = q4->Phi() - q4n->Phi();
      //p->SetTheta(p->Theta()+Dth);
      p->SetPhi(p->Phi()+Dphi);
    }
    if (fid && !p->checkFiducial()) return Npart;
    // std::cout<<__FILE__<<"::"<<__LINE__<<std::endl;
    kParticles.push_back(p);
    Npart++;

    return Npart;
  }

  Particle getSum()
  {
    //    Particle *p= new Particle();
    Particle p;
      
    for (int k=0;k<Npart;k++)
    {
      p+=*kParticles[k];
    }
    return p;
  }

  Int_t findPid(Int_t pid)
  {
    Int_t ret =0;
    for (int k=0;k<Npart;k++)
      if (pid==kParticles[k]->pid)
	ret++;
    return ret;
  }

  Bool_t isCompatible()
  {
    /////// using global variables, must be changed!
    if ( !( (-kQ2Tol< (kQ2 - Q2) && (kQ2 - Q2) < kQ2Tol) && (-kNuTol< (kNu - Nu) && (kNu - Nu) < kNuTol) ) )
      return kFALSE;

    return kTRUE;
  }


  
  inline Particle* operator [] (const int & i) const
  {
    if (i>=Npart||i<0)
    {
      std::cout << "Index out of bounds" <<std::endl; 
      exit(1);
    }
    return kParticles[i];
  } 


  inline Combo operator + (const Combo & c) const //const: the object that owns the method will not be modified by this method
  {
    
    Combo r;
    for (int k=0;k<c.Npart;k++)
    {
      r.addParticle(c[k]);
    }
    for (int k=0;k<this->Npart;k++)
    {
      r.addParticle((*this)[k]);
    }

    return r;
  }

  inline Combo operator += (const Combo &q) 
  {
    for (int k =0;k<q.Npart;k++)
    {
      addParticle( q[k] );
    }
    return *this;
  }
  
  void print() {
    for (int k=0;k<Npart;k++)
      std::cout<<kParticles[k]->getName()<<" ";
    std::cout << std::endl;
  }
};


class Reaction
{
public:
  char name[50];
  char filename[50];
  Combo *kPrimary; //primary

  TParticlePDG *kPdgInfo;
  std::vector<Particle*> *kSecondary; //all secondary

  //std::vector<Particle*> *kCombo; //partial combinations.
  std::vector <Combo *> *kCombo; //partial combinations.
  std::vector <Combo *> kBkgnd; //background combinations.

  static const Int_t kBUFFLIMIT=100;

  std::vector<TH1F*> hSPid;
  TFile *kOutFile;
  Float_t *kData;
  Float_t *keData;

  TNtuple  *kElecData;

  TTree *kOutData, *kOutBkgnd;

  //  TTree *kP4Tree;
  bool fEMatch;
  
  int kPPid;
  int kNSecondary;
  std::vector<int>::iterator kSIt;
  std::vector<int> kSPid; //Amount of secondary particles for a given pid.
  std::map<int,int> kNSPid; //Amount of secondary pid particles required.
  Reaction(){strcpy(name,"eta -> pi+ pi- a"),strcpy(filename,"test_eta_pippima.root");init();}
  Reaction(const char *n,const char *fn,bool fEMatch=false): fEMatch(fEMatch) {strcpy(name,n); strcpy(filename,fn); init();}
  int store() {
    for (int k=0;k<kSPid.size();k++) {
      hSPid[k]->Write("",TObject::kOverwrite);
    }
    kOutBkgnd->Write("",TObject::kOverwrite);
    kElecData->Write("",TObject::kOverwrite);
    //kP4Tree->Write("",TObject::kOverwrite);
    return kOutData->Write("",TObject::kOverwrite);
  }
  ~Reaction() {
    clear();
    delete[] kSecondary;
    delete[] kCombo;
    delete kOutData;
    delete kOutBkgnd;
    delete[] kData;
    delete[] keData;
    delete P4Arr;
    kOutFile->Close();
    delete kOutFile;
    delete kPdgInfo;
    hSPid.clear();
  }
  void clear() {
    for (int k = 0; k < kSPid.size(); k++) {
      kSecondary[k].clear();
      kCombo[k].clear();
    }
  }

  //// To store different information modify init() and fill()
  int init() {
    kNSecondary=0;
    kOutFile = new TFile(filename,"recreate");
    //kOutData=new TNtuple("outdata",Form("%s",name),"M:Phx:Phy:Phz:Nu:Q2:Z:Cospq:Pt2:Event:M2_01:M2_02:M_c:Phx_c:Phy_c:Phz_c:Z_c:Cospq_c:Pt2_c:Chi2:qx1:qy1:qz1:qx2:qy2:qz2");
    //kOutData = new TNtuple("outdata",Form("%s",name),
    TString varlist="M:Phx:Phy:Phz:Nu:Q2:Z:Cospq:Pt2:Event:M2_01:M2_02:vzec:z1:z2:z3:W:vxec:vyec:qx1:qy1:qz1:qx2:qy2:qz2:E1:E2:E1c:E2c:x1:y1:x2:y2:TargType:TargTypeO:PhiPQ";
    kElecData = new TNtuple("ElecData", Form("%s", name),"Nu:Q2:Event:vzec:Ee:Pex:Pey:Pez:W");

    kData = new Float_t[varlist.CountChar(':') + 1];
    keData = new Float_t[kElecData->GetNvar()];
    P4Arr = new TClonesArray("TLorentzVector");
    kOutData = new TTree("outdata", Form("%s", name));
    kOutData->Branch("P4", &P4Arr, 6);
    kOutData->Branch("primary", kData, varlist);

    kOutBkgnd = kOutData->CloneTree(0);
    kOutBkgnd->SetName("outbkgnd");
    
    return 0;
  }

  int fill() {
    fill(kPrimary,kOutData);
  }

  int fill_elec() {
    keData[0] = Nu_prev;
    keData[1] = Q2_prev;
    keData[2] = evnt_prev;
    keData[3] = Zec_prev;
    keData[4] = Ee_prev;
    keData[5] = Pex_prev;
    keData[6] = Pey_prev;
    keData[7] = Pez_prev;
    keData[8] = W_prev;
    kElecData->Fill(keData);
  }

  int fill(Combo *comb,TTree *ttree) {
    Double_t Px = comb->Px();
    Double_t Py = comb->Py();
    Double_t Pz = comb->Pz();
    Double_t E = comb->E();
    Double_t P2 = comb->P2();
    Double_t M2 = comb->M2();
    Double_t M = (M2>=0)?TMath::Sqrt(M2):-1.0;
    Float_t cospq = ((kEbeam-Pez_prev)*Pz - Pex_prev*Px - Pey_prev*Py)/( sqrt((Q2_prev + Nu_prev*Nu_prev)*P2) );
    Float_t Pt2 = P2*(1-cospq*cospq);

    Double_t phi_pq;
    TVector3 Vhad(Px,Py,Pz);
    TVector3 Vvirt(-Pex_prev,-Pey_prev,kEbeam-Pez_prev);
    Double_t phi_z = TMath::Pi()-Vvirt.Phi();
    Vvirt.RotateZ(phi_z);
    Vhad.RotateZ(phi_z);
    TVector3 Vhelp(0.,0.,1.);
    Double_t phi_y = Vvirt.Angle(Vhelp);
    Vvirt.RotateY(phi_y);
    Vhad.RotateY(phi_y);
    phi_pq = Vhad.Phi() * 180./(TMath::Pi());
    
    kData[0] = M;
    kData[1] = Px;
    kData[2] = Py;
    kData[3] = Pz;
    kData[4] = Nu_prev;
    kData[5] = Q2_prev;
    kData[6] = (Float_t)E/Nu_prev;
    kData[7] = cospq;
    kData[8] = Pt2;
    kData[9] = evnt_prev;

    kData[10]=((comb->Npart==3)? ( *(*comb)[0] + *(*comb)[1]).M2() : 0);
    kData[11]=((comb->Npart==3)? ( *(*comb)[0] + *(*comb)[2]).M2() : 0);
    kData[12] = Zec_prev; // z electron corrected.

    kData[13] = (*comb)[0]->vz;
    kData[14] = ((comb->Npart>1)?(*comb)[1]->vz:0);
    kData[15] = ((comb->Npart>2)?(*comb)[2]->vz:0);
    kData[16] = W_prev;
    kData[17] = Xec_prev;
    kData[18] = Yec_prev;

    Double_t qx1,qy1,qz1,qx2,qy2,qz2;
    qx1 = (*comb)[0]->Px();
    qy1 = (*comb)[0]->Py();
    qz1 = (*comb)[0]->Pz();
    qx2 = ((comb->Npart>1)?(*comb)[1]->Px():0);
    qy2 = ((comb->Npart>1)?(*comb)[1]->Py():0);
    qz2 = ((comb->Npart>1)?(*comb)[1]->Pz():0);
    Float_t E1 = (*comb)[0]->E(); 
    Float_t E2 = ((comb->Npart>1)?(*comb)[1]->E():0); 
    kData[19] = qx1;
    kData[20] = qy1;
    kData[21] = qz1;
    kData[22] = qx2;
    kData[23] = qy2;
    kData[24] = qz2;
    kData[25] = E1;
    kData[26] = E2;
    kData[27] = E1;
    kData[28] = E2;
    if (0.35 < E1 && E1 < 1.2) kData[27] /= hcfm->GetBinContent(hcfm->FindBin(E1));
    if (0.35 < E2 && E2 < 1.2) kData[28] /= hcfm->GetBinContent(hcfm->FindBin(E2));
    kData[29] = (*comb)[0]->vx;
    kData[30] = (*comb)[0]->vy;
    kData[31] = ((comb->Npart>1)?(*comb)[1]->vx:0);
    kData[32] = ((comb->Npart>1)?(*comb)[1]->vy:0);
    kData[33] = TargType_prev;
    kData[34] = TargTypeO_prev;
    kData[35] = phi_pq;

    for (int k = 0; k < kNSecondary; k++) {
      
      Double_t px= (*comb)[k]->Px();
      Double_t py= (*comb)[k]->Py();
      Double_t pz= (*comb)[k]->Pz();
      Double_t e= (*comb)[k]->E();
      
      new ((*P4Arr)[k]) TLorentzVector(px,py,pz,e);
    }
    return ttree->Fill();
  }
  
  inline Double_t kinFit(Double_t *W, Double_t *Wa, TMatrixD &V) {
    bool db = false;
    if (db) std::cout<<"V Matrix on  "<<__LINE__<<std::endl;
    if (db) V.Print();

    //    TMatrixD V(4,4) = *Vm;//covariance matrix
    TMatrixD V_new(4,4);//covariance matrix updated
    TMatrixD VD(1,1);//auxiliary matrix
    TMatrixD D(1,4);//derivatives of restriction on xa
    TMatrixD d(1,1);//restriction on xa
    TMatrixD x(4,1);//corrected point
    TMatrixD xa(4,1);//linearization point
    TMatrixD x0(4,1);// initial point
    TMatrixD lam(1,1);//constrain multiplier
    Double_t Chi2 =0;//chi2 value
       
    D(0,0) = -2*Wa[0]; D(0,1) = -2*Wa[1]; D(0,2) = -2*Wa[2]; D(0,3) = 2*Wa[3];
    if (db) std::cout<<"D Matrix on  "<<__LINE__<<std::endl;
    if (db) D.Print();
    //x(0,0) = W[0]; x(1,0) = W[1]; x(2,0) = W[2]; x(3,0) = W[3];
    xa(0,0) = Wa[0]; xa(1,0) = Wa[1]; xa(2,0) = Wa[2]; xa(3,0) = Wa[3];
    x0(0,0) = W[0]; x0(1,0) = W[1]; x0(2,0) = W[2]; x0(3,0) = W[3];
    
    //lam(0,0) = 0;
    d(0,0)=Wa[3]*Wa[3] - Wa[0]*Wa[0] - Wa[1]*Wa[1] - Wa[2]*Wa[2] - kPdgInfo->Mass()*kPdgInfo->Mass();
    
    TMatrixD DT =(D).Transpose(D);
    if (db) std::cout <<"DT Matrix on  " << __LINE__ << std::endl;
    if (db) DT.Print();
    D.Transpose(D);
    
    TMatrixD VDL = (D*V);
    if (db)std::cout<<"VDL Matrix on  "<<__LINE__<<std::endl;
    if (db)VDL.Print();
    VD=VDL*DT;
    if (db)std::cout<<"VD Matrix on  "<<__LINE__<<std::endl;
    if (db)VD.Print();
    VD.Invert();
    if (db)std::cout<<"VD Matrix on  "<<__LINE__<<std::endl;
    if (db)VD.Print();
    lam = VD*(D*(x0-xa) + d);
    
    TMatrixD lamT = lam.Transpose(lam);
    lam.Transpose(lam);
    
    x = x0 + (V*DT)*lam;
    W[0]=x(0,0);W[1]=x(1,0);W[2]=x(2,0);W[3]=x(3,0);
    V_new = V - (V*DT)*VD*(D*V);
    V=V_new;
    TMatrixD aux=(lamT*(D*(x0 - xa) + d));
    //aux.Print();
    Chi2=aux(0,0);
    
    return Chi2;
}
  int pushSecondary(int pid)
  {
    kNSecondary++;
    if (!isPid(pid))
    {
      kSPid.push_back(pid);
      kNSPid[pid]=1;
      int k = kSPid.size();
      hSPid.push_back(new TH1F(Form("hNPart%d",k), TDatabasePDG::Instance()->GetParticle(pid)->GetName(),20,0,20) );
    }
    else
      kNSPid[pid]++;


    return pid;
  }

  int addSecondary(int pid)
  {
    pid = TDatabasePDG::Instance()->GetParticle(pid)->PdgCode();
    pushSecondary(pid);
    return pid;
  }

  int addSecondary(const char *name) {
    int pid  = TDatabasePDG::Instance()->GetParticle(name)->PdgCode();
    pushSecondary(pid);
    return pid;
  }

  int addPrimary(int pid){
    kPdgInfo = TDatabasePDG::Instance()->GetParticle(pid);
    kPPid=kPdgInfo->PdgCode();
    return kPPid;
  }
  int addPrimary(const char *name){
    kPdgInfo = TDatabasePDG::Instance()->GetParticle(name);
    kPPid=kPdgInfo->PdgCode();
    return kPPid;
  }
  bool isPid(int pid) {
    kSIt = std::find (kSPid.begin(), kSPid.end(), pid);
    if (kSIt != kSPid.end())
      return true;
    else
      return false;
  }
  bool checkMinPart() {
    bool minPart=true;
    for (int k=0;k<kSPid.size();k++) {
      if (fEMatch)
	minPart=minPart && (kSecondary[k].size() == kNSPid[kSPid[k]]);
      else
	minPart=minPart && (kSecondary[k].size() >= kNSPid[kSPid[k]]);
      hSPid[k]->Fill(kSecondary[k].size());
    }
    return minPart;
  }
  bool checkMinPart(Combo *c) {
    bool minPart=true;
    for (int k=0;k<kSPid.size();k++) {
	minPart=minPart && (c->findPid(kSPid[k]) == kNSPid[kSPid[k]]);
    }
    return minPart;
  }
  void setElectVar() {
    Nu_prev = Nu;
    Q2_prev = Q2;
    Zec_prev = Zec;
    Xec_prev = Xec;
    Yec_prev = Yec;
    W_prev = W;
    Pex_prev = Pex;
    Pey_prev = Pey;
    Pez_prev = Pez;
    Ee_prev = Ee;
    TargType_prev = TargType;
    TargTypeO_prev = TargTypeO;
  }

  int takeN(int N, int kspid, int pos = 0, Combo *c = 0,int count = 0) {
    Combo *c_new;
    if (DEBUG) std::cout << "### take N:  " << N << "##############" << std::endl; 
    if (N<1) return -1;
    if (N!=1) {
      for (int k =pos;k<kSecondary[kspid].size()-N+1;k++) {
	if (c==0) c_new = new Combo();
	else c_new = new Combo(*c);
	c_new->addParticle(kSecondary[kspid][pos]);
	count = takeN(N-1,kspid,k+1,c_new,count);
      }
    } else {
      for (int k = pos; k < kSecondary[kspid].size(); k++) {
	if (c==0) c_new = new Combo();
	else c_new = new Combo(*c);
	c_new->addParticle(kSecondary[kspid][k]);
	if (DEBUG) std::cout << "############ Npart from takeN: " << c_new->Npart << "#### pid: " << kSPid[kspid] << "#############" << std::endl;
	//kCombo[kspid].push_back(new Combo(*c));
	kCombo[kspid].push_back(c_new);
	//kParticles.push_back(new Particle(*kSecondary[kspid][k]));
	//std::cout<<__LINE__<<" "<< kCombo[kspid].back()->M()<<std::endl;
	count++;
      }
    }
    return count;
  }
  
  int findSecondary() { 
    int count=0;
    for (int k = 0; k < kSPid.size(); k++) {
      if (pid == kSPid[k]) {
	kSecondary[k].push_back(new Particle(Px,Py,Pz,Ep,vx,vy,vz,pid));
	count++;
      }
    }
    return count;
  }
  
  int correct_momentum() {
    Float_t Rt = TMath::Sqrt( ECX*ECX + ECY*ECY );
    Float_t R = TMath::Sqrt( ECX*ECX + ECY*ECY + (ECZ-Zec)*(ECZ -Zec) );
    Float_t theta_gam = TMath::ASin(Rt/R);
    Float_t phi_gam = TMath::ATan2(ECY,ECX);
    Px = Ep*TMath::Sin(theta_gam)*TMath::Cos(phi_gam);
    Py = Ep*TMath::Sin(theta_gam)*TMath::Sin(phi_gam);
    Pz = Ep*TMath::Cos(theta_gam);
    return 0;
  }

  int getCombinations(TChain *t) {

    kSecondary = new std::vector<Particle*> [kSPid.size()];
    kCombo = new std::vector<Combo*> [kSPid.size()];
    
    t->GetEntry(0);
    setElectVar();
    evnt_prev = evnt;
    std::cout << "Processing..." << std::endl;
    std::cout.fill('.');
    for (int i = 0; i < Ne; i++) {
      t->GetEntry(i);
      Ep = E;
      if (data_type<2)// not gsim
      {
	Ep = (pid==22)? (E/0.272):( (pid==211 || pid==-211)?(sqrt(P*P+ TMath::Power(TDatabasePDG::Instance()->GetParticle("pi-")->Mass(),2)) ):E);

	if (pid==22)
	  correct_momentum();
	
      }
      if (evnt==evnt_prev)
      {
	//std::cout<<__LINE__<<" "<<findSecondary()<<std::endl;
	Particle *p = new Particle(Px,Py,Pz,Ep,vx,vy,vz,pid);
	
	push_bkgnd(p);
	
	findSecondary();
      }
      else
      {
	//	fill_elec();
	pop_bkgnd();
	if (checkMinPart())
	{
	  int Npart = 1;

	  for (int k =0;k<kSPid.size();k++)
	  {
	    if (DEBUG) std::cout<<"############ Nsecondary of pid: "<<kSPid[k]<<" ::: "<<kSecondary[k].size()<<"###########"<<std::endl; 
	    takeN(kNSPid[kSPid[k]] ,k);

	    Npart*=kCombo[k].size();
	  }
	  if (DEBUG) std::cout<<"############ N candidates for primary: "<<Npart<<"####################\n##########################"<<std::endl;
	  for(int k=0;k<Npart;k++)
	  {
	    kPrimary = new Combo();
	    int div=1;
	    for (int l =0;l<kSPid.size();l++)
	    {
	      int size = kCombo[l].size();
	      //std::cout<<__LINE__<<": \n";
	      *kPrimary+=*kCombo[l][ (k/div)%size ];
	      //std::cout<<__LINE__<<": \n";

	      div*=size;
	    }
	    //  if (DEBUG) std::cout<<"############ Npart from primary: "<<kPrimary->Npart<<"####################"<<std::endl;
  	    fill();
	    delete kPrimary;
	  }
	}
	clear();
	setElectVar();
	evnt_prev=evnt;
	Particle *p =new Particle(Px,Py,Pz,Ep,vx,vy,vz,pid);
	push_bkgnd(p);
	findSecondary();
      }
      std::cout<<setw(15)<<float(i+1)/Ne*100<<" %"<<"\r";
      std::cout.flush();
    } 
    return 0;
  }

  int push_bkgnd(Particle *p)
  {
    if (isPid(p->pid) &&  kBkgnd.size() < kBUFFLIMIT)
    { 
      if (!kBkgnd.empty())
      {
	int i;
	for (i =0;i<kBkgnd.size();i++)
	{
	  if ( (kBkgnd[i]->findPid(p->pid)<kNSPid[p->pid] ) && (kBkgnd[i]->lastEvent!=evnt) && (kBkgnd[i]->isCompatible()) )
	  {
	    kBkgnd[i]->addParticle(p,evnt,kTRUE);
	    break;
	  }
	  
	}
	if (i==kBkgnd.size())
	{
	  Combo *c = new Combo();
	  c->addParticle(p,evnt);
	  kBkgnd.push_back(c);
	}
      }
      else 
      {
	Combo *c = new Combo();
	c->addParticle(p,evnt);
	kBkgnd.push_back(c);
      }
    }
    return 0;
  }

  bool pop_bkgnd()
  {
    bool ret = false;
    if (!kBkgnd.empty())
    {
     for (int i =0;i<kBkgnd.size();i++)
      {
	if (checkMinPart(kBkgnd[i]))
	{
	  //	  std::cout<<__FILE__<<"::"<<__LINE__<<std::endl;
	  fill(kBkgnd[i],kOutBkgnd);
	  delete kBkgnd[i];
	  kBkgnd.erase(kBkgnd.begin()+i);
	  ret = true;
	}
      } 
    } 
    return ret;
  }

  int getBkgnd(TChain *t) {
    t->GetEntry(0);
    setElectVar();
    evnt_prev = evnt;
    std::cout << "Processing..." << std::endl;
    std::cout.fill('.');
    for ( int i = 0; i < Ne; i++)
    {
      t->GetEntry(i);
      Ep = (pid==22)? (E/0.272):sqrt(P*P+ TMath::Power(TDatabasePDG::Instance()->GetParticle("pi-")->Mass(),2));
      
      if (pid==22)
	correct_momentum();

      if (evnt==evnt_prev)
      {
	//std::cout<<__LINE__<<" "<<findSecondary()<<std::endl;
	Particle *p = new Particle(Px,Py,Pz,Ep,vx,vy,vz,pid);
	push_bkgnd(p);
      }
      else
      {
	pop_bkgnd();	
	setElectVar();
	evnt_prev=evnt;

	Particle *p = new Particle(Px,Py,Pz,Ep,vx,vy,vz,pid);
	push_bkgnd(p);
      }
      std::cout<<setw(15)<<float(i+1)/Ne*100<<" %"<<"\r";
      std::cout.flush();
    } 
    return 0;
  }
};

void check_dir(const char *outdir)
{
  struct stat sb;
  if (stat(outdir, &sb) != 0) {
      system(Form("mkdir %s",outdir));
  }
  
}

// Program begins here
int main(int argc, char *argv[]) {

  TBenchmark *bm = new TBenchmark();
  bm->Start("get_omega");
  
  parseopt(argc,argv);

  TFile *corrfile = new TFile("gammECorr.root", "read");
  hcfm = (TH1F*) corrfile->Get("hcfm");

  //char outdir[50];
  //strcpy(outdir,Form("test_eta_%sD_%s",st,tt));

  TChain *t = new TChain();
  if (data_type == 1) t->Add("/data/tsunami/user/b/borquez/EG2Pruned/prune_simul.root/ntuple_accept"); // important line 3
  else if (data_type == 2) t->Add("/data/tsunami/user/b/borquez/EG2Pruned/prune_simul.root/ntuple_thrown"); // important line 4
  else t->Add("/data/tsunami/user/b/borquez/EG2Pruned/prune_dataC-thickD2.root/ntuple_data"); // important line 5
  
  //check_dir(outdir);

  t->SetBranchStatus("*",0);
  t->SetBranchStatus("E",1);
  t->SetBranchStatus("Ee",1);
  t->SetBranchStatus("P",1);
  t->SetBranchStatus("Px",1);
  t->SetBranchStatus("Py",1);
  t->SetBranchStatus("Pz",1);
  t->SetBranchStatus("evnt",1);
  t->SetBranchStatus("Zec",1);
  t->SetBranchStatus("Yec",1);
  t->SetBranchStatus("Xec",1);
  t->SetBranchStatus("TEc",1);
  t->SetBranchStatus("Q2",1);
  t->SetBranchStatus("Nu",1);
  t->SetBranchStatus("W",1);
  t->SetBranchStatus("Pex",1);
  t->SetBranchStatus("Pey",1);
  t->SetBranchStatus("Pez",1);
  t->SetBranchStatus("pid",1);
  t->SetBranchStatus("vxh",1);
  t->SetBranchStatus("vyh",1);
  t->SetBranchStatus("vzh",1);
  t->SetBranchStatus("TargType",1);
  t->SetBranchStatus("ECX",1);
  t->SetBranchStatus("ECY",1);
  t->SetBranchStatus("ECZ",1);
  //  t->SetBranchStatus("TargTypeO",1);
  t->SetBranchAddress("E",&E);
  t->SetBranchAddress("Ee",&Ee);
  t->SetBranchAddress("P",&P);
  t->SetBranchAddress("Px",&Px);
  t->SetBranchAddress("Py",&Py);
  t->SetBranchAddress("Pz",&Pz);
  t->SetBranchAddress("Zec",&Zec);
  t->SetBranchAddress("Yec",&Yec);
  t->SetBranchAddress("Xec",&Xec);
  t->SetBranchAddress("evnt",&evnt);
  t->SetBranchAddress("TEc",&TEc);
  t->SetBranchAddress("Q2",&Q2);
  t->SetBranchAddress("Nu",&Nu);
  t->SetBranchAddress("W",&W);
  t->SetBranchAddress("Pex",&Pex);
  t->SetBranchAddress("Pey",&Pey);
  t->SetBranchAddress("Pez",&Pez);
  t->SetBranchAddress("pid",&pid);
  t->SetBranchAddress("vxh",&vx);
  t->SetBranchAddress("vyh",&vy);
  t->SetBranchAddress("vzh",&vz);
  t->SetBranchAddress("TargType",&TargType);
  t->SetBranchAddress("ECX",&ECX);
  t->SetBranchAddress("ECY",&ECY);
  t->SetBranchAddress("ECZ",&ECZ);
  //t->SetBranchAddress("TargTypeO",&TargTypeO);

  //  t->SetMaxEntryLoop(1e4);
  //  t->SetAlias("Eh",Form("(pid==22)*E/0.272 + (pid!=22)*(TMath::Sqrt(P**2 + %f**2)",TDatabasePDG::Instance()->GetParticle("pi-")->Mass() ));
  if (Ne == -1) Ne = t->GetEntries();

  std::cout << "Number of entries to be processed: " << Ne << std::endl;

  // w -> pi+ pi- a a
  Reaction r("w -> pi+ pi- a a","/data/tsunami/user/b/borquez/EG2Pruned/wout.root"); // woutC-thickD2.root -> important line 6
  r.addPrimary("omega");
  r.addSecondary("gamma");
  r.addSecondary("gamma");
  r.addSecondary("pi+");
  r.addSecondary("pi-");

  r.getCombinations(t);
  r.store();
  
  std::cout << std::endl;

  r.kOutData->Print();
  r.kOutBkgnd->Print();

  corrfile->Close();
  bm->Show("get_pi0");
  return 0;
}
