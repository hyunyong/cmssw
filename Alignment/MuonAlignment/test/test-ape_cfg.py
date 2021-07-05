import FWCore.ParameterSet.Config as cms

process = cms.Process("TEST")
process.load("FWCore.MessageService.MessageLogger_cfi")

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1)
)
process.source = cms.Source("EmptySource")

from CondCore.DBCommon.CondDBSetup_cfi import *

process.muonDTAPE = cms.ESSource("PoolDBESSource", CondDBSetup,
                                     connect = cms.string('sqlite_file:APEs_COV_t2_DT_6DOF_2017part1.db'),
                                     #connect = cms.string('sqlite_file:APEs.db'),
                                     toGet   = cms.VPSet(cms.PSet(record = cms.string("DTAlignmentErrorExtendedRcd"), tag = cms.string("DTAlignmentErrorExtendedRcd")))
                                     )
process.muonCSCAPE = cms.ESSource("PoolDBESSource", CondDBSetup,
                                     connect = cms.string('sqlite_file:APEs_COVfromH_CSC_3DOF_MCfromHW_for2017Data.db'),
                                     #connect = cms.string('sqlite_file:APEs.db'),
                                     toGet   = cms.VPSet(cms.PSet(record = cms.string("CSCAlignmentErrorExtendedRcd"), tag = cms.string("CSCAlignmentErrorExtendedRcd")))
                                     )
process.es_prefer_muonDTAPE = cms.ESPrefer("PoolDBESSource","muonDTAPE")
process.es_prefer_muonCSCAPE = cms.ESPrefer("PoolDBESSource","muonCSCAPE")


process.prod = cms.EDAnalyzer("TestAPE",
    fileName = cms.untracked.string('APE_DATA.root')
)

process.p1 = cms.Path(process.prod)
process.MessageLogger.cout = cms.untracked.PSet(
    threshold = cms.untracked.string('INFO'),
    default = cms.untracked.PSet(
        limit = cms.untracked.int32(10000000)
    )
)


