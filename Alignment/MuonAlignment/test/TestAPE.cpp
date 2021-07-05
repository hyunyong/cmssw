// -*- C++ -*-
//
// Package:    TestAPE
// Class:      TestAPE
//
//
// Description: Module to test the Alignment software
//
//

// system include files
#include <string>
#include <TTree.h>
#include <TFile.h>

// user include files
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "Alignment/CommonAlignment/interface/AlignableObjectId.h"
#include "CondFormats/Alignment/interface/AlignmentErrorsExtended.h"
#include "CondFormats/AlignmentRecord/interface/DTAlignmentErrorExtendedRcd.h"
#include "CondFormats/AlignmentRecord/interface/CSCAlignmentErrorExtendedRcd.h"


//
//
// class declaration
//

class TestAPE : public edm::EDAnalyzer {
public:
  explicit TestAPE(const edm::ParameterSet&);
  ~TestAPE();

  virtual void analyze(const edm::Event&, const edm::EventSetup&);

private:
  void fillTree(int isDT, int isCSC, align::ID id, CLHEP::HepSymMatrix error);

  // ----------member data ---------------------------
  TTree* theTree;
  TFile* theFile;
  float xx, xy, xz, xa, xb, xc, yy, yz, ya, yb, yc, zz, za, zb, zc, aa, ab, ac, bb, bc, cc;
  int Id_, isDT_, isCSC_;

  edm::ESGetToken<AlignmentErrorsExtended, DTAlignmentErrorExtendedRcd> esTokenDTAPE_;
  edm::ESGetToken<AlignmentErrorsExtended, CSCAlignmentErrorExtendedRcd> esTokenCSCAPE_;
};

//
// constructors and destructor
//
TestAPE::TestAPE(const edm::ParameterSet& iConfig) 
  :esTokenDTAPE_(esConsumes<AlignmentErrorsExtended, DTAlignmentErrorExtendedRcd>()),
   esTokenCSCAPE_(esConsumes<AlignmentErrorsExtended, CSCAlignmentErrorExtendedRcd>()) {
  // Open root file and define tree
  std::string fileName = iConfig.getUntrackedParameter<std::string>("fileName", "test.root");
  theFile = new TFile(fileName.c_str(), "RECREATE");
  theTree = new TTree("theTree", "Detector units positions");
 
  theTree->Branch("isDT", &isDT_, "isDT/I");
  theTree->Branch("isCSC", &isCSC_, "isCSC/I");
  theTree->Branch("Id", &Id_, "Id/I");
  theTree->Branch("xx", &xx, "xx/F");
  theTree->Branch("xy", &xy, "xy/F");
  theTree->Branch("xz", &xz, "xz/F");
  theTree->Branch("xa", &xa, "xa/F");
  theTree->Branch("xb", &xb, "xb/F");
  theTree->Branch("xc", &xc, "xc/F");
  theTree->Branch("yy", &yy, "yy/F");
  theTree->Branch("yz", &yz, "yz/F");
  theTree->Branch("ya", &ya, "ya/F");
  theTree->Branch("yb", &yb, "yb/F");
  theTree->Branch("yc", &yc, "yc/F");
  theTree->Branch("zz", &zz, "zz/F");
  theTree->Branch("za", &za, "za/F");
  theTree->Branch("zb", &zb, "zb/F");
  theTree->Branch("zc", &zc, "zc/F");
  theTree->Branch("aa", &aa, "aa/F");
  theTree->Branch("ab", &ab, "ab/F");
  theTree->Branch("ac", &ac, "ac/F");
  theTree->Branch("bb", &bb, "bb/F");
  theTree->Branch("bc", &bc, "bc/F");
  theTree->Branch("cc", &cc, "cc/F");
}

TestAPE::~TestAPE() {
  theTree->Write();
  theFile->Close();
}

void TestAPE::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
  edm::LogInfo("MuonAlignment") << "Starting!";

  //
  // Retrieve muon geometry from event setup
  //
  //edm::ESHandle<AlignmentErrorsExtended> dtAlignmentErrorsExtended = iSetup.getHandle(esTokenDTAPE_);
  auto dtAlignmentErrorsExtended = iSetup.getHandle(esTokenDTAPE_);
  edm::ESHandle<AlignmentErrorsExtended> cscAlignmentErrorsExtended = iSetup.getHandle(esTokenCSCAPE_);

  edm::LogInfo("MuonAlignment") << "DT loop, DT APEs size: " << dtAlignmentErrorsExtended->m_alignError.size(); 
  for (auto dtError: dtAlignmentErrorsExtended->m_alignError) {
    this->fillTree(1, 0, dtError.rawId(), dtError.matrix());
  }
  edm::LogInfo("MuonAlignment") << "CSC loop, CSC APEs size: " << cscAlignmentErrorsExtended->m_alignError.size(); 
  for (auto cscError: cscAlignmentErrorsExtended->m_alignError) {
    this->fillTree(0, 1, cscError.rawId(), cscError.matrix());
  }
  edm::LogInfo("MuonAlignment") << "Done!";
}

//__________________________________________________________________________________________________
void TestAPE::fillTree(int isDT, int isCSC, align::ID id, CLHEP::HepSymMatrix error) {
  isDT_ = isDT;
  isCSC_ = isCSC;
  Id_ = id;
  xx = error(1,1);
  xy = error(1,2);
  xz = error(1,3);
  xa = error(1,4);
  xb = error(1,5);
  xc = error(1,6);
  yy = error(2,2);
  yz = error(2,3);
  ya = error(2,4);
  yb = error(2,5);
  yc = error(2,6);
  zz = error(3,3);
  za = error(3,4);
  zb = error(3,5);
  zc = error(3,6);
  aa = error(4,4);
  ab = error(4,5);
  ac = error(4,6);
  bb = error(5,5);
  bc = error(5,6);
  cc = error(6,6);
  theTree->Fill();
}

//define this as a plug-in
DEFINE_FWK_MODULE(TestAPE);
