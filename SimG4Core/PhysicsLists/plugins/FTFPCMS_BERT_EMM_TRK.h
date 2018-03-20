#ifndef SimG4Core_PhysicsLists_FTFPCMS_BERT_EMM_TRK_H
#define SimG4Core_PhysicsLists_FTFPCMS_BERT_EMM_TRK_H

#include "SimG4Core/Physics/interface/PhysicsList.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

class FTFPCMS_BERT_EMM_TRK: public PhysicsList {

public:
  FTFPCMS_BERT_EMM_TRK(G4LogicalVolumeToDDLogicalPartMap& map, const HepPDT::ParticleDataTable * table_, sim::ChordFinderSetter *chordFinderSetter_, const edm::ParameterSet & p);
};

#endif



