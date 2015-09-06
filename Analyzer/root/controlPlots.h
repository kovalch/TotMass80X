#ifndef TOPMASSCONTROLPLOTS_H
#define TOPMASSCONTROLPLOTS_H

#include "TChain.h"
#include "TTreeFormula.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TProfile.h"
#include "Helper.h"

#include "ProgramOptionsReader.h"

#include <vector>
#include <boost/algorithm/string/replace.hpp>

typedef ProgramOptionsReader po;

enum sampleType {kData, kSig, kBkg, kSigVar};

class TopMassControlPlots{
public:
  TopMassControlPlots();
  ~TopMassControlPlots() {}

private:

  class MySample{
  public:
    MySample(std::string name_, std::string file_, int type_, int color_, int line_ = 1, double scale_ = 1., std::string replaceVar_="", double scaleunc_ = 0.) :
      name(name_), file(file_), type(type_), color(color_), line(line_), scale(scale_),replaceVar(replaceVar_), scaleunc(scaleunc_) {}
    
    std::string name, file;
    int type, color, line;
    double scale;
    std::string replaceVar;
    double scaleunc;
  };

  class MyHistogram{
  public:
    MyHistogram(std::string name_, std::string formulax_, std::string selection_, std::string title, int nBins, double min, double max) :
      varx(0), vary(0), sel(0), histoweight(0),
      name(name_), selection(selection_),CustomHistoWeight("1."),
      formulaex({formulax_}), formulaey({"1."}), legendHeader(""), extraLabel(""),
      plotRangeYMin(-99), plotRangeYMax(-99), plotRangeYRatioMin(po::GetOption<double>("analysisConfig.ratioYMin")),
      plotRangeYRatioMax(po::GetOption<double>("analysisConfig.ratioYMax")),
      data(new TH1F((std::string("hD")+name).c_str(), (std::string("Data")+title).c_str(), nBins, min, max)),
      unc(new TH1F((std::string("hUnc")+name).c_str(), (std::string("Unc")+title).c_str(), nBins, min, max)),
      histogramDimension(1), dataContainsMC(false), fitGaussToCore(false), 
      exportSigVarToRoot(false), logX(false), logY(false),
      plotStackNorm(false)
    {
      SetDataStyle(data);
      SetUncStyle(unc);
    }

    MyHistogram(std::string name_, std::vector<std::string> formulaex_, std::string selection_, std::string title, int nBins, double min, double max) :
      varx(0), vary(0), sel(0), histoweight(0),
      name(name_), selection(selection_),CustomHistoWeight("1."),
      formulaex(formulaex_), formulaey({"1."}), legendHeader(""), extraLabel(""),
      plotRangeYMin(-99), plotRangeYMax(-99), plotRangeYRatioMin(po::GetOption<double>("analysisConfig.ratioYMin")),
      plotRangeYRatioMax(po::GetOption<double>("analysisConfig.ratioYMax")),
      data(new TH1F((std::string("hD")+name).c_str(), (std::string("Data")+title).c_str(), nBins, min, max)),
      unc(new TH1F((std::string("hUnc")+name).c_str(), (std::string("Unc")+title).c_str(), nBins, min, max)),
      histogramDimension(1), dataContainsMC(false), fitGaussToCore(false), 
      exportSigVarToRoot(false), logX(false), logY(false),
      plotStackNorm(false)
   {
      SetDataStyle(data);
      SetUncStyle(unc);
    }

    MyHistogram(std::string name_, std::string formulax_, std::string formulay_, std::string selection_, std::string title, int x_nBins, double x_min, double x_max, int y_nBins, double y_min, double y_max/*, double y_plotmin =0., double y_plotmax=-1.*/) :
      varx(0), vary(0), sel(0), histoweight(0),
      name(name_), selection(selection_),CustomHistoWeight("1."),
      formulaex({formulax_}), formulaey({formulay_}), legendHeader(""), extraLabel(""),
      plotRangeYMin(-99), plotRangeYMax(-99), plotRangeYRatioMin(po::GetOption<double>("analysisConfig.ratioYMin")),
      plotRangeYRatioMax(po::GetOption<double>("analysisConfig.ratioYMax")),
      data(new TH2F((std::string("h2D")+name).c_str(), (std::string("Data")+title).c_str(), x_nBins, x_min, x_max, y_nBins, y_min, y_max)),
      histogramDimension(2), dataContainsMC(false), fitGaussToCore(false), 
      exportSigVarToRoot(false), logX(false), logY(false),
      plotStackNorm(false)
    {
      SetDataStyle(data);
    }

