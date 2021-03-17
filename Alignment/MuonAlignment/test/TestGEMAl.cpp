// -*- C++ -*-
//
// Package:    TestMisalign
// Class:      TestMisalign
//
//
// Description: Module to test the Alignment software
//
//

// system include files
#include <string>
#include <TTree.h>
#include <TFile.h>
#include <TRotMatrix.h>

// user include files
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "Geometry/Records/interface/MuonGeometryRecord.h"

#include "CondFormats/Alignment/interface/Alignments.h"
#include "CondFormats/AlignmentRecord/interface/GEMAlignmentRcd.h"
#include "DataFormats/MuonDetId/interface/GEMDetId.h"

//
//
// class declaration
//

class TestGEMAl : public edm::EDAnalyzer {
public:
  explicit TestGEMAl(const edm::ParameterSet&);
  ~TestGEMAl();

  virtual void analyze(const edm::Event&, const edm::EventSetup&);

private:
  // ----------member data ---------------------------
  TTree* theTree;
  TFile* theFile;
  float x, y, z, phi_x, phi_y, phi_z;
  int Id;

  edm::ESGetToken<Alignments, GEMAlignmentRcd> esTokenGEMAl_;
};

//
// constructors and destructor
//
TestGEMAl::TestGEMAl(const edm::ParameterSet& iConfig) 
  :esTokenGEMAl_(esConsumes<Alignments, GEMAlignmentRcd>()) {
  // Open root file and define tree
  std::string fileName = iConfig.getUntrackedParameter<std::string>("fileName", "testGEMAl.root");
  theFile = new TFile(fileName.c_str(), "RECREATE");
  theTree = new TTree("theTree", "Detector units positions");

  theTree->Branch("Id", &Id, "Id/I");
  theTree->Branch("x", &x, "x/F");
  theTree->Branch("y", &y, "y/F");
  theTree->Branch("z", &z, "z/F");
  theTree->Branch("phi_x", &phi_x, "phi_x/F");
  theTree->Branch("phi_y", &phi_y, "phi_y/F");
  theTree->Branch("phi_z", &phi_z, "phi_z/F");
}

TestGEMAl::~TestGEMAl() {
  theTree->Write();
  theFile->Close();
}

void TestGEMAl::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
  edm::LogInfo("MuonAlignment") << "Starting!";

  //
  // Retrieve muon geometry from event setup
  //
  auto gemAlignment = iSetup.getHandle(esTokenGEMAl_);
  // Now loop on detector units, and store position and orientation
  for (auto gemAl : gemAlignment->m_align) {
    Id = gemAl.rawId();
    GEMDetId gemId(Id);
    auto disp = gemAl.translation();
    auto rot = gemAl.rotation();
    theTree->Fill();
    std::cout << gemId << ", " << disp.x() << ", " << disp.y() << ", " <<disp.z() << ", " << rot.xx() << ", " << rot.xy() << ", " << rot.xz() << ", " << rot.yx() << ", " << rot.yy() << ", " << rot.yz() << ", " << rot.zx() << ", " << rot.zy() << ", " << rot.zz() << std::endl;
  }
  edm::LogInfo("MuonAlignment") << "Done!";
}

//define this as a plug-in
DEFINE_FWK_MODULE(TestGEMAl);
