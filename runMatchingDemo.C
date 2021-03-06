#if !defined(__CLING__) || defined(__ROOTCLING__)

#endif

#include "MUONMatcher.h"

//#ifdef __MAKECINT__
#pragma link C++ class GlobalMuonTrack+;
#pragma link C++ class std::vector<GlobalMuonTrack>+;
#pragma link C++ class tempMCHTrack+;
#pragma link C++ class std::vector<tempMCHTrack>+;
//#endif


MUONMatcher matcher;

//_________________________________________________________________________________________________
// Sample custom matching function that can be passed to MUONMatcher
double MyMatchingFunc (const GlobalMuonTrack& mchTrack, const MFTTrack& mftTrack) {
    auto dx = mchTrack.getX() - mftTrack.getX();
    auto dy = mchTrack.getY() - mftTrack.getY();
    auto score = dx*dx + dy*dy;
    return score;
 };


 //_________________________________________________________________________________________________
 // Sample custom cut criteria that can be passed to MUONMatcher
bool MyMatchingCut (GlobalMuonTrack& mchTrack, MFTTrack& mftTrack) {
   auto cutDistance = 1.0;
   auto dx = mchTrack.getX() - mftTrack.getX();
   auto dy = mchTrack.getY() - mftTrack.getY();
   auto distance = TMath::Sqrt(dx*dx + dy*dy);
   return distance < cutDistance;
  };


//_________________________________________________________________________________________________
// Set Matching function defined by shell variable
void loadAndSetMatchingConfig() {
std::string matching_fcn;
if (gSystem->Getenv("MATCHING_FCN")) {
  matching_fcn = gSystem->Getenv("MATCHING_FCN");
  std::cout << " MATCHING_FCN: " << matching_fcn << std::endl;

  if (matching_fcn.find("matchXY") < matching_fcn.length()) {
    std::cout << " Setting " << matching_fcn << std::endl;
    matcher.setMatchingFunction(&MUONMatcher::matchMFT_MCH_TracksXY);
  }
  if (matching_fcn.find("matchXYPhiTanl") < matching_fcn.length()) {
    std::cout << " Setting " << matching_fcn << std::endl;
    matcher.setMatchingFunction(&MUONMatcher::matchMFT_MCH_TracksXYPhiTanl);
  }
  if (matching_fcn.find("matchALL") < matching_fcn.length()) {
    std::cout << " Setting " << matching_fcn << std::endl;
    matcher.setMatchingFunction(&MUONMatcher::matchMFT_MCH_TracksAllParam);
  }
}


if (gSystem->Getenv("MATCHING_PLANEZ")) {
  double matching_planeZ = atof(gSystem->Getenv("MATCHING_PLANEZ"));
  std::cout << " MATCHING_PLANEZ: " << matching_planeZ << std::endl;
  matcher.SetMatchingPlaneZ(matching_planeZ);
}

std::string matching_cutfcn;
if (gSystem->Getenv("MATCHING_CUTFCN")) {
        matching_cutfcn = gSystem->Getenv("MATCHING_CUTFCN");
        std::cout << " MATCHING_CUTFCN: " << matching_cutfcn << std::endl;

        if (matching_cutfcn.find("cutDisabled") < matching_cutfcn.length()) {
          std::cout << " Setting " << matching_cutfcn << std::endl;
          matcher.setCutFunction(&MUONMatcher::matchCutDisabled);
        }
        if (matching_cutfcn.find("cutDistance") < matching_cutfcn.length()) {
          std::cout << " Setting " << matching_cutfcn << std::endl;
          matcher.setCutFunction(&MUONMatcher::matchCutDistance);
        }
        if (matching_cutfcn.find("cutDistanceSigma") < matching_cutfcn.length()) {
          std::cout << " Setting " << matching_cutfcn << std::endl;
          matcher.setCutFunction(&MUONMatcher::matchCutDistanceSigma);
        }
}


if (gSystem->Getenv("MATCHING_CUTPARAM0")) {
  double matching_cutparam0 = atof(gSystem->Getenv("MATCHING_CUTPARAM0"));
  std::cout << " MATCHING_CUTPARAM0: " << matching_cutparam0 << std::endl;
  matcher.setCutParam(0 , matching_cutparam0);
}

if (gSystem->Getenv("MATCHING_CUTPARAM1")) {
  double matching_cutparam1 = atof(gSystem->Getenv("MATCHING_CUTPARAM1"));
  std::cout << " MATCHING_CUTPARAM1: " << matching_cutparam1 << std::endl;
  matcher.setCutParam(1 , matching_cutparam1);
}


}

//_________________________________________________________________________________________________
int runMatching()  {

// Configure matcher according command line options
// loadAndSetMatchingConfig(); // Commenting this line overrides all command line options

//Custom matching function
matcher.setCustomMatchingFunction(&MyMatchingFunc, "_aliasForMyMatchingFunction");

// Built-in matching functions
//matcher.setMatchingFunction(&MUONMatcher::matchMFT_MCH_TracksXY);
//matcher.setMatchingFunction(&MUONMatcher::matchMFT_MCH_TracksXYPhiTanl);
//matcher.setMatchingFunction(&MUONMatcher::matchMFT_MCH_TracksFull);
//matcher.SetMatchingPlaneZ(-45.3);
matcher.SetMatchingPlaneZ(-80.0);

// Set function to cut MFT candidates
matcher.setCutFunction(&MUONMatcher::matchCutDistance); // By distance
matcher.setCutParam(0 , 1.0); // 1 cm
//matcher.SetVerbosity(true);


// Load MFT tracks and propagates to matching plane
matcher.loadMFTTracksOut();

// Load MCH tracks
matcher.loadMCHTracks();

// Propagate MCH tracks to matching plane and convert parameters and covariances matrix to MFT coordinate system
matcher.initGlobalTracks();

// Runs track matching event-by-event
matcher.runEventMatching();

// Kalman filter
matcher.fitTracks();
matcher.saveGlobalMuonTracks();


/*
// Example: how to access results
std::cout << " *** Matching Summary ***" << std::endl;
auto globalTrackID=0;
for (auto gTrack: matcher.getGlobalMuonTracks() ) {
  if (globalTrackID < 15) std::cout << "Best match to MCH Track " << globalTrackID << " is MFT track " << gTrack.getBestMFTTrackMatchID() << " with chi^2 = " <<  gTrack.getMatchingChi2() << std::endl;
  globalTrackID++;
}
*/
return 0;

}
