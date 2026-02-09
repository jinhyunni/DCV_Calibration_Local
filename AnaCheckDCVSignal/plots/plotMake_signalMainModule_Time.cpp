#include "../analysisHeader.h"

void plotMake_signalMainModule_Time
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
	// =========
  
  int runIdx = ReturnRunIndex(runId);

  auto CalConfig = ParForCal[runIdx][periodId];
  if (changeConfigs) {
    CalConfig -> setAttPathL(changeAttPathL);
    CalConfig -> setN2SubQC(changeN2SubQC);
    CalConfig -> setModEneUpdated(changeModEneUpdated);
    CalConfig -> setTrigCtr(changeTrigCtr);
  }

	TFile* inputFile = new TFile(Form("../Run%s_Period%02d_DCVSignalMain%s.root", runId.c_str(), periodId, CalConfig->infoTrigCtr.Data()), "read");
  if (inputFile -> IsZombie()) {
    DCV_LOG_ABORT("Required input file not found");
  }

	vector<TH1D*> h1MainTime;
	vector<TString> hisFlags;

	for (auto trackIt = TrackMPPC.begin(); trackIt != TrackMPPC.end(); trackIt++) {

		int trackIndex = std::distance( TrackMPPC.begin(), trackIt );
		auto const& mods = *trackIt;

		for (auto modIt = mods.begin(); modIt != mods.end(); modIt++) {

			int hisIndex = std::distance( mods.begin(), modIt );
			auto const& subModId = *modIt;

			TString hisInputName = Form("DCVMainSig_trk%02d_mainMod%02d_time_vs_iadp_vs_peak", trackIndex, subModId);
			TString outputHisName = Form("DCVMainSig_trk%02d_mainMod%02d_time", trackIndex, subModId);
			TString outputHisFlag = Form("Trk%02d_MainMod%02d", trackIndex, subModId);

			h1MainTime.push_back(  (TH1D*)((TH3D*)inputFile -> Get(hisInputName)) -> ProjectionX(outputHisName) );
			hisFlags.push_back( outputHisFlag );
		}
	}

	// Draw iadp's to canvas
	//

	TCanvas* c1 = new TCanvas("c1", "c1", 4*1.2*350, 4*350);
	c1 -> Divide(4, 4);
	
	for (int pad=0; pad <16; pad++) {

		int hisIndex = dcvMod*16+pad;
		h1MainTime[hisIndex] -> Rebin(1);

		float ymax = h1MainTime[hisIndex] -> GetMaximum();
		float xmax = h1MainTime[hisIndex] -> GetBinCenter(h1MainTime[hisIndex]->GetMaximumBin());
		float sig = h1MainTime[hisIndex] -> GetRMS();

		c1 -> cd(pad+1);
		gStyle -> SetOptStat(0);
		
		gPad -> SetMargin(1.2, 1.2, 1.2, 1.2);
		gPad -> SetTicks();

		TH1D* htmp = (TH1D*)gPad -> DrawFrame(-5, -5, 70, h1MainTime[hisIndex] -> GetMaximum() * 1.5);
		htmp -> GetXaxis() -> SetTitle("DCVTime ");
		htmp -> GetYaxis() -> SetTitle("Entries");
	
		// Draw histogram
		h1MainTime[hisIndex] -> SetMarkerStyle(25);
		h1MainTime[hisIndex] -> SetMarkerColor(1);
		h1MainTime[hisIndex] -> SetMarkerSize(0.4);
		h1MainTime[hisIndex] -> SetLineColor(1);
		h1MainTime[hisIndex] -> Draw("same");

		// Draw legend
		TLegend* l1 = new TLegend(0.15, 0.60, 0.4, 0.8);
		l1 -> SetFillStyle(0);
		l1 -> SetBorderSize(0);
		l1 -> SetTextSize(0.05);
		l1 -> AddEntry("", Form("Run%s_Period%02d", runId.c_str(), periodId), "h");
		l1 -> AddEntry("", hisFlags[hisIndex], "h");
		l1 -> AddEntry("", Form("# of entries : %.0f", h1MainTime[hisIndex] -> GetEntries()), "h");
		l1 -> Draw("same");
	}

	c1 -> Draw();
}
