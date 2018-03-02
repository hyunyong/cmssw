#include "CondTools/GEM/interface/GEMEMapSourceHandler.h"
#include "CondCore/CondDB/interface/ConnectionPool.h"
#include "CondCore/DBOutputService/interface/PoolDBOutputService.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "RelationalAccess/ISessionProxy.h"
#include "RelationalAccess/ITransaction.h"
#include "RelationalAccess/ISchema.h"
#include "RelationalAccess/ITable.h"
#include "RelationalAccess/IQuery.h"
#include "RelationalAccess/ICursor.h"
#include "CoralBase/AttributeList.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeSpecification.h"
#include <TString.h>

#include <fstream>
#include <cstdlib>
#include <vector>

#include <DataFormats/MuonDetId/interface/GEMDetId.h>

popcon::GEMEMapSourceHandler::GEMEMapSourceHandler( const edm::ParameterSet& ps ):
  m_name( ps.getUntrackedParameter<std::string>( "name", "GEMEMapSourceHandler" ) ),
  m_dummy( ps.getUntrackedParameter<int>( "WriteDummy", 0 ) ),
  m_validate( ps.getUntrackedParameter<int>( "Validate", 1 ) ),
  m_connect( ps.getParameter<std::string>( "connect" ) ),
  m_connectionPset( ps.getParameter<edm::ParameterSet>( "DBParameters" ) )
{
}

popcon::GEMEMapSourceHandler::~GEMEMapSourceHandler()
{
}

void popcon::GEMEMapSourceHandler::getNewObjects()
{
  
  edm::LogInfo( "GEMEMapSourceHandler" ) << "[" << "GEMEMapSourceHandler::" << __func__ << "]:" << m_name << ": "
                                         << "BEGIN" << std::endl;
  
  edm::Service<cond::service::PoolDBOutputService> mydbservice;
  
  // first check what is already there in offline DB
  Ref payload;
  if(m_validate==1 && tagInfo().size>0) {
    payload = lastPayload();
  }
  
  // now construct new cabling map from online DB
  // FIXME: use boost::ptime
  time_t rawtime;
  time(&rawtime); //time since January 1, 1970
  tm * ptm = gmtime(&rawtime);//GMT time
  char buffer[20];
  strftime(buffer,20,"%d/%m/%Y_%H:%M:%S",ptm);
  std::string eMap_version( buffer );
  eMap =  new GEMELMap(eMap_version);
  
  std::string baseCMS = std::string(getenv("CMSSW_BASE"))+std::string("/src/CondTools/GEM/data/");  
  std::vector<std::string> mapfiles;

  mapfiles.push_back("vfatTypeList.csv");
  mapfiles.push_back("chMap.csv");
  // VFAT Postion Map 
  GEMELMap::GEMVFatMap vmtype;
  vmtype.VFATmapTypeId=1;
  std::string field, line;
  std::string filename(baseCMS+mapfiles[0]);
  std::ifstream maptype(filename.c_str());
  std::string buf("");
  while(std::getline(maptype, line)){
    int vfat_pos, z_dir, ieta, iphi, dep, vfat_type, sec;
    uint16_t vfat_add, amc_ID, geb_ID;
    std::stringstream ssline(line);
    getline( ssline, field, ',' );
    std::stringstream Sec(field);
    getline( ssline, field, ',' );
    std::stringstream Z_dir(field);
    getline( ssline, field, ',' );
    std::stringstream Ieta(field);
    getline( ssline, field, ',' );
    std::stringstream Iphi(field);
    getline( ssline, field, ',' );
    std::stringstream Dep(field);
    getline( ssline, field, ',' );
    std::stringstream Vfat_pos(field);
    getline( ssline, field, ',' );
    std::stringstream VFAT_type(field);
    getline( ssline, field, ',' );
    char* chr = strdup(field.c_str());
    std::cout << chr << std::endl;
    vfat_add = strtol(chr,NULL,16);
    getline( ssline, field, ',' );
    std::stringstream AMC_ID(field);
    getline( ssline, field, ',' );
    std::stringstream GEB_ID(field);

    Sec >> sec;Z_dir >> z_dir; Ieta >> ieta; Iphi >> iphi; Dep >> dep; Vfat_pos >> vfat_pos;
    VFAT_type >> vfat_type; AMC_ID >> amc_ID; GEB_ID >> geb_ID;
   
    std::cout<<" Sector="<<sec<<" z_direction="<<z_dir<<" ieta="<<ieta<<" iphi="<<iphi<<" depth="<<dep<<" vfat position="<<vfat_pos<<" vfat address = " << vfat_add << " vfat type = " << vfat_type << " amc ID = " << amc_ID << " geb ID = " << geb_ID << std::endl;

    vmtype.sec.push_back(sec);
    vmtype.vfat_position.push_back(vfat_pos);
    vmtype.z_direction.push_back(z_dir);
    vmtype.iEta.push_back(ieta);
    vmtype.iPhi.push_back(iphi);
    vmtype.depth.push_back(dep);
    vmtype.vfatId.push_back(vfat_add);
    vmtype.vfatType.push_back(vfat_type);
    vmtype.amcId.push_back(amc_ID);
    vmtype.gebId.push_back(geb_ID);
  }
  eMap->theVFatMap_.push_back(vmtype);

  // VFAT Channel-Strip Map
  GEMELMap::GEMStripMap chStMap;
  std::string filename2(baseCMS+mapfiles[1]);
  std::ifstream maptype2(filename2.c_str());
  std::string buf2("");
  
  
  std::string field2, line2;
  while(std::getline(maptype2, line2)){
    
    int vfat_type, vfat_ch, vfat_strip ;
    std::stringstream ssline(line2);   
    getline( ssline, field, ',' );
    std::stringstream VFAT_type(field);
    getline( ssline, field, ',' );
    std::stringstream DET_strip(field);
    getline( ssline, field, ',' );
    std::stringstream VFAT_ch(field);
    VFAT_type >> vfat_type; DET_strip >> vfat_strip; VFAT_ch >> vfat_ch;
    
    std::cout << "vfat type = " << vfat_type << " vfat strip = " << vfat_strip << " vfat channel = " << vfat_ch << std::endl;  

    chStMap.vfatType.push_back(vfat_type);
    chStMap.vfatStrip.push_back(vfat_strip);
    chStMap.vfatCh.push_back(vfat_ch);
  }
  eMap->theStripMap_.push_back(chStMap); 
    
  cond::Time_t snc = mydbservice->currentTime();  
  // look for recent changes
  int difference=1;
  if (difference==1) {
    m_to_transfer.push_back(std::make_pair((GEMELMap*)eMap,snc));
  }
}

// // additional work (I added these two functions: ConnectOnlineDB and DisconnectOnlineDB)
void popcon::GEMEMapSourceHandler::ConnectOnlineDB( const std::string& connect, const edm::ParameterSet& connectionPset )
{
  cond::persistency::ConnectionPool connection;
  connection.setParameters( connectionPset );
  connection.configure();
  session = connection.createSession( connect,true );
}

void popcon::GEMEMapSourceHandler::DisconnectOnlineDB()
{
  session.close();
}
