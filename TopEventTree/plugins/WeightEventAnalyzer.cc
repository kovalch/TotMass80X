/*
 * WeightEventAnalyzer.cc
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
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "AnalysisDataFormats/TopObjects/interface/TtSemiLeptonicEvent.h"
#include "AnalysisDataFormats/TopObjects/interface/TtFullHadronicEvent.h"
//#include "LHAPDF/LHAPDF.h"

#include "TopMass/TopEventTree/plugins/WeightEventAnalyzer.h"

WeightEventAnalyzer::WeightEventAnalyzer(const edm::ParameterSet& cfg):
mcWeight_    (cfg.getParameter<double>("mcWeight")),
puSrc_       (cfg.getParameter<edm::InputTag>("puSrc")),
vertexSrc_   (cfg.getParameter<edm::InputTag>("vertexSrc")),

puWeightSrc_    (cfg.getParameter<edm::InputTag>("puWeightSrc")),
puWeightUpSrc_  (cfg.getParameter<edm::InputTag>("puWeightUpSrc")),
puWeightDownSrc_(cfg.getParameter<edm::InputTag>("puWeightDownSrc")),

jets_                   (cfg.getParameter<edm::InputTag>("jets")),
bWeightSrc_             (cfg.getParameter<edm::InputTag>("bWeightSrc")),
bWeightSrc_bTagSFUp_    (cfg.getParameter<edm::InputTag>("bWeightSrc_bTagSFUp")),
bWeightSrc_bTagSFDown_  (cfg.getParameter<edm::InputTag>("bWeightSrc_bTagSFDown")),
bWeightSrc_misTagSFUp_  (cfg.getParameter<edm::InputTag>("bWeightSrc_misTagSFUp")),
bWeightSrc_misTagSFDown_(cfg.getParameter<edm::InputTag>("bWeightSrc_misTagSFDown")),

bJESSrc_fNuUp_    (cfg.getParameter<edm::InputTag>("bJESSrc_fNuUp")),
bJESSrc_fNuDown_  (cfg.getParameter<edm::InputTag>("bJESSrc_fNuDown")),
bJESSrc_frag_     (cfg.getParameter<edm::InputTag>("bJESSrc_frag")),
bJESSrc_fragHard_ (cfg.getParameter<edm::InputTag>("bJESSrc_fragHard")),
bJESSrc_fragSoft_ (cfg.getParameter<edm::InputTag>("bJESSrc_fragSoft")),

triggerWeightSrc_ (cfg.getParameter<edm::InputTag>("triggerWeightSrc")),

muWeightSrc_ (cfg.getParameter<edm::InputTag>("muWeightSrc")),
elWeightSrc_ (cfg.getParameter<edm::InputTag>("elWeightSrc")),

genEventSrc_   (cfg.getParameter<edm::InputTag>("genEventSrc")),
ttEvent_       (cfg.getParameter<edm::InputTag>("ttEvent")),
savePDFWeights_(cfg.getParameter<bool>("savePDFWeights")),
brCorrection_  (cfg.getParameter<bool>("brCorrection")),
weight(0)
{
  //LHAPDF::initPDFSet(1, "cteq66.LHgrid");
}

void
WeightEventAnalyzer::analyze(const edm::Event& evt, const edm::EventSetup& setup)
{
  //////////////////////////////////////////////////////////////////////////////
  // INIT WeightEvent
  ////////////////////////////////////////////////////////////////////////////

  weight->init();

  //////////////////////////////////////////////////////////////////////////
  // MC & PDF weights
  ////////////////////////////////////////////////////////////////////////

  edm::Handle<GenEventInfoProduct> genEventInfo_h;
  if(!genEventSrc_.label().empty()) evt.getByLabel(genEventSrc_, genEventInfo_h);

  weight->mcWeight = mcWeight_;
  weight->combinedWeight *= weight->mcWeight;
  if(genEventInfo_h.isValid()){
    //weight->mcWeight = genEventInfo_h->weight();
    if(genEventInfo_h->weight() < 0.) {
      weight->mcWeight       *= -1.;
      weight->combinedWeight *= -1.;
    }

    if(savePDFWeights_){
      // variables needed for PDF uncertainties
      weight->x1  = genEventInfo_h->pdf()->x.first;
      weight->x2  = genEventInfo_h->pdf()->x.second;
      weight->Q   = genEventInfo_h->pdf()->scalePDF;
      weight->id1 = genEventInfo_h->pdf()->id.first;
      weight->id2 = genEventInfo_h->pdf()->id.second;
/*
      double w0 = 0.;
      for(unsigned i=0; i <=44; ++i) {
        LHAPDF::usePDFMember(1,i);
        double xpdf1 = LHAPDF::xfx(1, weight->x1, weight->Q, weight->id1);
        double xpdf2 = LHAPDF::xfx(1, weight->x2, weight->Q, weight->id2);
        if(i<1)
          w0 = xpdf1 * xpdf2;
        else
          weight->pdfWeight.push_back(xpdf1 * xpdf2 / w0);
      }
*/
    }
  }
  
  //////////////////////////////////////////////////////////////////////////
  // BR correction for MadGraph
  ////////////////////////////////////////////////////////////////////////
  
  if (brCorrection_) {
    edm::Handle<TtSemiLeptonicEvent> hSemiLepTtEvent;
    edm::Handle<TtFullHadronicEvent> hFullHadTtEvent;
    const TtSemiLeptonicEvent *semiLepTtEvent = 0;
    const TtFullHadronicEvent *fullHadTtEvent = 0;
    const TtEvent *ttEvent = 0;

    if(ttEvent_.label().find("SemiLep")!=std::string::npos){
      evt.getByLabel(ttEvent_, hSemiLepTtEvent);
      semiLepTtEvent = hSemiLepTtEvent.product();
      ttEvent = semiLepTtEvent;
    }
    else if(ttEvent_.label().find("FullHad")!=std::string::npos){
      evt.getByLabel(ttEvent_, hFullHadTtEvent);
      fullHadTtEvent = hFullHadTtEvent.product();
      ttEvent = fullHadTtEvent;
    }
    else{
      std::cout << "The given 'ttEvent' label (" << ttEvent_.label() << ") is not allowed.\n"
          << "It has to contain either 'SemiLep' or 'FullHad'!" << std::endl;
    }
    
    if(      !ttEvent->genEvent()            ) weight->brWeight = 1;
    else if( !ttEvent->genEvent()->isTtBar() ) weight->brWeight = 1;
    else if (ttEvent->genEvent()->isFullHadronic()) {
      weight->brWeight        = 0.456976 / (36./81.);
      weight->combinedWeight *= weight->brWeight;
    }
    else if (ttEvent->genEvent()->isSemiLeptonic()) {
      weight->brWeight        = 0.438048 / (36./81.);
      weight->combinedWeight *= weight->brWeight;
    }
    else if (ttEvent->genEvent()->isFullLeptonic()) {
      weight->brWeight        = 0.104976 / (1./9.);
      weight->combinedWeight *= weight->brWeight;
    }
  }
  
  //////////////////////////////////////////////////////////////////////////
  // PU weights & control variables
  ////////////////////////////////////////////////////////////////////////

  edm::Handle<edm::View<PileupSummaryInfo> > pu_h;
  evt.getByLabel(puSrc_, pu_h);

  if(pu_h.isValid()){
    weight->nPU.resize(3);
    for(edm::View<PileupSummaryInfo>::const_iterator iterPU = pu_h->begin(); iterPU != pu_h->end(); ++iterPU){
      // vector size is 3
      // -1: previous BX, 0: current BX,  1: next BX
      if     (iterPU->getBunchCrossing()==-1)   weight->nPU[0]  = iterPU->getPU_NumInteractions();
      else if(iterPU->getBunchCrossing()== 0) { weight->nPU[1]  = iterPU->getPU_NumInteractions();
                                                weight->nPUTrue = iterPU->getTrueNumInteractions(); }
      else if(iterPU->getBunchCrossing()== 1)   weight->nPU[2]  = iterPU->getPU_NumInteractions();
    }
  }

  edm::Handle<std::vector<reco::Vertex> > vertecies_h;
  evt.getByLabel(vertexSrc_, vertecies_h);

  if(vertecies_h.isValid()) weight->nVertex = vertecies_h->size();

  edm::Handle<double> puWeight_h;
  evt.getByLabel(puWeightSrc_, puWeight_h);

  edm::Handle<double> puWeightUp_h;
  evt.getByLabel(puWeightUpSrc_, puWeightUp_h);

  edm::Handle<double> puWeightDown_h;
  evt.getByLabel(puWeightDownSrc_, puWeightDown_h);

  if(puWeight_h    .isValid()) { weight->puWeight     = *puWeight_h    ; weight->combinedWeight *= weight->puWeight; }
  if(puWeightUp_h  .isValid())   weight->puWeightUp   = *puWeightUp_h  ;
  if(puWeightDown_h.isValid())   weight->puWeightDown = *puWeightDown_h;
  
  //////////////////////////////////////////////////////////////////////////
  // btag weights
  ////////////////////////////////////////////////////////////////////////

  edm::Handle<double> bWeight_h;
  evt.getByLabel(bWeightSrc_, bWeight_h);

  edm::Handle<double> bWeight_bTagSFUp_h;
  evt.getByLabel(bWeightSrc_bTagSFUp_, bWeight_bTagSFUp_h);

  edm::Handle<double> bWeight_bTagSFDown_h;
  evt.getByLabel(bWeightSrc_bTagSFDown_, bWeight_bTagSFDown_h);

  edm::Handle<double> bWeight_misTagSFUp_h;
  evt.getByLabel(bWeightSrc_misTagSFUp_, bWeight_misTagSFUp_h);

  edm::Handle<double> bWeight_misTagSFDown_h;
  evt.getByLabel(bWeightSrc_misTagSFDown_, bWeight_misTagSFDown_h);

  if(bWeight_h             .isValid()) { weight->bTagWeight              = *bWeight_h             ; weight->combinedWeight *= weight->bTagWeight; }
  if(bWeight_bTagSFUp_h    .isValid())   weight->bTagWeight_bTagSFUp     = *bWeight_bTagSFUp_h    ;
  if(bWeight_bTagSFDown_h  .isValid())   weight->bTagWeight_bTagSFDown   = *bWeight_bTagSFDown_h  ;
  if(bWeight_misTagSFUp_h  .isValid())   weight->bTagWeight_misTagSFUp   = *bWeight_misTagSFUp_h  ;
  if(bWeight_misTagSFDown_h.isValid())   weight->bTagWeight_misTagSFDown = *bWeight_misTagSFDown_h;

  //////////////////////////////////////////////////////////////////////////
  // bJES weights
  ////////////////////////////////////////////////////////////////////////
  
  edm::Handle<double> bJESWeight_fNuUp_h; evt.getByLabel(bJESSrc_fNuUp_, bJESWeight_fNuUp_h);
  if(bJESWeight_fNuUp_h.isValid()) weight->bJESWeight_fNuUp = *bJESWeight_fNuUp_h;
  
  edm::Handle<double> bJESWeight_fNuDown_h; evt.getByLabel(bJESSrc_fNuDown_, bJESWeight_fNuDown_h);
  if(bJESWeight_fNuDown_h.isValid()) weight->bJESWeight_fNuDown = *bJESWeight_fNuDown_h;
  
  edm::Handle<double> bJESWeight_frag_h; evt.getByLabel(bJESSrc_frag_, bJESWeight_frag_h);
  if(bJESWeight_frag_h.isValid()) weight->bJESWeight_frag = *bJESWeight_frag_h;
  
  edm::Handle<double> bJESWeight_fragHard_h; evt.getByLabel(bJESSrc_fragHard_, bJESWeight_fragHard_h);
  if(bJESWeight_fragHard_h.isValid()) weight->bJESWeight_fragHard = *bJESWeight_fragHard_h;
  
  edm::Handle<double> bJESWeight_fragSoft_h; evt.getByLabel(bJESSrc_fragSoft_, bJESWeight_fragSoft_h);
  if(bJESWeight_fragSoft_h.isValid()) weight->bJESWeight_fragSoft = *bJESWeight_fragSoft_h;
  
  //////////////////////////////////////////////////////////////////////////
  // trigger weights
  ////////////////////////////////////////////////////////////////////////
  
  edm::Handle<double> triggerWeight_h;
  evt.getByLabel(triggerWeightSrc_, triggerWeight_h);
  if(triggerWeight_h.isValid()) { weight->triggerWeight = *triggerWeight_h; weight->combinedWeight *= weight->triggerWeight; }

  trs->Fill();
}

void
WeightEventAnalyzer::beginJob()
{
  if( !trs ) throw edm::Exception( edm::errors::Configuration, "TreeRegistryService is not registered in cfg file!" );

  weight = new WeightEvent();
  trs->Branch("weight.", weight);
}

void
WeightEventAnalyzer::endJob()
{
}

WeightEventAnalyzer::~WeightEventAnalyzer()
{
}

//define this as a plug-in
#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(WeightEventAnalyzer);
