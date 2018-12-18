#include "EventFilter/GEMRawToDigi/interface/VFATdata.h"

using namespace gem;

VFATdata::VFATdata() {
  ver_ = 1;
}    

VFATdata::VFATdata(const int vfatType,
                   const uint16_t BC,
		   const uint8_t EC,
		   const uint16_t chipID,
		   const uint64_t lsDatas,
		   const uint64_t msDatas)
{
  // this constructor only used for packing sim digis
  fw_.word = 0;
  sw_.word = 0;
  tw_.word = 0;  
  fw_.header = 0x1E;

  if (vfatType == 3) {
    fw_.bc = BC;
    fw_.ec = EC;
    fw_.pos = chipID;
  }
  else {
    fw_.chipID = chipID;
    fw_.b1110 = 14;
    fw_.b1100 = 12;
    fw_.b1010 = 10;
    fw_.ecV2  = EC;
    fw_.bcV2  = BC;    
  }
  
  sw_.lsData1 = lsDatas >> 48;
  tw_.lsData2 = lsDatas & 0x0000ffffffffffff;
  
  fw_.msData1 = msDatas >> 48;
  sw_.msData2 = msDatas & 0x0000ffffffffffff;
  ver_ = vfatType;
  
  tw_.crc = checkCRC();// crc check not yet implemented for v3
}

uint8_t VFATdata::quality() {  
  uint8_t q = 0;  
  if (ver_ == 2) {
    if (tw_.crc != checkCRC()) q |= 1UL << 1;
    if (fw_.b1010 != 10) q |= 1UL << 2;
    if (fw_.b1100 != 12) q |= 1UL << 3;
    if (fw_.b1110 != 14) q |= 1UL << 4;
  }
  // quality test not yet implemented in v3  
  return q;
}

uint16_t VFATdata::crc_cal(uint16_t crc_in, uint16_t dato)
{
  uint16_t v = 0x0001;
  uint16_t mask = 0x0001;
  uint16_t d=0x0000;
  uint16_t crc_temp = crc_in;
  unsigned char datalen = 16;
  for (int i=0; i<datalen; i++) {
    if (dato & v) d = 0x0001;
    else d = 0x0000;
    if ((crc_temp & mask)^d) crc_temp = crc_temp>>1 ^ 0x8408;
    else crc_temp = crc_temp>>1;
    v<<=1;
  }
  return crc_temp;
}

uint16_t VFATdata::checkCRC()
{
  uint16_t vfatBlockWords[12];
  vfatBlockWords[11] = ((0x000f & fw_.b1010)<<12) | bc();
  vfatBlockWords[10] = ((0x000f & fw_.b1100)<<12) | ((0x00ff & ec()) <<4) | (0x000f & fw_.flag);
  vfatBlockWords[9]  = ((0x000f & fw_.b1110)<<12) | fw_.chipID;
  vfatBlockWords[8]  = (0xffff000000000000 & msData()) >> 48;
  vfatBlockWords[7]  = (0x0000ffff00000000 & msData()) >> 32;
  vfatBlockWords[6]  = (0x00000000ffff0000 & msData()) >> 16;
  vfatBlockWords[5]  = (0x000000000000ffff & msData());
  vfatBlockWords[4]  = (0xffff000000000000 & lsData()) >> 48;
  vfatBlockWords[3]  = (0x0000ffff00000000 & lsData()) >> 32;
  vfatBlockWords[2]  = (0x00000000ffff0000 & lsData()) >> 16;
  vfatBlockWords[1]  = (0x000000000000ffff & lsData());

  uint16_t crc_fin = 0xffff;
  for (int i = 11; i >= 1; i--) {
    crc_fin = crc_cal(crc_fin, vfatBlockWords[i]);
  }
  return crc_fin;
}
