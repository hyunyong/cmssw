import ROOT, numpy
ROOT.gROOT.SetBatch(1)
ROOT.gStyle.SetOptStat(0)

def getAPE(tr):
  apeDT = [[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[]]
  apeCSC = [[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[]]
  for e in tr:
    if e.isDT:
      apeDT[0].append(e.xx)
      apeDT[1].append(e.xy)
      apeDT[2].append(e.xz)
      apeDT[3].append(e.xa)
      apeDT[4].append(e.xb)
      apeDT[5].append(e.xc)
      apeDT[6].append(e.yy)
      apeDT[7].append(e.yz)
      apeDT[8].append(e.ya)
      apeDT[9].append(e.yb)
      apeDT[10].append(e.yc)
      apeDT[11].append(e.zz)
      apeDT[12].append(e.za)
      apeDT[13].append(e.zb)
      apeDT[14].append(e.zc)
      apeDT[15].append(e.aa)
      apeDT[16].append(e.ab)
      apeDT[17].append(e.ac)
      apeDT[18].append(e.bb)
      apeDT[19].append(e.bc)
      apeDT[20].append(e.cc)
    if e.isCSC:
      apeCSC[0].append(e.xx)
      apeCSC[1].append(e.xy)
      apeCSC[2].append(e.xz)
      apeCSC[3].append(e.xa)
      apeCSC[4].append(e.xb)
      apeCSC[5].append(e.xc)
      apeCSC[6].append(e.yy)
      apeCSC[7].append(e.yz)
      apeCSC[8].append(e.ya)
      apeCSC[9].append(e.yb)
      apeCSC[10].append(e.yc)
      apeCSC[11].append(e.zz)
      apeCSC[12].append(e.za)
      apeCSC[13].append(e.zb)
      apeCSC[14].append(e.zc)
      apeCSC[15].append(e.aa)
      apeCSC[16].append(e.ab)
      apeCSC[17].append(e.ac)
      apeCSC[18].append(e.bb)
      apeCSC[19].append(e.bc)
      apeCSC[20].append(e.cc)
  
  return apeDT, apeCSC

dataF = ROOT.TFile("APE_DATA.root")
mcF = ROOT.TFile("APE_MC.root")
dataTr = dataF.Get("theTree")
mcTr = mcF.Get("theTree")
apesData = getAPE(dataTr)
apesMC = getAPE(mcTr)
hData = ROOT.TH1D("APEsData","APEs",21,0,21)
hMC = ROOT.TH1D("APEsMC","APEs",21,0,21)
xLabel = ["xx", "xy", "xz", "xa", "xb", "xc", "yy", "yz", "ya", "yb", "yc", "zz", "za", "zb", "zc", "aa", "ab", "ac", "bb", "bc", "cc"]
for i, x in enumerate(xLabel):
  hData.GetXaxis().SetBinLabel(i+1,x)
  hMC.GetXaxis().SetBinLabel(i+1,x)

hData.SetMarkerStyle(20)
hMC.SetMarkerStyle(26)
hData.SetMarkerColor(ROOT.kBlack)
hData.SetLineColor(ROOT.kBlack)
hMC.SetMarkerColor(ROOT.kRed)
hMC.SetLineColor(ROOT.kRed)
hData.SetMaximum(0.1)
hData.SetMinimum(0.000000001)

c = ROOT.TCanvas("","",800,600)
c.SetLogy(1)
for detLoop in [[0,"DT"],[1,"CSC"]]:
  iType = detLoop[0]
  DET = detLoop[1]

  for i in range(21):
    hData.SetBinContent(i+1,abs(numpy.mean(apesData[iType][i])))
    hMC.SetBinContent(i+1,abs(numpy.mean(apesMC[iType][i])))
    hData.SetBinError(i+1,abs(numpy.std(apesData[iType][i])))
    hMC.SetBinError(i+1,abs(numpy.std(apesMC[iType][i])))
    print numpy.mean(apesData[1][i]), numpy.mean(apesMC[iType][i])
  hData.Draw("p E1")
  hMC.Draw("p E1 same")
  le = ROOT.TLegend(0.7,0.8,0.9,0.88)
  le.SetBorderSize(0)
  le.AddEntry(hData, "Data {} 2017".format(DET))
  le.AddEntry(hMC, "MC {} 2017".format(DET))
  le.Draw()
  c.SaveAs("APEs{}.png".format(DET))
