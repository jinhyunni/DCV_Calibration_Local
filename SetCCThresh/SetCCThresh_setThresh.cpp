#include "analysisHeader.h"

//Preset
//=====
std::string runID;
int periodID;
cout << "RunID: ";
cin >> runID;
cout << "PeriodID: ";
cin >> periodID;

//int runIndex		= runID - DEFAULTSTARTRUNID;
int runIndex    = ReturnRunIndex(runID);
int periodIndex	= periodID; 

//CsI Mod ID
//adjont & default option: Make two different thresh factor value for
//						   CsI modulse near DCV and others
//
//===================================================================
const double defaultThreshFactor = 0.7;
const double adjointThreshFactor = 0.9;


vector<int> departCsiMod_CC04=
{
		40, 26, 12,		13, 27, 41,
		38, 24, 10,		11, 25, 39,
		//adjoint		//adjoint
	56, 52, 48, 44,		45, 49, 53, 57,
		 34, 20, 6,		7, 21, 35,
	54, 50, 46, 42,		43, 47, 51, 55,
		//adjoint		//adjoint
	  	 30, 16, 2,		3, 17, 31,
		 28, 14, 0,		1, 15, 29
};

vector<int> adjointCsiMod_CC04=
{
		8, 22, 36,		9, 23, 37, 
		4, 18, 32, 		5, 19, 33
};

vector<int> departCsiMod_CC05=
{
		16, 34, 52,		17, 35, 53,
		14, 32, 50,		15, 33, 51,
		//adjoint		//adjoint
		10, 28, 46,		11, 29, 47,
		8, 26, 44,		9, 27, 45,
		6, 24, 42,		7, 25, 43,
		//adjoint		//adjoint
		2, 20, 38,		3, 21, 39,
		0, 18, 36,		1, 19, 37
};

vector<int> adjointCsiMod_CC05=
{
		4, 22, 40,		5, 23, 41, 
		12, 30, 48, 	13, 31, 49
};

//Fn: Fitting procedure
//=====================
void fnSetThresh(
		vector<int>& inputCsiMod,		// Module # list
		TFile *input, 						// TFile which holds CsI ADC distribution
		int ccType, 						// 0: CC04, 1: CC05
		TH1D *inputMPV, 					// MPV with first fitting
		TH1D *inputWidth, 					// Width with first fitting 
		TH1D *outputHis,					// Output histogram to store Threshold(ch vs Thresh)
		const double firstFitStart,
		const double firstFitFinish,
		const double aFactor,				// aFactor
		const double bFactor,				// bFactor
		const double mulFactor				// multiplication factor to determine threshold
		)
{
	for(const auto &modID: inputCsiMod)
	{
		double MPV				=	inputMPV	-> GetBinContent(modID+1);
		double width			=	inputWidth	-> GetBinContent(modID+1);

		double fitStart			= 	MPV - aFactor * width;
		double fitFinish		= 	MPV + bFactor * width;
	
		TString inputDirName	= 	(ccType == 0) ? Form("CC04Ene") : Form("CC05Ene");
		TString inputFileName	= 	(ccType == 0) ? Form("pCC04Ene_Mod%02d", modID) : Form("pCC05Ene_Mod%02d", modID);

		TH1D *adc = (TH1D*)input -> GetDirectory(inputDirName) -> Get(inputFileName);
		adc -> Rebin(2);
	
		//0. Get initial parameter

 //*		double FirstFitStart	= (ccType == 0) ? RunConfigs[runIndex][periodIndex].CC04FirstFitStart : RunConfigs[runIndex][periodIndex].CC05FirstFitStart;
 //*		double FirstFitFinish	= (ccType == 0) ? RunConfigs[runIndex][periodIndex].CC04FirstFitFinish : RunConfigs[runIndex][periodIndex].CC05FirstFitFinish;

		adc -> SetAxisRange(firstFitStart, firstFitFinish); //Run90, Period0
		double ymax = adc -> GetMaximum();
		double xmax = adc -> GetBinCenter(adc -> GetMaximumBin());
		adc -> SetAxisRange(0, 90);
		adc -> SetMaximum(ymax * 1.15);
		
		//1. Final Fit
		TF1 *ffit = new TF1("ffit", "[0]*TMath::Landau(x, [1], [2])", fitStart, fitFinish);
		ffit -> SetParameters(ymax, xmax, 5);
		adc -> Fit(ffit, "R0Q");
		
		double thresh	=	ffit -> GetParameter(1) * mulFactor;
		double chi2ndf	= 	ffit -> GetChisquare() / ffit -> GetNDF(); 
		cout << Form("Module%02d, MPV: %.04f, MulFactor: %f, Thresh: %.04f, chi2/ndf: %.04f", modID, ffit -> GetParameter(1), mulFactor, thresh, chi2ndf) << endl;
		outputHis -> SetBinContent(modID+1, thresh);
	}
}

