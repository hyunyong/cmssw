// -*- C++ -*-
//
// Package:     MuonAlignment
// Class  :     MuonAlignmentInputMethod
//
// Implementation:
//     <Notes on implementation>
//
// Original Author:  Jim Pivarski
//         Created:  Thu Mar  6 14:25:07 CST 2008
// $Id: MuonAlignmentInputMethod.cc,v 1.3 2009/01/19 11:07:37 flucke Exp $
//

// system include files
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/ESTransientHandle.h"

// user include files
#include "Alignment/MuonAlignment/interface/MuonAlignmentInputMethod.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
MuonAlignmentInputMethod::MuonAlignmentInputMethod() {}

// MuonAlignmentInputMethod::MuonAlignmentInputMethod(const MuonAlignmentInputMethod& rhs)
// {
//    // do actual copying here;
// }

MuonAlignmentInputMethod::~MuonAlignmentInputMethod() {}

//
// assignment operators
//
// const MuonAlignmentInputMethod& MuonAlignmentInputMethod::operator=(const MuonAlignmentInputMethod& rhs)
// {
//   //An exception safe implementation is
//   MuonAlignmentInputMethod temp(rhs);
//   swap(rhs);
//
//   return *this;
// }

//
// member functions
//

AlignableMuon* MuonAlignmentInputMethod::newAlignableMuon(const edm::EventSetup& iSetup) const {
  edm::ESHandle<DTGeometry> dtGeometry = idealDTGeometry(iSetup);
  edm::ESHandle<CSCGeometry> cscGeometry = idealCSCGeometry(iSetup);

  return new AlignableMuon(&(*dtGeometry), &(*cscGeometry));
}

edm::ESHandle<DTGeometry> MuonAlignmentInputMethod::idealDTGeometry(const edm::EventSetup& iSetup) const {
  edm::ESHandle<DTGeometry> muonDTGeometry_;
  iSetup.get<MuonGeometryRecord>().get("idealForAl", muonDTGeometry_);
  return muonDTGeometry_;
}

edm::ESHandle<CSCGeometry> MuonAlignmentInputMethod::idealCSCGeometry(const edm::EventSetup& iSetup) const {
  edm::ESHandle<CSCGeometry> muonCSCGeometry_;
  iSetup.get<MuonGeometryRecord>().get("idealForAl", muonCSCGeometry_);
  return muonCSCGeometry_;
}

//
// const member functions
//

//
// static member functions
//
