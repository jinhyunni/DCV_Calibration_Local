#include "../analysisHeader.h"

void plotMake_signalMainModule_IADP
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

	TFile* inputFile = nullptr;
  TString inputFileName =Form("../Run%s_Period%02d_DCVSignalMain%s.root", runId.c_str(), periodId, CalConfig -> infoTrigCtr.Data()); 
  
  if (!std::filesystem::exists(inputFileName.Data())) {
    DCV_LOG_ABORT("Required inputfile doesn't exist: " << inputFileName);
  } else {
    inputFile = new TFile(inputFileName, "read");
  }

	vector<TH1D*> h1MainIadp;
	vector<TString> hisFlags;
  vector<TF1*> hisFit;

  auto PerformFit = [&](TH1D* dataHist) {

    TString outhisname = dataHist -> GetName();
    
    //~~1. Find peak position / RMS
    float yMainPeak = dataHist -> GetMaximum();
    float xMainPeak = dataHist -> GetBinCenter(dataHist -> GetMaximumBin());
    float rms = dataHist -> GetRMS();

    //~~2. Define fit function
    TF1* ffit = new TF1("GaussFit_"+outhisname, "[0]*TMath::Gaus(x, [1], [2])", xMainPeak - 0.5*rms, xMainPeak + 0.5*rms);
    ffit -> SetParameters(yMainPeak, xMainPeak, rms*0.5);

    ffit -> SetParLimits(0, 1.0*yMainPeak, 2.0*yMainPeak);
    ffit -> SetParLimits(2, 0.0, 2*rms);

    //~~3. Perform fit
    dataHist -> Fit(ffit, "R0Q");
    
    //~~4. Return fit function
    return ffit;
  };

  int storeIndex=0;
	for (auto trackIt = TrackMPPC.begin(); trackIt != TrackMPPC.end(); trackIt++) {
		int trackIndex = std::distance(TrackMPPC.begin(), trackIt);
		auto const& mods = *trackIt;

		for( auto modIt = mods.begin(); modIt != mods.end(); modIt++ ) {
			//int hisIndex = std::distance( mods.begin(), modIt );
			auto const& mainModId = *modIt;

			TString hisInputName = Form("DCVMainSig_trk%02d_mainMod%02d_time_vs_iadp_vs_peak", trackIndex, mainModId);
			TString outputHisName = Form("DCVMainSig_trk%02d_mainMod%02d_iadp", trackIndex, mainModId);
			TString outputHisFlag = Form("Trk%02d_MainMod%02d", trackIndex, mainModId);

      cout << outputHisFlag << endl;

			h1MainIadp.push_back((TH1D*)((TH3D*)inputFile -> Get(hisInputName))->ProjectionY(outputHisName));

      if (h1MainIadp[storeIndex] == NULL) {
        cout << "Histogram is not safely stored! Abort!" << endl;
        abort();
      } else {
			  hisFlags.push_back(outputHisFlag);
        hisFit.push_back(PerformFit(h1MainIadp[storeIndex]));
        storeIndex++;
      }
		}
	}

	// Draw iadp's to canvas
	//

	TCanvas* c1 = new TCanvas("c1", "c1", 4*1.2*350, 4*350);
	c1 -> Divide(4, 4);
	
	for( int pad=0; pad <16; pad++)
	{
		int hisIndex = dcvMod*16+pad;
		h1MainIadp[hisIndex] -> Rebin(2);

		c1 -> cd( pad+1 );
		gStyle -> SetOptStat(0);
		
		gPad -> SetMargin(1.2, 1.2, 1.2, 1.2);
		gPad -> SetTicks();

		TH1D* htmp = (TH1D*)gPad -> DrawFrame(-50, -10, 100, h1MainIadp[hisIndex] -> GetMaximum() * 1.5);
		htmp -> GetXaxis() -> SetTitle("IntegratedADC / Peak ");
		htmp -> GetYaxis() -> SetTitle("Entries");
		//htmp -> GetXaxis() -> SetTitleSize(0.5);
		//htmp -> GetYaxis() -> SetTitleSize(0.5);
	
		// Draw histogram
		h1MainIadp[hisIndex] -> SetMarkerStyle(25);
		h1MainIadp[hisIndex] -> SetMarkerColor(1);
		h1MainIadp[hisIndex] -> SetMarkerSize(0.4);
		h1MainIadp[hisIndex] -> SetLineColor(1);
		h1MainIadp[hisIndex] -> Draw("same");
		
		// Draw legend
    float yMax = h1MainIadp[hisIndex] -> GetMaximum();
    float xMax = h1MainIadp[hisIndex] -> GetBinCenter(h1MainIadp[hisIndex]->GetMaximumBin());

    // Find Full width half maximum
    int binMinFwhm = h1MainIadp[hisIndex] -> FindFirstBinAbove(yMax/2);
    int binMaxFwhm = h1MainIadp[hisIndex] -> FindLastBinAbove(yMax/2);
    double fwhm =  h1MainIadp[hisIndex] -> GetBinCenter(binMaxFwhm) - h1MainIadp[hisIndex] -> GetBinCenter(binMinFwhm);

    int overflow = h1MainIadp[hisIndex] -> GetBinContent(h1MainIadp[hisIndex]->GetNbinsX()+1);
    int underflow = h1MainIadp[hisIndex] -> GetBinContent(0);
    int entries = h1MainIadp[hisIndex] -> GetEntries();

		TLegend* l1 = new TLegend(0.5, 0.35, 0.8, 0.8);
		l1 -> SetFillStyle(0);
		l1 -> SetBorderSize(0);
		l1 -> SetTextSize(0.05);
		l1 -> AddEntry("", Form("Run%s_Period%02d", runId.c_str(), periodId), "h");
		l1 -> AddEntry("", hisFlags[hisIndex], "h");
    l1 -> AddEntry("", Form("# of entries: %d", entries + overflow + underflow), "h");
    l1 -> AddEntry("", Form("xPeak: %.2f", xMax), "h");
    l1 -> AddEntry("", Form("RMS: %.2f", h1MainIadp[hisIndex]->GetRMS()), "h");
    l1 -> AddEntry("", Form("FWHM: %.2f", fwhm), "h");
		l1 -> Draw("same");

    TLine* maxLine = new TLine(xMax, 0.f, xMax, yMax);
    maxLine -> SetLineStyle(7);
    maxLine -> SetLineWidth(2);
    maxLine -> SetLineColor(kRed);
    maxLine -> Draw("same");  

    TLine* minFwhmLine = new TLine(h1MainIadp[hisIndex]->GetBinCenter(binMinFwhm), 0.f, h1MainIadp[hisIndex]->GetBinCenter(binMinFwhm), h1MainIadp[hisIndex]->GetBinContent(binMinFwhm));
    minFwhmLine -> SetLineStyle(7);
    minFwhmLine -> SetLineWidth(2);
    minFwhmLine -> SetLineColor(kBlue);
    minFwhmLine -> Draw("same");

    TLine* maxFwhmLine = new TLine(h1MainIadp[hisIndex]->GetBinCenter(binMaxFwhm), 0.f, h1MainIadp[hisIndex]->GetBinCenter(binMaxFwhm), h1MainIadp[hisIndex]->GetBinContent(binMaxFwhm));
    maxFwhmLine -> SetLineStyle(7);
    maxFwhmLine -> SetLineWidth(2);
    maxFwhmLine -> SetLineColor(kBlue);
    maxFwhmLine -> Draw("same");

    // Draw Fit function
    #if 0
    auto currentFit = hisFit[hisIndex];
    currentFit -> SetLineColor(kGreen);
    currentFit -> SetLineStyle(7);
    currentFit -> Draw("same");
    l1 -> AddEntry("", "", "h");
    l1 -> AddEntry("", Form("Chi2/NDF: %.2f/%d(%.2f)", currentFit->GetChisquare(), currentFit->GetNDF(), currentFit->GetChisquare()/(float)currentFit->GetNDF()), "h");
    l1 -> AddEntry("", Form("max: %.2f#pm%.3f)", currentFit->GetParameter(0), currentFit->GetParError(0)), "h");
    l1 -> AddEntry("", Form("#mu: %.2f#pm%.3f)", currentFit->GetParameter(1), currentFit->GetParError(1)), "h");
    l1 -> AddEntry("", Form("#sigma: %.2f#pm%.3f)", currentFit->GetParameter(2), currentFit->GetParError(2)), "h");
    #endif
	}

	c1 -> Draw();
}
