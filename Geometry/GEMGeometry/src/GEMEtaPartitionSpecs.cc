#include "Geometry/GEMGeometry/interface/GEMEtaPartitionSpecs.h"
//#include "Geometry/CommonTopologies/interface/RectangularStripTopology.h"
#include "Geometry/CommonTopologies/interface/GEMStripTopology.h"

using namespace GeomDetEnumerators;

GEMEtaPartitionSpecs::GEMEtaPartitionSpecs(SubDetector rss, const std::string& name, const GEMSpecs& pars)
    : GeomDetType(name, rss), _p(pars), _n(name) {
  if (rss == GEM) {
    float b = _p[0];
    float B = _p[1];
    float h = _p[2];
    float r0 = h * (B + b) / (B - b);
    float striplength = h * 2;
    float strips = _p[3];
    //float pitch = (b + B) / strips;
    int nstrip = static_cast<int>(strips);
    float phiPitch = _p[5] / strips;
    _top = new GEMStripTopology(nstrip, phiPitch, striplength, r0);

    float pads = _p[4];
    //float pad_pitch = (b + B) / pads;
    int npad = static_cast<int>(pads);
    float padPhiPitch = _p[5] / pads;
    _top_pad = new GEMStripTopology(npad, padPhiPitch, striplength, r0);
  } else {
    _top = nullptr;
    _top_pad = nullptr;
  }
}

GEMEtaPartitionSpecs::~GEMEtaPartitionSpecs() {
  if (_top)
    delete _top;
  if (_top_pad)
    delete _top_pad;
}

const Topology& GEMEtaPartitionSpecs::topology() const { return *_top; }

const GEMStripTopology& GEMEtaPartitionSpecs::specificTopology() const { return *_top; }

const Topology& GEMEtaPartitionSpecs::padTopology() const { return *_top_pad; }

const GEMStripTopology& GEMEtaPartitionSpecs::specificPadTopology() const { return *_top_pad; }

const std::string& GEMEtaPartitionSpecs::detName() const { return _n; }

const std::vector<float>& GEMEtaPartitionSpecs::parameters() const { return _p; }