    MyHistogram(std::string name_, std::vector<std::string> formulaex_, std::vector<std::string> formulaey_, std::string selection_, std::string title, int x_nBins, double x_min, double x_max, int y_nBins, double y_min, double y_max/*, double y_plotmin =0., double y_plotmax=-1.*/) :
      varx(0), vary(0), sel(0), histoweight(0),
      name(name_), selection(selection_),CustomHistoWeight("1."),
      formulaex(formulaex_), formulaey(formulaey_), legendHeader(""), extraLabel(""),
      plotRangeYMin(-99), plotRangeYMax(-99), plotRangeYRatioMin(po::GetOption<double>("analysisConfig.ratioYMin")),
      plotRangeYRatioMax(po::GetOption<double>("analysisConfig.ratioYMax")),
      data(new TH2F((std::string("h2D")+name).c_str(), (std::string("Data")+title).c_str(), x_nBins, x_min, x_max, y_nBins, y_min, y_max)),
      histogramDimension(2), dataContainsMC(false), fitGaussToCore(false), 
      exportSigVarToRoot(false), logX(false), logY(false),
      plotStackNorm(false)
    {
      SetDataStyle(data);
    }


    void SetDataStyle(TH1* data){
      data->SetLineWidth(1);
      data->SetLineColor(kBlack);
      data->SetMarkerStyle(20);
      data->SetMarkerColor(kBlack);
    }
    
    void SetUncStyle(TH1* unc){
      unc->SetLineWidth(0);
      unc->SetLineColor(kBlack);
      unc->SetMarkerStyle(0);
      unc->SetMarkerColor(kBlack);
      unc->SetFillColor(kBlack);
      unc->SetFillStyle(3254);
    }

    void Init(TChain* chain, std::string topBranchName, std::string replaceVar)
    {
      for(auto& formulax : formulaex)
        boost::replace_all(formulax,        "top.", topBranchName);
      for(auto& formulay : formulaey)
        boost::replace_all(formulay,        "top.", topBranchName);
      boost::replace_all(selection,         "top.", topBranchName);
      boost::replace_all(CustomHistoWeight, "top.", topBranchName);
      std::vector<std::string> tempformulax = formulaex;
      std::vector<std::string> tempformulay = formulaey;
      std::string tempCustomHistoWeight = CustomHistoWeight;
      //re-initialize varx/vary if needed
      if(replaceVar!=""){
    	  std::vector<std::string> vsPars;
    	  boost::split(vsPars, replaceVar, boost::is_any_of("|"));
    	  assert(vsPars.size()==2);
    	  for(auto& formulax : formulaex) boost::replace_all(formulax,  vsPars.at(0), vsPars.at(1));
    	  for(auto& formulay : formulaey) boost::replace_all(formulay,  vsPars.at(0), vsPars.at(1));
    	  boost::replace_all(CustomHistoWeight,  vsPars.at(0), vsPars.at(1));
      }
      varx.clear(); vary.clear();
      for(auto& formulax : formulaex) varx.push_back(new TTreeFormula((std::string("fx")+name).c_str(), formulax.c_str() , chain));
      for(auto& formulay : formulaey) vary.push_back(new TTreeFormula((std::string("fy")+name).c_str(), formulay.c_str() , chain));
      histoweight = new TTreeFormula((std::string("h_weight")+name).c_str(), CustomHistoWeight.c_str() , chain);
      formulaex = tempformulax;
      formulaey = tempformulay;
      CustomHistoWeight = tempCustomHistoWeight;
      for(auto& var : varx){
        if(var->GetNdim() == 0){
          SetInvalid();
        }
      }
      for(auto& var : vary){
        if(var->GetNdim() == 0){
          SetInvalid();
        }
      }
      if (selection.size() > 0) sel = new TTreeFormula((std::string("s")+name).c_str(), selection.c_str(), chain);
    }
    void SetupData(MySample* sample)
    {
        data->SetLineColor(sample->color);
        //data->SetFillColor(sample->color);
        data->SetMarkerColor(sample->color);
        data->SetTitle(sample->name.c_str());
        std::size_t found = sample->name.find("Data");
        if (found==std::string::npos){
        	dataContainsMC=true;
        	//std::cout << "sample defined as data is assumed to contain MC" << std::endl;
        }
    }
    void AddSignal(MySample* sample)
    {
      Helper* helpertemp = new Helper();
      std::vector<std::string> reuseCombinationTypes = helpertemp->readParametersString("analysisConfig.renameCombinationTypes");
      std::vector<double> redefineColorShifts = helpertemp->readParameters("analysisConfig.redefineCombinationTypeColorShifts");
      
      if(redefineColorShifts.size()!=reuseCombinationTypes.size() || reuseCombinationTypes.size()>3){
        std::cout << "Size of analysisConfig.renameCombinationTypes and analysisConfig.redefineCombinationTypeColorShifts configurations do not match or are larger than 3 (currently just reusing correct/wrong/unmatched slots); Check config... exiting!" << std::endl;
        std::_Exit(99);
      }
      
      //      int colorShift[] =  {0, -8, -11};
      //      std::string combinationType[] = {" correct", " wrong", " unmatched"};
      for(unsigned int i = 0; i < redefineColorShifts.size(); ++i) { //has to be smaller than 3 so far
        sig.push_back((TH1*)data->Clone());
        sig.back()->Reset();
        sig.back()->SetLineWidth(1);
        sig.back()->SetFillColor(sample->color+redefineColorShifts[i]);
        sig.back()->SetName(HelperFunctions::cleanedName(data->GetName()+sample->name + reuseCombinationTypes[i]).c_str());
        sig.back()->SetTitle((sample->name + reuseCombinationTypes[i]).c_str());
      }
      
      delete helpertemp;
    }
    void AddBackground(MySample* sample)
    {
      bkg.push_back((TH1*)data->Clone());
      bkg.back()->Reset();
      bkg.back()->SetLineWidth(1);
      bkg.back()->SetFillColor(sample->color);
      bkg.back()->SetName(HelperFunctions::cleanedName(data->GetName()+sample->name).c_str());
      bkg.back()->SetTitle(sample->name.c_str());
    }
    void AddSignalVariation(MySample* sample)
    {
      sigvar.push_back((TH1*)data->Clone());
      sigvar.back()->Reset();
      sigvar.back()->SetLineWidth(2);
      sigvar.back()->SetLineColor(sample->color);
      sigvar.back()->SetLineStyle(sample->line);
      sigvar.back()->SetName(HelperFunctions::cleanedName(data->GetName()+sample->name).c_str());
      sigvar.back()->SetTitle(sample->name.c_str());
    }
    void AddNormVariation(MySample* sample, std::string normUnc)
    {
      sigvar.push_back((TH1*)data->Clone());
      sigvar.back()->Reset();
      sigvar.back()->SetLineWidth(2);
      sigvar.back()->SetLineColor(sample->color);
      sigvar.back()->SetLineStyle(sample->line);
      sigvar.back()->SetFillStyle(0);
      sigvar.back()->SetName(HelperFunctions::cleanedName(data->GetName()+sample->name+normUnc).c_str());
      sigvar.back()->SetTitle(std::string(sample->name+std::string(" ")+normUnc).c_str());
    }
    
