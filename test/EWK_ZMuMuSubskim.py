import FWCore.ParameterSet.Config as cms

process = cms.Process("TestZMuMuSubskim")

process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )

# source
process.source = cms.Source("PoolSource", 
     fileNames = cms.untracked.vstring(
#"file:/tmp/degrutto/F8EE38AF-1EBE-DE11-8D19-00304891F14E.root"
     'rfio:/castor/cern.ch/user/f/fabozzi/mc7tev/F8EE38AF-1EBE-DE11-8D19-00304891F14E.root'
#    'file:/scratch1/cms/data/summer09/aodsim/zmumu/0016/889E7356-0084-DE11-AF48-001E682F8676.root'
#    'file:testEWKMuSkim.root'
    )
)
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.GlobalTag.globaltag = cms.string('START37_V1A::All')
process.load("Configuration.StandardSequences.MagneticField_cff")

############
## no MC truth and  on data
process.load("ElectroWeakAnalysis.Skimming.zMuMu_SubskimPaths_cff")

# Output module configuration
process.load("ElectroWeakAnalysis.Skimming.zMuMuSubskimOutputModule_cfi")
process.zMuMuSubskimOutputModule.fileName = 'testZMuMuSubskim.root'



############
# MC truth matching sequence
#process.load("ElectroWeakAnalysis.Skimming.zMuMu_SubskimPathsWithMCTruth_cff")
#process.zMuMuSubskimOutputModule.outputCommands.extend(process.mcEventContent.outputCommands)
############

process.outpath = cms.EndPath(process.zMuMuSubskimOutputModule)


