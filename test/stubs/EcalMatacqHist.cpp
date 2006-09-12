#include "EcalMatacqHist.h"

#include "TH1D.h"
#include "TProfile.h"
#include <sstream>
#include <iostream>

using namespace std;

EcalMatacqHist::EcalMatacqHist(const edm::ParameterSet& ps):
  iEvent(0){
  outFileName= ps.getUntrackedParameter<std::string>("outputRootFile", "matacqHist.root");
  nTimePlots = ps.getUntrackedParameter<int>("nTimePlots", 10);
  firstTimePlotEvent = ps.getUntrackedParameter<int>("firstTimePlotEvent",
						     1);
  TDirectory* dsave = gDirectory;
  outFile = auto_ptr<TFile> (new TFile(outFileName.c_str(), "RECREATE"));
  if(outFile->IsZombie()){
    cout << "EcalMatacqHist: Failed to create file " << outFileName
	 << " No histogram will be created.\n";
  }
  dsave->cd();
}

EcalMatacqHist::~EcalMatacqHist(){
  if(!outFile->IsZombie()){
    TDirectory* dsave = gDirectory;
    outFile->cd();
    profile->Write();
    dsave->cd();
  }
}

void
EcalMatacqHist:: analyze( const edm::Event & e, const  edm::EventSetup& c){
  ++iEvent;
  if(outFile->IsZombie()) return;
  TDirectory* dsave = gDirectory;
  outFile->cd();
  // retrieving MATACQ digis:
  edm::Handle<EcalMatacqDigi> digis;
  e.getByLabel("ecalEBunpacker", digis);

  if(iEvent >= firstTimePlotEvent && iEvent < firstTimePlotEvent + nTimePlots){
    int nSamples = digis->size();
    stringstream title;
    stringstream name;
    name << "matacq" << iEvent;
    title << "Matacq channel " <<  digis->chId() << ", event " << iEvent
	  << ", Ts = " << digis->ts()*1.e9 << "ns"; 
    TH1D h1(name.str().c_str(), title.str().c_str(),
	    nSamples, -.5, -.5+nSamples);
    for(int i=0; i<digis->size(); ++i){
      h1.Fill(i, digis->adcCount(i));
    }
    h1.Write();
  }

  //profile
  //init:
  if(iEvent==1){
    stringstream profTitle;
    profTitle << "Matacq channel " <<  digis->chId()
	      << " profile";
    profile = new TProfile("matacq",
			   profTitle.str().c_str(),
			   digis->size(),
			   -.5,
			   -.5+digis->size(),
			   "I");
  }
  for(int i=0; i<digis->size(); ++i){
    profile->Fill(i, digis->adcCount(i));
  }
  dsave->cd();
} // analyze


DEFINE_FWK_MODULE(EcalMatacqHist)