    std::vector<TH1F*> Sigvar1D(){
      std::vector<TH1F*> sigvar1D;
      if(histogramDimension == 1){
        for (size_t i=0; i<sigvar.size(); i++)sigvar1D.push_back((TH1F*) sigvar.at(i));
      }
      return sigvar1D;
    }
    std::vector<TH1F*> Sig1D(){
      std::vector<TH1F*> sig1D;
      if(histogramDimension == 1){
        for (size_t i=0; i<sig.size(); i++)sig1D.push_back((TH1F*) sig.at(i));
      }
      return sig1D;
    }
    std::vector<TH1F*> Bkg1D(){
      std::vector<TH1F*> bkg1D;
      if(histogramDimension == 1){
        for (size_t i=0; i<bkg.size(); i++)bkg1D.push_back((TH1F*) bkg.at(i));
      }
      return bkg1D;
    }
    TH1F* Data1D(){
      return (TH1F*) data;
    }

    void setData1D(TH1F* newData){
      data = newData;
    }
    
    TH1F* Unc1D(){
      return (TH1F*) unc;
    }

    std::vector<TH2F*> Sigvar2D(){
      std::vector<TH2F*> sigvar2D;
      if(histogramDimension == 2){
        for (size_t i=0; i<sigvar.size(); i++)sigvar2D.push_back((TH2F*) sigvar.at(i));
      }
      return sigvar2D;
    }
    std::vector<TH2F*> Sig2D(){
      std::vector<TH2F*> sig2D;
      if(histogramDimension == 2){
        for (size_t i=0; i<sig.size(); i++)sig2D.push_back((TH2F*) sig.at(i));
      }
      return sig2D;
    }
    std::vector<TH2F*> Bkg2D(){
      std::vector<TH2F*> bkg2D;
      if(histogramDimension == 2){
        for (size_t i=0; i<bkg.size(); i++)bkg2D.push_back((TH2F*) bkg.at(i));
      }
      return bkg2D;
    }
    TH2F* Data2D(){
      return (TH2F*) data;
    }

    const short Dimension(){
      return histogramDimension;
    }

    void SetInvalid(){
      histogramDimension = -1;
    }

    bool DataContainsMC(){
      return dataContainsMC;
    }

    void ConfigureExtraOptions(bool SetFitGaussToCore, std::string SetCustomHistoweight="1.", bool ExportSigVarToRoot = false, std::string LegendHeader="", bool useLogXBins = false, bool setLogYOnPads = false, bool useLogYBins = false);

    void ConfigureMoreExtraOptions(bool plotStackNormalized=false);
    
    void SetExtraLabel(std::string label);

    void ConfigurePlotRanges(double PlotRangeYRatioMin, double PlotRangeYRatioMax, double PlotRangeYMin=-99, double PlotRangeYMax=-99);


    void SetFitGaussToCore(){
      fitGaussToCore = true;
    }

    bool FitGaussToCore(){
      return fitGaussToCore;
    }

    bool ExportSigVarToRoot(){
      return exportSigVarToRoot;
    }

    bool LogX(){
      return logX;
    }

    bool LogY(){
      return logY;
    }

