#include "analysisHeader.h"

// DCV MPPC Number
#define DCVCH 32
void analysis_DCVSignalMain
(
  std::string RunID         = "92",
  int PeriodID              = 0,
  bool MakeSubChCut         = false,

  bool changeConfigs        = true,
  bool changeAttPathL       = true,
  bool changeN2SubQC        = false,
  bool changeModEneUpdated  = true,
  bool changeTrigCtr        = true
)
{
	// Inputs
	// ======
  if (PeriodID >= ParForCal[ReturnRunIndex(RunID)].size()) {
    DCV_LOG_ABORT("No Parameter set exists");
  }
  
  auto CalConfig = ParForCal[ReturnRunIndex(RunID)][PeriodID];
  if (changeConfigs) {
    CalConfig -> setAttPathL(changeAttPathL);
    CalConfig -> setN2SubQC(changeN2SubQC);
    CalConfig -> setModEneUpdated(changeModEneUpdated);
    CalConfig -> setTrigCtr(changeTrigCtr);
  }

	// Input information
  DCV_LOG_INFO("RunID: " << RunID);
  DCV_LOG_INFO("PeriodID: " << PeriodID);
	
	// Input file
  TFile* Input = nullptr;
  TTree* Tin = nullptr;
	TString InputFileName = Form("%s/Run%s_Period%02d_trackMapping_ttree%s.root", std::getenv("DIR_COSMIC_TRACKED_FILE"), RunID.c_str(), PeriodID, CalConfig -> infoTrigCtr.Data());
	
	if (!gSystem -> IsFileInIncludePath(InputFileName.Data())) {
    DCV_LOG_ABORT("Required input file not found: " << InputFileName);
	} else {
    Input = new TFile(InputFileName, "read");
    Tin = (TTree*)Input->Get("tout");

    if (Tin == nullptr) {
      DCV_LOG_ABORT("Input file exists but ttree not found");
    }
  }

	int CosmicTrackNumber;
	int CosmicTrackID[10];
	int DCVNumber;
	int DCVModID[DCVCH];
	float DCVIntegratedADC[DCVCH];
	short DCVPeak[DCVCH];
	float DCVTime[DCVCH];

	Tin -> SetBranchAddress("CosmicTrackNumber", &CosmicTrackNumber);
	Tin -> SetBranchAddress("CosmicTrackID", CosmicTrackID);
	Tin -> SetBranchAddress("DCVNumber", &DCVNumber);
	Tin -> SetBranchAddress("DCVModID", DCVModID);
	Tin -> SetBranchAddress("DCVIntegratedADC", DCVIntegratedADC);
	Tin -> SetBranchAddress("DCVPeak", DCVPeak);
	Tin -> SetBranchAddress("DCVTime", DCVTime);

	// Outputs
	// =======

	//OutputFile
	TFile* Output = new TFile(Form("Run%s_Period%02d_DCVSignalMain%s.root", RunID.c_str(), PeriodID, CalConfig -> infoTrigCtr.Data()), "recreate");
  Output -> mkdir("WithSubChConstraint");
	
	vector<vector<TH3D*>> DCVMainSig_Time_vs_Iadp_vs_Peak(12);
	vector<vector<TH3D*>> DCVMainSig_Time_vs_Iadp_vs_Peak_withGoodSub(12);
	TObjArray* hList              = new TObjArray(0);
	TObjArray* hList_withGoodSub  = new TObjArray(0);

	// Making and storing histograms on 2D vector
	for (auto tracksIt = TrackMPPC.begin(); tracksIt != TrackMPPC.end(); tracksIt++) {

		int TID = std::distance(TrackMPPC.begin(), tracksIt);
		auto const& modIds = TrackMPPC[TID];
		
		for (auto it = modIds.begin(); it != modIds.end(); it++) {

			auto const& content = *it;
			auto hisIndex = std::distance( modIds.begin(), it); 

			cout << Form("TrkID: %02d, MainModuleID: %02d", TID, content) << endl;
			
			TString hisName = Form("DCVMainSig_trk%02d_mainMod%02d_time_vs_iadp_vs_peak", TID, content);
			
			DCVMainSig_Time_vs_Iadp_vs_Peak[TID].push_back(new TH3D(hisName, hisName, 500, 0, 100, 160, -50, 30, 160, -100, 1500));
			DCVMainSig_Time_vs_Iadp_vs_Peak_withGoodSub[TID].push_back(new TH3D(hisName+"_withGoodSub", hisName+"_withGoodSub", 500, 0, 100, 160, -50, 30, 160, -100, 1500));
			DCVMainSig_Time_vs_Iadp_vs_Peak[TID][hisIndex] -> Sumw2();
			DCVMainSig_Time_vs_Iadp_vs_Peak_withGoodSub[TID][hisIndex] -> Sumw2();

			if (!DCVMainSig_Time_vs_Iadp_vs_Peak[TID][hisIndex]) {
        DCV_LOG_ABORT("Histogram not safely stored!");
			} else {
			  hList -> Add(DCVMainSig_Time_vs_Iadp_vs_Peak[TID][hisIndex]);
			  hList_withGoodSub -> Add(DCVMainSig_Time_vs_Iadp_vs_Peak_withGoodSub[TID][hisIndex]);
      }
		}
	}

	//Analysis
	for (int i=0; i<Tin -> GetEntries(); i++) {

		Tin -> GetEntry(i);
		
		for (int ii=0; ii<CosmicTrackNumber; ii++) {

			int TrackID = CosmicTrackID[ii];
 			auto const& Modules = TrackPairMPPC[TrackID];
			
			for (auto hisIndex = Modules.begin(); hisIndex != Modules.end(); hisIndex++) {
				// index != module Number
				// index == n-th stored histogram in vector
				// ----------------------------------------
				int index = std::distance(Modules.begin(), hisIndex);
				auto const& pair = *hisIndex; // preventing any changes on original data

				// module ids	
				int mainModId = pair.mainMod;
				int subModId  = pair.subMod;
		    
        // Handling channel swap in Run92b
        if (RunID == "92b" && PeriodID >= 2) {
          if (TrackID == 0 && mainModId == 12) {
            mainModId = pair.subMod;
            subModId = pair.mainMod;
          }

          if (TrackID == 1 && mainModId == 13) {
            mainModId = pair.subMod;
            subModId = pair.mainMod;
          }
        }

				// valuse to be used
				short peak[2]   = {DCVPeak[mainModId], DCVPeak[subModId]};
				float time[2]   = {DCVTime[mainModId], DCVTime[subModId]};
				float intAdc[2] = {DCVIntegratedADC[mainModId], DCVIntegratedADC[subModId]};
				float iadp[2]   = {intAdc[0]/(float)peak[0], intAdc[1]/(float)peak[1]};	
					
				DCVMainSig_Time_vs_Iadp_vs_Peak[TrackID][index] -> Fill(time[0], iadp[0], peak[0]);

        // Save main peaks if sub channel's ADC exceedes 100
        if (MakeSubChCut && peak[1]>100) {
				  //DCVMainSig_Time_vs_Iadp_vs_Peak_withGoodSub[TrackID][index] -> Fill(time[0], iadp[0], peak[0]);
        }
			
			}

		}//Cosmic Track loop

	}//Entry loop
  Input -> Close();		
	Output -> cd("");
	hList -> Write();
  Output -> cd("WithSubChConstraint");
  if (MakeSubChCut) {
    hList_withGoodSub -> Write();
  }
	Output -> Close();
  delete hList;

}
