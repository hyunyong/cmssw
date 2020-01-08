#ifndef Geometry_GEMGeometry_GEMEtaPartitionSpecs_H
#define Geometry_GEMGeometry_GEMEtaPartitionSpecs_H

/** \class GEMEtaPartitionSpecs
 *  Storage of the parameters of the GEM Chamber
 *  using standard topologies
 *
 * \author M. Maggi - INFN Bari
 *
 */
#include <vector>
#include <string>

class GEMStripTopology;

#include "Geometry/CommonDetUnit/interface/GeomDetType.h"

class GEMEtaPartitionSpecs : public GeomDetType {
public:
  typedef std::vector<float> GEMSpecs;

  GEMEtaPartitionSpecs(SubDetector rss, const std::string& name, const GEMSpecs& pars);

  ~GEMEtaPartitionSpecs() override;

  const Topology& topology() const override;

  const GEMStripTopology& specificTopology() const;

  const Topology& padTopology() const;

  const GEMStripTopology& specificPadTopology() const;

  const std::string& detName() const;

  const GEMSpecs& parameters() const;

private:
  /// topology of strips
  GEMStripTopology* _top;

  /// topology of trigger pads (pad = bundle of strips, basically, a "fat" strip)
  GEMStripTopology* _top_pad;

  std::vector<float> _p;
  std::string _n;
};
#endif
