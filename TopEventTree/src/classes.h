//#include "TopAnalysis/TopUtils/interface/ResolutionVariables.h"

//#include "DataFormats/JetReco/interface/CaloJetCollection.h"
//#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Jet.h"

//#include "DataFormats/Common/interface/Association.h"
//#include "DataFormats/Candidate/interface/Candidate.h"


#include "DataFormats/Common/interface/RefProd.h"
#include "DataFormats/Common/interface/RefToBase.h"
#include "DataFormats/Common/interface/RefHolder.h"
#include "DataFormats/Common/interface/Holder.h"

#include "TLorentzVector.h"
#include "TopMass/TopEventTree/interface/TopEvent.h"
#include "TopMass/TopEventTree/interface/JetEvent.h"
#include "TopMass/TopEventTree/interface/BRegJetEvent.h"
#include "TopMass/TopEventTree/interface/WeightEvent.h"
#include "TopMass/TopEventTree/interface/SherpaGenEvent.h"

namespace {
  struct dictionary {
    std::vector<TLorentzVector> vlv;
    std::vector<std::vector<TLorentzVector> > vvlv;
    
    TopEvent     te;
    JetEvent     je;
    BRegJetEvent BRegje;
    WeightEvent  we;
    SherpaGenEvent sge;

    edm::RefToBase<pat::Jet> rtb;
    pat::UserHolder<edm::RefToBase<pat::Jet> > uhrtb;
  };
}
