#include "IdeogramSampleLikelihood.h"

#include "IdeogramCombLikelihoodAllJets.h"
#include "IdeogramCombLikelihoodLeptonJets.h"
#include "ProgramOptionsReader.h"

typedef ProgramOptionsReader po;

double IdeogramSampleLikelihood::DoEval(const double *x) const {
  double pullWidth(po::GetOption<double>("pullWidth"));
  double pullWidthEle(po::GetOption<double>("ele_pullWidth"));
  
  double sampleResult  = 0;
  double sampleSumProb = 0;
  double sampleNEvent  = 0;
  for (const auto& event : eventFunctions_) {
    bool eventIsActive = false;
    int flavour = 0;
    double weight = 1;
    double eventResult  = 0;
    double eventSumProb = 0;
    for (const auto& permutation : event) {
      if (permutation->IsActive()) {
        eventIsActive = true;
        double temp[] = {172.5, 1., 1.,0.5};
        temp[0] = x[0];
        temp[1] = x[1];
        temp[2] = x[2];
        temp[3] = x[3];
        double p[] = {0., 0., 0., 0., 0., 0., 0., 0., 0.};
        eventResult  += permutation->Evaluate(temp, p);
        eventSumProb += permutation->GetFixedParam(0);
        flavour       = permutation->GetFixedParam(3);
        weight        = permutation->GetFixedParam(8);
        if(eventResult==0){
//           std::cout << x[0] << " " << x[1] << " " << x[2] << " " << x[3] << std::endl;
//           std::cout << permutation->GetFixedParam(0) << " " << permutation->GetFixedParam(1) << " " << permutation->GetFixedParam(2) << " " << permutation->GetFixedParam(3) << " " << permutation->GetFixedParam(4) << " " << permutation->GetFixedParam(5) << " " << permutation->GetFixedParam(6) << " " << permutation->GetFixedParam(7) << " " << permutation->GetFixedParam(8) << " " << std::endl;
       }
     }
    }
    if (eventIsActive) {
      double pullWidthFlavour = (flavour == 11) ? pullWidthEle : pullWidth;
      sampleResult  += weight * -2.*log(eventResult)*eventSumProb / (pullWidthFlavour*pullWidthFlavour);
      assert(weight==weight);
      assert(eventSumProb==eventSumProb);
      assert(eventResult!=0);
      sampleSumProb += weight * eventSumProb;
      sampleNEvent  += weight;
    }
  }
  //std::cout << x[0] << " " << x[1] << " " << sampleResult << " * " << sampleNEvent << " / " << sampleSumProb << std::endl;
  return sampleResult * sampleNEvent / sampleSumProb;
}
