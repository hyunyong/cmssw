set -x
#cmsRun unpackData-GEM.py inputFiles=file:/eos/cms/store/group/dpg_gem/comm_gem/data_p5/run305026/run305026_ls0001_streamA_StorageManager.dat streamer=True skipEvents=0 maxEvents=1 edm=True
cmsRun unpackData-QC8try.py inputFiles=file:/afs/cern.ch/work/g/gmocelli/public/PhD/QC8runs/run000035/run000035_Cosmic_CERNQC8_2019-02-13_chunk_357.dat localMode=True edm=True 
