/****************************************************************************

****************************************************************************/

#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/PluginManager/interface/ModuleDef.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"


#include "DQMServices/Core/interface/DQMEDAnalyzer.h"
#include "DQMServices/Core/interface/DQMStore.h"
#include "DQMServices/Core/interface/MonitorElement.h"

#include "DataFormats/GEMRecHit/interface/GEMRecHit.h"
#include "DataFormats/GEMRecHit/interface/GEMRecHitCollection.h"
#include "DataFormats/GEMDigi/interface/GEMDigiCollection.h"


#include "Geometry/GEMGeometry/interface/GEMGeometry.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"


#include <string>

//----------------------------------------------------------------------------------------------------
 
class GEMDQMSourceDigi: public DQMEDAnalyzer
{
  public:
    GEMDQMSourceDigi(const edm::ParameterSet& cfg);
    virtual ~GEMDQMSourceDigi();
  
  protected:
    void dqmBeginRun(edm::Run const &, edm::EventSetup const &) override;
    void bookHistograms(DQMStore::IBooker &, edm::Run const &, edm::EventSetup const &) override;
    void analyze(edm::Event const& e, edm::EventSetup const& eSetup) override;
    void beginLuminosityBlock(edm::LuminosityBlock const& lumi, edm::EventSetup const& eSetup) override;
    void endLuminosityBlock(edm::LuminosityBlock const& lumi, edm::EventSetup const& eSetup) override;
    void endRun(edm::Run const& run, edm::EventSetup const& eSetup) override;

  private:
    unsigned int verbosity;
   
    int nCh;


    edm::EDGetToken tagDigi;

    const GEMGeometry* initGeometry(edm::EventSetup const & iSetup);
    int findVFAT(float min_, float max_, float x_, int roll_);
     
    const GEMGeometry* GEMGeometry_; 

    std::vector<GEMChamber> gemChambers;

    std::unordered_map<UInt_t,  MonitorElement*> Digi_Strip_vs_eta;

};

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

using namespace std;
using namespace edm;

int GEMDQMSourceDigi::findVFAT(float min_, float max_, float x_, int roll_) {
  float step = abs(max_-min_)/3.0;
  if ( x_ < (min(min_,max_)+step) ) { return 8 - roll_;}
  else if ( x_ < (min(min_,max_)+2.0*step) ) { return 16 - roll_;}
  else { return 24 - roll_;}
}

const GEMGeometry* GEMDQMSourceDigi::initGeometry(edm::EventSetup const & iSetup) {
  const GEMGeometry* GEMGeometry_ = nullptr;
  try {
    edm::ESHandle<GEMGeometry> hGeom;
    iSetup.get<MuonGeometryRecord>().get(hGeom);
    GEMGeometry_ = &*hGeom;
  }
  catch( edm::eventsetup::NoProxyException<GEMGeometry>& e) {
    edm::LogError("MuonGEMBaseValidation") << "+++ Error : GEM geometry is unavailable on event loop. +++\n";
    return nullptr;
  }

  return GEMGeometry_;
}


//----------------------------------------------------------------------------------------------------
GEMDQMSourceDigi::GEMDQMSourceDigi(const edm::ParameterSet& cfg)
{

  tagDigi = consumes<GEMDigiCollection>(cfg.getParameter<edm::InputTag>("digisInputLabel")); 

}

//----------------------------------------------------------------------------------------------------

GEMDQMSourceDigi::~GEMDQMSourceDigi()
{
}

//----------------------------------------------------------------------------------------------------

void GEMDQMSourceDigi::dqmBeginRun(edm::Run const &, edm::EventSetup const &)
{
}

//----------------------------------------------------------------------------------------------------

void GEMDQMSourceDigi::bookHistograms(DQMStore::IBooker &ibooker, edm::Run const &, edm::EventSetup const & iSetup)
{

  GEMGeometry_ = initGeometry(iSetup);
  if ( GEMGeometry_ == nullptr) return ;  

  const std::vector<const GEMSuperChamber*>& superChambers_ = GEMGeometry_->superChambers();   
  for (auto sch : superChambers_){
    int n_lay = sch->nChambers();
    for (int l=0;l<n_lay;l++){
      gemChambers.push_back(*sch->chamber(l+1));
    }
  }
  nCh = gemChambers.size();
  ibooker.cd();
  ibooker.setCurrentFolder("GEM/digi");
  for (auto ch : gemChambers){
    GEMDetId gid = ch.id();
    string hName_digi = "Digi_Strips_Gemini_"+to_string(gid.superChamberId())+"_la_"+to_string(gid.layer());
    string hTitle_digi = "Digi Strip Gemini ID : "+to_string(gid.superChamberId())+", layer : "+to_string(gid.layer());
//     string hName_digi = "digi_"+to_string(gid.chamber());
//     string hTitle_digi = "digi "+to_string(gid.chamber());
    Digi_Strip_vs_eta[ ch.id() ] = ibooker.book2D(hName_digi, hTitle_digi, 384, 0.5, 384.5, 8, 0.5,8.5);
    TH2F *hist_3 = Digi_Strip_vs_eta[ ch.id() ]->getTH2F();
    hist_3->SetMarkerStyle(20);
    hist_3->SetMarkerSize(0.5);
  }
}

//----------------------------------------------------------------------------------------------------

void GEMDQMSourceDigi::beginLuminosityBlock(edm::LuminosityBlock const& lumiSeg, 
                                            edm::EventSetup const& context) 
{
}

//----------------------------------------------------------------------------------------------------

void GEMDQMSourceDigi::analyze(edm::Event const& event, edm::EventSetup const& eventSetup)
{
  const GEMGeometry* GEMGeometry_  = initGeometry(eventSetup);
  if ( GEMGeometry_ == nullptr) return; 

  ////////////////
  ///// Digi /////
  ////////////////
  edm::Handle<GEMDigiCollection> gemDigis;
  event.getByToken( this->tagDigi, gemDigis);
//   if (!gemDigis.isValid()){
//   		edm::LogError("GEMDQMSourceDigi") << "GEM Digi is not valid.\n";
//   		return;
//   }
  for (auto ch : gemChambers){
    GEMDetId cId = ch.id();
    for(auto roll : ch.etaPartitions()){
      GEMDetId rId = roll->id();      
      const auto& digis_in_det = gemDigis->get(rId);
      for (auto d = digis_in_det.first; d != digis_in_det.second; ++d){
      		Digi_Strip_vs_eta[ cId ]->Fill(d->strip(), rId.roll());
//       		std::cout<<d->strip()<<std::endl;
      }
    }
  }
  
}

//----------------------------------------------------------------------------------------------------

void GEMDQMSourceDigi::endLuminosityBlock(edm::LuminosityBlock const& lumiSeg, edm::EventSetup const& eSetup) 
{
}

//----------------------------------------------------------------------------------------------------

void GEMDQMSourceDigi::endRun(edm::Run const& run, edm::EventSetup const& eSetup)
{
}

//----------------------------------------------------------------------------------------------------

DEFINE_FWK_MODULE(GEMDQMSourceDigi);