    bool PlotStackNorm(){
      return plotStackNorm;
    }


    std::vector<TTreeFormula*> varx;
    std::vector<TTreeFormula*> vary;
    TTreeFormula* sel;
    TTreeFormula* histoweight;
    std::string name, selection, CustomHistoWeight;
    std::vector<std::string> formulaex, formulaey;
    std::string legendHeader, extraLabel;
    double plotRangeYMin, plotRangeYMax, plotRangeYRatioMin, plotRangeYRatioMax;
  private:
    TH1* data;
    TH1* unc;
    std::vector<TH1*> sig, bkg, sigvar, normvar;
    short histogramDimension;
    bool dataContainsMC;
    bool fitGaussToCore;
    bool exportSigVarToRoot, logX, logY, plotStackNorm;
  };

  class MyBRegVarInfo{
  public:
    MyBRegVarInfo(){
      init();
      CummulativeVars.push_back("BRegJet_jetPtCorr");
      CummulativeVars.push_back("BRegJet_jetEta");
      CummulativeVars.push_back("BRegJet_Rho25");
      CummulativeVars.push_back("BRegJet_jetArea");
      CummulativeVars.push_back("BRegJet_EtWeightedSigmaPhi");
      CummulativeVars.push_back("jet_fChargedHadron");
      CummulativeVars.push_back("jet_fElectron");
      CummulativeVars.push_back("jet_fMuon");
      CummulativeVars.push_back("BRegJet_leadingChargedConstPt");
      CummulativeVars.push_back("BRegJet_SV3DLength");
      CummulativeVars.push_back("BRegJet_SV3DLengthError");
      CummulativeVars.push_back("BRegJet_SVMass");
      CummulativeVars.push_back("BRegJet_SVPt");
      CummulativeVars.push_back("jet_bTagCSV");
      CummulativeVars.push_back("BRegJet_SoftMuonPt");
      CummulativeVars.push_back("BRegJet_SoftMuonRatioRel");
      CummulativeVars.push_back("BRegJet_SoftMuonDeltaR");
      CummulativeVars.push_back("BRegJet_SoftElectronPt");
      CummulativeVars.push_back("BRegJet_SoftElectronRatioRel");
      CummulativeVars.push_back("BRegJet_SoftElectronDeltaR");
      CummulativeVars.push_back("BRegJet_jetPtRaw");
      CummulativeVars.push_back("BRegJet_jetMt");
      CummulativeVars.push_back("BRegJet_jesTotUnc");
      CummulativeVars.push_back("jet_nConstituents");
      CummulativeVars.push_back("jet_nChargedHadrons");
      CummulativeVars.push_back("BRegJet_nChargedPFConstituents");
      CummulativeVars.push_back("BRegJet_RlbReco");
      CummulativeVars.push_back("BRegJet_QGaxis1");
      CummulativeVars.push_back("BRegJet_QGaxis2");
      CummulativeVars.push_back("BRegJet_QGMult");
      CummulativeVars.push_back("BRegJet_QGPtD");
      CummulativeVars.push_back("BRegJet_QGMLP");
      CummulativeVars.push_back("BRegJet_PUIddZ");
      CummulativeVars.push_back("BRegJet_PUIddRMean");
      CummulativeVars.push_back("BRegJet_PUIddr2Mean");
      CummulativeVars.push_back("BRegJet_PUIdfrac01");
      CummulativeVars.push_back("BRegJet_PUIdfrac02");
      CummulativeVars.push_back("BRegJet_PUIdfrac03");
      CummulativeVars.push_back("BRegJet_PUIdfrac04");
      CummulativeVars.push_back("BRegJet_PUIdfrac05");
      CummulativeVars.push_back("BRegJet_PUIdbeta");

    }
    void addVariable(std::string varName, std::string varForm, float xMin, float xMax){
      varNames.push_back(varName);
      varForms.push_back(varForm);
      xMins.push_back(xMin);
      xMaxs.push_back(xMax);
    }
    void addBasicVariables(){
        addVariable("#rho_{25}"                               , "BRegJet.Rho25"                     , 0   , 50    );
        addVariable("jet area"                                , "BRegJet.jetArea"                   , 0.5 , 1     );
        addVariable("width in #varphi"                        , "BRegJet.EtWeightedSigmaPhi"        , 0   , 0.3   );  //5 width and PU
        addVariable("leading chargedConstPt"                  , "BRegJet.leadingChargedConstPt"     , 0   , 200   ); 
        addVariable("p_{T}^{leading charged const.}/corr. p_{T}", "BRegJet.leadingChargedConstPt/BRegJet.jetPtCorr"     , 0   , 1   );
        addVariable("CHF"                                     , "jet.fChargedHadron"                , 0   , 1     );
        addVariable("electron fraction"                       , "jet.fElectron"                     , 0   , 1     );
        addVariable("muon fraction"                           , "jet.fMuon"                         , 0   , 1     );  //9 charged constituents
        addVariable("electron fraction "                      , "jet.fElectron"                     , 0.01, 1     );
        addVariable("muon fraction "                          , "jet.fMuon"                         , 0.01, 1     );  //9 charged constituents
        addVariable("SV flight length"                        , "BRegJet.SV3DLength"                , 0   , 5     );
        addVariable("SV flight length unc."                   , "BRegJet.SV3DLengthError"           , 0   , 1     );
        addVariable("SV mass"                                 , "BRegJet.SVMass"                    , 0   , 20    );
        addVariable("SV p_{T}"                                , "BRegJet.SVPt"                      , 0   , 200   );
        addVariable("relative SV p_{T} fraction"              , "BRegJet.SVPt/BRegJet.jetPtCorr"    , 0   , 1     );
        addVariable("CSV b-tag"                               , "jet.bTagCSV"                       , 0   , 1     );  //14 SVtx and btag
        addVariable("p_{T}^{SoftMuon}"                        , "BRegJet.SoftMuonPt"                , 0   , 200   );
        addVariable("relative p_{T}^{SoftMuon} fraction"      , "BRegJet.SoftMuonRatioRel"          , 0   , 2     );
        addVariable("#Delta R(SoftMuon)"                      , "BRegJet.SoftMuonDeltaR"            , 0   , 0.5   );
        addVariable("p_{T}^{SoftElectron}"                    , "BRegJet.SoftElectronPt"            , 0   , 200   );
        addVariable("relative p_{T}^{SoftElectron} fraction"  , "BRegJet.SoftElectronRatioRel"      , 0   , 2     );
        addVariable("#Delta R(SoftElectron)"                  , "BRegJet.SoftElectronDeltaR"        , 0   , 0.5   );  //20 soft lepton variables
        addVariable("raw p_{T}"                               , "BRegJet.jetPtRaw"                  , 0   , 500   );
        addVariable("JES corr. factor"                        , "BRegJet.jetPtCorr/BRegJet.jetPtRaw", 0   , 3     );
        addVariable("m_{T}"                                   , "BRegJet.jetMt"                     , 0   , 500   );
        addVariable("JES uncertainty"                         , "BRegJet.jesTotUnc"                 , 0   , 0.05  );
        addVariable("number of constituents"                  , "jet.nConstituents"                 , 0   , 100   );
        addVariable("number of charged hadrons"               , "jet.nChargedHadrons"               , 0   , 50    );
        addVariable("number of charged PF candidates"         , "BRegJet.nChargedPFConstituents"    , 0   , 50    );  //26 more jet info, multiplicities
        addVariable("RlbReco"                                 , "BRegJet.RlbReco"                   , 0   , 2     );  //27 add ATLAS 3D variable into training
        //add QGL and PU Id variables
        addVariable("BRegJet.QGaxis1"      ,"BRegJet.QGaxis1"      , 0    ,   0.3);
        addVariable("BRegJet.QGaxis2"      ,"BRegJet.QGaxis2"      , 0    ,   0.3);
        addVariable("BRegJet.QGMult"       ,"BRegJet.QGMult"       , 0    ,   30 );
        addVariable("BRegJet.QGPtD"        ,"BRegJet.QGPtD"        , 0    ,   1  );
        addVariable("BRegJet.QGMLP"        ,"BRegJet.QGMLP"        , 0    ,   1  );

        addVariable("BRegJet.PUIddZ"       ,"BRegJet.PUIddZ"       , 0    ,   2  );
        addVariable("BRegJet.PUIddRMean"   ,"BRegJet.PUIddRMean"   , 0    ,   0.3  );
        addVariable("BRegJet.PUIddr2Mean"  ,"BRegJet.PUIddr2Mean"  , 0    ,   0.1  );
        addVariable("BRegJet.PUIdfrac01"   ,"BRegJet.PUIdfrac01"   , 0    ,   1  );
        addVariable("BRegJet.PUIdfrac02"   ,"BRegJet.PUIdfrac02"   , 0    ,   1  );
        addVariable("BRegJet.PUIdfrac03"   ,"BRegJet.PUIdfrac03"   , 0    ,   1  );
        addVariable("BRegJet.PUIdfrac04"   ,"BRegJet.PUIdfrac04"   , 0    ,   1  );
        addVariable("BRegJet.PUIdfrac05"   ,"BRegJet.PUIdfrac05"   , 0    ,   1  );
        addVariable("BRegJet.PUIdbeta"     ,"BRegJet.PUIdbeta"     , 0    ,   1  );
        addVariable("BRegJet.PUIdbetaStar" ,"BRegJet.PUIdbetaStar" , 0    ,   1  );
        addVariable("BRegJet.PUIdptD"      ,"BRegJet.PUIdptD"      , 0    ,   1  );
    }
    void init(){
      varNames.clear();
      varForms.clear();
      xMins.clear();
      xMaxs.clear();

      addBasicVariables();
      addVariable("corr. p_{T}"                             , "BRegJet.jetPtCorr"                , 0   , 500   );                                 
      addVariable("p_{T}^{parton}"                          , "BRegJet.genPartonPt"              , 0   , 500   );                                 
      addVariable("p_{T}^{gen}"                             , "BRegJet.genJetPt"                 , 0   , 500   );                                 
      addVariable("jet #eta"                                , "BRegJet.jetEta"                    , -3  , 3     );                              
      addVariable("GBR factor"                              , "BRegJet.BRegGBRTrainResult"        , 0.5 , 1.5   );
      addVariable("TMVA factor"                             , "BRegJet.BRegResult"                , 0.5 , 1.5   );
    }

