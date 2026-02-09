#include "analysisHeader.h"

/*
	Calibration factor calculation strategy based on previous study
*/

void analysis_CalFactor_GainFactor
(
  std::string runId         = "92", 
  int periodId              = 0,
  int usedCut               = IADPTIME,
  bool changeConfigs        = true,
  bool changeAttPathL       = true,
  bool changeN2SubQC        = false,
  bool changeModEneUpdated  = true,
  bool changeTrigCtr        = true 
)
{
	// Input
	// =====
	
	const int RunConfigIdx = ReturnRunIndex(runId);
	auto CalConfig = ParForCal[RunConfigIdx][periodId];
	if (changeConfigs) {
		CalConfig -> setAttPathL(changeAttPathL);
		CalConfig -> setN2SubQC(changeN2SubQC);
		CalConfig -> setModEneUpdated(changeModEneUpdated);
		CalConfig -> setTrigCtr(changeTrigCtr);
	}

  TString cutinfo;
  if (usedCut >= nCUTS) {
    DCV_LOG_ABORT("Wrong input given at usedCut(0~3): " << usedCut); 
  } else if (usedCut == MB) {
    cutinfo = "";
  } else if (usedCut == IADP) {
    cutinfo = Form("_%sCutApplied", CalConfig->IADPCut.CutName.Data());
  } else if (usedCut == TIME) {
    cutinfo = Form("_%sCutApplied", CalConfig->TimeCut.CutName.Data());
  } else if (usedCut == IADPTIME) { 
    cutinfo = Form("_%s%sCutApplied", CalConfig->IADPCut.CutName.Data(), CalConfig->TimeCut.CutName.Data());
  }

	TString inputDir = Form("Run%s_Period%02d_Nx", runId.c_str(), periodId);

	TString inputN1 = Form("Run%s_Period%02d_N1%s%s.root", runId.c_str(), periodId, cutinfo.Data(), CalConfig->infoTrigCtr.Data());
	TString inputN2 = Form("Run%s_Period%02d_N2%s%s.root", runId.c_str(), periodId, cutinfo.Data(), CalConfig->infoTrigCtr.Data());
	TString inputN3 = Form("Run%s_Period%02d_N3%s%s.root", runId.c_str(), periodId, cutinfo.Data(), CalConfig->infoTrigCtr.Data());
	
	array<TFile*, 3> inputFiles;
	inputFiles[0] = new TFile(Form("%s/%s", inputDir.Data(), inputN1.Data()), "read");
	inputFiles[1] = new TFile(Form("%s/%s", inputDir.Data(), inputN2.Data()), "read");
	inputFiles[2] = new TFile(Form("%s/%s", inputDir.Data(), inputN3.Data()), "read");

  for (auto& file : inputFiles) {
    if (file->IsZombie()) {
      DCV_LOG_ABORT("Input file not found");
    } else {
      DCV_LOG_INFO(">>>>> READ IN: " << file->GetName());
    }
  }

	TString inputDirMc = Form("Run91_Period16_Nx");
	TString inputMcName = Form("%s/Run91_Period16_LanGausMpvWithError%s.root", inputDirMc.Data(), CalConfig->infoTrigCtr.Data());
	TFile* inputMc = new TFile(inputMcName, "read");

	// Input histograms
	array<TH1F*, 3> Nx;
	array<TH1F*, 3> NxErr;

	//TH1D* Nx[3];
	Nx[0] = (TH1F*)inputFiles[0] -> Get("hisN1");
	Nx[1] = (TH1F*)inputFiles[1] -> Get("hisN2");
	Nx[2] = (TH1F*)inputFiles[2] -> Get("hisN3");
	
	NxErr[0] = (TH1F*)inputFiles[0] -> Get("mpv_err");
	NxErr[1] = (TH1F*)inputFiles[1] -> Get("mpv_err");
	NxErr[2] = (TH1F*)inputFiles[2] -> Get("mpv_err");

	TH1F* hisMc = (TH1F*)inputMc -> Get("LanGausMpv");	

	// Calculate center point of calibration factor : 1./(N1 x N2 x N3)
	TH1F* NxCombined = (TH1F*)Nx[0] -> Clone("NxCombined");
	NxCombined -> Multiply(Nx[1]);
	NxCombined -> Multiply(Nx[2]);
	
	array<float, 32> gainFactor;
	array<float, 32> gainFactor_err;

	// Functions for calculating errors
	auto fnCalNxErr = [=](int x/* 1 for N1, 2 for N2, 3 for N3*/, int ch) -> float {
		// delta MPV
		float NxVal = Nx[x-1] -> GetBinContent(Nx[x-1]->FindBin(ch));
		float delNxVal = NxErr[x-1] -> GetBinContent(Nx[x-1]->FindBin(ch));
	
    // attenation factor
    float traveldistance;
    if (ch<16) { // DCV1
      if (ch%4<2) { // upstream
       traveldistance = halfL_DCV1-(pos_DCV1-pos_CC04);
      } else {
       traveldistance = halfL_DCV1+(pos_DCV1-pos_CC04);
      }
    } else { // DCV2
      if (ch%4<2) { // upstream 
       traveldistance = halfL_DCV2-(pos_DCV2-pos_CC05);
      } else {
       traveldistance = halfL_DCV2-(pos_DCV2-pos_CC05);
      }
    }
		float AttVal = (ch<16) ? lambda_DCV1 : lambda_DCV2;
		float delAttVal = (ch<16) ? lambda_DCV1_Err : lambda_DCV2_Err;

		//float CosmicPath = CosmicPathLength[ch];
		//float delCosmicPath = CosmicPathLength_Err[ch];

		float CosmicPath = hisMc->GetBinContent(hisMc->FindBin(ch));
		float delCosmicPath = hisMc->GetBinError(hisMc->FindBin(ch));

		float delNx;
		if( x != 3 ){
			delNx = NxVal * TMath::Sqrt( TMath::Power(delNxVal/NxVal, 2) + TMath::Power((traveldistance*delAttVal)/(AttVal*AttVal), 2) + TMath::Power( delCosmicPath/CosmicPath, 2) ); 
		} else {
			delNx = NxVal * TMath::Sqrt( TMath::Power(delNxVal/NxVal, 2) + TMath::Power( delCosmicPath/CosmicPath, 2) ); 
		}

		return delNx;
	};

	auto fnCalGainErr = [=](int ch, float gain, float delN1, float delN2, float delN3) -> float
	{
		float N1 = Nx[0] -> GetBinContent( Nx[0] -> FindBin(ch) );
		float N2 = Nx[1] -> GetBinContent( Nx[1] -> FindBin(ch) );
		float N3 = Nx[2] -> GetBinContent( Nx[2] -> FindBin(ch) );

		float gainErr = gain * TMath::Sqrt( TMath::Power( delN1/N1, 2 ) + TMath::Power( delN2/N2, 2 )+ TMath::Power( delN3/N3, 2 ) );

		return gainErr;
	};

	// Calculate GainFactor and GainFactorErr of each channels
	for (int i=0; i<32; i++) {
			
    // Gain Factor:
		gainFactor[i]     = 1./NxCombined -> GetBinContent( NxCombined -> FindBin(i) );
		//gainFactor_err[i] = NxCombined -> GetBinError( NxCombined -> FindBin(i) );
		
		// !Error calculation based on error propagation
	
		float delN1 = fnCalNxErr(1, i);
		float delN2 = fnCalNxErr(2, i);
		float delN3 = fnCalNxErr(3, i);

		gainFactor_err[i] = fnCalGainErr( i, gainFactor[i], delN1, delN2, delN3 );
		float gainFactor_rel = (gainFactor_err[i]/gainFactor[i])*100;
		
		// Printout
		cout << "Calibration Factor Ch" << i << " " 
			<< gainFactor[i] 
			<< " / Calculated err based on error prop "<< gainFactor_err[i] 
			<< Form(" (Relative Err : %.7f)", gainFactor_rel) <<endl;
    DCV_LOG_INFO(">>>>> CHECK: " << "DelN1: " << delN1 << " DelN2: " << delN2 << " DelN3: " << delN3);
		
		//gainFactor_err[i] = 0;
	}

	// Output
	// ======
	TFile* output = new TFile(Form("%s/Run%s_Period%02d_GainFactor%s.root", inputDir.Data(), runId.c_str(), periodId, CalConfig->infoTrigCtr.Data()), "recreate");
	TH1D* h1GainFactor= new TH1D("gain", "gain", 32, 0, 32);

	// Filling gainFactor & gainFactor error to histogram
	for(int i=0; i<32; i++)
	{
		h1GainFactor -> SetBinContent( h1GainFactor -> FindBin(i), gainFactor[i]);
		h1GainFactor -> SetBinError( h1GainFactor -> FindBin(i), gainFactor_err[i]);
	}

	TString storeDir = Form("FormatedGainFactors_Run%s", runId.c_str());
	if (!std::filesystem::exists(storeDir.Data())) {
		std::filesystem::create_directory(storeDir.Data());
	}
	
	TString outputTxtFileName = Form("%s/calib_DCV_%d%s.txt", storeDir.Data(), periodId, CalConfig->infoTrigCtr.Data());
	ofstream file(outputTxtFileName.Data());
	if (file.is_open()) {
		file << 32 << endl;
		for (int ich=0; ich<32; ich++) {
			TString format = Form("%d %f 0 1", ich, 1./gainFactor[ich]);
			file << format.Data() << endl;
		}
	}
	file.close();
	
	output -> cd();
	h1GainFactor-> Write();
	output -> Close();

}
