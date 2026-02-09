#include "analysisHeaderDCVSignal.h"

void plotMake_signalMainModule_IADP_vs_Time
(
	std::string runId         = "92",
	int periodId              = 0,
	int dcvMod                = 0,

  bool changeConfigs        = true,
  bool changeAttPathL       = true,
  bool changeN2SubQC        = false,
  bool changeModEneUpdated  = true,
  bool changeTrigCtr        = true
)
{
	// Input File
	// ===========
  const int RunConfigIdx = ReturnRunIndex(runId);
  if (periodId >= ParForCal[RunConfigIdx].size()) {
    DCV_LOG_ABORT("No parameters set for this period");
  }
  
  auto CalConfig = ParForCal[RunConfigIdx][periodId];
  if (changeConfigs) {
    CalConfig -> setAttPathL(changeAttPathL);
    CalConfig -> setN2SubQC(changeN2SubQC);
    CalConfig -> setModEneUpdated(changeModEneUpdated);
    CalConfig -> setTrigCtr(changeTrigCtr);
  }

  TFile* inputFile;
  TString inputfilename = Form("Run%s_Period%02d_DCVSignalMain%s.root", runId.c_str(), periodId, CalConfig -> infoTrigCtr.Data());
  
  if (!std::filesystem::exists(inputfilename.Data())) {
    DCV_LOG_ABORT("Required inputfile not found: " << inputfilename.Data()); 
  } else {
    inputFile = new TFile(inputfilename, "read");
  }

	vector<TH2D*> h2MainPeakTime;
	vector<TString> hisFlags;

	for (auto trackIt = TrackMPPC.begin(); trackIt != TrackMPPC.end(); trackIt++) {

		int trackIndex = std::distance( TrackMPPC.begin(), trackIt );
		auto const& mods = *trackIt;

		for (auto modIt = mods.begin(); modIt != mods.end(); modIt++) {

			int hisIndex = std::distance( mods.begin(), modIt );
			auto const& mainModId = *modIt;

      DCV_LOG_INFO("TrackIndex : " << trackIndex << " mod: " << mainModId); 

			TString hisInputName = Form("DCVMainSig_trk%02d_mainMod%02d_time_vs_iadp_vs_peak", trackIndex, mainModId);
			TString outputHisName = Form("DCVMainSig_trk%02d_mainMod%02d_peak_time", trackIndex, mainModId);
			TString outputHisFlag = Form("Trk%02d_MainMod%02d", trackIndex, mainModId);
      
      TH3D* inputhis = (TH3D*)inputFile -> Get(hisInputName);
      if (inputhis == NULL) {
        DCV_LOG_ABORT("Required input histogram not found: " << hisInputName.Data());
      } else {
			  h2MainPeakTime.push_back((TH2D*)inputhis -> Project3D("yx"));
			  hisFlags.push_back(outputHisFlag);
      }
		}
	}

	// Draw iadp's to canvas
	//

	TCanvas* c1 = new TCanvas("c1", "c1", 4*1.2*350, 4*350);
	c1 -> Divide(4, 4);
	
	for (int pad=0; pad <16; pad++) {

		int hisIndex = dcvMod*16+pad;
		//h2MainPeakTime[hisIndex] -> Rebin(1);

		c1 -> cd(pad+1);
		gStyle -> SetOptStat(0);
		
		gPad -> SetMargin(1.2, 1.2, 1.2, 1.2);
		gPad -> SetTicks();

		TH1D* htmp = (TH1D*)gPad -> DrawFrame(-10, -10, 100, 100);
		htmp -> GetXaxis() -> SetTitle("Peak ADC ch");
		htmp -> GetYaxis() -> SetTitle("DCVTime");
	
		// Draw histogram
		h2MainPeakTime[hisIndex] -> SetMarkerStyle(25);
		h2MainPeakTime[hisIndex] -> SetMarkerColor(1);
		h2MainPeakTime[hisIndex] -> SetMarkerSize(0.8);
		h2MainPeakTime[hisIndex] -> SetLineColor(1);
		h2MainPeakTime[hisIndex] -> Draw("same");
		
		// Draw legend
		TLegend* l1 = new TLegend(0.5, 0.65, 0.8, 0.8);
		l1 -> SetFillStyle(0);
		l1 -> SetBorderSize(0);
		l1 -> SetTextSize(0.05);
		l1 -> AddEntry("", Form("Run%s_Period%02d", runId.c_str(), periodId), "h");
		l1 -> AddEntry("", hisFlags[hisIndex], "h");
		l1 -> Draw("same");
	}

	c1 -> Draw();
}
