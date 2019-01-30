import csv
import os
import sys
import io
import subprocess
import time

#import excel_to_csv
import config_creator
import geometry_files_creator

if __name__ == '__main__':
    run_number = sys.argv[1]
    
    # Conversion from excel to csv files
    #excel_to_csv.excel_to_csv('StandGeometryConfiguration_run%u.xlsx'%int(run_number))
    #excel_to_csv.excel_to_csv('StandAlignmentValues_run%u.xlsx'%int(run_number))
    
    # Generate configuration file
    config_creator.config_creator(run_number)
    time.sleep(1)
    
    # Generate geometry files
    geometry_files_creator.geometry_files_creator(run_number)
    time.sleep(1)
    
    # Compiling after the generation of the geometry files
    scramCommand = "scramv1 b -j 8"
    srcPath = os.path.abspath("launcher.py").split('gemcrs')[0]+'gemcrs/src'
    scramming = subprocess.Popen(scramCommand.split(),stdout=subprocess.PIPE,universal_newlines=True,cwd=srcPath)
    while scramming.poll() is None:
    	line = scramming.stdout.readline()
    	print(line)
	print scramming.stdout.read()
    scramming.communicate()
    time.sleep(1)
    
    # Running the CMSSW code
    runCommand = "cmsRun runGEMCosmicStand_sim.py"
    runPath = os.path.abspath("launcher.py").split('gemcrs')[0] + 'gemcrs/src/Validation/GEMCosmicMuonStand/test'
    running = subprocess.Popen(runCommand.split(),stdout=subprocess.PIPE,universal_newlines=True,cwd=runPath)
    while running.poll() is None:
    	line = running.stdout.readline()
    	print(line)
	print running.stdout.read()
    running.communicate()
    time.sleep(1)
    
    # Creating folder outside the CMMSW release to put the output files and plots
    outDirName = "Results_QC8_run_"+run_number
    resDirPath = os.path.abspath("launcher.py").split('gemcrs')[0]
    #---# Remove old version if want to recreate
    if (os.path.exists(resDirPath+outDirName)):
    	rmDirCommand = "rm -rf "+outDirName
    	rmDir = subprocess.Popen(rmDirCommand.split(),stdout=subprocess.PIPE,universal_newlines=True,cwd=resDirPath)
    	rmDir.communicate()
    #---# Create the new empty folder
    resDirCommand = "mkdir "+outDirName
    resDir = subprocess.Popen(resDirCommand.split(),stdout=subprocess.PIPE,universal_newlines=True,cwd=resDirPath)
    resDir.communicate()
    time.sleep(1)
    
    # Selecting the correct output file, changing the name and moving to the output folder
    out_name = 'out_run_'
    for i in range(8-len(run_number)):
        out_name = out_name + '0'
    out_name = out_name + run_number + '.root'
    
    outFilePath = os.path.abspath("launcher.py").split('gemcrs')[0] + 'gemcrs/src/Validation/GEMCosmicMuonStand/test'
    
    rmCommand = "rm " + out_name
    removing = subprocess.Popen(rmCommand.split(),stdout=subprocess.PIPE,universal_newlines=True,cwd=outFilePath)
    removing.communicate()
    time.sleep(1)
    
    mvCommand = "mv temp_" + out_name + " " + out_name
    moving = subprocess.Popen(mvCommand.split(),stdout=subprocess.PIPE,universal_newlines=True,cwd=outFilePath)
    moving.communicate()
    time.sleep(1)
    
    mvToDirCommand = "mv " + out_name + " " + resDirPath+outDirName + "/" + out_name
    movingToDir = subprocess.Popen(mvToDirCommand.split(),stdout=subprocess.PIPE,universal_newlines=True,cwd=outFilePath)
    movingToDir.communicate()
    time.sleep(1)
    
    # Efficiency computation & output
    effcalcPath = os.path.abspath("launcher.py").split('gemcrs')[0] + 'gemcrs/src/Validation/GEMCosmicMuonStand/test/'
    effoutDir = os.path.abspath("launcher.py").split('gemcrs')[0] + outDirName
    effCommand = "root -l -q " + effcalcPath + "efficiency_calculation.c(" + run_number + ",\"" + effcalcPath + "\")"
    efficiency = subprocess.Popen(effCommand.split(),stdout=subprocess.PIPE,universal_newlines=True,cwd=effoutDir)
    while efficiency.poll() is None:
    	line = efficiency.stdout.readline()
    	print(line)
	print efficiency.stdout.read()
    efficiency.communicate()
    time.sleep(1)
    
