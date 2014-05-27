import FWCore.ParameterSet.Config as cms

process = cms.Process("Ana")
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.Geometry_cff')
process.load('RecoJets.JetAssociationProducers.trackExtrapolator_cfi')
process.load('RecoJets.JetAssociationProducers.ak7JTA_cff') 
process.load('RecoJets.JetAssociationProducers.ak5JTA_cff')
process.load('L1TriggerConfig.L1GtConfigProducers.L1GtTriggerMaskTechTrigConfig_cff')
process.load('HLTrigger.HLTfilters.hltLevel1GTSeed_cfi')
#process.load('Configuration.Geometry.GeometryIdeal_cff')
process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')
#############   Set the number of events #############
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)


process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring("root://uosaf0007.sscc.uos.ac.kr//cms/store/data/Run2012A/Jet/AOD/13Jul2012-v1/00000/FEB517F1-ADD9-E111-A589-848F69FD294F.root")
)
#############   Monster Event Removal  ###############
process.monster = cms.EDFilter(
"FilterOutScraping",
applyfilter = cms.untracked.bool(True),
debugOn     = cms.untracked.bool(False),
numtrack    = cms.untracked.uint32(10),
thresh      = cms.untracked.double(0.25)
)  
#############  Primary Vertex Filter  ################
process.primaryVertexFilter = cms.EDFilter("GoodVertexFilter",
       vertexCollection = cms.InputTag('offlinePrimaryVertices'),
       minimumNDOF = cms.uint32(4) ,
       maxAbsZ = cms.double(15),
       maxd0 = cms.double(2)
)
#############   inclusive jet tree producer ##################
process.TFileService = cms.Service("TFileService",fileName = cms.string('InclusiveJetTree_data_Run2012.root'))


process.GlobalTag.globaltag = 'FT_53_V6C_AN4::All'

process.ak5calo = cms.EDAnalyzer('InclusiveJetTreeProducer',
    pfjets                 = cms.string('ak5PFJetsL2L3'),
    jets                   = cms.string('ak5CaloJets'), 
    jetsID                 = cms.string('ak5JetID'),
    jetExtender            = cms.string('ak5JetExtender'),
    met                    = cms.string('met'),
    metNoHF                = cms.string('metNoHF'),
    #hcalNoiseTag           = cms.InputTag('hcalnoise'),
    minJetPt               = cms.double(6.0), 
    minPFJetPt             = cms.double(15.0),
    jetTriggerNames        = cms.vstring('HLT_PFJet40','HLT_PFJet80','HLT_PFJet140','HLT_PFJet200','HLT_PFJet260','HLT_PFJet320'),
    triggerResultsTag      = cms.InputTag('TriggerResults','','HLT'),
    l1TriggerNames         = cms.vstring(),     
    L1GTReadoutRcdSource   = cms.InputTag('gtDigis'),
    L1GTObjectMapRcdSource = cms.InputTag('hltL1GtObjectMap','','HLT')
)


process.load('JetMETCorrections/Configuration/DefaultJEC_cff')

process.load("RecoJets.JetProducers.ak5JetID_cfi")

process.PFJetsLooseId = cms.EDProducer("PFJetIdSelector",
    src = cms.InputTag( "ak5PFJets" ),
    idLevel = cms.string("LOOSE")
)
#process.ak5PFJetsL2L3.src = cms.InputTag("PFJetLooseId")

process.p = cms.Path(process.primaryVertexFilter * process.ak5JTA * process.PFJetsLooseId * process.ak5PFJetsL2L3 * process.ak5calo)
#process.p = cms.Path(process.primaryVertexFilter * process.trackExtrapolator *  process.ak7JTA * process.ak7calo)
#############   Format MessageLogger #################
process.MessageLogger.cerr.FwkReport.reportEvery = 10


