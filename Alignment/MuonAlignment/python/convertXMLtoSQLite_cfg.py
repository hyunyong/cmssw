import FWCore.ParameterSet.Config as cms
import sys

process = cms.Process("CONVERT")
process.source = cms.Source("EmptySource")
process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(1))

process.load("Configuration.Geometry.GeometryIdeal_cff")
process.load("Geometry.MuonNumbering.muonNumberingInitialization_cfi")

import Geometry.DTGeometryBuilder.dtGeometry_cfi
process.idealForAlDTGeometry = Geometry.DTGeometryBuilder.dtGeometry_cfi.DTGeometryESModule.clone()
process.idealForAlDTGeometry.applyAlignment = False
process.idealForAlDTGeometry.appendToDataLabel = 'idealForAl'
import Geometry.CSCGeometryBuilder.cscGeometry_cfi
process.idealForAlCSCGeometry = Geometry.CSCGeometryBuilder.cscGeometry_cfi.CSCGeometryESModule.clone()
process.idealForAlCSCGeometry.appendToDataLabel = 'idealForAl'
process.idealForAlCSCGeometry.applyAlignment = False


process.MuonGeometryDBConverter = cms.EDAnalyzer("MuonGeometryDBConverter",
    input = cms.string("xml"),
    fileName = cms.string(str(sys.argv[2])),
    shiftErr = cms.double(1000.),
    angleErr = cms.double(6.28),
    output = cms.string("db"))

process.load("CondCore.DBCommon.CondDBSetup_cfi")
process.PoolDBOutputService = cms.Service("PoolDBOutputService",
    process.CondDBSetup,
    connect = cms.string("sqlite_file:"+str(sys.argv[2])[:-3]+"db"),
    toPut = cms.VPSet(
        cms.PSet(record = cms.string("DTAlignmentRcd"), tag = cms.string("DTAlignmentRcd")),
        cms.PSet(record = cms.string("DTAlignmentErrorExtendedRcd"), tag = cms.string("DTAlignmentErrorExtendedRcd")),
        cms.PSet(record = cms.string("CSCAlignmentRcd"), tag = cms.string("CSCAlignmentRcd")),
        cms.PSet(record = cms.string("CSCAlignmentErrorExtendedRcd"), tag = cms.string("CSCAlignmentErrorExtendedRcd"))))

process.inertGlobalPositionRcd = cms.ESSource("PoolDBESSource",
    process.CondDBSetup,
    connect = cms.string("sqlite_file:inertGlobalPositionRcd.db"),
    toGet = cms.VPSet(cms.PSet(record = cms.string("GlobalPositionRcd"), tag = cms.string("inertGlobalPositionRcd"))))

process.Path = cms.Path(process.MuonGeometryDBConverter)
