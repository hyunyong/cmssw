#include "Rivet/Analysis.hh"
#include "Rivet/Projections/FinalState.hh"
#include "Rivet/Projections/FastJets.hh"
#include <vector>
#include <string>


using namespace std;
namespace Rivet {


  class jetShape : public Analysis {
  public:

    jetShape()
      : Analysis("jetShape")
    {    }
    float jr = 0.4; 
    float pT1Cut = 200.0;
    int drBin = int((1.5-jr-0.1)*10.0);
    void init() {
      const FastJets jets(FinalState(-10, 10, 0.0*GeV), FastJets::ANTIKT, jr);
      addProjection(jets, "Jets");

      std::vector<double> ptrBin = {0.1,0.2,0.3,0.4,0.6,0.9};
      _h1 = bookHisto1D("non_eta_high_pt_sdr_pt3_rivet_jet3_pt_jet2_pt", ptrBin);
      _h2 = bookHisto1D("non_eta_high_pt_ldr_pt3_rivet_jet3_pt_jet2_pt", ptrBin);
      _h3 = bookHisto1D("non_eta_high_pt_dr_lpt3_rivet_del_r23", drBin,jr+0.1,1.5);
      _h4 = bookHisto1D("non_eta_high_pt_dr_hpt3_rivet_del_r23", drBin,jr+0.1,1.5);
    }
    void analyze(const Event& event) {
      const Jets& jets = applyProjection<FastJets>(event, "Jets").jetsByPt(30.0*GeV);
      if (jets.size() != 2) vetoEvent;
      const FourMomentum jet1 = jets[0].momentum();
      const FourMomentum jet2 = jets[1].momentum();
      cout << jetE(jets[0]) << ", " << jetE(jets[1]) << endl;
      if (jet1.pT() < pT1Cut*GeV) vetoEvent;
      /*
      double del_phi12 = mapAngleMPiToPi(jet2.phi() - jet1.phi());
      if (abs(abs(del_phi12) - M_PI) > 1.0) vetoEvent;

      double del_phi23 = mapAngleMPiToPi(jet3.phi() -  jet2.phi());
      double del_eta23 = sign(jet2.rapidity())*(jet3.rapidity() - jet2.rapidity());
      const double del_r23 = add_quad(del_eta23, del_phi23);
      if (!inRange(del_r23, jr+0.1, 1.5)) vetoEvent;
       

      double w = event.weight();
           if (inRange(jet3_pt_jet2_pt, 0.6, 0.9)) {
            _h4->fill(del_r23, w);
            if (tester23) co9++;
            if (tester13) co10++;
          }
    */
    }
    void finalize() {
    }

  private:

    Histo1DPtr  _h1;
    Histo1DPtr  _h2;
    Histo1DPtr  _h3;
    Histo1DPtr  _h4;
    double jetE(const Jet& jet){
      double eta_ = jet.momentum().eta();
      double phi_ = jet.momentum().phi();
      const Particles& jetConst = jet.constituents();
      double theta_ = 0.0;
      double sumE = 0.0;
      for (auto p:jetConst){
        theta_ += cos(2.0*atan2(mapAngleMPiToPi(p.phi() - phi_), sign(eta_)*(p.eta() - eta_)));
        sumE += p.E();
      }     
      double v2 = acos(theta_/jetConst.size())/2.0;
      double xb = 0.0;
      double xxb = 0.0;
      double yb = 0.0;
      double yyb = 0.0;
      for (auto p:jetConst){
        double y_ = mapAngleMPiToPi(p.phi() - phi_);
        double x_ = sign(eta_)*(p.eta() - eta_);
        double xp = cos(v2)*x_ + sin(v2) *y_;
        double yp = -sin(v2)*x_ + cos(v2)*y_;
        xb +=  abs(xp);
        xxb += xp*xp*p.E();
        yb += abs(yp);
        yyb += yp*yp*p.E();
      }
      return v2/3.141592*180.0;
      //int n_ = (int)jetConst.size();
      //return sqrt(yyb/sumE/jetConst.size() - (yb/sumE/jetConst.size())*(yb/sumE/jetConst.size()))/sqrt(xxb/sumE/jetConst.size() - (xb/sumE/jetConst.size())*(xb/sumE/jetConst.size()));
    }
  };

  DECLARE_RIVET_PLUGIN(jetShape);

}
