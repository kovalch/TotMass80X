/*
 * JetEventAnalyzer.cc
 *
 *  Created on: Feb 6, 2013
 *      Author: eschliec
 */

//#include <memory>

// user include files
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "TopMass/TopEventTree/interface/TreeRegistryService.h"

#include "DataFormats/PatCandidates/interface/Jet.h"

#include "TopMass/TopEventTree/plugins/JetEventAnalyzer.h"

JetEventAnalyzer::JetEventAnalyzer(const edm::ParameterSet& cfg):
jets_        (cfg.getParameter<edm::InputTag>("jets")),
//allJets_     (cfg.getParameter<edm::InputTag>("allJets")),
//noPtEtaJets_ (cfg.getParameter<edm::InputTag>("noPtEtaJets")),
gluonTagName_  (cfg.getParameter<edm::InputTag>("gluonTagSrc").encode()),
kJetMAX_(cfg.getParameter<int>("maxNJets")),
jet(0),
checkedIsPFJet(false), checkedJERSF(false), checkedJESSF(false), checkedTotalSF(false), checkedQGTag(false),
       isPFJet(false),     hasJERSF(false),     hasJESSF(false),     hasTotalSF(false),     hasQGTag(false)
{
}

void
JetEventAnalyzer::analyze(const edm::Event& evt, const edm::EventSetup& setup)
{
  //////////////////////////////////////////////////////////////////////////////
  // INIT JetEvent
  ////////////////////////////////////////////////////////////////////////////

  jet->init();

  //////////////////////////////////////////////////////////////////////////
  // JETS
  ////////////////////////////////////////////////////////////////////////

  edm::Handle<std::vector<pat::Jet> > jets;
  evt.getByLabel(jets_, jets);

  unsigned short jetIndex = 0;
  for(std::vector< pat::Jet >::const_iterator ijet = jets->begin(); ijet != jets->end(); ++ijet, ++jetIndex) {
    // write only kJetMAX_ jets into the event
    if(jetIndex == kJetMAX_) break;

    jet->jet.push_back(TLorentzVector(ijet->px(), ijet->py(), ijet->pz(), ijet->energy()));
    if(ijet->genJet()) jet->genJet.push_back(TLorentzVector(ijet->genJet()->px(), ijet->genJet()->py(), ijet->genJet()->pz(), ijet->genJet()->energy()));
    else               jet->genJet.push_back(TLorentzVector(0,0,0,-1));
    if(ijet->genParton()) jet->genParton.push_back(TLorentzVector(ijet->genParton()->px(), ijet->genParton()->py(), ijet->genParton()->pz(), ijet->genParton()->energy()));
    else                  jet->genParton.push_back(TLorentzVector(0,0,0,-1));

    // check only once per module run if the needed collections are available
    if(!checkedIsPFJet) { checkedIsPFJet = true; isPFJet = ijet->isPFJet(); }
    if(!checkedQGTag  ) { checkedQGTag   = true; if(ijet->hasUserFloat(gluonTagName_)) hasQGTag   = true; }
    if(!checkedJERSF  ) { checkedJERSF   = true; if(ijet->hasUserFloat("jerSF"      )) hasJERSF   = true; }
    if(!checkedJESSF  ) { checkedJESSF   = true; if(ijet->hasUserFloat("jesSF"      )) hasJESSF   = true; }
    if(!checkedTotalSF) { checkedTotalSF = true; if(ijet->hasUserFloat("totalSF"    )) hasTotalSF = true; }

    if(isPFJet){
      jet->fChargedHadron .push_back(ijet->chargedHadronEnergyFraction());
      jet->fNeutralHadron .push_back(ijet->neutralHadronEnergyFraction());
      jet->fElectron      .push_back(ijet->electronEnergyFraction());
      jet->fPhoton        .push_back(ijet->photonEnergyFraction());
      jet->fMuon          .push_back(ijet->muonEnergyFraction());
      jet->nConstituents  .push_back(ijet->nConstituents());
      jet->nChargedHadrons.push_back(ijet->chargedHadronMultiplicity());
      jet->nNeutralHadrons.push_back(ijet->neutralHadronMultiplicity());
      jet->nElectrons     .push_back(ijet->electronMultiplicity());
      jet->nPhotons       .push_back(ijet->photonMultiplicity());
      jet->nMuons         .push_back(ijet->muonMultiplicity());
    }
    jet->charge         .push_back(ijet->jetCharge());
    jet->flavour        .push_back(ijet->partonFlavour());
    jet->bTagCSV        .push_back(ijet->bDiscriminator("combinedSecondaryVertexBJetTags"));

    if(hasQGTag  ) jet->gluonTag.push_back(ijet->userFloat(gluonTagName_));
    if(hasJERSF  ) jet->jerSF   .push_back(ijet->userFloat("jerSF"      ));
    if(hasJESSF  ) jet->jesSF   .push_back(ijet->userFloat("jesSF"      ));
    if(hasTotalSF) jet->totalSF .push_back(ijet->userFloat("totalSF"    ));

    std::pair<TVector2, TVector2> pulls = getPullVector( ijet );
    jet->pull       .push_back(pulls.first );
    jet->pullCharged.push_back(pulls.second);
  }

  trs->Fill();
}

