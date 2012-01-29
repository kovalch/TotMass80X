#include <vector>
#include <cmath>
#include <fstream>

#include "Analysis.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"
#include "TPaveStats.h"
#include "TLatex.h"
#include "TH1.h"
#include "TH3F.h"
#include "TVector.h"
#include "TLegend.h"

#include "tinyxml.h"

#include "Helper.h"

bool fexists(const char *filename)
{
  ifstream ifile(filename);
  return ifile;
}

struct massPoint {
  double genMass;
  double genJES;
  double genLumi;
  TString identifier;
  TString fileName;
  Analysis* analysis;
  
  TH1D* hMass;
  TH1D* hMassError;
  TH1D* hMassPull;
  TH2F* h2Mass;
  TH2F* h2MassError;
  TH3F* h3Mass;
  TH3F* h3MassError;
  TH3F* h3MassPull;
  TH3F* h3JES;
  TH3F* h3JESError;
  TH3F* h3JESPull;
  
  massPoint(double pGenMass, double pGenJES, TString pIdentifier) :
      genMass(pGenMass), genJES(pGenJES), identifier(pIdentifier) {
    if (fexists("/scratch/hh/current/cms/user/mseidel/Summer11_TTJets1725_1.00/analyzeTop.root")) {
      fileName = "/scratch/hh/lustre/cms/user/mseidel/Summer11_TTJets";
    }
    else fileName = "root/analyzeTop_";
    fileName += identifier;
    fileName += "/analyzeTop.root";
  };
};

class TopMass {
  private:
    TString fMethod;
    int fBins;
    double fLumi;
    
    std::vector<massPoint> massPoints;
    std::vector<massPoint>::iterator iMassPoint;
    
    std::vector< std::vector<Analysis*> > calibrationAnalyses;
    
    std::vector<Analysis*> analyses;
    std::vector<Analysis*>::iterator iAnalysis;
    
    Analysis* aSim;
    
    double fCalibFitParameter[6][6][2];
    double fCalibFitParError[6][6][2];
    
    void LoadXML();
  
  public:
    TopMass(TString method, int bins, double lumi);
    
    void WriteEnsembleTest(bool readCalibration = false);
    void EvalEnsembleTest(bool writeCalibration = false);
    
    void QuickCalibration();
    void QuickSystematics();
    
    TH2F* Measure(Analysis* a);
};
