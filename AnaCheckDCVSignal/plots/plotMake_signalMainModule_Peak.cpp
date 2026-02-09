#include "../analysisHeader.h"

void plotMake_signalMainModule_Peak
(
	std::string runId = "92",
	int periodId = 0,
	int dcvMod = 0,

  bool changeConfigs = true,
  bool changeAttPathL = true,
  bool changeN2SubQC = false,
  bool changeModEneUpdated = true,
  bool changeTrigCtr = true 
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

	TFile* inputFile = new TFile(Form("../Run%s_Period%02d_DCVSignalMain%s.root", runId.c_str(), periodId, CalConfig -> infoTrigCtr.Data()), "read");
  if (inputFile->IsZombie()) {
    DCV_LOG_ABORT("Required inputfile not found");
  }

	vector<TH1D*> h1MainPeak;
	vector<TH1D*> h1MainPeak_withSubConstraint;
	vector<TString> hisFlags;

  int storeIndex=0;
	for (auto trackIt = TrackMPPC.begin(); trackIt != TrackMPPC.end(); trackIt++) {
		int trackIndex = std::distance(TrackMPPC.begin(), trackIt);
		auto const& mods = *trackIt;

		for( auto modIt = mods.begin(); modIt != mods.end(); modIt++ ) {
			//int hisIndex = std::distance( mods.begin(), modIt );
			auto const& mainModId = *modIt;

			TString hisInputName0 = Form("DCVMainSig_trk%02d_mainMod%02d_time_vs_iadp_vs_peak", trackIndex, mainModId);
			TString hisInputName1 = Form("WithSubChConstraint/DCVMainSig_trk%02d_mainMod%02d_time_vs_iadp_vs_peak_withGoodSub", trackIndex, mainModId);
			TString outputHisName0 = Form("DCVMainSig_trk%02d_mainMod%02d_peak", trackIndex, mainModId);
			TString outputHisName1 = Form("DCVMainSig_trk%02d_mainMod%02d_peak_withGoodSubSig", trackIndex, mainModId);
			TString outputHisFlag = Form("Trk%02d_MainMod%02d", trackIndex, mainModId);

      cout << outputHisFlag << endl;

			h1MainPeak.push_back((TH1D*)((TH3D*)inputFile -> Get(hisInputName0))->ProjectionZ(outputHisName0));
			h1MainPeak_withSubConstraint.push_back((TH1D*)((TH3D*)inputFile -> Get(hisInputName1))->ProjectionZ(outputHisName1));

      if (h1MainPeak[storeIndex] == NULL) {
        cout << "Histogram is not safely stored! Abort!" << endl;
        abort();
      } else {
			  hisFlags.push_back(outputHisFlag);
        storeIndex++;
      }
		}
	}

	// Draw iadp's to canvas
	//

	TCanvas* c1 = new TCanvas("c1", "c1", 4*1.2*350, 4*350);
	c1 -> Divide(4, 4);
	
	for (int pad=0; pad <16; pad++) {

		int hisIndex = dcvMod*16+pad;
		h1MainPeak[hisIndex] -> Rebin(1);

		c1 -> cd(pad+1);
		gStyle -> SetOptStat(0);
		
		gPad -> SetMargin(1.2, 1.2, 1.2, 1.2);
		gPad -> SetTicks();

		TH1D* htmp = (TH1D*)gPad -> DrawFrame(-150, 0, 1600, h1MainPeak[hisIndex] -> GetMaximum() * 1.5);
		htmp -> GetXaxis() -> SetTitle("Peak ");
		htmp -> GetYaxis() -> SetTitle("Entries");
	
		// Draw histogram
		h1MainPeak[hisIndex] -> SetMarkerStyle(25);
		h1MainPeak[hisIndex] -> SetMarkerColor(kBlack);
		h1MainPeak[hisIndex] -> SetLineColor(kBlack);
		h1MainPeak[hisIndex] -> SetMarkerSize(0.4);
		h1MainPeak[hisIndex] -> Draw("same");
		
    h1MainPeak_withSubConstraint[hisIndex] -> SetMarkerStyle(25);
		h1MainPeak_withSubConstraint[hisIndex] -> SetMarkerColor(kRed);
		h1MainPeak_withSubConstraint[hisIndex] -> SetLineColor(kRed);
		h1MainPeak_withSubConstraint[hisIndex] -> SetMarkerSize(0.4);
		h1MainPeak_withSubConstraint[hisIndex] -> Draw("same");
		
		// Draw legend
    float yMax = h1MainPeak[hisIndex] -> GetMaximum();
    float xMax = h1MainPeak[hisIndex] -> GetBinCenter(h1MainPeak[hisIndex]->GetMaximumBin());

		TLegend* l1 = new TLegend(0.5, 0.35, 0.8, 0.8);
		l1 -> SetFillStyle(0);
		l1 -> SetBorderSize(0);
		l1 -> SetTextSize(0.05);
		l1 -> AddEntry("", Form("Run%s_Period%02d", runId.c_str(), periodId), "h");
		l1 -> AddEntry("", hisFlags[hisIndex], "h");
    //l1 -> AddEntry(h1MainPeak[hisIndex], Form("No const - # of entries: %.0f", h1MainPeak[hisIndex] -> GetEntries()), "p");
    //l1 -> AddEntry(h1MainPeak_withSubConstraint[hisIndex], Form("Sub constraint - # of entries: %.0f", h1MainPeak_withSubConstraint[hisIndex] -> GetEntries()), "p");
		l1 -> Draw("same");
	}

	c1 -> Draw();
}
