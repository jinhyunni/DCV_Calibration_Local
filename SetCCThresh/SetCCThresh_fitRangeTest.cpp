//Preset
#include "analysisHeader.h"

//input file information
//=====================
std::string runID;
int periodID;
cout << "Run: ";
cin >> runID;
cout << "PeriodID: ";
cin >> periodID;
cout << endl;

//int runIndex		= runID - DEFAULTSTARTRUNID;
int runIndex		= ReturnRunIndex(runID);
int periodIndex = periodID;

const int fitRebin=2;

// Main analysis
//==============
void SetCCThresh_fitRangeTest()
{
	// Class test
	// ---------
	if (!gROOT -> LoadClass("DCVAnalysis")) {
		cout << "Problem in calling DCVAnalysis class" << endl;
	}

	//Set Timer
	//--------
	TStopwatch sw;
	sw.Start();
	
	//output first-fit result
	TString dirName = Form("fitTryTest_Run%s_Period%02d", runID.c_str(), periodID);
	gSystem -> MakeDirectory(dirName);

	TString output_firstFit = ( runID == "91" and (periodID>10) ) ? 
  Form("Run%s_Period%02d_SetCCThresh_firstFitResult_smallFraction.root", runID.c_str(), periodID) :  // -> MC
  Form("Run%s_Period%02d_SetCCThresh_firstFitResult.root", runID.c_str(), periodID); // -> Data

	TFile *firstFitResult 			= new TFile(output_firstFit, "recreate");

	TH1D *firstFitResult_CC04_MPV 	= new TH1D("firstFit_CC04_MPV",		"firstFit_CC04_MPV",	58, 0, 58);
	TH1D *firstFitResult_CC04_Width	= new TH1D("firstFit_CC04_Width",	"firstFit_CC04_Width",	58, 0, 58);
	TH1D *firstFitResult_CC05_MPV	= new TH1D("firstFit_CC05_MPV",		"firstFit_CC05_MPV",	54, 0, 54);
	TH1D *firstFitResult_CC05_Width	= new TH1D("firstFit_CC05_Width", 	"firstFit_CC05_Width",	54, 0, 54);

	const double landauHighDrop = 7.0;

	for (int FitTry=0; FitTry<15; FitTry++) {

		//inputfile open
		TString inputFileDir = "InputHistos";

		TString inputFile_CCHist = (runID == 91 and (runID > 10)) ? 
    Form("%s/Run%s_Period%02d_CCHist_smallFraction.root", inputFileDir.Data(), runID.c_str(), periodID) : // -> MC
    Form("%s/Run%s_Period%02d_CCHist.root", inputFileDir.Data(), runID.c_str(), periodID); // -> Data

		TFile *input = new TFile(inputFile_CCHist, "read");

		double landauLowDrop	= 0.1 * FitTry + 0.5;	
		cout << landauLowDrop << " : " << landauHighDrop << endl;

		//output
		//======	
		TString outputname = Form("%s/Run%s_Period%02d_SetCCThresh_fitRangeTest_%02d_%02d.root", dirName.Data(), runID.c_str(), periodID, (int)(landauLowDrop * 10), (int)(landauHighDrop * 10));
		TFile *output = new TFile(outputname, "recreate");
		ofstream file04(Form("Run%s_Period%02d_CC04_firstFitResult.txt", runID.c_str(), periodID), ios_base::out);
		ofstream file05(Form("Run%s_Period%02d_CC05_firstFitResult.txt", runID.c_str(), periodID), ios_base::out);

		TCanvas *cc04_5550 = new TCanvas("cc04_5550", "cc04_5550", 1500, 1000);
		TCanvas *cc04_7730 = new TCanvas("cc04_7730", "cc04_7730", 1500, 1000);
		TCanvas *cc05_7730 = new TCanvas("cc05_7730", "cc05_7730", 1500, 1000);
		
		cc04_5550 -> Divide(4, 4);
		cc04_7730 -> Divide(6, 7);
		cc05_7730 -> Divide(6, 9);

		TH1D *h1D_cc04_5550[16], *h1D_cc04_7730[42], *h1D_cc05_7730[54];

		TH1D *cc04_mpv = new TH1D("cc04_mpv", Form("x:channel, y:MPV value, FitRange: MPV-width*%.01f~MPV+width*%.01f", landauLowDrop, landauHighDrop), 58, 0, 58);
		TH1D *cc05_mpv = new TH1D("cc05_mpv", Form("x:channel, y:MPV value, FitRange: MPV-width*%.01f~MPV+width*%.01f", landauLowDrop, landauHighDrop), 54, 0, 54);

		TH1D *cc04_mpv_error = new TH1D("cc04_mpv_error", Form("x: channel, y: MPV error, FitRange: MPV-width*%.01f~MPV+width*%.01f", landauLowDrop, landauHighDrop), 58, 0, 58);
		TH1D *cc05_mpv_error = new TH1D("cc05_mpv_error", Form("x: channel, y: MPV error, FitRange: MPV-width*%.01f~MPV+width*%.01f", landauLowDrop, landauHighDrop), 54, 0, 54);

		TH1D *cc04_chi2ndf = new TH1D("cc04_chi2ndf", Form("x: channel, y: chi2/ndf, FitRangeL MPV-width*%.01f~MPV+width*%.01f", landauLowDrop, landauHighDrop), 58, 0, 58);
		TH1D *cc05_chi2ndf = new TH1D("cc05_chi2ndf", Form("x: channel, y: chi2/ndf, FitRangeL MPV-width*%.01f~MPV+width*%.01f", landauLowDrop, landauHighDrop), 54, 0, 54);
		
		//Fitting CC04
		//============
		cout << "CC04 fitting started"<<endl;
		for (int i=0; i<58; i++) {

			TH1D *adc = (TH1D*)(input -> GetDirectory("CC04Ene") -> Get(Form("pCC04Ene_Mod%02d", i)));
			//TH1D *adc = (TH1D*)(input -> GetDirectory("CC04TrueEne") -> Get(Form("pCC04TrueEne_Mod%02d", i)));
			adc -> Rebin(fitRebin);
			adc -> SetAxisRange(ParForCal[runIndex][periodIndex]->CC04FirstFitStart, ParForCal[runIndex][periodIndex]->CC04FirstFitFinish);
			//adc -> SetAxisRange(20, 90);

			double ymax = adc -> GetMaximum();
			double xmax = adc -> GetBinCenter(adc -> GetMaximumBin());
			cout << Form("CsI module ID: %02d, maxBin#: %f, maxBinContent: %f", i, xmax, ymax) << endl;
		
			//First fit
			adc -> SetAxisRange(0, 100);
			adc -> SetMaximum(1.15 * ymax);
			TF1 *ffit = new TF1("ffit", "[0]*TMath::Landau(x, [1], [2])", xmax-5, xmax+10);
			ffit -> SetParameters(ymax, xmax, 5);
			adc -> Fit(ffit, "R0Q");

			double par1 = ffit -> GetParameter(1);
			double par2 = ffit -> GetParameter(2);
		
			//writing out first fit result
			firstFitResult_CC04_MPV		-> SetBinContent(i+1, par1);
			firstFitResult_CC04_Width	-> SetBinContent(i+1, par2);
			firstFitResult_CC04_MPV		-> SetBinError(i+1, 0);
			firstFitResult_CC04_Width	-> SetBinError(i+1, 0);


			//Writing first fit result into txt file
			TString firstFitResult = Form("%02d %04f %04f\n", i, par1, par2);
			file04 << firstFitResult.Data();

			//second fit
			//CC04_7730
			if (i<42) {

				//fitting parameter for second fitting
				double secondFitStart = par1 - landauLowDrop * par2;
				double secondFitFinish= par1 + landauHighDrop * par2;
				if(secondFitFinish > 100) secondFitFinish = 100;

				ffit -> SetRange(secondFitStart, secondFitFinish);
				cout << Form("FitRange: %f ~ %f", secondFitStart, secondFitFinish) << endl;
				adc -> Fit(ffit, "R0Q");

				cc04_mpv -> SetBinContent(i+1, ffit -> GetParameter(1));
				cc04_mpv -> SetBinError(i+1, ffit -> GetParError(1));
				cc04_mpv_error -> SetBinContent(i+1, ffit -> GetParError(1));
				cc04_chi2ndf -> SetBinContent(i+1, ffit -> GetChisquare()/ffit -> GetNDF());
				cc04_chi2ndf -> SetBinError(i+1, 0);

				//Drawing to canvas
				cc04_7730 -> cd(i+1);
				
				//TH1D *htmp = (TH1D*)gPad -> DrawFrame(0, 0, 1.5e2, 1.5e3);
				TH1D *htmp = (TH1D*)gPad -> DrawFrame(0, 0, 100, ymax*1.5);
				htmp -> GetXaxis() -> SetTitle("Ene");
				htmp -> GetYaxis() -> SetTitle("Entries");

				TLegend *leg = new TLegend(0.5, 0.4, 0.8, 0.9);
				leg -> SetFillStyle(0);
				leg -> SetBorderSize(0);
				leg -> SetTextSize(0.05);
        leg -> SetTextFont(62);
				leg -> AddEntry("", Form("Run%s-Period%02d", runID.c_str(), periodID), "h");
				leg -> AddEntry("", Form("CC04 ModID: %02d", i), "h");
				leg -> AddEntry("", Form("Entry#: %.0f", adc->GetEntries()), "h");
				leg -> AddEntry("", Form("MPV: %.1f#pm%.2f", ffit->GetParameter(1), ffit->GetParError(1)), "h");
				leg -> AddEntry("", Form("EneThresh(0.7 #times MPV): %.2f", 0.7*ffit->GetParameter(1)), "h");
				leg -> AddEntry("", Form("Chi2/NDF = %.2f/%d", ffit -> GetChisquare(), ffit->GetNDF()), "h");
				leg -> Draw();

				ffit -> SetLineWidth(3);
				ffit -> SetLineStyle(7);

        TLine* mpv = new TLine(ffit->GetParameter(1), 0, ffit->GetParameter(1), adc->GetBinContent(adc->FindBin(ffit->GetParameter(1))));
        mpv -> SetLineColor(kRed);
        mpv -> SetLineStyle(2);
        mpv -> Draw("same");

        TLine* threshold = new TLine(0.7*ffit->GetParameter(1), 0, 0.7*ffit->GetParameter(1), adc->GetBinContent(adc->FindBin(0.7*ffit->GetParameter(1))));
        threshold -> SetLineColor(kBlue);
        threshold -> SetLineStyle(2);
        threshold -> Draw("same");

				adc -> Draw("same");
				ffit -> Draw("same");

			} else {
        
				//fitting parameter for second fitting
				double secondFitStart = par1 - landauLowDrop * par2;
				double secondFitFinish= par1 + landauHighDrop * par2;
				if(secondFitFinish > 100) secondFitFinish = 100;

				ffit -> SetRange(secondFitStart, secondFitFinish);
				cout << Form("FitRange: %f ~ %f", secondFitStart, secondFitFinish) << endl;
				adc -> Fit(ffit, "R0Q");

				cc04_mpv -> SetBinContent(i+1, ffit -> GetParameter(1));
				cc04_mpv -> SetBinError(i+1, ffit -> GetParError(1));
				cc04_mpv_error -> SetBinContent(i+1, ffit -> GetParError(1));
				cc04_chi2ndf -> SetBinContent(i+1, ffit -> GetChisquare()/ffit -> GetNDF());
				cc04_chi2ndf -> SetBinError(i+1, 0);
				
				//Drawing to canvas
				cc04_5550 -> cd(i-41);

				//TH1D *htmp = (TH1D*)gPad -> DrawFrame(0, 0, 1.5e2, 1.5e3);
				TH1D *htmp = (TH1D*)gPad -> DrawFrame(0, 0, 100, ymax*1.5);
				htmp -> GetXaxis() -> SetTitle("Ene");
				htmp -> GetYaxis() -> SetTitle("Entries");

				TLegend *leg = new TLegend(0.5, 0.4, 0.8, 0.9);
				leg -> SetFillStyle(0);
				leg -> SetBorderSize(0);
				leg -> SetTextSize(0.05);
        leg -> SetTextFont(62);
				leg -> AddEntry("", Form("Run%s-Period%02d", runID.c_str(), periodID), "h");
				leg -> AddEntry("", Form("CC04 ModID: %02d", i), "h");
				leg -> AddEntry("", Form("Entry#: %.0f", adc->GetEntries()), "h");
				leg -> AddEntry("", Form("MPV: %.1f#pm%.2f", ffit->GetParameter(1), ffit->GetParError(1)), "h");
				leg -> AddEntry("", Form("EneThresh(0.7 #times MPV): %.2f", 0.7*ffit->GetParameter(1)), "h");
				leg -> AddEntry("", Form("Chi2/NDF = %.2f/%d", ffit -> GetChisquare(), ffit->GetNDF()), "h");
				leg -> Draw();

				ffit -> SetLineWidth(3);
				ffit -> SetLineStyle(7);        

        TLine* mpv = new TLine(ffit->GetParameter(1), 0, ffit->GetParameter(1), adc->GetBinContent(adc->FindBin(ffit->GetParameter(1))));
        mpv -> SetLineColor(kRed);
        mpv -> SetLineStyle(2);
        mpv -> Draw("same");

        TLine* threshold = new TLine(0.7*ffit->GetParameter(1), 0, 0.7*ffit->GetParameter(1), adc->GetBinContent(adc->FindBin(0.7*ffit->GetParameter(1))));
        threshold -> SetLineColor(kBlue);
        threshold -> SetLineStyle(2);
        threshold -> Draw("same");

				adc -> Draw("same");
				ffit -> Draw("same");
			}
				
		}
		cout << "CC04 fitting Finish\n"<<endl;

		//Fitting CC05
		//============
		cout << "CC05 drawing Started"<<endl;
		for (int i=0; i<54; i++) {

			TH1D *adc = (TH1D*)(input -> GetDirectory("CC05Ene") -> Get(Form("pCC05Ene_Mod%02d", i)));
			//TH1D *adc = (TH1D*)(input -> GetDirectory("CC05TrueEne") -> Get(Form("pCC05TrueEne_Mod%02d", i)));
			adc -> Rebin(fitRebin);
			adc -> SetAxisRange(ParForCal[runIndex][periodIndex]->CC05FirstFitStart, ParForCal[runIndex][periodIndex]->CC05FirstFitFinish);
			//adc -> SetAxisRange(20, 90);

			double ymax = adc -> GetMaximum();
			double xmax = adc -> GetBinCenter(adc -> GetMaximumBin());
			cout << Form("CsI module ID: %02d, maxBin#: %f, maxBinContent: %f", i, xmax, ymax) << endl;
		
			//First fit
			adc -> SetAxisRange(0, 100);
			adc -> SetMaximum(1.15 * ymax);
			TF1 *ffit = new TF1("ffit", "[0]*TMath::Landau(x, [1], [2])", xmax-5, xmax+10);
			ffit -> SetParameters(ymax, xmax, 5);
			adc -> Fit(ffit, "R0Q");

			double par1 = ffit -> GetParameter(1);
			double par2 = ffit -> GetParameter(2);
			
			//writing out first-fit result
			firstFitResult_CC05_MPV		-> SetBinContent(i+1, par1);
			firstFitResult_CC05_Width	-> SetBinContent(i+1, par2);
			firstFitResult_CC05_MPV		-> SetBinError(i+1, 0);
			firstFitResult_CC05_Width	-> SetBinError(i+1, 0);

			//Writing first fit result into txt-file
			TString firstFitResult = Form("%02d %04f %04f\n", i, par1, par2);
			file05 << firstFitResult.Data();

			//fitting parameter for second fitting
			double secondFitStart = par1 - landauLowDrop * par2;
			double secondFitFinish= par1 + landauHighDrop * par2;
			if(secondFitFinish > 100) secondFitFinish = 100;

			ffit -> SetRange(secondFitStart, secondFitFinish);
			cout << Form("FitRange: %f ~ %f", secondFitStart, secondFitFinish) << endl;
			adc -> Fit(ffit, "R0Q");

			cc05_mpv -> SetBinContent(i+1, ffit -> GetParameter(1));
			cc05_mpv -> SetBinError(i+1, ffit -> GetParError(1));
			cc05_mpv_error -> SetBinContent(i+1, ffit -> GetParError(1));
			cc05_chi2ndf -> SetBinContent(i+1, ffit -> GetChisquare()/ffit -> GetNDF());
			cc05_chi2ndf -> SetBinError(i+1, 0);
				
			//second fitting
			cc05_7730 -> cd(i+1);

			//TH1D *htmp = (TH1D*)gPad -> DrawFrame(0, 0, 1.5e2, 1.5e3);
			TH1D *htmp = (TH1D*)gPad -> DrawFrame(0, 0, 1.5e2, ymax*1.5);
			htmp -> GetXaxis() -> SetTitle("Ene");
			htmp -> GetYaxis() -> SetTitle("Entries");

			ffit -> SetLineWidth(3);
			ffit -> SetLineStyle(7);

			TLegend *leg = new TLegend(0.5, 0.4, 0.8, 0.9);
			leg -> SetFillStyle(0);
			leg -> SetBorderSize(0);
			leg -> SetTextSize(0.05);
      leg -> SetTextFont(62);
			leg -> AddEntry("", Form("Run%s-Period%02d", runID.c_str(), periodID), "h");
			leg -> AddEntry("", Form("CC05 ModID: %02d", i), "h");
			leg -> AddEntry("", Form("Entry#: %.0f", adc->GetEntries()), "h");
			leg -> AddEntry("", Form("MPV: %.1f#pm%.2f", ffit->GetParameter(1), ffit->GetParError(1)), "h");
			leg -> AddEntry("", Form("EneThresh(0.7 #times MPV): %.2f", 0.7*ffit->GetParameter(1)), "h");
			leg -> AddEntry("", Form("Chi2/NDF = %.2f/%d", ffit -> GetChisquare(), ffit->GetNDF()), "h");
			leg -> Draw();

      TLine* mpv = new TLine(ffit->GetParameter(1), 0, ffit->GetParameter(1), adc->GetBinContent(adc->FindBin(ffit->GetParameter(1))));
      mpv -> SetLineColor(kRed);
      mpv -> SetLineStyle(2);
      mpv -> Draw("same");

      TLine* threshold = new TLine(0.7*ffit->GetParameter(1), 0, 0.7*ffit->GetParameter(1), adc->GetBinContent(adc->FindBin(0.7*ffit->GetParameter(1))));
      threshold -> SetLineColor(kBlue);
      threshold -> SetLineStyle(2);
      threshold -> Draw("same");

			adc -> Draw("same");
			ffit -> Draw("same");
		}
		cout << "CC05 drawing Finish"<<endl;
		
		//Write output
		//============
		output -> cd();

		cc04_5550 -> Write();
		cc04_7730 -> Write();
		cc05_7730 -> Write();

		cc04_mpv -> Write();
		cc05_mpv -> Write();

		cc04_mpv_error -> Write();
		cc05_mpv_error -> Write();

		cc04_chi2ndf -> Write();
		cc05_chi2ndf -> Write();

		output -> Close();
		input -> Close();

		file04.close();
		file05.close();

	}//Fit Try

	firstFitResult -> cd();
	firstFitResult_CC04_MPV 	-> Write();
	firstFitResult_CC04_Width	-> Write();
	firstFitResult_CC05_MPV		-> Write();
	firstFitResult_CC05_Width	-> Write();
	firstFitResult -> Close();

	//Stop timewatch
	sw.Stop();
	sw.Print();
	
}