// Main analysis
//==============
void SetCCThresh_setThresh()
{
	//Set Timer
	//--------
	TStopwatch sw;
	sw.Start();
	
	//input
	//=====
	TString info = (runID=="91" and periodID>10) ? "_smallFraction" : "";

	//Get Fisrst fit result
	TString infilename = Form("Run%s_Period%02d_SetCCThresh_firstFitResult%s.root", runID.c_str(), periodID, info.Data());
	if (! gSystem -> IsFileInIncludePath(infilename)) {
		cout << "No inputfile: " << infilename << endl;
		return;
	}
	TFile *firstFitResult = new TFile(infilename, "read");

	TH1D *firstFit_CC04_MPV		= (TH1D*)firstFitResult -> Get("firstFit_CC04_MPV");
	TH1D *firstFit_CC05_MPV		= (TH1D*)firstFitResult -> Get("firstFit_CC05_MPV");
	TH1D *firstFit_CC04_Width	= (TH1D*)firstFitResult -> Get("firstFit_CC04_Width");
	TH1D *firstFit_CC05_Width	= (TH1D*)firstFitResult -> Get("firstFit_CC05_Width");
	
	//Get ADC Ene distribution
	TFile *input1 = new TFile(Form("/Users/jinhyunpark/npl/Analysis/KOTO/DCV/calibration/data_derived/Run%s_Period%02d_CCHist%s.root",runID.c_str(), periodID, info.Data()), "read");

	//output
	//====== 
	TFile *output = new TFile(Form("Run%s_Period%02d_SetCCThresh_getThres%s.root", runID.c_str(), periodID, info.Data()), "recreate");
	TH1D *CC04_Thresh = new TH1D("CC04_Thresh", "CC04_Thresh", 58, 0, 58);
	TH1D *CC05_Thresh = new TH1D("CC05_Thresh", "CC05_Thresh", 54, 0, 54);

	//Analysis
	//========
	
	DCVAnalysis* target = ParForCal[runIndex][periodIndex];
	cout << Form("RunIndex: %d, PeriodIndex: %d", runIndex, periodIndex) << endl;

	//CC04 departMod
	cout << "<<<CC04 depart module>>>" << endl;
	fnSetThresh(departCsiMod_CC04, input1, 0, firstFit_CC04_MPV, firstFit_CC04_Width, CC04_Thresh, target->CC04FirstFitStart, target->CC04FirstFitFinish, target->aFactor, target->bFactor, defaultThreshFactor);
	//CC04 adjointMod
	cout << "\n<<<CC04 adjoint module>>>" << endl;
	fnSetThresh(adjointCsiMod_CC04, input1, 0, firstFit_CC04_MPV, firstFit_CC04_Width, CC04_Thresh, target->CC04FirstFitStart, target->CC04FirstFitFinish, target->aFactor, target->bFactor, adjointThreshFactor);
	//CC05 departMod
	cout << "\n<<<CC05 depart module>>>" << endl;
	fnSetThresh(departCsiMod_CC05, input1, 1, firstFit_CC05_MPV, firstFit_CC05_Width, CC05_Thresh, target->CC05FirstFitStart, target->CC05FirstFitFinish, target->aFactor, target->bFactor, defaultThreshFactor);
	//CC05 adjointMod
	cout << "\n<<<CC05 adjoint module>>>" << endl;
	fnSetThresh(adjointCsiMod_CC05, input1, 1, firstFit_CC05_MPV, firstFit_CC05_Width, CC05_Thresh, target->CC05FirstFitStart, target->CC05FirstFitFinish, target->aFactor, target->bFactor, adjointThreshFactor);

	//=============
	//Analysis done

	//Writing outputs
	output -> cd();
	CC04_Thresh -> Write();
	CC05_Thresh -> Write();
	output -> Close();

	//Stop timewatch
	sw.Stop();
	sw.Print();
	
}
