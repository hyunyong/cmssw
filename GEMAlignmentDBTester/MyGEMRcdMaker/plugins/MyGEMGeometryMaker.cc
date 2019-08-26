// -*- C++ -*-
//
// Package:    MyDB/MyGEMGeometryMaker
// Class:      MyGEMGeometryMaker
//
/**\class MyGEMGeometryMaker MyGEMGeometryMaker.cc MyDB/MyGEMGeometryMaker/plugins/MyGEMGeometryMaker.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Ian James Watson
//         Created:  Wed, 13 Feb 2019 14:29:19 GMT
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
 #include "FWCore/Utilities/interface/InputTag.h"
 #include "DataFormats/TrackReco/interface/Track.h"
 #include "DataFormats/TrackReco/interface/TrackFwd.h"

#include "CondFormats/AlignmentRecord/interface/GEMAlignmentRcd.h"
#include "CondFormats/AlignmentRecord/interface/GEMAlignmentErrorRcd.h"
#include "CondFormats/AlignmentRecord/interface/GEMAlignmentErrorExtendedRcd.h"

#include "CondFormats/Alignment/interface/Alignments.h"
#include "CondFormats/Alignment/interface/AlignmentErrors.h"
#include "CondFormats/Alignment/interface/AlignmentErrorsExtended.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CondCore/DBOutputService/interface/PoolDBOutputService.h"

#include "CLHEP/Vector/EulerAngles.h"
#include "CLHEP/Vector/Rotation.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "CondFormats/Alignment/interface/AlignTransform.h"

 #include "DataFormats/DetId/interface/DetId.h"
 #include "DataFormats/MuonDetId/interface/MuonSubdetId.h"
 #include "DataFormats/MuonDetId/interface/GEMDetId.h"

#include "CondFormats/GeometryObjects/interface/RecoIdealGeometry.h"
#include "FWCore/Framework/interface/EventSetup.h"

//
// class declaration
//

// If the analyzer does not use TFileService, please remove
// the template argument to the base class so the class inherits
// from  edm::one::EDAnalyzer<>
// This will improve performance in multithreaded jobs.


using reco::TrackCollection;

class MyGEMGeometryMaker : public edm::one::EDAnalyzer<edm::one::SharedResources>  {
   public:
      explicit MyGEMGeometryMaker(const edm::ParameterSet&);
      ~MyGEMGeometryMaker();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


   private:
      virtual void beginJob() override;
      virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;

      // ----------member data ---------------------------
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
MyGEMGeometryMaker::MyGEMGeometryMaker(const edm::ParameterSet& iConfig)

{
   //now do what ever initialization is needed

}


MyGEMGeometryMaker::~MyGEMGeometryMaker()
{

   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//
#include "Geometry/Records/interface/GEMRecoGeometryRcd.h"
// ------------ method called for each event  ------------
void
MyGEMGeometryMaker::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  auto gemRcd = iSetup.get<GEMRecoGeometryRcd>();
  edm::ESHandle<RecoIdealGeometry> MyGEM;
  gemRcd.get(MyGEM);
  
  edm::Service<cond::service::PoolDBOutputService> poolDbService;
  if( poolDbService.isAvailable() ) {
      poolDbService->writeOne( &*MyGEM, poolDbService->currentTime(),
                                               "GEMRecoGeometryRcd"  );
  }
  else
      throw std::runtime_error("PoolDBService required.");
}


// ------------ method called once each job just before starting event loop  ------------
void
MyGEMGeometryMaker::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void
MyGEMGeometryMaker::endJob()
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
MyGEMGeometryMaker::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);

  //Specify that only 'tracks' is allowed
  //To use, remove the default given above and uncomment below
  //ParameterSetDescription desc;
  //desc.addUntracked<edm::InputTag>("tracks","ctfWithMaterialTracks");
  //descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(MyGEMGeometryMaker);