void
JetEventAnalyzer::beginJob()
{
  if( !trs ) throw edm::Exception( edm::errors::Configuration, "TreeRegistryService is not registered in cfg file!" );

  jet = new JetEvent();
  trs->Branch("jet", jet);
}

void
JetEventAnalyzer::endJob()
{
}

JetEventAnalyzer::~JetEventAnalyzer()
{
}

std::pair<TVector2, TVector2>
JetEventAnalyzer::getPullVector( std::vector<pat::Jet>::const_iterator patJet )
{
  TVector2 null(0,0);

  if (patJet->isPFJet() == false) {
    return std::make_pair(null,null);
  }

  //re-reconstruct the jet direction with the charged tracks
  TLorentzVector chargedJet(0,0,0,0);
  TLorentzVector constituent(0,0,0,0);
  unsigned int nCharged = 0;

  std::vector<reco::PFCandidatePtr> constituents = patJet->getPFConstituents();
  for(size_t idx = 0, length = constituents.size(); idx < length; ++idx){
    if( constituents.at(idx)->charge() != 0 ){
      constituent.SetPtEtaPhiE( constituents.at(idx)->pt(), constituents.at(idx)->eta(), constituents.at(idx)->phi(), constituents.at(idx)->energy() );
      chargedJet += constituent;
      ++nCharged;
    }
  }

  double jetPt        = patJet   ->pt(), jetPhi        = patJet   ->phi(), jetRapidity        = patJet   ->rapidity();
  double jetPtCharged = chargedJet.Pt(), jetPhiCharged = chargedJet.Phi(), jetRapidityCharged = chargedJet.Rapidity();
  TVector2 r(0,0);
  TVector2 pullAll(0,0);
  TVector2 pullCharged(0,0);

  for(size_t idx = 0, length = constituents.size(); idx < length; ++idx){
    double constituentPt       = constituents.at(idx)->pt();
    double constituentPhi      = constituents.at(idx)->phi();
    double constituentRapidity = constituents.at(idx)->rapidity();
    r.Set( constituentRapidity - jetRapidity, TVector2::Phi_mpi_pi( constituentPhi - jetPhi ) );
    pullAll += ( constituentPt / jetPt ) * r.Mod() * r;
    //calculate TVector using only charged tracks
    if( constituents.at(idx)->charge() != 0  )
      r.Set( constituentRapidity - jetRapidityCharged, TVector2::Phi_mpi_pi( constituentPhi - jetPhiCharged ) );
    pullCharged += ( constituentPt / jetPtCharged ) * r.Mod() * r;
  }

  // if there are less than two charged tracks do not calculate the pull (there is not enough info), return null vector
  //TODO really needed???????
  if( nCharged < 2 )
    pullCharged = null;

  return std::make_pair(pullAll, pullCharged);
}

//define this as a plug-in
#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(JetEventAnalyzer);
