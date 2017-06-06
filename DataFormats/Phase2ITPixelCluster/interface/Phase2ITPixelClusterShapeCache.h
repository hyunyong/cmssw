// -*- c++ -*-
#ifndef DataFormats_Phase2ITPixelCluster_Phase2ITPixelClusterShapeData_h
#define DataFormats_Phase2ITPixelCluster_Phase2ITPixelClusterShapeData_h

#include "DataFormats/Provenance/interface/ProductID.h"
#include "DataFormats/Common/interface/HandleBase.h"
#include "DataFormats/Common/interface/Ref.h"
#include "DataFormats/Common/interface/DetSetVectorNew.h"
#include "DataFormats/Phase2ITPixelCluster/interface/Phase2ITPixelCluster.h"

#include <utility>
#include <vector>
#include <algorithm>
#include <cassert>
#include <memory>

class PixelGeomDetUnit;

class Phase2ITPixelClusterShapeData {
public:
  typedef std::vector<std::pair<int, int> >::const_iterator const_iterator;
  typedef std::pair<const_iterator, const_iterator> Range;
  Phase2ITPixelClusterShapeData(const_iterator begin, const_iterator end, bool isStraight, bool isComplete, bool hasBigPixelsOnlyInside):
    begin_(begin), end_(end), isStraight_(isStraight), isComplete_(isComplete), hasBigPixelsOnlyInside_(hasBigPixelsOnlyInside)
  {}
  ~Phase2ITPixelClusterShapeData();

  Range size() const { return std::make_pair(begin_, end_); }

  bool isStraight() const { return isStraight_; }
  bool isComplete() const { return isComplete_; }
  bool hasBigPixelsOnlyInside() const { return hasBigPixelsOnlyInside_; }

private:
  const_iterator begin_, end_;
  const bool isStraight_, isComplete_, hasBigPixelsOnlyInside_;
};

class Phase2ITPixelClusterShapeCache {
public:
  typedef edm::Ref<edmNew::DetSetVector<Phase2ITPixelCluster>, Phase2ITPixelCluster> ClusterRef;

  struct Field {
    Field(): offset(0), size(0), straight(false), complete(false), has(false), filled(false) {}

    Field(unsigned off, unsigned siz, bool s, bool c, bool h):
      offset(off), size(siz), straight(s), complete(c), has(h), filled(true) {}
    unsigned offset: 24; // room for 2^24/9 = ~2.8e6 clusters, should be enough
    unsigned size: 4; // max 9 elements / cluster (2^4=16)
    unsigned straight:1;
    unsigned complete:1;
    unsigned has:1;
    unsigned filled:1;
  };

  Phase2ITPixelClusterShapeCache() {};
  explicit Phase2ITPixelClusterShapeCache(const edm::HandleBase& handle): productId_(handle.id()) {}
  explicit Phase2ITPixelClusterShapeCache(const edm::ProductID& id): productId_(id) {}
  ~Phase2ITPixelClusterShapeCache();

  void resize(size_t size) {
    data_.resize(size);
    sizeData_.reserve(size);
  }

  void swap(Phase2ITPixelClusterShapeCache& other) {
    data_.swap(other.data_);
    sizeData_.swap(other.sizeData_);
    std::swap(productId_, other.productId_);
  }

#if !defined(__CINT__) && !defined(__MAKECINT__) && !defined(__REFLEX__)
  void shrink_to_fit() {
    data_.shrink_to_fit();
    sizeData_.shrink_to_fit();
  }

  template <typename T>
  void insert(const ClusterRef& cluster, const T& data) {
    static_assert(T::ArrayType::capacity() <= 16, "T::ArrayType::capacity() more than 16, bit field too narrow");
    checkRef(cluster);

    data_[cluster.index()] = Field(sizeData_.size(), data.size.size(), data.isStraight, data.isComplete, data.hasBigPixelsOnlyInside);
    std::copy(data.size.begin(), data.size.end(), std::back_inserter(sizeData_));
  }

  bool isFilled(const ClusterRef& cluster) const {
    checkRef(cluster);
    return data_[cluster.index()].filled;
  }

  Phase2ITPixelClusterShapeData get(const ClusterRef& cluster, const PixelGeomDetUnit *pixDet) const {
    checkRef(cluster);
    Field f = data_[cluster.index()];
    assert(f.filled);

    auto beg = sizeData_.begin()+f.offset;
    auto end = beg+f.size;

    return Phase2ITPixelClusterShapeData(beg, end, f.straight, f.complete, f.has);
  }
#endif

private:
  void checkRef(const ClusterRef& cluster) const;

  std::vector<Field> data_;
  std::vector<std::pair<int, int> > sizeData_;
  edm::ProductID productId_;
};

#endif
