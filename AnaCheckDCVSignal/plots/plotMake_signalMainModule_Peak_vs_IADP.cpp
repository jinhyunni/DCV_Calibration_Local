#include "analysisHeaderDCVSignal.h"

void plotMake_signalMainModule_Peak_vs_IADP
(
	std::string runId         = "92",
	int periodId              = 0,
	int dcvMod                = 1,

  bool changeConfigs        = true,
  bool changeAttPathL       = true,
  bool changeN2SubQC        = false,
  bool changeModEneUpdated  = true,
  bool changeTrigCtr        = true
)
{
	// Input File
	// =========
  auto CalConfig = ParForCal[ReturnRunIndex(runId)][periodId];
  if (changeConfigs) {
    CalConfig -> setAttPathL(changeAttPathL);
    CalConfig -> setN2SubQC(changeN2SubQC);
    CalConfig -> setModEneUpdated(changeModEneUpdated);
    CalConfig -> setTrigCtr(changeTrigCtr);
  }
  

  TString inputFileName = Form("Run%s_Period%02d_DCVSignalMain%s.root", runId.c_str(), periodId, CalConfig->infoTrigCtr.Data());
  if (!std::filesystem::exists(inputFileName.Data())) {
    DCV_LOG_ABORT("Inputfile not found : " << inputFileName.Data());
  }

	TFile* inputFile = new TFile(inputFileName, "read");

	vector<TH2D*> h2MainPeakIadp;
	vector<TString> hisFlags;

	for (auto trackIt = TrackMPPC.begin(); trackIt != TrackMPPC.end(); trackIt++) {

		int trackIndex = std::distance( TrackMPPC.begin(), trackIt );
		auto const& mods = *trackIt;

		for (auto modIt = mods.begin(); modIt != mods.end(); modIt++ ) {

			int hisIndex = std::distance( mods.begin(), modIt );
			auto const& subModId = *modIt;

			TString hisInputName = Form("DCVMainSig_trk%02d_mainMod%02d_time_vs_iadp_vs_peak", trackIndex, subModId);
			TString outputHisName = Form("DCVMainSig_trk%02d_mainMod%02d_peak_iadp", trackIndex, subModId);
			TString outputHisFlag = Form("Trk%02d_MainMod%02d", trackIndex, subModId);

			h2MainPeakIadp.push_back((TH2D*)((TH3D*)inputFile -> Get(hisInputName)) -> Project3D("yz"));
			hisFlags.push_back( outputHisFlag );
		}
	}

	// Draw iadp's to canvas
	//

	TCanvas* c1 = new TCanvas("c1", "c1", 4*1.2*350, 4*350);
	c1 -> Divide(4, 4);
	
	for (int pad=0; pad <16; pad++) {

		int hisIndex = dcvMod*16+pad;
		h2MainPeakIadp[hisIndex] -> Rebin(1);

		c1 -> cd( pad+1 );
		gStyle -> SetOptStat(0);
		
		gPad -> SetMargin(1.2, 1.2, 1.2, 1.2);
		gPad -> SetTicks();

		TH1D* htmp = (TH1D*)gPad -> DrawFrame(-100, -100, 1500, 100);
		htmp -> GetXaxis() -> SetTitle("Peak ADC ch");
		htmp -> GetYaxis() -> SetTitle("IntegratedADC/Peak");
	
		// Draw histogram
		h2MainPeakIadp[hisIndex] -> SetMarkerStyle(25);
		h2MainPeakIadp[hisIndex] -> SetMarkerColor(1);
		h2MainPeakIadp[hisIndex] -> SetMarkerSize(0.8);
		h2MainPeakIadp[hisIndex] -> SetLineColor(1);
		h2MainPeakIadp[hisIndex] -> Draw("same");
		
		// Draw legend
		TLegend* l1 = new TLegend(0.5, 0.65, 0.8, 0.8);
		l1 -> SetFillStyle(0);
		l1 -> SetBorderSize(0);
		l1 -> SetTextSize(0.05);
		l1 -> AddEntry("", Form("Run%s_Period%02d", runId.c_str(), periodId), "h");
		l1 -> AddEntry("", hisFlags[hisIndex], "h");
		l1 -> AddEntry("", Form("Entrie#: %.0f", h2MainPeakIadp[hisIndex] -> GetEntries()), "h");
		l1 -> Draw("same");
	}

	c1 -> Draw();
}
