import FWCore.ParameterSet.Config as cms

process = cms.Process('DQMTEST')


process.MessageLogger = cms.Service("MessageLogger",
  statistics = cms.untracked.vstring(),
  destinations = cms.untracked.vstring('cerr'),
  cerr = cms.untracked.PSet(
      threshold = cms.untracked.string('WARNING')
  )
)

process.load('Configuration.Geometry.GeometryExtended2017Reco_cff')
#process.load("DQM.Integration.config.FrontierCondition_GT_cfi")



process.load("DQM.Integration.config.environment_cfi")
process.dqmEnv.subSystemFolder = "GEM"
process.dqmEnv.eventInfoFolder = "EventInfo"
process.dqmSaver.path = ""
process.dqmSaver.tag = "GEM"
"""
import os
fpath =  "/cms/scratch/hyunyong/QC8/"
# Input source
process.source = cms.Source(
        "GEMLocalModeDataSource",
        #fileNames = cms.untracked.vstring (options.inputFiles),
        fileNames = cms.untracked.vstring ([fpath+x for x in os.listdir(fpath) if x.endswith(".dat")]),
        skipEvents=cms.untracked.uint32(0),
        fedId = cms.untracked.int32( 1472 ),  # which fedID to assign
        hasFerolHeader = cms.untracked.bool(False),
        #runNumber = cms.untracked.int32( options.runNum ),
        #processEvents =cms.untracked.vuint32( 118 , 153 , 250 , 282 , 533 , 534 , 595 , 603 , 630 , 794 , 797 , 885 , 915 , 928 , 1128 , 1151 , 1269 , 1302 , 1377 , 1630 , 1883 , 1946 , 1988 , 2197 , 2384 , 2387 , 2405 , 2424 , 2458 , 2461 , 2813 , 2827 , 2987 , 3067 , 3120 , 3208 , 3209 , 3215 , 3265 , 3305 , 3422 , 3628 , 3649 , 3706 , 3831 , 3979 , 4137 , 4290 , 4293 , 4495 , 4533 , 4585 , 4633 , 4656 , 4713 , 4738 , 4817 , 4850 , 4860 , 4887 , 4895 , 4922 , 5013 , 5030 , 5068 , 5085 , 5120 , 5167 , 5300 , 5366 , 5898 , 5953 , 6295 , 6403 , 6494 , 6574 , 6763 , 6792 , 6844 , 6852 , 6867 , 6879 , 6900 , 7004 , 7024 , 7109 , 7115 , 7123 , 7236 , 7353 , 7408 , 7495 , 7500 , 7673 )
)

process.source = cms.Source(
    "NewEventStreamFileReader",
    #fileNames = cms.untracked.vstring ('file:/cms/scratch/jlee/minidaq/run327372_ls0018_streamA_StorageManager.dat'),
    fileNames = cms.untracked.vstring ('file:/cms/ldap_home/hyunyong/QC8FED/src/EventFilter/GEMRawToDigi/test/gem_EDM-qc8spec.root'),
    #skipEvents=cms.untracked.uint32(options.skipEvents)
)
"""
process.source = cms.Source("PoolSource",
  fileNames = cms.untracked.vstring(
    # 'file:/eos/cms/store/express/Commissioning2018/ExpressCosmics/FEVT/Express-v1/000/310/292/00000/6C23251D-4F18-E811-AEC5-02163E01A41D.root'    
    #'file:/xrootd/store/data/Run2018D/SingleMuon/RAW/v1/000/322/617/00000/24C20607-7CB4-E811-A2CD-FA163E6E9DB6.root'
    # 'file:/cms/ldap_home/hyunyong/QC8FED/src/EventFilter/GEMRawToDigi/test/gem_EDM-qc8spec.root'
  ),
  inputCommands = cms.untracked.vstring(
    'drop *',
    'keep FEDRawDataCollection_*_*_*'
  )
)

process.maxEvents = cms.untracked.PSet(
  input = cms.untracked.int32(1000)
)

process.load("EventFilter.GEMRawToDigi.muonGEMDigis_cfi")
process.load('RecoLocalMuon.GEMRecHit.gemRecHits_cfi')
process.load("DQM.GEM.GEMDQM_cff")
process.muonGEMDigis.InputLabel = cms.InputTag("source","gemLocalModeDataSource")
process.muonGEMDigis.useDBEMap = True
process.muonGEMDigis.unPackStatusDigis = True

############## DB file ################# 
from CondCore.CondDB.CondDB_cfi import *
CondDB.DBParameters.authenticationPath = cms.untracked.string('/afs/cern.ch/cms/DB/conddb')
CondDB.connect = cms.string('sqlite_fip:EventFilter/GEMRawToDigi/test/GEMeMap.db')

process.GEMCabling = cms.ESSource("PoolDBESSource",
    CondDB,
    toGet = cms.VPSet(cms.PSet(
        record = cms.string('GEMeMapRcd'),
        tag = cms.string('GEMeMap_v3')
    )),
)
####################################
process.path = cms.Path(
  process.muonGEMDigis 
  +process.gemRecHits 
  +process.GEMDQM
)
process.end_path = cms.EndPath(
  process.dqmEnv +
  process.dqmSaver
)

process.schedule = cms.Schedule(
  process.path,
  process.end_path
)
"""
process.output = cms.OutputModule(
    "PoolOutputModule",
    outputCommands = cms.untracked.vstring("keep *"),
    SelectEvents = cms.untracked.PSet(
        SelectEvents = cms.vstring('path')
    ),
    fileName = cms.untracked.string('gem_EDM.root')
)

process.out = cms.EndPath(
    process.output
)

"""