    void initBRegSystematicsTest(){
        varNames.clear();
        varForms.clear();
        xMins.clear();
        xMaxs.clear();
        addVariable("24Pl_1000Tr_Min00250_MCS3TQ07","24PlVars1000TreesMin00250EvtsMCS3TQ07"         ,0.5,1.5  ); //////////////////////////////////////////// default
        addVariable("24Pl_PlPUIdFracSVPtRel_Min00250_MCS3TQ07","24PlVarsPlPUIdFrac01SVPTRelMin00250EvtsMCS3TQ07"         ,0.5,1.5  );


    }

    void initBRegInputVarTest(){
    	init();
        //keep this for nice labels
        for(std::string& varForm : varForms){
        	boost::replace_all(varForm,".","_"      );
        	//when using standalone to define plots:  varForm = "GBRResult"+varForm;
        }
    }

    void initBRegTest(){
        varNames.clear();
        varForms.clear();
        xMins.clear();
        xMaxs.clear();
        addBasicVariables();
        //keep this for nice labels
        for(std::string& varForm : varForms){
        	boost::replace_all(varForm,".","_"      );
        	//when using standalone to define plots:  varForm = "GBRResult"+varForm;
        }
        for(float& xMin : xMins)xMin = 0.5;
        for(float& xMax : xMaxs)xMax = 1.5;

        std::vector<std::string> tempVarForms = varForms;
        varNames.clear();
        varForms.clear();
        xMins.clear();
        xMaxs.clear();
    	char buffer [200];
    	char buffer2 [200];

        for(unsigned int j = 0; j < tempVarForms.size(); ++j){
   	    	sprintf(buffer,"%02d_%s",j,tempVarForms.at(j).c_str());
        	addVariable(buffer,("GBRResult"+tempVarForms.at(j)).c_str()      ,0.5,1.5);
        }
        for(unsigned int j = 0; j < tempVarForms.size(); ++j){
   	    	sprintf(buffer,"%02d_WPtEta_%s",j,tempVarForms.at(j).c_str());
        	addVariable(buffer,("GBRResultWPtEta"+tempVarForms.at(j)).c_str()      ,0.5,1.5);
        }

        for(unsigned int j = 0; j < CummulativeVars.size(); ++j){
   	    	sprintf(buffer,"Cumm_%02d_%s",j,CummulativeVars.at(j).c_str());
   	    	sprintf(buffer2,"GBRResultCumm%02d_%s",j,CummulativeVars.at(j).c_str());
        	addVariable(buffer,buffer2      ,0.5,1.5);
        }

        addVariable("26","26TrainingUsedForNTupels"                      ,0.5,1.5);
        addVariable("26SemiLept","26SemiLeptTrainingUsedForNTupels"              ,0.5,1.5);
        addVariable("24CleanedVar","26CleanedVarTraining"                          ,0.5,1.5);
        addVariable("40_100Trees","40Cleaned100TreesVarTraining"                  ,0.5,1.5);
        addVariable("40_500Trees","40Cleaned500TreesVarTraining"                  ,0.5,1.5);
        addVariable("40_100Trees_Flat","40Cleaned100TreesVarTrainingWithFlatPtTraining",0.5,1.5);
        addVariable("24_100Trees_Flat","24Cleaned100TreesVarTrainingWithFlatPtTraining",0.5,1.5);
        addVariable("42_200Trees_Flat","42Cleaned200TreesVarTrainingWithFlatPtTraining",0.5,1.5);
        addVariable("17_200Trees_Flat","17Cleaned200TreesVarTrainingWithFlatPtTraining",0.5,1.5);
        addVariable("20Sel_200Trees_Flat","20SelectedNoLessPt200TreesVarTrainingWithFlatPtTraining"  ,0.5,1.5);
        addVariable("24Pl_100Trees_Flat","24PlainTradSelection100TreesVarTrainingWithFlatPtTraining",0.5,1.5);
        addVariable("20Sel_100Trees_Flat","20SelectedNoLessPt100TreesVarTrainingWithFlatPtTraining"  ,0.5,1.5);
        addVariable("20Sel_100Trees_Min1000Evt_Flat","20SelectedNoLessPt100TreesMin1000EvtsVarTrainingWithFlatPtTraining"  ,0.5,1.5);
        addVariable("20Sel_200Trees_Min1000Evt_Flat","20SelectedNoLessPt200TreesMin1000EvtsVarTrainingWithFlatPtTraining"  ,0.5,1.5);
        //add 10 December
        addVariable("20Sel_1000Tr_Min2k","20SelVars1000TreesMin2000Evts"                   ,0.5,1.5  );
        addVariable("26Pl_100Tr_Min2k_MCS3","26PlVars100TreesMin2000EvtsMCS3"              ,0.5,1.5  );
        addVariable("20Sel_200Tr_Min2k_MCS3","20SelVars200TreesMin2000EvtsMCS3"            ,0.5,1.5  );
        addVariable("20Sel_200Tr_Min2k_MCS3_JERC","20SelVars200TreesMin2000EvtsMCS3_JERC"  ,0.5,1.5  );
        addVariable("20Sel_200Tr_Min500_MCS3_JERC","20SelVars200TreesMin500EvtsMCS3_JERC"  ,0.5,1.5  );
        addVariable("43Pl_200Tr_Min2k_JERC","43PlVars200TreesMin2000EvtsMCS3_JERC"         ,0.5,1.5  );

        addVariable("20Sel_TMVA_JERC","TMVAtest20SelVars"         ,0.5,1.5  );
        addVariable("24Pl_TMVA_small","TMVAtest24CleanedVarSmallTr"         ,0.5,1.5  );

        addVariable("43Pl_1000Tr_Min2k_MCS3_JERC","43PlVars1000TreesMin2000EvtsMCS3_JERC"         ,0.5,1.5  );

        addVariable("26Pl_1000Tr_Min2k_MCS8_JERC_NoFlat","26PlVars1000TreesMin2000EvtsMCS8_NoFlat_JERC"         ,0.5,1.5  );
        addVariable("24Pl_1000Tr_Min2k_MCS8_JERC_NoFlat","24PlVars1000TreesMin2000EvtsMCS8_NoFlat_JERC"         ,0.5,1.5  );
        addVariable("43Pl_1000Tr_Min2k_MCS3_JERC_NoFlat","43PlVars1000TreesMin2000EvtsMCS3_NoFlat_JERC"         ,0.5,1.5  );
        addVariable("43Pl_1000Tr_Min2k_MCS5_JERC_NoFlat","43PlVars1000TreesMin2000EvtsMCS5_NoFlat_JERC"         ,0.5,1.5  );

        addVariable("10Sel1KTMin2KMCS8TQ07_NoFlat_JERC", "10Sel1KTMin2KMCS8TQ07_NoFlat_JERC",0.5,1.5);
        addVariable("10Sel1KTMin2KMCS8TQ05_NoFlat_JERC", "10Sel1KTMin2KMCS8TQ05_NoFlat_JERC",0.5,1.5);
        addVariable("10Sel1KTMin2KMCS8TQ09_NoFlat_JERC", "10Sel1KTMin2KMCS8TQ09_NoFlat_JERC",0.5,1.5);
        addVariable("20Sel1KTMin2KMCS8TQ09_NoFlat_JERC", "20Sel1KTMin2KMCS8TQ09_NoFlat_JERC",0.5,1.5);
        addVariable("20Sel1KTMin2KMCS8TQ07_NoFlat_JERC", "20Sel1KTMin2KMCS8TQ07_NoFlat_JERC",0.5,1.5);
        addVariable("24Pl_1000Tr_Min2k_MCS8","24PlVars1000TreesMin2000EvtsMCS8"         ,0.5,1.5  );
        addVariable("24Pl_1000Tr_Min2k_MCS5","24PlVars1000TreesMin2000EvtsMCS5"         ,0.5,1.5  );
        addVariable("24Pl_1000Tr_Min1k_MCS5TQ07","24PlVars1000TreesMin1000EvtsMCS5TQ07"         ,0.5,1.5  );
        addVariable("24Pl_1000Tr_Min500_MCS5TQ07","24PlVars1000TreesMin500EvtsMCS5TQ07"         ,0.5,1.5  );
        addVariable("24Pl_1000Tr_Min250_MCS5TQ07","24PlVars1000TreesMin250EvtsMCS5TQ07"         ,0.5,1.5  );

        addVariable("24Pl_1000Tr_Min00250_MCS1TQ07","24PlVars1000TreesMin00250EvtsMCS1TQ07"         ,0.5,1.5  );
        addVariable("24Pl_1000Tr_Min00250_MCS2TQ07","24PlVars1000TreesMin00250EvtsMCS2TQ07"         ,0.5,1.5  );
        addVariable("24Pl_1000Tr_Min00250_MCS3TQ07","24PlVars1000TreesMin00250EvtsMCS3TQ07"         ,0.5,1.5  ); //////////////////////////////////////////// default
        addVariable("24Pl_1000Tr_Min00500_MCS3TQ07","24PlVars1000TreesMin00500EvtsMC35TQ07"         ,0.5,1.5  );//MCS3, actually
        addVariable("24Pl_1000Tr_Min01000_MCS3TQ07","24PlVars1000TreesMin01000EvtsMC35TQ07"         ,0.5,1.5  );//MCS3, actually

        addVariable("24Pl_1000Tr_Min2k_MCS3","24PlVars1000TreesMin2000EvtsMCS3"         ,0.5,1.5  );
        addVariable("24Pl_1000Tr_Min2k_MCS8TQ09","24PlVars1000TreesMin2000EvtsMCS8TQ09"         ,0.5,1.5  );
        addVariable("24Pl_1000Tr_Min2k_MCS8TQ07","24PlVars1000TreesMin2000EvtsMCS8TQ07"         ,0.5,1.5  );
        addVariable("24Pl_1000Tr_Min2k_MCS8TQ05","24PlVars1000TreesMin2000EvtsMCS8TQ05"         ,0.5,1.5  );

        addVariable("24Pl_PlPUIdFrac_Min00250_MCS3TQ07","24PlVarsPlPUIdFrac01Min00250EvtsMCS3TQ07"         ,0.5,1.5  );
        addVariable("24Pl_PlPUIdFracSVPtRel_Min00250_MCS3TQ07","24PlVarsPlPUIdFrac01SVPTRelMin00250EvtsMCS3TQ07"         ,0.5,1.5  );
        addVariable("20Pl_1000Tr_Min00250_MCS3TQ07","20PlVars1000TreesMin00250EvtsMCS3TQ07"         ,0.5,1.5  );
        addVariable("22PlPlJECLCCPt_1000Tr_Min00250_MCS3TQ07","22PlVarsPlJECLCCPt1000TreesMin00250EvtsMCS3TQ07"         ,0.5,1.5  );
        addVariable("17Pl_1000Tr_Min00250_MCS3TQ07","17PlVars1000TreesMin00250EvtsMCS3TQ07"         ,0.5,1.5  );
        addVariable("22PlPlPUId_1000Tr_Min00250_MCS3TQ07","22PlVarsPlPUId1000TreesMin00250EvtsMCS3TQ07"         ,0.5,1.5  );

        addVariable("16Pl_1000Tr_Min00250_MCS3TQ07","16PlVars1000TreesMin00250EvtsMCS3TQ07"         ,0.5,1.5  );
        addVariable("15Pl_1000Tr_Min00250_MCS3TQ07","15PlVars1000TreesMin00250EvtsMCS3TQ07"         ,0.5,1.5  );
        addVariable("14Pl_1000Tr_Min00250_MCS3TQ07","14PlVars1000TreesMin00250EvtsMCS3TQ07"         ,0.5,1.5  );
        addVariable("12Pl_1000Tr_Min00250_MCS3TQ07","12PlVars1000TreesMin00250EvtsMCS3TQ07"         ,0.5,1.5  );

	//genpt tests
	
	addVariable("20SpecialGenPtTraining00250EvtsMCS3TQ07","20SpecialGenPtTraining00250EvtsMCS3TQ07",0.5,1.5); //85 trees
	addVariable("20SpecialGenPtTraining00250EvtsMCS3TQ07PeterTrain","20SpecialGenPtTraining00250EvtsMCS3TQ07PeterTrain",0.5,1.5); //92 trees
	addVariable("17SpecialGenPtTraining00250EvtsMCS3TQ07PeterTrain","17SpecialGenPtTraining00250EvtsMCS3TQ07PeterTrain",0.5,1.5); //
	addVariable("20SpecialGenPtTraining00250EvtsMCS3TQ07PeterTrainRatioRel","20SpecialGenPtTraining00250EvtsMCS3TQ07PeterTrainRatioRel",0.5,1.5);

	//test reweightings
	addVariable("24PlVarsNoPtReweight","24PlVarsNoPtReweight",0.5,1.5); // 140 trees, 10 identical
	addVariable("24PlVarsCorrPtReweight","24PlVarsCorrPtReweight",0.5,1.5); //63 trees
	addVariable("24PlVarsGenPtReweight","24PlVarsGenPtReweight",0.5,1.5);  //95 trees, 6 identical
	addVariable("24PlVarsGenPtTrainAndReweight","24PlVarsGenPtTrainAndReweight",0.5,1.5);  //131 trees
	addVariable("24PlVarsGenPtTrainAndCorrPtReweight","24PlVarsGenPtTrainAndCorrPtReweight",0.5,1.5);  //127 trees
	addVariable("24PlVarsGenPtTrainAndNoPtReweight","24PlVarsGenPtTrainAndNoPtReweight",0.5,1.5);  //364 trees


    }

    std::vector<std::string> varNames;
    std::vector<std::string> varForms;
    std::vector<float> xMins;
    std::vector<float> xMaxs;
    std::vector<std::string> CummulativeVars;

  };


  void doPlots();

  std::vector<MyHistogram> hists;
  std::vector<MySample>    samples;

  std::string selection_   ;
  std::string selectionCP_ ;
  std::string selectionWP_ ;
  std::string selectionUN_ ;

  std::string path_;
  std::string outPath_;
  std::string channel_;
  std::string topBranchName_;
  double lumi_;

};

#endif /* TOPMASSCONTROLPLOTS_H */
