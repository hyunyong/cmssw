
#include "GeneratorInterface/GenFilters/interface/MCSingleParticleFilter.h"

#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"
#include <iostream>

using namespace edm;
using namespace std;


MCSingleParticleFilter::MCSingleParticleFilter(const edm::ParameterSet& iConfig) :
token_(consumes<edm::HepMCProduct>(edm::InputTag(iConfig.getUntrackedParameter("moduleLabel",std::string("generator")),"unsmeared"))),
betaBoost(iConfig.getUntrackedParameter("BetaBoost",0.))
{
   //here do whatever other initialization is needed
   vector<int> defpid ;
   defpid.push_back(0) ;
   particleID = iConfig.getUntrackedParameter< vector<int> >("ParticleID",defpid);  
   vector<double> defptmin ;
   defptmin.push_back(0.);
   ptMin = iConfig.getUntrackedParameter< vector<double> >("MinPt", defptmin);

   vector<double> defetamin ;
   defetamin.push_back(-10.);
   etaMin = iConfig.getUntrackedParameter< vector<double> >("MinEta", defetamin);
   vector<double> defetamax ;
   defetamax.push_back(10.);
   etaMax = iConfig.getUntrackedParameter< vector<double> >("MaxEta", defetamax);
   vector<int> defstat ;
   defstat.push_back(0);
   status = iConfig.getUntrackedParameter< vector<int> >("Status", defstat);


    // check for same size
    if ( (ptMin.size() > 1 &&  particleID.size() != ptMin.size()) 
     ||  (etaMin.size() > 1 && particleID.size() != etaMin.size()) 
     ||  (etaMax.size() > 1 && particleID.size() != etaMax.size())
     ||  (status.size() > 1 && particleID.size() != status.size()) ) {
      cout << "WARNING: MCPROCESSFILTER : size of MinPthat and/or MaxPthat not matching with ProcessID size!!" << endl;
    }

    // if ptMin size smaller than particleID , fill up further with defaults
    if (particleID.size() > ptMin.size() ){
       vector<double> defptmin2 ;
       for (unsigned int i = 0; i < particleID.size(); i++){ defptmin2.push_back(0.);}
       ptMin = defptmin2;   
    } 
    // if etaMin size smaller than particleID , fill up further with defaults
    if (particleID.size() > etaMin.size() ){
       vector<double> defetamin2 ;
       for (unsigned int i = 0; i < particleID.size(); i++){ defetamin2.push_back(-10.);}
       etaMin = defetamin2;   
    } 
    // if etaMax size smaller than particleID , fill up further with defaults
    if (particleID.size() > etaMax.size() ){
       vector<double> defetamax2 ;
       for (unsigned int i = 0; i < particleID.size(); i++){ defetamax2.push_back(10.);}
       etaMax = defetamax2;   
    }     
    // if status size smaller than particleID , fill up further with defaults
    if (particleID.size() > status.size() ){
       vector<int> defstat2 ;
       for (unsigned int i = 0; i < particleID.size(); i++){ defstat2.push_back(0);}
       status = defstat2;   
    } 

    // check if beta is smaller than 1
    if (std::abs(betaBoost) >= 1 ){
      edm::LogError("MCSingleParticleFilter") << "Input beta boost is >= 1 !";
    }

}


MCSingleParticleFilter::~MCSingleParticleFilter()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


// ------------ method called to skim the data  ------------
bool MCSingleParticleFilter::filter(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;
   bool accepted = false;
   Handle<HepMCProduct> evt;
   iEvent.getByToken(token_, evt);

   const HepMC::GenEvent * myGenEvent = evt->GetEvent();
     
   
   for ( HepMC::GenEvent::particle_const_iterator p = myGenEvent->particles_begin();
	 p != myGenEvent->particles_end(); ++p ) {
     
    
     for (unsigned int i = 0; i < particleID.size(); i++){
       if (particleID[i] == (*p)->pdg_id() || particleID[i] == 0) {
    
	 if ( (*p)->momentum().perp() > ptMin[i]
	      && ((*p)->status() == status[i] || status[i] == 0)) {

           HepMC::FourVector mom = zboost((*p)->momentum());
           if ( mom.eta() > etaMin[i] && mom.eta() < etaMax[i] ) {
             accepted = true;
           }

         }
	 
       } 
     }
     

   }
   
   if (accepted){ return true; } else {return false;}
   
}


HepMC::FourVector MCSingleParticleFilter::zboost(const HepMC::FourVector& mom) {
   //Boost this Lorentz vector (from TLorentzVector::Boost)
   double b2 = betaBoost*betaBoost;
   double gamma = 1.0 / sqrt(1.0 - b2);
   double bp = betaBoost*mom.pz();
   double gamma2 = b2 > 0 ? (gamma - 1.0)/b2 : 0.0;

   return HepMC::FourVector(mom.px(), mom.py(), mom.pz() + gamma2*bp*betaBoost + gamma*betaBoost*mom.e(), gamma*(mom.e()+bp));
}

