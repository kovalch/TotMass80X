#ifndef BRegJetEventAnalyzer_h
#define BRegJetEventAnalyzer_h

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "TopMass/TopEventTree/interface/BRegJetEvent.h"

// for event setup handles needed for "computer"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"


//for "computer" that is needed for more advanced b-tagging variables
#include "RecoBTau/JetTagComputer/interface/GenericMVAJetTagComputer.h"
#include "RecoBTau/JetTagComputer/interface/GenericMVAJetTagComputerWrapper.h"
#include "RecoBTau/JetTagComputer/interface/JetTagComputer.h"
#include "RecoBTau/JetTagComputer/interface/JetTagComputerRecord.h"
#include "RecoBTag/SecondaryVertex/interface/CombinedSVComputer.h"

//to read in regression results
#include "TMVA/Tools.h"
#include "TMVA/Reader.h"

//GBRTrain
#include "CondFormats/EgammaObjects/interface/GBRForest.h"


#include "TFile.h"

//#include <utility>

class BRegJetEventAnalyzer : public edm::EDAnalyzer {

  public:

  explicit BRegJetEventAnalyzer(const edm::ParameterSet&);
  ~BRegJetEventAnalyzer();

 private:

  virtual void beginJob() ;
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
  virtual void endJob();

  edm::Service<TreeRegistryService> trs;

  edm::InputTag jets_;
  //edm::InputTag allJets_;
  //edm::InputTag noPtEtaJets_;
  edm::InputTag rho_tag_;
  edm::InputTag rho25_tag_;
  std::string gluonTagName_;

  // max possible number of jets in events
  const int kJetMAX_;

  // THE BRegJetEvent to store the information
  BRegJetEvent* BRegJet;

  //TMVA reader to determine b-regression result
  TMVA::Reader *reader_;
  std::string mva_name_,mva_path_,GBRmva_path_;



  bool writeOutVariables_;
  /// jet output collection 
  std::string outputJets_;

  // check only once per module run if the needed collections are available
  bool checkedIsPFJet, checkedJERSF, checkedJESSF, checkedTotalSF, checkedQGTag, checkedJESTotUnc;
  bool        isPFJet,     hasJERSF,     hasJESSF,     hasTotalSF,     hasQGTag,     hasJESTotUnc;

  double tempJetPtCorr_,tempJetMt_,tempJetEta_,
  tempfChargedHadrons_,tempfElectrons_, tempfMuons_,
  tempBTagCSV_;
  double tempnChargedHadrons_,tempnChargedPFConstituents_,tempnPFConstituents_ ;

  double readerJetPtRaw_, readerJetArea_, readerJetEtWEightedSigmaPhi_,
  	  readerJesUncert_, readerSVtx3dLength_, readerSVtx3dLengthError_,
  	  readerSVtxMass_, readerSVtxPt_, readerlChTrackPt_, readerRho25_;

  double readerSoftMuonPt_, readerSoftMuonRatioRel_, readerSoftMuonDeltaR_,
  	  readerSoftElectronPt_, readerSoftElectronRatioRel_,
  	  readerSoftElectronDeltaR_;

  GBRForest *gbropt_;
  std::vector <double*> variablePointer_;
  Float_t *vals_;
  std::vector<std::string> *varlist_;

};

#endif
