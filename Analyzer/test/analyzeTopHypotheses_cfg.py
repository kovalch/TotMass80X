import FWCore.ParameterSet.Config as cms

import FWCore.ParameterSet.VarParsing as VarParsing
# setup 'standard' options
options = VarParsing.VarParsing ('standard')
# cone size variation
options.register('jes','abs',VarParsing.VarParsing.multiplicity.singleton,
VarParsing.VarParsing.varType.string   , "Jet Energy Scale")
# get and parse the command line arguments
options.parseArguments()

process = cms.Process("TEST")

## configure message logger
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 100
process.MessageLogger.cerr.threshold = cms.untracked.string('INFO')
process.MessageLogger.categories.append('TtSemiLeptonicEvent')
process.MessageLogger.cerr.TtSemiLeptonicEvent = cms.untracked.PSet(
    limit = cms.untracked.int32(-1)
)

## define input
readFiles = cms.untracked.vstring()
secFiles = cms.untracked.vstring() 
process.source = cms.Source ("PoolSource",fileNames = readFiles, secondaryFileNames = secFiles)
readFiles.extend( [
#       '/store/user/snaumann/TTJets_TuneD6T_7TeV-madgraph-tauola/SingleMuSkim_1/77c3ad1ea85046d30b430315aa7f3138/PAT_9_1_CJg.root',
#       '/store/user/snaumann/TTJets_TuneD6T_7TeV-madgraph-tauola/SingleMuSkim_1/77c3ad1ea85046d30b430315aa7f3138/PAT_8_1_wR2.root',
#       '/store/user/snaumann/TTJets_TuneD6T_7TeV-madgraph-tauola/SingleMuSkim_1/77c3ad1ea85046d30b430315aa7f3138/PAT_7_1_E8P.root',
       '/store/user/snaumann/TTJets_TuneD6T_7TeV-madgraph-tauola/SingleMuSkim_1/77c3ad1ea85046d30b430315aa7f3138/PAT_6_1_nVC.root',
       '/store/user/snaumann/TTJets_TuneD6T_7TeV-madgraph-tauola/SingleMuSkim_1/77c3ad1ea85046d30b430315aa7f3138/PAT_5_1_3U0.root',
       '/store/user/snaumann/TTJets_TuneD6T_7TeV-madgraph-tauola/SingleMuSkim_1/77c3ad1ea85046d30b430315aa7f3138/PAT_4_1_trz.root',
       '/store/user/snaumann/TTJets_TuneD6T_7TeV-madgraph-tauola/SingleMuSkim_1/77c3ad1ea85046d30b430315aa7f3138/PAT_3_1_bG3.root',
       '/store/user/snaumann/TTJets_TuneD6T_7TeV-madgraph-tauola/SingleMuSkim_1/77c3ad1ea85046d30b430315aa7f3138/PAT_30_1_Iz0.root',
       '/store/user/snaumann/TTJets_TuneD6T_7TeV-madgraph-tauola/SingleMuSkim_1/77c3ad1ea85046d30b430315aa7f3138/PAT_2_1_2Fs.root',
       '/store/user/snaumann/TTJets_TuneD6T_7TeV-madgraph-tauola/SingleMuSkim_1/77c3ad1ea85046d30b430315aa7f3138/PAT_29_1_5n5.root',
       '/store/user/snaumann/TTJets_TuneD6T_7TeV-madgraph-tauola/SingleMuSkim_1/77c3ad1ea85046d30b430315aa7f3138/PAT_28_1_rvr.root',
       '/store/user/snaumann/TTJets_TuneD6T_7TeV-madgraph-tauola/SingleMuSkim_1/77c3ad1ea85046d30b430315aa7f3138/PAT_27_1_kh4.root',
       '/store/user/snaumann/TTJets_TuneD6T_7TeV-madgraph-tauola/SingleMuSkim_1/77c3ad1ea85046d30b430315aa7f3138/PAT_26_1_o23.root',
       '/store/user/snaumann/TTJets_TuneD6T_7TeV-madgraph-tauola/SingleMuSkim_1/77c3ad1ea85046d30b430315aa7f3138/PAT_25_1_FNk.root',
       '/store/user/snaumann/TTJets_TuneD6T_7TeV-madgraph-tauola/SingleMuSkim_1/77c3ad1ea85046d30b430315aa7f3138/PAT_24_1_lZT.root',
       '/store/user/snaumann/TTJets_TuneD6T_7TeV-madgraph-tauola/SingleMuSkim_1/77c3ad1ea85046d30b430315aa7f3138/PAT_23_1_DYq.root',
       '/store/user/snaumann/TTJets_TuneD6T_7TeV-madgraph-tauola/SingleMuSkim_1/77c3ad1ea85046d30b430315aa7f3138/PAT_22_1_qOI.root',
       '/store/user/snaumann/TTJets_TuneD6T_7TeV-madgraph-tauola/SingleMuSkim_1/77c3ad1ea85046d30b430315aa7f3138/PAT_21_1_4UB.root',
       '/store/user/snaumann/TTJets_TuneD6T_7TeV-madgraph-tauola/SingleMuSkim_1/77c3ad1ea85046d30b430315aa7f3138/PAT_20_1_m9t.root',
       '/store/user/snaumann/TTJets_TuneD6T_7TeV-madgraph-tauola/SingleMuSkim_1/77c3ad1ea85046d30b430315aa7f3138/PAT_1_1_lrs.root',
       '/store/user/snaumann/TTJets_TuneD6T_7TeV-madgraph-tauola/SingleMuSkim_1/77c3ad1ea85046d30b430315aa7f3138/PAT_19_1_dNE.root',
       '/store/user/snaumann/TTJets_TuneD6T_7TeV-madgraph-tauola/SingleMuSkim_1/77c3ad1ea85046d30b430315aa7f3138/PAT_18_1_q7z.root',
       '/store/user/snaumann/TTJets_TuneD6T_7TeV-madgraph-tauola/SingleMuSkim_1/77c3ad1ea85046d30b430315aa7f3138/PAT_17_1_BYv.root',
       '/store/user/snaumann/TTJets_TuneD6T_7TeV-madgraph-tauola/SingleMuSkim_1/77c3ad1ea85046d30b430315aa7f3138/PAT_16_1_TTE.root',
       '/store/user/snaumann/TTJets_TuneD6T_7TeV-madgraph-tauola/SingleMuSkim_1/77c3ad1ea85046d30b430315aa7f3138/PAT_15_1_0Pf.root',
       '/store/user/snaumann/TTJets_TuneD6T_7TeV-madgraph-tauola/SingleMuSkim_1/77c3ad1ea85046d30b430315aa7f3138/PAT_14_1_rj6.root',
       '/store/user/snaumann/TTJets_TuneD6T_7TeV-madgraph-tauola/SingleMuSkim_1/77c3ad1ea85046d30b430315aa7f3138/PAT_13_1_Zp3.root',
       '/store/user/snaumann/TTJets_TuneD6T_7TeV-madgraph-tauola/SingleMuSkim_1/77c3ad1ea85046d30b430315aa7f3138/PAT_12_1_KtO.root',
       '/store/user/snaumann/TTJets_TuneD6T_7TeV-madgraph-tauola/SingleMuSkim_1/77c3ad1ea85046d30b430315aa7f3138/PAT_11_1_ljE.root',
       '/store/user/snaumann/TTJets_TuneD6T_7TeV-madgraph-tauola/SingleMuSkim_1/77c3ad1ea85046d30b430315aa7f3138/PAT_10_1_MRa.root' ] );



