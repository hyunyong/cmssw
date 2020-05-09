import FWCore.ParameterSet.Config as cms

process = cms.Process("CONVERT")
process.source = cms.Source("EmptySource")
process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(1))

process.load("Geometry.CMSCommonData.cmsIdealGeometryXML_cfi")
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
                                                 fileName = cms.string("REPLACEME.xml"),
                                                 shiftErr = cms.double(1000.),
                                                 angleErr = cms.double(6.28),

                                                 output = cms.string("xml"),
                                                 outputXML = cms.PSet(fileName = cms.string("REPLACEME.xml"),
                                                                      relativeto = cms.string("ideal"),
                                                                      survey = cms.bool(False),
                                                                      rawIds = cms.bool(False),
                                                                      eulerAngles = cms.bool(False),
                                                                      precision = cms.int32(10),
                                                                      suppressDTBarrel = cms.untracked.bool(True),
                                                                      suppressDTWheels = cms.untracked.bool(True),
                                                                      suppressDTStations = cms.untracked.bool(True),
                                                                      suppressDTChambers = cms.untracked.bool(False),
                                                                      suppressDTSuperLayers = cms.untracked.bool(False),
                                                                      suppressDTLayers = cms.untracked.bool(False),
                                                                      suppressCSCEndcaps = cms.untracked.bool(True),
                                                                      suppressCSCStations = cms.untracked.bool(True),
                                                                      suppressCSCRings = cms.untracked.bool(True),
                                                                      suppressCSCChambers = cms.untracked.bool(False),
                                                                      suppressCSCLayers = cms.untracked.bool(False)
                                                                      ))

process.Path = cms.Path(process.MuonGeometryDBConverter)
