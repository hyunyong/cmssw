import FWCore.ParameterSet.Config as cms

process = cms.Process("TEST")
# -- Load default module/services configurations -- //
# Message logger service
process.load("FWCore.MessageService.MessageLogger_cfi")

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1)
)

process.source = cms.Source("EmptySource")


# Database output service if you want to store soemthing in MisalignedMuon
from CondCore.DBCommon.CondDBSetup_cfi import CondDBSetup
process.muonGEMAlignment = cms.ESSource("PoolDBESSource", CondDBSetup,
                                     connect = cms.string('sqlite_file:GEM_phi_z_0d7v4.db'),
                                     toGet   = cms.VPSet(cms.PSet(record = cms.string("GEMAlignmentRcd"),  tag = cms.string("GEM")))
                                     )
process.es_prefer_muonGEMAlignment = cms.ESPrefer("PoolDBESSource","muonGEMAlignment")

process.prod = cms.EDAnalyzer("TestGEMAl",
    fileName = cms.untracked.string('testGEMAl.root')
)

process.p1 = cms.Path(process.prod)
process.MessageLogger.cout = cms.untracked.PSet(
    threshold = cms.untracked.string('INFO'),
    default = cms.untracked.PSet(
        limit = cms.untracked.int32(10000000)
    )
)


