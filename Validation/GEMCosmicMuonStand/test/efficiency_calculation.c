#include <TH1D.h>
#include <TH2D.h>
#include <TH3D.h>
#include <TEfficiency.h>
#include <TFile.h>
#include <TTree.h>
#include "TGraphAsymmErrors.h"
#include <TBranch.h>
#include <TCanvas.h>
#include <TSpectrum.h>
#include <TF1.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

void efficiency_calculation(int run, string configDir)
{
	// Getting the root file
	
	string filename = "out_run_";
	for (int i=0; i<(8-to_string(run).size()); i++)
	{
		filename += "0";
	}
	filename += to_string(run) + ".root";
	const char *file = filename.c_str();
	
	TFile *infile = new TFile(file,"UPDATE");
	if (infile->IsOpen()) cout << "File opened successfully" << endl;
	
	// Getting the tree
	
	TTree *tree = (TTree*)infile->Get("gemcrValidation/tree");
	
	// Getting the 3D numerator & denominator
	
	TH3D *num3D = (TH3D*)infile->Get("gemcrValidation/hitsVFATnum");
	TH3D *denom3D = (TH3D*)infile->Get("gemcrValidation/hitsVFATdenom");
	
	// Generating 1D histograms (num, denom, eff) for the 30 chambers
	
	char *name = new char[40];
	
	TH1D *num1D[30];
	TH1D *denom1D[30];
	TGraphAsymmErrors *eff1D[30];
	
	for (int ch=0; ch<30; ch++)
	{
    	sprintf(name,"Eff_Numerator_ch_%u",ch);
    	num1D[ch] = new TH1D(name,"",24,0,24);
    	sprintf(name,"Eff_Denominator_ch_%u",ch);
    	denom1D[ch] = new TH1D(name,"",24,0,24);
    	eff1D[ch] = new TGraphAsymmErrors;
    	
		for (int eta=0; eta<8; eta++)
		{
			for (int phi=0; phi<3; phi++)
			{
				num1D[ch]->SetBinContent((8*(2-phi)+(7-eta)+1),num3D->GetBinContent(phi+1,eta+1,ch+1));
				denom1D[ch]->SetBinContent((8*(2-phi)+(7-eta)+1),denom3D->GetBinContent(phi+1,eta+1,ch+1));
			}
		}
		eff1D[ch]->Divide(num1D[ch],denom1D[ch]);
	}
	
	// Open stand configuration file for present run & get names + positions of chambers
	
	ifstream standConfigFile;
	string configName = configDir + "StandGeometryConfiguration_run" + to_string(run) + ".csv";
	standConfigFile.open(configName);
	string line, split, comma = ",", slash = "/";
	vector<string> chamberName;
	int ChPos = 0;
	vector<int> chamberPos;
	size_t pos = 0;
	while (getline(standConfigFile, line))
    {    	
    	pos = line.find(comma);
    	split = line.substr(0, pos);
    	if (split == "RunNumber" || split == "ChamberName") continue;
    	chamberName.push_back(split);
    	line.erase(0, pos + comma.length());
    	
    	pos = line.find(slash);
    	split = line.substr(0, pos);
    	ChPos = (stoi(split)-1)*2; // (Row-1)*2
    	line.erase(0, pos + slash.length());
    	
    	pos = line.find(slash);
    	split = line.substr(0, pos);
    	ChPos += (stoi(split)-1)*10; // (Row-1)*2 + (Col-1)*10
    	line.erase(0, pos + slash.length());
    	
    	pos = line.find(comma);
    	split = line.substr(0, pos);
    	if (split == "B") ChPos += 0; // (Row-1)*2 + (Col-1)*10 + 0
    	if (split == "T") ChPos += 1; // (Row-1)*2 + (Col-1)*10 + 1
    	line.erase(0, pos + comma.length());
    	
    	chamberPos.push_back(ChPos);
    }
    
    // Getting 3D digi histogram
    
    TH3D *digi3D = (TH3D*)infile->Get("gemcrValidation/digiStrips");
    
    // Digi plots per chamber
	
	TH2D *digi2D[30];
	
	for (int ch=0; ch<30; ch++)
	{
    	sprintf(name,"Digi_ch_%u",ch);
    	digi2D[ch] = new TH2D(name,"",384,0,384,8,0,8);
    	
		for (int eta=0; eta<8; eta++)
		{
			for (int phi=0; phi<384; phi++)
			{
				digi2D[ch]->SetBinContent((phi+1),(eta+1),digi3D->GetBinContent(phi+1,eta+1,ch+1));
			}
		}
	}
	
	// Results for the 30 chambers
	
	TCanvas *Canvas = new TCanvas("Canvas","Canvas",0,0,1000,1000);
	TF1 *target97 = new TF1("target97","0.97",0,24);
	target97->SetLineColor(kRed);
    
    string namename = "";
    ofstream outfile;
    
    for (int i=0; i<chamberPos.size(); i++)
	{
		int c = chamberPos[i];
		
		// Plot efficiency per chamber
		
		namename = "Efficiency_" + chamberName[i] + "_in_position_" + to_string(chamberPos[i]);
		eff1D[c]->SetTitle(namename.c_str());
		eff1D[c]->GetXaxis()->SetTitle("VFAT");
		eff1D[c]->GetYaxis()->SetTitle("Efficiency");
		eff1D[c]->GetYaxis()->SetRangeUser(0.7,1.0);
		eff1D[c]->SetMarkerStyle(20);
		eff1D[c]->Draw();
		eff1D[c]->Write(namename.c_str());
		target97->Draw("SAME");
		namename = "Efficiency_Ch_Pos_" + to_string(chamberPos[i]) + ".png";
		Canvas->SaveAs(namename.c_str());
		Canvas->Clear();
		
		// Efficiency results in csv files
		
		string outFileName = "Efficiency_Ch_Pos_" + to_string(chamberPos[i]) + ".csv";
		outfile.open(outFileName);
		double eff_value, error_value;
		string entry;				
		for (int vfat=0; vfat<24; vfat++)
		{
			eff_value = eff1D[c]->GetY()[vfat];
			error_value = (eff1D[c]->GetEYhigh()[vfat] + eff1D[c]->GetEYlow()[vfat]) / 2.0;
			entry = chamberName[i] + "," + to_string(vfat) + "," + to_string(eff_value) + "," + to_string(error_value) + "," + to_string(run) + "\n";
			outfile << entry;
		}
		outfile.close();
		
		// Plotting digi per chamber
		namename = "Digi_" + chamberName[i] + "_in_position_" + to_string(chamberPos[i]);
		digi2D[c]->SetTitle(namename.c_str());
		digi2D[c]->GetXaxis()->SetTitle("Strip Number");
		digi2D[c]->GetYaxis()->SetTitle("ieta");
		digi2D[c]->Draw("colz");
		digi2D[c]->Write(namename.c_str());
		namename = "Digi_Ch_Pos_" + to_string(chamberPos[i]) + ".png";
		Canvas->SaveAs(namename.c_str());
		Canvas->Clear();
	}
	
	standConfigFile.close();
	infile->Close();
}
