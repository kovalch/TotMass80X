#include "Analysis.h"

#include <string>

#include "TCanvas.h"
#include "TROOT.h"

#include "GenMatchAnalyzer.h"
#include "Helper.h"
#include "IdeogramAnalyzer.h"
#include "MVAAnalyzer.h"
#include "ProgramOptionsReader.h"
#include "RandomSubsetCreatorLeptonJets.h"
#include "RandomSubsetCreatorAllJets.h"
#include "RooFitTemplateAnalyzer.h"
#include "XMLConfigReader.h"

#include "LHAPDF/LHAPDF.h"

typedef ProgramOptionsReader po;
typedef XMLConfigReader xml;

Analysis::Analysis(std::vector<float> v):
  fIdentifier_(po::GetOption<std::string>("input" )),
  fMethod_    (po::GetOption<std::string>("method")),
  fBinning_   (po::GetOption<std::string>("binning")),
  vBinning_   (v),
  fChannel_(po::GetOption<std::string>("channel")),
  fTree_(0)
{
}

Analysis::~Analysis()
{
  for(std::map<TString, TH1F*>::iterator hist = histograms_.begin(); hist != histograms_.end(); ++hist){
    delete hist->second;
  }
  histograms_.clear();
  //delete fTree_; // deletion is taken care of by MassAnalyzer
}

