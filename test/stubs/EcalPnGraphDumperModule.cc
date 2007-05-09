/**
 * \file EcalPnGraphDumperModule.h 
 * module dumping TGraph with 50 data frames from Pn Diodes
 *   
 * 
 * $Date: 2007/04/12 16:41:18 $
 * $Revision: 1.1 $
 * \author N. Amapane - S. Argiro'
 * \author G. Franzoni
 * \author J. Haupt
 *
 */

#include <FWCore/Framework/interface/EDAnalyzer.h>
#include <FWCore/Framework/interface/Event.h>
#include <FWCore/Framework/interface/MakerMacros.h>
#include <DataFormats/EcalDigi/interface/EcalDigiCollections.h>
#include <DataFormats/EcalDetId/interface/EcalDetIdCollections.h>

#include <DataFormats/EcalDigi/interface/EcalTriggerPrimitiveDigi.h>
#include <DataFormats/EcalDigi/interface/EcalTriggerPrimitiveSample.h>

#include <iostream>
#include <vector>

#include "TFile.h"
#include "TGraph.h"



class EcalPnGraphDumperModule: public edm::EDAnalyzer{
  
public:

  EcalPnGraphDumperModule(const edm::ParameterSet& ps);   
  ~EcalPnGraphDumperModule();
    
  std::string intToString(int num);
  
  void analyze( const edm::Event & e, const  edm::EventSetup& c);
  
  
protected:
  int verbosity;
  int eventCounter;
   
  int ieb_id;
  int first_Pn;
  

  bool inputIsOk;

  std::string fileName;

  std::vector<int> listChannels;
  std::vector<int> listAllChannels;
  std::vector<int> listPns;
  std::vector<int> listAllPns;

  int numPn;

  int abscissa[50];
  int ordinate[50];
  
  std::vector<TGraph> graphs;
  
  TFile * root_file;
  
};






EcalPnGraphDumperModule::EcalPnGraphDumperModule(const edm::ParameterSet& ps){  

  fileName         = ps.getUntrackedParameter<std::string >("fileName", std::string("toto") );

  ieb_id             = ps.getUntrackedParameter< int >("ieb_id", 1);
  first_Pn            = 0;

  listPns          = ps.getUntrackedParameter<std::vector<int> >("listPns", std::vector<int>());

  numPn                = ps.getUntrackedParameter< int >("numPn", 9);

  // consistency checks checks
  inputIsOk       = true;
    
  std::vector<int>::iterator intIter;

  for (intIter = listPns.begin(); intIter != listPns.end(); intIter++)
      {  
	if ( ((*intIter) < 1) ||  (10 < (*intIter)) )       {  
	  std::cout << "[EcalPnGraphDumperModule] pn number : " << (*intIter) << " found in listPns. "
	       << " Valid range is 1-10. Returning." << std::endl;
	  inputIsOk = false;
	  return;
	}
      if (!   first_Pn )   first_Pn = (*intIter);	
      }
  
  // setting the abcissa array once for all
  for (int i=0; i<50; i++)        abscissa[i] = i;
  
  // local event counter (in general different from LV1)
  eventCounter =0;
}



EcalPnGraphDumperModule::~EcalPnGraphDumperModule(){  

  
  fileName += ( std::string("_iEB")  + intToString(ieb_id) ) ;
  fileName +=  ( std::string("_Pn")    + intToString(first_Pn) ); 
  fileName += ".graph.root";

  root_file = new TFile( fileName.c_str() , "RECREATE" );
  std::vector<TGraph>::iterator gr_it;
  for ( gr_it = graphs.begin(); gr_it !=  graphs.end(); gr_it++ )      (*gr_it).Write();
  root_file->Close();
  
}


std::string EcalPnGraphDumperModule::intToString(int num)
{
  //
  // outputs the number into the string stream and then flushes
  // the buffer (makes sure the output is put into the stream)
  //
  std::ostringstream myStream; //creates an ostringstream object
  myStream << num << std::flush;

  return(myStream.str()); //returns the string form of the stringstream object
}




void EcalPnGraphDumperModule::analyze( const edm::Event & e, const  edm::EventSetup& c){

  eventCounter++;
  if (!inputIsOk) return;

  // retrieving crystal data from Event
  edm::Handle<EBDigiCollection>  digis;
  e.getByLabel("ecalEBunpacker", digis);

  // retrieving crystal PN diodes from Event
  edm::Handle<EcalPnDiodeDigiCollection>  PNs;
  e.getByLabel("ecalEBunpacker", PNs);


  // getting the list of all the Pns which will be dumped on TGraph
  std::vector<int>::iterator pn_it;
  for ( pn_it = listPns.begin();  pn_it != listPns.end() ; pn_it++  )
    {
      int ipn    = (*pn_it);
      int hpn = (numPn/2);
      
      for (int u = (-hpn) ; u<=hpn; u++){	  
	  int ipn_c = ipn + u;
	  if (ipn_c < 1 || ipn_c > 10) continue;
	  listAllPns.push_back ( ipn_c ) ;
      }
    }
  for ( EcalPnDiodeDigiCollection::const_iterator pnItr = PNs->begin(); pnItr != PNs->end();
        ++pnItr )  {
    {
      int ipn = (*pnItr).id().iPnId();    
      int ieb    = EcalPnDiodeDetId((*pnItr).id()).iDCCId();
      if (ieb != ieb_id) return;
      
      // selecting desired Pns only
      std::vector<int>::iterator iPnIter;
      iPnIter     = find( listAllPns.begin() , listAllPns.end() , ipn);
      if (iPnIter == listAllPns.end()) { continue; }
	    

      for ( int i=0; i< (*pnItr).size() ; ++i ) {
	ordinate[i] = (*pnItr).sample(i).adc();
      }
	    
      TGraph oneGraph(50, abscissa,ordinate);
      std::string title;
      title = "Graph_ev" + intToString( eventCounter ) + "_ipn" + intToString( ipn );
      oneGraph.SetTitle(title.c_str());
      oneGraph.SetName(title.c_str());
      graphs.push_back(oneGraph);

    }// loop in crystals
  } 


    
}


DEFINE_FWK_MODULE(EcalPnGraphDumperModule);