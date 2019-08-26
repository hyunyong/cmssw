# GEM Alignment Test

Run Alignment DB creator from from 10_5_0_pre1:
```
cmsrel CMSSW_10_5_0_pre1
cd CMSSW_10_5_0_pre1/src
cmsenv
git cms-merge-topic -u watson-ij:gem-alignment-rcd-1050p1
git clone https://github.com/watson-ij/GEMAlignmentDBTester.git
scram b -j4
cd GEMAlignmentDBTester/MyGEMRcdMaker/test
# Code you might want to run
cmsRun MakeDB.py  # Makes the GEM Alignment Rcd DB
cmsRun TestRead.py  # Checks that the db created in previous cmd has the right tags
cmsRun ReadGEMGeometry.py  # Reads GEM DB tag into annother db file
cmsRun TestGEMBuild.py  # Tries to build and analyse (GEMTestGeometryAnalyzer.cc) GEM Geo
cmsRun dumpSimGeometry_cfg.py tag=2023 version=D21 # Dump Sim Geo (has all components, eg electronics)
cmsShow --sim-geom-file ./cmsSimGeom-2023D21.root -c $CMSSW_RELEASE_BASE/src/Fireworks/Core/macros/simGeo.fwc
cmsRun dumpRecoGeometry_cfg.py tag=2023 version=D21 tgeo=True # Dump nominal
cmsRun dumpRecoGeometry_cfg.py tag=2023-GEMAL version=D21 tgeo=True # Dump Aligned
cmsShow --sim-geom-file ./cmsTGeoRecoGeom-2023-GEMAL.root -c $CMSSW_RELEASE_BASE/src/Fireworks/Core/macros/simGeo.fwc
```

or from a recent nightly

```
cmsrel CMSSW_10_5_X_2019-02-12-1100
cd CMSSW_10_5_X_2019-02-12-1100/src
git cms-merge-topic -u watson-ij:gem-alignment-rcd
git clone https://github.com/watson-ij/GEMAlignmentDBTester.git
scram b -j4
cd GEMAlignmentDBTester/MyGEMRcdMaker/test
cmsRun MakeDB.py
```