void Analysis::Analyze() {

  std::cout << "Analyze " << fIdentifier_ << " with method " << fMethod_ << std::endl;

  // random subset creation
  RandomSubsetCreator* fCreator = 0;
  if (!strcmp(fChannel_, "electron") || !strcmp(fChannel_, "muon") || !strcmp(fChannel_, "all")) {
    fCreator = new RandomSubsetCreatorLeptonJets();
  }
  else if (!strcmp(fChannel_, "AllJets")) {
    fCreator = new RandomSubsetCreatorAllJets();
  }
  else {
    std::cerr << "Stopping analysis! Specified decay channel *" << fChannel_ << "* not known!" << std::endl;
    return;
  }
  fTree_ = fCreator->CreateRandomSubset();

  MassAnalyzer* fAnalyzer = 0;

  if (!strcmp(fMethod_, "GenMatch")) {
    fAnalyzer = new GenMatchAnalyzer(fIdentifier_, fTree_);
  }
  else if (!strcmp(fMethod_, "MVA")) {
    fAnalyzer = new MVAAnalyzer(fIdentifier_, fTree_);
  }
  else if (!strcmp(fMethod_, "Ideogram")) {
    fAnalyzer = new IdeogramAnalyzer(fIdentifier_, fTree_);
  }
  else if (!strcmp(fMethod_, "RooFit")) {
    fAnalyzer = new RooFitTemplateAnalyzer(fIdentifier_, fTree_);
  }
  else {
    std::cerr << "Stopping analysis! Specified analysis method *" << fMethod_ << "* not known!" << std::endl;
    return;
  }
   
  Helper* helper = new Helper(fBinning_, vBinning_);
  helper->SetTDRStyle();
  delete helper;

  TCanvas* canvas = new TCanvas("canvas", "Top mass", 900, 600);
  canvas->cd();
  
  for(unsigned int i = 0; i < vBinning_.size()-1; i++) {
    // calculate cuts
    TString cuts;
    std::stringstream stream;
    stream << vBinning_[i] << " < " << fBinning_ << " & "
           << fBinning_ << " < " << vBinning_[i+1];
    cuts = stream.str();
    
    if (!strcmp(fMethod_, "GenMatch")) {
      cuts += " & target == 1";
    }
    else if (!strcmp(fMethod_, "MVA")) {
      cuts += " & mvaDisc > 0";
    }
    else if (!strcmp(fMethod_, "Ideogram")) {
    }
    else if (!strcmp(fMethod_, "RooFit")) {
    }

    int entries = fTree_->GetEntries(cuts);
    std::cout << cuts << std::endl;
    std::cout << entries << std::endl;

    CreateHisto("Entries");
    GetH1("Entries")->SetBinContent(i+1, entries);

    if (entries > 25) {
      fAnalyzer->Analyze(cuts, i, 0);
      const std::map<TString, std::pair<double, double> > values = fAnalyzer->GetValues();

      for(std::map<TString, std::pair<double, double> >::const_iterator value = values.begin(); value != values.end(); ++value){
        CreateHisto(value->first);
        CreateHisto(value->first+TString("_Error"));
        CreateHisto(value->first+TString("_Pull"));
      }
      std::cout << std::endl;
      double genMass   = po::GetOption<double>("mass");
      double genJES    = po::GetOption<double>("jes" );
      double genfSig   = po::GetOption<double>("fsig");
      for(std::map<TString, std::pair<double, double> >::const_iterator value = values.begin(); value != values.end(); ++value){
        double val      = value->second.first;
        double valError = value->second.second;
        double gen = 0;
        if     (value->first.BeginsWith("mass")) gen = genMass;
        else if(value->first.BeginsWith("JES" )) gen = genJES;
        else if(value->first.BeginsWith("fSig")) gen = genfSig;
        GetH1(value->first                 ) ->SetBinContent(i+1, val);
        GetH1(value->first+TString("_Error"))->SetBinContent(i+1, valError);
        GetH1(value->first+TString("_Pull" ))->SetBinContent(i+1, (val - gen)/valError);
        std::cout << "Measured " << value->first << ": " << val << " +/- " << valError << std::endl;
      }
      std::cout << std::endl;
    }
  }

  canvas->Clear();
  canvas->Divide(2,2);

  canvas->cd(1);
  GetH1("Entries")->Draw("E1");

  canvas->cd(2);
  GetH1("mass_mTop_JES")->Draw("E1");
  GetH1("mass_mTop_JES")->SetAxisRange(GetH1("mass_mTop_JES")->GetMinimum(0.05), GetH1("mass_mTop_JES")->GetMaximum(), "Z");
  GetH1("mass_mTop_JES")->Fit("pol0");

  canvas->cd(3);
  GetH1("mass_mTop_JES_Error")->Draw("E1");
  GetH1("mass_mTop_JES_Error")->SetAxisRange(0.05, 5, "Z");
  GetH1("mass_mTop_JES_Error")->Fit("pol0");
  
  /*
  canvas->cd(4);
  hJES->Draw("E1");
  hJES->Fit("pol0");
  */
  TString path("plot/"); path += fMethod_; path += "_"; path += fIdentifier_; path += "_"; path += fBinning_; path += ".eps";
  canvas->Print(path);
  
  TString pathr("plot/"); pathr += fMethod_; pathr += "_"; pathr += fIdentifier_; pathr += "_"; pathr += fBinning_; pathr += ".root";
  canvas->Print(pathr);
  
  delete canvas;
  delete fAnalyzer;
  delete fCreator;
}

void Analysis::CreateHisto(TString name) {
  std::map<TString, TH1F*>::iterator hist = histograms_.find(name);
  if(hist != histograms_.end()){
    hist->second->Reset();
  }
  else{
    gROOT->cd();
    Helper* helper = new Helper(fBinning_, vBinning_);
    SetH1(name, helper->GetH1(name));
    delete helper;
  }
}

TH1F*
Analysis::GetH1(TString histName){
  std::map<TString, TH1F*>::const_iterator hist_iterator = histograms_.find(histName);
  if(hist_iterator != histograms_.end()){
    return hist_iterator->second;
  }
  else{
    std::cerr << "The searched histogram *" << histName << "* does not exist!" << std::endl;
    assert(0);
  }
  return 0;
}

const std::map<TString, TH1F*>
Analysis::GetH1s() const{
  return histograms_;
}

void Analysis::SetH1(TString histName, TH1F* hist){
  histograms_[histName] = hist;
}

TString Analysis::GetIdentifier() {
  return fIdentifier_;
}
