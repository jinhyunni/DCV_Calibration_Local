#include "analysisHeader.h"

enum ChType {
  MainChId=0,
  SubChId
};

void analysis_Make_TimePeakCorr
(
  std::string runId         = "92",
  int periodId              = 0,
  bool changeConfigs        = true,
  bool changeAttPathL       = true,
  bool changeN2SubQC        = false,
  bool changeModEneUpdated  = true,
  bool changeTrigCtr        = true 
)
{
  // Making Inputs
  // ------------- 
  int runIdx = ReturnRunIndex(runId);

  if (periodId >= ParForCal[runIdx].size()) {
    DCV_LOG_ABORT("This period does not contain parameters for calibration");
  } else {
    DCV_LOG_INFO("Run: " << runId);
    DCV_LOG_INFO("Period: " << periodId);
  }

  auto CalConfig = ParForCal[runIdx][periodId];
  if (changeConfigs) {
    CalConfig -> setAttPathL(changeAttPathL);
    CalConfig -> setN2SubQC(changeN2SubQC);
    CalConfig -> setModEneUpdated(changeModEneUpdated);
    CalConfig -> setTrigCtr(changeTrigCtr);
  }

	// Input file
	TString InputFileName = Form("%s/Run%s_Period%02d_trackMapping_ttree%s.root", std::getenv("DIR_COSMIC_TRACKED_FILE"), runId.c_str(), periodId, CalConfig->infoTrigCtr.Data());
  TFile* Input = nullptr;
  TTree* Tin = nullptr;

  if (!std::filesystem::exists(InputFileName.Data())) {
    DCV_LOG_ABORT(Form("No cosmic tracked TTree in Run%s_Period%02d", runId.c_str(), periodId));
  } else {
    Input = new TFile(InputFileName, "read");
    Tin = (TTree*)Input->Get("tout");

    if (Tin == nullptr) {
      DCV_LOG_ABORT("Input file is valid but cannot retrieve ttree for calibration");
    }
  }
  
	int CosmicTrackNumber;
	int CosmicTrackID[10];
	int DCVNumber;
	int DCVModID[32];
	float DCVIntegratedADC[32];
	short DCVPeak[32];
	float DCVTime[32];

	Tin -> SetBranchAddress("CosmicTrackNumber", &CosmicTrackNumber);
	Tin -> SetBranchAddress("CosmicTrackID", CosmicTrackID);
	Tin -> SetBranchAddress("DCVNumber", &DCVNumber);
	Tin -> SetBranchAddress("DCVModID", DCVModID);
	Tin -> SetBranchAddress("DCVIntegratedADC", DCVIntegratedADC);
	Tin -> SetBranchAddress("DCVPeak", DCVPeak);
	Tin -> SetBranchAddress("DCVTime", DCVTime);

  // Making outputs
  // --------------

  // Output file
  TString storeDir = "BaseHists";
  if (!std::filesystem::exists(storeDir.Data())) {
    std::filesystem::create_directory(storeDir.Data());
  }
  TString outputFileName = Form("%s/Run%s_Period%02d_TimePeakCorr%s.root", storeDir.Data(), runId.c_str(), periodId, CalConfig->infoTrigCtr.Data());
  TFile* ofile = new TFile(outputFileName, "recreate");

  // Output histogram -> Histograms are for checking timing signal of sub channel
  //1. Time vs Peak
  //2. Time vs IADP/Peak
  //3. Correlaton of peak of main channel and sub channel
  TObjArray* hList = new TObjArray(0);
  TObjArray* hListMain = new TObjArray(0);

  // QA histograms
  std::vector<std::vector<TH2D*>> h2TimeAdcMB(12);                         // 2
  std::vector<std::vector<TH2D*>> h2TimeIADPMB(12);                        // 3
  std::vector<std::vector<TH2D*>> h2TimeIADPWithSubAdcCut(12);             // 4 
  std::vector<std::vector<TH2D*>> h2MainAdcSubAdcMB(12);                   // 6 
  std::vector<std::vector<TH2D*>> h2MainTimeSubTimeMB(12);                 // 8 
  
  std::vector<std::vector<TH2D*>> h2MainTimeAdcMB(12);                     // 18
  std::vector<std::vector<TH2D*>> h2MainTimeIADPMB(12);                     // 18
  std::vector<std::vector<TH2D*>> h2MainTimeAdcWithGoodSubSignal(12);      // 19

  // Main analysis results
  std::vector<std::vector<TH2D*>> h2DeltaTimeMainSubMBVsSubAdc(12);        // 20 
  std::vector<std::vector<TH2D*>> h2DeltaTimeMainSubMBVsSubTdc(12);        // 21 


  for (auto tracksIt = TrackPairMPPC.begin(); tracksIt != TrackPairMPPC.end(); tracksIt++) {

    int TID = std::distance(TrackPairMPPC.begin(), tracksIt);
    auto const& pairs= TrackPairMPPC[TID];

    for (auto pair = pairs.begin(); pair != pairs.end(); pair++) {
      auto const& currentPair = *pair;
      auto hisIndex = std::distance(pairs.begin(), pair);

      TString hisName2 = Form("DCVSubSig_trk%02d_subMod%02d_time_vs_adc_MB", TID, currentPair.subMod);
      TString hisName3 = Form("DCVSubSig_trk%02d_subMod%02d_time_vs_iadp_MB", TID, currentPair.subMod);
      TString hisName4 = Form("DCVSubSig_trk%02d_subMod%02d_time_vs_iadp_WithSubAdcCut", TID, currentPair.subMod);
      TString hisName6 = Form("DCVCorr_trk%02d_mainMod%02d_subMod%02d_adcCorr_MB", TID, currentPair.mainMod, currentPair.subMod);
      TString hisName8 = Form("DCVCorr_trk%02d_mainMod%02d_subMod%02d_timeCorr_MB", TID, currentPair.mainMod, currentPair.subMod);
      TString hisName18 = Form("DCVMainSig_trk%02d_mainMod%02d_time_vs_adc_MB", TID, currentPair.mainMod);
      TString hisName19 = Form("DCVMainSig_trk%02d_mainMod%02d_time_vs_iadp_MB", TID, currentPair.mainMod);
      TString hisName20 = Form("DCVMainSig_trk%02d_mainMod%02d_time_vs_adc_WithSubAdcCut", TID, currentPair.mainMod);

      TString hisName21 = Form("DCVDeltaTime_Vs_SubModAdc_trk%02d_mainMod%02d_subMod%02d_MB", TID, currentPair.mainMod, currentPair.subMod);
      TString hisName22 = Form("DCVDeltaTime_Vs_SubModTdc_trk%02d_mainMod%02d_subMod%02d_MB", TID, currentPair.mainMod, currentPair.subMod);


      // Make histogram and store them to vectors
      int nTimeBin{4000}, minTime{0}, maxTime{100}; // 1 time bin = 8 ns
      //int nPeakBin{160}, minPeak{-100}, maxPeak{1500};
      int nPeakBin{320}, minPeak{-100}, maxPeak{1500};
      int nIadpBin{80}, minIadp{-30}, maxIadp{50};
      int nBinDeltaTime{4000}, minDeltaTime{-50}, maxDeltaTime{50};

      h2TimeAdcMB[TID].push_back(new TH2D(hisName2, hisName2,                           /*Time*/nTimeBin, minTime, maxTime, /*Peak*/nPeakBin, minPeak, maxPeak));
      h2MainAdcSubAdcMB[TID].push_back(new TH2D(hisName6, hisName6,                     /*Peak*/nPeakBin, minPeak, maxPeak, /*Peak*/nPeakBin, minPeak, maxPeak));
      h2TimeIADPMB[TID].push_back(new TH2D(hisName3, hisName3,                          /*Time*/nTimeBin, minTime, maxTime, /*Peak*/nIadpBin, minIadp, maxIadp));
      h2TimeIADPWithSubAdcCut[TID].push_back(new TH2D(hisName4, hisName4,               /*Time*/nTimeBin, minTime, maxTime, /*Peak*/nIadpBin, minIadp, maxIadp));
      h2MainTimeSubTimeMB[TID].push_back(new TH2D(hisName8, hisName8,                   /*Time*/nTimeBin, minTime, maxTime, /*Time*/nTimeBin, minTime, maxTime));
 
      h2MainTimeAdcMB[TID].push_back(new TH2D(hisName18, hisName18,                     /*Time*/nTimeBin, minTime, maxTime, /*Peak*/nPeakBin, minPeak, maxPeak));
      h2MainTimeIADPMB[TID].push_back(new TH2D(hisName19, hisName19,                     /*Time*/nTimeBin, minTime, maxTime, /*Peak*/nIadpBin, minIadp, maxIadp));
      h2MainTimeAdcWithGoodSubSignal[TID].push_back(new TH2D(hisName20, hisName20,      /*Time*/nTimeBin, minTime, maxTime, /*Peak*/nPeakBin, minPeak, maxPeak));

      h2DeltaTimeMainSubMBVsSubAdc[TID].push_back(new TH2D(hisName21, hisName21,        /*Peak*/nPeakBin, minPeak, maxPeak, /*Delta Time*/nBinDeltaTime, minDeltaTime, maxDeltaTime));
      h2DeltaTimeMainSubMBVsSubTdc[TID].push_back(new TH2D(hisName22, hisName22,        /*Time*/nTimeBin, minTime, maxTime, /*Delta Time*/nBinDeltaTime, minDeltaTime, maxDeltaTime));

      // check if they were filled properly
      if (!h2TimeAdcMB[TID][hisIndex] or !h2TimeIADPMB[TID][hisIndex]) {
        cout << "Histograms not safely stored. Abort!" << endl;
        abort();
      } else {
        // if histograms were safely stored, add them to hList
        for (auto& hist : std::vector<TH2D*>{
                                             //h2TimeAdcMB[TID][hisIndex], 
                                             //h2MainAdcSubAdcMB[TID][hisIndex],
                                             //h2TimeIADPMB[TID][hisIndex],
                                             //h2TimeIADPWithSubAdcCut[TID][hisIndex],
                                             //h2MainTimeSubTimeMB[TID][hisIndex],
                                             //h2MainTimeAdcMB[TID][hisIndex],
                                             h2MainTimeIADPMB[TID][hisIndex],
                                             //h2MainTimeAdcWithGoodSubSignal[TID][hisIndex],
                                             }) {
          hList -> Add(hist);
        }

        for (auto& hist : std::vector<TH2D*>{h2DeltaTimeMainSubMBVsSubAdc[TID][hisIndex], h2DeltaTimeMainSubMBVsSubTdc[TID][hisIndex]}) {
          hListMain -> Add(hist);
        }

      }
    }
  }

  // Analysis
  /// Apply quality cut on main peak
  /// If main peak satisfies qualtiy cut, then fill each sub channel's correlation

  for (int ientry = 0; ientry < Tin->GetEntries(); ientry++) {
    
    Tin -> GetEntry(ientry);
    
    // Loop over all cosimic
    for (int icos = 0; icos < CosmicTrackNumber; icos++) {
      
      int TrackID = CosmicTrackID[icos];
      const auto& pairs = TrackPairMPPC[TrackID];

      // Loop over all associated pairs
      for (auto pair = pairs.begin(); pair != pairs.end(); pair++) {

        int index =  std::distance(pairs.begin(), pair); 
        const auto& currentPair= *pair;

        // Extract module id
        int mainMod = currentPair.mainMod;
        int subMod = currentPair.subMod;

        //! Manual swap of channel change due in Run92b
        if (runId == "92b" && periodId >= 2) {
          if (TrackID == 0) {
            if (mainMod == 12) {
              mainMod = 13;
              subMod = 12;
              //DCV_LOG_INFO("Run92b ch swap fix/TrackID: " << TrackID << " mainMod: " << mainMod << " SubMod: " << subMod);
            }
          }

          if (TrackID == 1) {
            if (mainMod == 13) {
              mainMod = 12;
              subMod = 13;
              //DCV_LOG_INFO("Run92b ch swap fix/TrackID: " << TrackID << " mainMod: " << mainMod << " SubMod: " << subMod);
            }
          }
        }

        // values used in analysis
        float Times[2] = {DCVTime[mainMod], DCVTime[subMod]};
        short Peaks[2] = {DCVPeak[mainMod], DCVPeak[subMod]};
        float Iadps[2] = {DCVIntegratedADC[mainMod]/static_cast<float>(DCVPeak[mainMod]), DCVIntegratedADC[subMod]/static_cast<float>(DCVPeak[subMod])};

        // Fill MB correlations
        //h2TimeAdcMB[TrackID][index] -> Fill(Times[SubChId], Peaks[SubChId]);
        //h2TimeIADPMB[TrackID][index] -> Fill(Times[SubChId], Iadps[SubChId]);
        //h2MainAdcSubAdcMB[TrackID][index] -> Fill(Peaks[MainChId], Peaks[SubChId]);
        //h2MainTimeSubTimeMB[TrackID][index] -> Fill(Times[MainChId], Times[SubChId]);
        //h2MainTimeAdcMB[TrackID][index] -> Fill(Times[MainChId], Peaks[MainChId]);
        h2MainTimeIADPMB[TrackID][index] -> Fill(Times[MainChId], Iadps[MainChId]);
        //h2DeltaTimeMainSubMBVsSubAdc[TrackID][index] -> Fill(Peaks[SubChId], Times[MainChId] - Times[SubChId]);
        //h2DeltaTimeMainSubMBVsSubTdc[TrackID][index] -> Fill(Times[SubChId], Times[MainChId] - Times[SubChId]);

        // If main channels passes main signal, fill corresponding histograms 
        bool mainChTimeCut = (Times[MainChId] >= CalConfig->TimeCut.LE) && (Times[MainChId] < CalConfig->TimeCut.HE);
        bool mainChIadpCut = (Iadps[MainChId] >= CalConfig->IADPCut.LE) && (Iadps[MainChId] < CalConfig->IADPCut.HE);
        if (mainChTimeCut && mainChIadpCut) {
          //h2TimeAdc[TrackID][index] -> ll(Times[SubChId], Peaks[SubChId]);
          //h2TimeIADP[TrackID][index] -> Fill(Times[SubChId], Iadps[SubChId]);
          //h2MainAdcSubAdc[TrackID][index] -> Fill(Peaks[MainChId], Peaks[SubChId]);
          //h2MainTimeSubTime[TrackID][index] -> Fill(Times[MainChId], Times[SubChId]);
        }

        // Apply qualtiy cut on sub channel
        bool subChIadpCut = (Iadps[SubChId] >= CalConfig->IADPCut.LE) && (Iadps[SubChId] < CalConfig->IADPCut.HE);
        bool subChAdcCut = (Peaks[SubChId] >= 100);
        if (subChIadpCut) {
          //h2SubTimeAdcMB[TrackID][index] -> Fill(Times[SubChId], Peaks[SubChId]);
          //h2SubTimeIadpMB[TrackID][index] -> Fill(Times[SubChId], Iadps[SubChId]);
        }

        if (subChAdcCut) {
            //h2TimeAdcWithAdcCut[TrackID][index] -> Fill(Times[SubChId], Peaks[SubChId]);
            //h2MainTimeAdcWithGoodSubSignal[TrackID][index] -> Fill(Times[MainChId], Peaks[MainChId]);
            //h2TimeIADPWithSubAdcCut[TrackID][index] -> Fill(Times[SubChId], Iadps[SubChId]);
          }

      }
    }
  }

  // Write output files
  ofile -> cd();
  hList -> Write();
  ofile -> mkdir("mainRes");
  ofile -> cd("mainRes");
  //hListMain -> Write();
  ofile -> Close();
  Input -> Close();
}