secFiles.extend( [
               ] )

## define maximal number of events to loop over
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

## configure process options
process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool(False)
)

## configure geometry & conditions
process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.GlobalTag.globaltag = cms.string('MC_38Y_V14::All')

## std sequence for pat
process.load("PhysicsTools.PatAlgos.patSequences_cff")

process.load("TopAnalysis.TopFilter.sequences.semiLeptonicSelection_cff")

## configure JetEnergyScale tool
process.load("TopAnalysis.TopUtils.JetEnergyScale_cff")
from TopAnalysis.TopUtils.JetEnergyScale_cff import *

scaledJetEnergy.scaleType   = cms.string(options.jes)
scaledJetEnergy.inputJets   = "selectedPatJetsAK5PF"
#scaledJetEnergy.scaleFactor = 1.053
#scaledJetEnergy.resolutionFactor = 1.1


## sequences for ttGenEvent and TtSemiLeptonicEvent
process.load("TopQuarkAnalysis.TopEventProducers.sequences.ttGenEvent_cff")

process.load("TopQuarkAnalysis.TopEventProducers.sequences.ttSemiLepEvtBuilder_cff")

## enable additional per-event printout from the TtSemiLeptonicEvent
process.ttSemiLepEvent.verbosity = 0
## change maximum number of jets taken into account per event (default: 4)
from TopQuarkAnalysis.TopEventProducers.sequences.ttSemiLepEvtBuilder_cff import *
setForAllTtSemiLepHypotheses(process, "jets", "selectedPatJetsAK5PF")
setForAllTtSemiLepHypotheses(process, "maxNComb", -1)

process.TtSemiLepJetCombMVAFileSource = cms.ESSource("TtSemiLepJetCombMVAFileSource",
  ttSemiLepJetCombMVA = cms.FileInPath('TopMass/Configuration/data/TtSemiLepJetComb.mva')
)

## change jet-parton matching algorithm
process.ttSemiLepJetPartonMatch.algorithm = "unambiguousOnly"
#process.ttSemiLepJetPartonMatch.maxDist   = 0.3

process.ttSemiLepJetPartonMatch.maxNComb = -1
#process.findTtSemiLepJetCombMVA.maxNComb = -1

#process.kinFitTtSemiLepEventHypothesis.maxNComb = -1
process.kinFitTtSemiLepEventHypothesis.useBTagging = True
# 1: Whad-mass, 2: Wlep-mass, 3: thad-mass, 4: tlep-mass, 5: nu-mass, 6: equal t-masses
process.kinFitTtSemiLepEventHypothesis.constraints = 1, 2, 6

## choose which hypotheses to produce
addTtSemiLepHypotheses(process,
                       ["kMVADisc","kKinFit"]
                       )
#removeTtSemiLepHypGenMatch(process)

## load HypothesisAnalyzer
process.load("TopMass.Analyzer.EventHypothesisAnalyzer_cff")

# register TFileService
process.TFileService = cms.Service("TFileService",
    fileName = cms.string('analyzeTop.root')
)

## end path   
process.path = cms.Path(#process.patDefaultSequence *
                        process.scaledJetEnergy *
                        process.semiLeptonicSelection *
                        process.semiLeptonicEvents *
                        process.makeGenEvt *
                        process.makeTtSemiLepEvent *
                        process.analyzeHypotheses
                        )
