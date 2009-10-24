#include <memory>
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/EDFilter.h"

//
// class declaration
//
class EWKMuTkSelector : public edm::EDFilter {
   public:
      explicit EWKMuTkSelector(const edm::ParameterSet&);
      ~EWKMuTkSelector();

   private:
      virtual void beginJob(const edm::EventSetup&) ;
      virtual bool filter(edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;

      edm::InputTag muonTag_;
      edm::InputTag trackTag_;
      double ptCut_;
};

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Common/interface/View.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"

/////////////////////////////////////////////////////////////////////////////////////
EWKMuTkSelector::EWKMuTkSelector(const edm::ParameterSet& pset) {

  // What is being produced
      produces<std::vector<reco::Track> >();
      produces<std::vector<reco::Muon> >();

  // Input products
      muonTag_ = pset.getUntrackedParameter<edm::InputTag> ("MuonTag", edm::InputTag("muons"));
      trackTag_ = pset.getUntrackedParameter<edm::InputTag> ("TrackTag", edm::InputTag("gneralTracks"));
      ptCut_ = pset.getUntrackedParameter<double> ("PtCut");

} 

/////////////////////////////////////////////////////////////////////////////////////
EWKMuTkSelector::~EWKMuTkSelector(){
}

/////////////////////////////////////////////////////////////////////////////////////
void EWKMuTkSelector::beginJob(const edm::EventSetup&) {
}

/////////////////////////////////////////////////////////////////////////////////////
void EWKMuTkSelector::endJob(){}

/////////////////////////////////////////////////////////////////////////////////////
bool EWKMuTkSelector::filter(edm::Event& ev, const edm::EventSetup&) {

      bool selected = false;

      // Muon collection
      edm::Handle<edm::View<reco::Muon> > muonCollection;
      if (!ev.getByLabel(muonTag_, muonCollection)) {
            edm::LogError("") << ">>> Muon collection does not exist !!!";
            return false;
      }
      unsigned int muonCollectionSize = muonCollection->size();

      // Track collection
      edm::Handle<edm::View<reco::Track> > trackCollection;
      if (!ev.getByLabel(trackTag_, trackCollection)) {
            edm::LogError("") << ">>> Track collection does not exist !!!";
            return false;
      }
      unsigned int trackCollectionSize = trackCollection->size();

      // new muon and track collections
      std::auto_ptr<reco::MuonCollection> newmuons (new reco::MuonCollection);
      std::auto_ptr<reco::TrackCollection> newtracks (new reco::TrackCollection);
      reco::TrackRefProd trackRefProd = ev.getRefBeforePut<reco::TrackCollection>();

      // Select tracks for the new collection and set links in the new muon collection
      for (unsigned int j=0; j<trackCollectionSize; ++j) {
            reco::TrackRef tk = (trackCollection->refAt(j)).castTo<reco::TrackRef>(); 
            if (tk->pt()<ptCut_) continue;
            newtracks->push_back(*tk);

            for (unsigned int i=0; i<muonCollectionSize; ++i) {
                  const reco::Muon& mu = muonCollection->at(i);
                  if (mu.innerTrack().isNull()) continue;
                  if (mu.pt()<ptCut_) continue;
                  reco::TrackRef tkInMuon = mu.innerTrack();
                  if (tk==tkInMuon) {
                        reco::Muon* newmu = mu.clone();
                        selected = true;
                        newmu->setInnerTrack(reco::TrackRef(trackRefProd,newtracks->size()-1));
                        // insert it ordered by pt
                        unsigned int newmuonCollectionSize = newmuons->size();
                        double newpt = newmu->pt();
                        bool inserted = false;
                        for (unsigned int k=0; k<newmuonCollectionSize; ++k) {
                              const reco::Muon& mu2 = newmuons->at(k);
                              if (newpt>mu2.pt()) {
                                    newmuons->insert(newmuons->begin()+k,*newmu);
                                    inserted = true;
                                    break;
                              } 
                        }
                        if (!inserted) {
                              newmuons->push_back(*newmu);
                              inserted = true;
                        }
                        break;
                  }
            }
      }

      // Add standalone muons
      for (unsigned int i=0; i<muonCollectionSize; ++i) {
            const reco::Muon& mu = muonCollection->at(i);
            if (!mu.innerTrack().isNull()) continue;
            if (mu.pt()<ptCut_) continue;
            reco::Muon* newmu = mu.clone();
            selected = true;

            // insert it ordered by pt
            unsigned int newmuonCollectionSize = newmuons->size();
            double newpt = newmu->pt();
            bool inserted = false;
            for (unsigned int k=0; k<newmuonCollectionSize; ++k) {
                  const reco::Muon& mu2 = newmuons->at(k);
                  if (newpt>mu2.pt()) {
                        newmuons->insert(newmuons->begin()+k,*newmu);
                        inserted = true;
                        break;
                  } 
            }
            if (!inserted) {
                  newmuons->push_back(*newmu);
                  inserted = true;
            }
      }

      // Write new products
      ev.put(newtracks);
      ev.put(newmuons);
      return selected;
}

DEFINE_FWK_MODULE(EWKMuTkSelector);
