import FWCore.ParameterSet.Config as cms
from Configuration.Generator.Pythia8CommonSettings_cfi import *
from Configuration.Generator.Pythia8CUEP8M1Settings_cfi import *

process = cms.Process('GEN')

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('SimGeneral.MixingModule.mixNoPU_cfi')
process.load('Configuration.StandardSequences.Generator_cff')
process.load('IOMC.EventVertexGenerators.VtxSmearedRealistic8TeVCollision_cfi')
process.load('GeneratorInterface.Core.genFilterSummary_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')

import sys
max_num = 1000 #int(sys.argv[2])
f_num = 314 #int(sys.argv[3])
min_pt = 700 #int(sys.argv[4])
max_pt = 2500 #int(sys.argv[5])

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(max_num)
)

# Input source
process.source = cms.Source("EmptySource")

process.options = cms.untracked.PSet(
#SkipEvent = cms.untracked.vstring('ProductNotFound')
)

# Production Info
process.configurationMetadata = cms.untracked.PSet(
    version = cms.untracked.string('\\$Revision$'),
    annotation = cms.untracked.string('PYTHIA8: QCD dijet production, pThat = %d .. %d GeV, Tune4C'%(min_pt, max_pt)),
    name = cms.untracked.string('\\$Source$')
)

# Output definition
process.RAWSIMoutput = cms.OutputModule("PoolOutputModule",
)

# Additional output definition

# Other statements
process.genstepfilter.triggerConditions=cms.vstring("generation_step")
#from Configuration.AlCa.GlobalTag import GlobalTag
#process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:mc', '')

process.generator = cms.EDFilter("Pythia8GeneratorFilter",
                         pythiaPylistVerbosity = cms.untracked.int32(0),
                         filterEfficiency = cms.untracked.double(1.0),
                         pythiaHepMCVerbosity = cms.untracked.bool(False),
                         comEnergy = cms.double(8000.0),
                         maxEventsToPrint = cms.untracked.int32(0),
                         PythiaParameters = cms.PSet(
        pythia8CommonSettingsBlock,
        pythia8CUEP8M1SettingsBlock,
        processParameters = cms.vstring(
            'HardQCD:all = on',
            'PhaseSpace:pTHatMin = %d  '%min_pt,
            'PhaseSpace:pTHatMax = %d  '%max_pt,
            ),
        parameterSets = cms.vstring('pythia8CommonSettings',
                                    'pythia8CUEP8M1Settings',
                                    'processParameters',
                                    )
        )
                         )




# Path and EndPath definitions
process.generation_step = cms.Path(process.pgen)
process.genfiltersummary_step = cms.EndPath(process.genFilterSummary)
process.endjob_step = cms.EndPath(process.endOfProcess)
process.RAWSIMoutput_step = cms.EndPath(process.RAWSIMoutput)

# Schedule definition
process.schedule = cms.Schedule(process.generation_step,process.genfiltersummary_step,process.endjob_step,process.RAWSIMoutput_step)
# filter all path with the production filter sequence
for path in process.paths:
	getattr(process,path)._seq = process.generator * getattr(process,path)._seq 
process.source.firstRun = cms.untracked.uint32(f_num)
process.RandomNumberGeneratorService.generator.initialSeed = f_num


process.load('GeneratorInterface.RivetInterface.rivetAnalyzer_cfi')
process.rivetAnalyzer.AnalysisNames = cms.vstring('jetShape')
process.generation_step+=process.rivetAnalyzer
process.schedule.remove(process.RAWSIMoutput_step)
process.rivetAnalyzer.OutputFile = cms.string('QCD_powheg_ct10_pythia8_cuetp8m1_13TeV_rivet_%03d.yoda'%(f_num))

