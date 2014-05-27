import FWCore.ParameterSet.Config as cms

process = cms.Process("Ana")
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load("Configuration.StandardSequences.Geometry_cff")
process.load('RecoJets.JetAssociationProducers.ak7JTA_cff') 
process.load('RecoJets.JetAssociationProducers.ak5JTA_cff')
process.load('RecoJets.JetAssociationProducers.iterativeCone5JTA_cff')
process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')
#############   Set the number of events #############
process.maxEvents = cms.untracked.PSet(
     input = cms.untracked.int32(-1)
)


#############   Define the source file ###############
"""
directory = '/pnfs/user/QCD/QCD_Pt-15to3000_Tune1_Flat_8TeV_pythia8_AODSIM_PU_S7_START52_V9-v1/'
inputDir = [directory]
theInputFiles = []
import os
for d in range(len(inputDir)):
  my_dir = inputDir[d]
  if not os.path.isdir(my_dir):
    print "ERROR: This is not a valid directory: ", my_dir
    if d==len(inputDir)-1:
      print "ERROR: No input files were selected"
      exit()
    continue
  print "Proceed to next directory"
  ls = os.listdir(my_dir)
  ## this works only if you pass the location on pnfs - FIXME for files staring with store/user/...
#  theInputFiles.extend([my_dir[:16] + x for x in ls if x.endswith('root')])
  theInputFiles.extend(['file:' + my_dir + x for x in ls if x.endswith('.root')])

theInputFiles = theInputFiles
##inputFiles = ['file:out_SingleMuPt10Fwd_GEM2019_8PartIncRad_DIGI_L1.root']
print "InputFiles: ", theInputFiles
print len(theInputFiles)
"""

process.source = cms.Source("PoolSource",
     fileNames = cms.untracked.vstring()
)

#############   ZSP + JPT Correction ##################
process.load('JetMETCorrections.Configuration.DefaultJEC_cff')

#############   inclusive jet tree producer ##################
process.TFileService = cms.Service("TFileService",fileName = cms.string('InclusiveJetTree_mc.root'))

# Other statements
process.GlobalTag.globaltag = 'START53_V19::All'

process.ak5calo = cms.EDAnalyzer('InclusiveJetTreeProducer',
    pfjets                 = cms.string('ak5PFJets'),
    jets                   = cms.string('ak5CaloJets'), 
    jetsID                 = cms.string('ak5JetID'),
    jetExtender            = cms.string('ak5JetExtender'),
    #jptZSP                 = cms.string('ZSPJetCorJetAntiKt5'),
    #jptCorrector           = cms.string('JetPlusTrackZSPCorrectorAntiKt5'),
    met                    = cms.string('met'),
    metNoHF                = cms.string('metNoHF'),
    hcalNoiseTag           = cms.InputTag('hcalnoise'),
    minJetPt               = cms.double(3.0), 
    minPFJetPt             = cms.double(3.0),
    jetTriggerNames        = cms.vstring('HLT_PFJet40','HLT_PFJet80','HLT_PFJet140','HLT_PFJet200','HLT_PFJet260','HLT_PFJet320'),
    triggerProcessName     = cms.string('HLT'), 
    triggerResultsTag      = cms.InputTag('TriggerResults','','HLT'),
    #l1TriggerNames         = cms.vstring('L1_SingleJet6','L1_SingleJet30','L1_SingleJet40','L1_SingleJet50','L1_SingleJet60'),     
    l1TriggerNames         = cms.vstring(),     
    L1GTReadoutRcdSource   = cms.InputTag('gtDigis'),
    L1GTObjectMapRcdSource = cms.InputTag('hltL1GtObjectMap','','HLT'),
    ########### Only in MC ################################
    #isMCarlo               = cms.untracked.bool(True),
    isMCarlo               = cms.untracked.bool(False),
    genjets                = cms.untracked.string('ak5GenJets')
)

#process.p = cms.Path(process.ZSPJetCorrectionsAntiKt5 * process.ZSPrecoJetAssociationsAntiKt5 * process.ak5JTA * process.ak5calo)
process.p = cms.Path(process.ak5JTA * process.ak5calo)
#############   Format MessageLogger #################
process.MessageLogger.cerr.FwkReport.reportEvery = 10

