import FWCore.ParameterSet.Config as cms

process = cms.Process("MyGEMAlignmentRcdWriter")

# Load CondDB service
process.load("CondCore.CondDB.CondDB_cfi")

process.load("Geometry.CMSCommonData.cmsExtendedGeometry2023D39XML_cfi")
process.load("Geometry.MuonNumbering.muonNumberingInitialization_cfi")
process.load("Geometry.GEMGeometryBuilder.gemGeometry_cfi")
process.GEMGeometryESModule.applyAlignment = cms.bool(False)
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
#from Configuration.AlCa.GlobalTag import GlobalTag
#process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:phase2_realistic', '')

# output database (in this case local sqlite file)
process.OutDB = process.CondDB.clone()
process.OutDB.connect = 'sqlite_file:GEMAl.db'

# A data source must always be defined. We don't need it, so here's a dummy one.
process.source = cms.Source("EmptyIOVSource",
    timetype = cms.string('runnumber'),
    firstValue = cms.uint64(1),
    lastValue = cms.uint64(1),
    interval = cms.uint64(1)
)

# We define the output service.
process.PoolDBOutputService = cms.Service("PoolDBOutputService",
    process.OutDB,
    timetype = cms.untracked.string('runnumber'),
    toPut = cms.VPSet(cms.PSet(
        record = cms.string('GEMAlignmentRcd'),
        tag = cms.string('GEMAlignment_test')
    ),
    # cms.PSet(
    #     record = cms.string('GEMAlignmentErrorRcd'),
    #     tag = cms.string('myGEMAlignment_test')
    # ),
    cms.PSet(
        record = cms.string('GEMAlignmentErrorExtendedRcd'),
        tag = cms.string('GEMAlignmentErrorExtended_test')
    ))
)

process.gem_maker = cms.EDAnalyzer("MyGEMRcdMaker",
    loggingOn= cms.untracked.bool(True),
    SinceAppendMode=cms.bool(True),
    Source=cms.PSet(
        IOVRun=cms.untracked.uint32(1)
    ),
    xShift = cms.double(1.5),
    yShift = cms.double(0),
    zShift = cms.double(0)
)

process.path = cms.Path(process.gem_maker)
