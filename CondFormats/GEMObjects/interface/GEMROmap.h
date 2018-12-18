#ifndef CondFormats_GEMObjects_GEMROmap_h
#define CondFormats_GEMObjects_GEMROmap_h
#include "DataFormats/MuonDetId/interface/GEMDetId.h"
#include <map>

class GEMROmap{
 public:
  
  struct eCoord{
    unsigned int fedId;
    uint8_t amcNum;
    uint8_t gebId;
    bool operator < (const eCoord& r) const{
      if (fedId == r.fedId){
        if ( amcNum == r.amcNum){
          return gebId < r.gebId;
        }
	else{
          return amcNum < r.amcNum;
	}
      }
      else{
	return fedId < r.fedId;
      }
    }
  };
  
  struct dCoord{
    GEMDetId gemDetId;
    int vfatVer;
    int chamberType;
    bool operator < (const dCoord& r) const{
      if (gemDetId == r.gemDetId){
        if (vfatVer == r.vfatVer){
	  return chamberType < r.chamberType;
        }
	else{
          return vfatVer < r.vfatVer;
        }
      }
      else{
	return gemDetId < r.gemDetId;
      }
    }
  };


  struct vfatEC{
    uint16_t vfatAdd;
    int vfatVer;
    int chamberType;
    bool operator < (const vfatEC& r) const{
      if (vfatAdd == r.vfatAdd){
        if (vfatVer  == r.vfatVer){
          return chamberType < r.chamberType;  
        }
        else{
          return vfatVer < r.vfatVer;
        }
      }
      else{
        return vfatAdd < r.vfatAdd;
      }
    }
  };

  struct vfatDC{
    int vfatType;
    int vfatPos;
    int iEta;
    int localPhi;
    bool operator < (const vfatDC& r)  const{
      if (vfatType == r.vfatType){
        if (vfatPos == r.vfatPos){
          if (iEta == r.iEta){
            return localPhi < r.localPhi;
          }
          else{
            return iEta < r.iEta;
          }
        }
        else{
          return vfatPos < r.vfatPos;
        }
      }
      else{
        return vfatType < r.vfatType;
      }
    }
  };

  struct channelNum{
    int vfatType;
    int chNum;
    bool operator < (const channelNum& c) const{
      if (vfatType == c.vfatType)
        return chNum < c.chNum;
      else
        return vfatType < c.vfatType;
    }
  };

  struct stripNum{
    int vfatType;
    int stNum;
    bool operator < (const stripNum& s) const{
      if (vfatType == s.vfatType) 
        return stNum < s.stNum;
      else
        return vfatType < s.vfatType;
    }
  };

  GEMROmap(){};
  
  bool isValidChipID(const eCoord& r) const {
    return roMapED_.find(r) != roMapED_.end();
  }
  const dCoord& hitPosition(const eCoord& r) const {return roMapED_.at(r);}
  const eCoord& hitPosition(const dCoord& r) const {return roMapDE_.at(r);}
  
  void add(eCoord e,dCoord d) {roMapED_[e]=d;}
  void add(dCoord d,eCoord e) {roMapDE_[d]=e;}
  
  const std::map<eCoord, dCoord> * getRoMap() const {return &roMapED_;}

  void add(vfatEC e,vfatDC d) {vMapED_[e]=d;}
  void add(vfatDC d,vfatEC e) {vMapDE_[d]=e;}

  const vfatDC& hitPosition(const vfatEC& r) const {return vMapED_.at(r);}
  const vfatEC& hitPosition(const vfatDC& r) const {return vMapDE_.at(r);}

  void add(channelNum c, stripNum s) {chStMap_[c]=s;} 
  void add(stripNum s, channelNum c) {stChMap_[s]=c;} 
 
  const channelNum& hitPosition(const stripNum& s) const {return stChMap_.at(s);}
  const stripNum& hitPosition(const channelNum& c) const {return chStMap_.at(c);}

 private:
  std::map<eCoord,dCoord> roMapED_;
  std::map<dCoord,eCoord> roMapDE_;

  std::map<vfatEC, vfatDC> vMapED_;
  std::map<vfatDC, vfatEC> vMapDE_;

  std::map<channelNum, stripNum> chStMap_;
  std::map<stripNum, channelNum> stChMap_;
  
};
#endif
