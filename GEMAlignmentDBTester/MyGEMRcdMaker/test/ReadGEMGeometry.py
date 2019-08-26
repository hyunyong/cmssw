import FWCore.ParameterSet.Config as cms

process = cms.Process("MyGEMAlignmentRcdRetrieveTest")

process.load("CondCore.CondDB.CondDB_cfi")
# input database (in this case the local sqlite file)
process.CondDB.connect = 'frontier://FrontierProd/CMS_CONDITIONS'

process.PoolDBESSource = cms.ESSource("PoolDBESSource",
    process.CondDB,
    DumpStat=cms.untracked.bool(True),
    toGet = cms.VPSet(cms.PSet(
        record = cms.string('GEMRecoGeometryRcd'),
        tag = cms.string("GEMRECO_Geometry_10YV4"),
    ),
    ),
)

process.get = cms.EDAnalyzer("EventSetupRecordDataGetter",
    toGet = cms.VPSet(
        cms.PSet(
            record = cms.string('GEMRecoGeometryRcd'),
            data = cms.vstring('RecoIdealGeometry')
        ),
    ),
    verbose = cms.untracked.bool(True)
)

process.set = cms.EDAnalyzer("MyGEMGeometryMaker",
    loggingOn= cms.untracked.bool(True),
    SinceAppendMode=cms.bool(True),
    Source=cms.PSet(
        IOVRun=cms.untracked.uint32(1)
    )
)

process.OutDB = process.CondDB.clone()
process.OutDB.connect = 'sqlite_file:MyGEMGeometry.db'

process.PoolDBOutputService = cms.Service("PoolDBOutputService",
    process.OutDB,
    timetype = cms.untracked.string('runnumber'),
    toPut = cms.VPSet(cms.PSet(
        record = cms.string('GEMRecoGeometryRcd'),
        tag = cms.string("GEMRECO_Geometry_10YV4"),
    ))
)

# A data source must always be defined. We don't need it, so here's a dummy one.
process.source = cms.Source("EmptyIOVSource",
    timetype = cms.string('runnumber'),
    firstValue = cms.uint64(1),
    lastValue = cms.uint64(1),
    interval = cms.uint64(1)
)

process.path = cms.Path(process.get+process.set)

