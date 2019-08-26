// -*- C++ -*-
//
// Package:    MyDB/MyGEMRcdMaker
// Class:      MyGEMRcdMaker
//
/**\class MyGEMRcdMaker MyGEMRcdMaker.cc MyDB/MyGEMRcdMaker/plugins/MyGEMRcdMaker.cc

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

#include "Geometry/GEMGeometry/interface/GEMGeometry.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"

#include "Alignment/CommonAlignment/interface/Utilities.h"

//
// class declaration
//

// If the analyzer does not use TFileService, please remove
// the template argument to the base class so the class inherits
// from  edm::one::EDAnalyzer<>
// This will improve performance in multithreaded jobs.


using reco::TrackCollection;

class MyGEMRcdMaker : public edm::one::EDAnalyzer<edm::one::SharedResources>  {
   public:
      explicit MyGEMRcdMaker(const edm::ParameterSet&);
      ~MyGEMRcdMaker();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


   private:
      virtual void beginJob() override;
      virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;

      float xShift,yShift,zShift;

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
MyGEMRcdMaker::MyGEMRcdMaker(const edm::ParameterSet& p)
{
  xShift = p.getParameter<double>("xShift");
  yShift = p.getParameter<double>("yShift");
  zShift = p.getParameter<double>("zShift");
}


MyGEMRcdMaker::~MyGEMRcdMaker()
{

   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)
}


//
// member functions
//

using CLHEP::Hep3Vector;
using CLHEP::HepRotation;
// ------------ method called for each event  ------------
void
MyGEMRcdMaker::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  // Get the GEM Geometry
  edm::ESHandle<GEMGeometry> gemGeo;
  iSetup.get<MuonGeometryRecord>().get(gemGeo);

  Alignments* MyGEMAlignment = new Alignments();
  // AlignmentErrors* MyGEMAlignmentError = new AlignmentErrors();
  AlignmentErrorsExtended* MyGEMAlignmentErrorExtended = new AlignmentErrorsExtended();

  // Form the data here

  // Needs to match with what GEMGeometry hands out, which is:
  //
  // 1 detid per eta partition
  // 1 det id for chamber with roll=0
  // 1 det id for the superchamber with layer=0, roll=0
  //

  for (auto roll : gemGeo->etaPartitions()) {
    auto center = roll->surface().toGlobal(LocalPoint(xShift,yShift,zShift));
    auto rot = roll->surface().rotation();

    auto hrot = HepRotation(Hep3Vector(rot.xx(), rot.xy(), rot.xz()).unit(),
			    Hep3Vector(rot.yx(), rot.yy(), rot.yz()).unit(),
    			    Hep3Vector(rot.zx(), rot.zy(), rot.zz()).unit()
    			    );
    auto euler = hrot.inverse().eulerAngles();
    MyGEMAlignment->m_align.push_back(AlignTransform(AlignTransform::Translation(center.x(), center.y(), center.z()),
						     euler,
    						     roll->id()));
    MyGEMAlignmentErrorExtended->m_alignError.push_back(AlignTransformErrorExtended(AlignTransformErrorExtended::SymMatrix(6),
										    roll->id()));
  }

  for (auto chmb : gemGeo->chambers()) {
    auto center = chmb->surface().toGlobal(LocalPoint(xShift,yShift,zShift));
    auto rot = chmb->surface().rotation();
    auto hrot = HepRotation(Hep3Vector(rot.xx(), rot.xy(), rot.xz()).unit(),
			    Hep3Vector(rot.yx(), rot.yy(), rot.yz()).unit(),
    			    Hep3Vector(rot.zx(), rot.zy(), rot.zz()).unit()
    			    );
    auto euler = hrot.inverse().eulerAngles();
    MyGEMAlignment->m_align.push_back(AlignTransform(AlignTransform::Translation(center.x(), center.y(), center.z()),
						     euler,
						     chmb->id()));
    MyGEMAlignmentErrorExtended->m_alignError.push_back(AlignTransformErrorExtended(AlignTransformErrorExtended::SymMatrix(6),
										    chmb->id()));
  }

  for (auto sch : gemGeo->superChambers()) {
    auto center = sch->surface().toGlobal(LocalPoint(xShift,yShift,zShift));
    auto rot = sch->surface().rotation();
    auto hrot = HepRotation(Hep3Vector(rot.xx(), rot.xy(), rot.xz()).unit(),
			    Hep3Vector(rot.yx(), rot.yy(), rot.yz()).unit(),
    			    Hep3Vector(rot.zx(), rot.zy(), rot.zz()).unit()
    			    );
    auto euler = hrot.inverse().eulerAngles();
    MyGEMAlignment->m_align.push_back(AlignTransform(AlignTransform::Translation(center.x(), center.y(), center.z()),
						     euler,
						     sch->id()));
    MyGEMAlignmentErrorExtended->m_alignError.push_back(AlignTransformErrorExtended(AlignTransformErrorExtended::SymMatrix(6),
										    sch->id()));
  }
  
  // GeometryAligner expects ordering by raw ID
  std::sort(MyGEMAlignment->m_align.begin(), MyGEMAlignment->m_align.end(), [](AlignTransform a, AlignTransform b){return a.rawId() < b.rawId();});
  std::sort(MyGEMAlignmentErrorExtended->m_alignError.begin(), MyGEMAlignmentErrorExtended->m_alignError.end(), [](auto a, auto b){return a.rawId() < b.rawId();});
  edm::Service<cond::service::PoolDBOutputService> poolDbService;
  if( poolDbService.isAvailable() ) {
      poolDbService->writeOne( MyGEMAlignment, poolDbService->currentTime(),
                                               "GEMAlignmentRcd"  );
      // poolDbService->writeOne( MyGEMAlignmentError, poolDbService->currentTime(),
      //                                          "GEMAlignmentErrorRcd"  );
      poolDbService->writeOne( MyGEMAlignmentErrorExtended, poolDbService->currentTime(),
                                               "GEMAlignmentErrorExtendedRcd"  );
  }
  else
      throw std::runtime_error("PoolDBService required.");
}


// ------------ method called once each job just before starting event loop  ------------
void
MyGEMRcdMaker::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void
MyGEMRcdMaker::endJob()
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
MyGEMRcdMaker::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
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
DEFINE_FWK_MODULE(MyGEMRcdMaker);
