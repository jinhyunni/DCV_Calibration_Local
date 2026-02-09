#include "analysisHeader.h"
#include "SystematicSources.h"

void analysis_Make_TimeCalibration_Syst
(
  std::string runId         = "92",
  int periodId              = 0,

  bool saveTcTxt            = false,
  int usedDataSet           = 0,

  bool changeConfigs        = true,
  bool changeAttPathL       = true,
  bool changeN2SubQC        = false,
  bool changeModEneUpdated  = true,
  bool changeTrigCtr        = true
)
{
  // Making inputs & outputs
  // -----------------------
  int RunConfigIdx = ReturnRunIndex(runId);
  if (periodId >= ParForCal[RunConfigIdx].size()) {
    DCV_LOG_ABORT("Calibration parameters not found for this period");
  }

  auto CalConfig = ParForCal[RunConfigIdx][periodId];
  if (changeConfigs) {
    CalConfig -> setAttPathL(changeAttPathL);
    CalConfig -> setN2SubQC(changeN2SubQC);
    CalConfig -> setModEneUpdated(changeModEneUpdated);
    CalConfig -> setTrigCtr(changeTrigCtr);
  }

  DCV_LOG_INFO("runId : " << runId );
  DCV_LOG_INFO("periodId : " << periodId );

  // Input
  TString infilename = Form("GaussFitRes/Run%s_Period%02d_GaussFitToMainTime%s_SystIadp.root", runId.c_str(), periodId, CalConfig->infoTrigCtr.Data());
  TFile* infile = new TFile(infilename, "read");
  if (infile -> IsZombie()) {
    DCV_LOG_ABORT("Required inputfile not found");
  } else {
    DCV_LOG_INFO(">>>>> READ-IN :" << infile->GetName());
  }
  
  TString dataset;
  if (usedDataSet == 0) {
    dataset = "MB";
  } else if(usedDataSet == 1) {
    dataset = "IADP";
  } else if(usedDataSet == 2) {
    dataset = "GoodSubSig";
  } else {
    abort();
  }

  std::vector<std::vector<std::vector<std::vector<TF1*>>>> h1Fit(12);

  std::vector<std::vector<TH1D*>> timeCalib_adjoint(SYSTEMATIC::SystIadp.size());
  std::vector<std::vector<TH1D*>> timeCalib_updown(SYSTEMATIC::SystIadp.size());
  std::vector<std::vector<TH1D*>> timeCalib_betweenMods(SYSTEMATIC::SystIadp.size());
  TObjArray* hRes = new TObjArray(0);

  #if 1
  int saveOnlyOnce = 0;
  for (auto trackIt = TrackPairMPPC.begin(); trackIt != TrackPairMPPC.end(); trackIt++) {
    
    int trackId = std::distance(TrackPairMPPC.begin(), trackIt);
    const auto& currentPairs = *trackIt;

    h1Fit[trackId].resize(currentPairs.size());

    for (auto pairsIt = currentPairs.begin(); pairsIt != currentPairs.end(); pairsIt++) {

      int hisIndex = std::distance(currentPairs.begin(), pairsIt);
      const auto& currentPair = *pairsIt;

      h1Fit[trackId][hisIndex].resize(SYSTEMATIC::SystIadp.size());

      for (int icutIadp = 0; icutIadp < SYSTEMATIC::SystIadp.size(); icutIadp++) {

        for (int icutFit = 0; icutFit < SYSTEMATIC::SystFitRangeTunePar.size(); icutFit++) {
          
          // Making inputs
          TString inputFuncKey = Form("%s/GaussFit_Trk%02d_mainMod%02d_SystIadpCutset%d%s_SystFitRangeTuneset%d", dataset.Data(), trackId, currentPair.mainMod, icutIadp, dataset.Data(), icutFit);
          h1Fit[trackId][hisIndex][icutIadp].push_back((TF1*)infile->Get(inputFuncKey));

          if (h1Fit[trackId][hisIndex][icutIadp][icutFit] == NULL) {
            DCV_LOG_ABORT("Histogram is not safely stored: " << inputFuncKey.Data());
          } else {
            DCV_LOG_INFO("Histogram is safely stord: " << inputFuncKey.Data());
          }

          // Making outputs - Summary plots
          if (saveOnlyOnce == 0) {
            timeCalib_adjoint[icutIadp].push_back(new TH1D(Form("TimeCalib_adjoint_SystIadpCutset%d_SystFitRangeTuneset%d", icutIadp, icutFit), "x: ch, y: AdjointCalibrated", 32, 0, 32));
            timeCalib_updown[icutIadp].push_back(new TH1D(Form("TimeCalib_updown_SystIadpCutset%d_SystFitRangeTuneset%d", icutIadp, icutFit), "x: ch, y: AdjointCalibrated", 32, 0, 32));
            timeCalib_betweenMods[icutIadp].push_back(new TH1D(Form("TimeCalib_betweenMods_SystIadpCutset%d_SystFitRangeTuneset%d", icutIadp, icutFit), "x: ch, y: AdjointCalibrated", 32, 0, 32));

            if (timeCalib_adjoint[icutIadp][icutFit] != NULL) hRes -> Add(timeCalib_adjoint[icutIadp][icutFit]);
            if (timeCalib_updown[icutIadp][icutFit] != NULL) hRes -> Add(timeCalib_updown[icutIadp][icutFit]);
            if (timeCalib_betweenMods[icutIadp][icutFit] != NULL) hRes -> Add(timeCalib_betweenMods[icutIadp][icutFit]);
          }
        }
      }
      saveOnlyOnce++;
    }
  }
  #endif

  // Output
  TString outDir = "TimeCalibConstant";
  if (!std::filesystem::exists(outDir.Data())) {
    std::filesystem::create_directory(outDir.Data());
  }

  TString ofilename = Form("%s/Run%s_Period%02d_TimeCalibration%s_Syst.root", outDir.Data(), runId.c_str(), periodId, CalConfig->infoTrigCtr.Data());
  TFile* ofile = new TFile(ofilename, "recreate");
  DCV_LOG_INFO(">>>>> OUTPUT: " << ofile -> GetName());
  
  // Analysis
  //~~ Perform Timing calibration
  // ----------------------------
  #if 0
  struct AdjointChannels {
    TF1* reference;
    TF1* adjusted;
  };
  #endif

  struct AdjointChannels {

    std::vector<std::vector<TF1*>> reference;
    std::vector<std::vector<TF1*>> adjusted;
  };


  std::array<std::array<AdjointChannels, 2>, 8> adjointTimeCalib;

  // DCV1
  adjointTimeCalib[0][0] = {h1Fit[4][0], h1Fit[5][0]};
  adjointTimeCalib[0][1] = {h1Fit[4][1], h1Fit[5][1]};
  adjointTimeCalib[1][0] = {h1Fit[1][0], h1Fit[0][0]};
  adjointTimeCalib[1][1] = {h1Fit[1][1], h1Fit[0][1]};
  adjointTimeCalib[2][0] = {h1Fit[3][0], h1Fit[2][0]};
  adjointTimeCalib[2][1] = {h1Fit[3][1], h1Fit[2][1]};
  adjointTimeCalib[3][0] = {h1Fit[0][2], h1Fit[1][2]};
  adjointTimeCalib[3][1] = {h1Fit[0][3], h1Fit[1][3]};
  // DCV2 
  adjointTimeCalib[4][0] = {h1Fit[10][0], h1Fit[11][0]};
  adjointTimeCalib[4][1] = {h1Fit[10][1], h1Fit[11][1]};
  adjointTimeCalib[5][0] = {h1Fit[7][0], h1Fit[6][0]};
  adjointTimeCalib[5][1] = {h1Fit[7][1], h1Fit[6][1]};
  adjointTimeCalib[6][0] = {h1Fit[9][0], h1Fit[8][0]};
  adjointTimeCalib[6][1] = {h1Fit[9][1], h1Fit[8][1]};
  adjointTimeCalib[7][0] = {h1Fit[6][2], h1Fit[7][2]};
  adjointTimeCalib[7][1] = {h1Fit[6][3], h1Fit[7][3]};

  // Calculate timing calibration constant and save the result
  for (auto modIt = adjointTimeCalib.begin(); modIt != adjointTimeCalib.end(); modIt++) {
    
    const auto& pairs = *modIt;
    const int modId = std::distance(adjointTimeCalib.begin(), modIt);

    for (auto pair = pairs.begin(); pair != pairs.end(); pair++) {
      
      const auto& currentPair = *pair;
      const int pairId = std::distance(pairs.begin(), pair);
     
      for (int icutIadp=0; icutIadp<SYSTEMATIC::SystIadp.size(); icutIadp++) {

        for (int icutFit=0; icutFit<SYSTEMATIC::SystFitRangeTunePar.size(); icutFit++) {

          const auto& fitToRef      = currentPair.reference[icutIadp][icutFit];
          const auto& fitToAdjusted = currentPair.adjusted[icutIadp][icutFit];

          int refMod      = 4*modId + 2*pairId;
          int adjustedMod = 4*modId + 2*pairId + 1;

          float calibConstant = fitToRef -> GetParameter(1) - fitToAdjusted -> GetParameter(1);
          float errCalibConstant = std::sqrt(std::pow(fitToRef->GetParError(1), 2) + std::pow(fitToAdjusted->GetParError(1), 2));
          
          DCV_LOG_INFO("Ref: " << fitToRef -> GetParameter(1) << " / Adjusted: " << fitToAdjusted -> GetParameter(1));
          DCV_LOG_INFO("Mod" << modId << " RefMod : " << refMod << " / AdjustedMod:" << adjustedMod << " TimeCalibConstant: " << calibConstant << " AbsErr: " << errCalibConstant);

          // Set bin contents
          timeCalib_adjoint[icutIadp][icutFit] -> SetBinContent(timeCalib_adjoint[icutIadp][icutFit]->FindBin(refMod), 0.0);
          timeCalib_adjoint[icutIadp][icutFit] -> SetBinContent(timeCalib_adjoint[icutIadp][icutFit]->FindBin(adjustedMod), calibConstant);
          // Set bin errors
          timeCalib_adjoint[icutIadp][icutFit] -> SetBinError(timeCalib_adjoint[icutIadp][icutFit]->FindBin(refMod), 1e-10);
          timeCalib_adjoint[icutIadp][icutFit] -> SetBinError(timeCalib_adjoint[icutIadp][icutFit]->FindBin(adjustedMod), errCalibConstant);
        }
      }
    }
  }
 
  // Calculate timing calibration constant - Up&downstream channels - T2
  for (auto modIt = adjointTimeCalib.begin(); modIt != adjointTimeCalib.end(); modIt++) {
    
    const auto& pairs = *modIt;
    const int modId = std::distance(adjointTimeCalib.begin(), modIt);

    for (int icutIadp=0; icutIadp<SYSTEMATIC::SystIadp.size(); icutIadp++) {

      for (int icutFit=0; icutFit<SYSTEMATIC::SystFitRangeTunePar.size(); icutFit++) {

        // Upstream reference
        const auto& refUp = pairs[0].reference[icutIadp][icutFit];
        // Downstream reference
        const auto& refDown = pairs[1].reference[icutIadp][icutFit];

        // Calculate calibration constant
        //float calibConstant = 0.25 + (refUp -> GetParameter(1) - refDown -> GetParameter(1)); // 0.25 calculated by hand...This can be defined more precisely in the futre
        float calibConstant = 0.41 + (refUp -> GetParameter(1) - refDown -> GetParameter(1)); // Assuming 18cm / 1ns
        float errCalibConstant = std::sqrt(std::pow(refUp -> GetParError(1), 2) + std::pow(refDown -> GetParError(1), 2));

        int mod0 = 4*modId;
        int mod1 = 4*modId+1;
        int mod2 = 4*modId+2;
        int mod3 = 4*modId+3;

        // Set bin contents
        timeCalib_updown[icutIadp][icutFit] -> SetBinContent(timeCalib_updown[icutIadp][icutFit]->FindBin(mod0), 0);
        timeCalib_updown[icutIadp][icutFit] -> SetBinContent(timeCalib_updown[icutIadp][icutFit]->FindBin(mod1), 0);
        timeCalib_updown[icutIadp][icutFit] -> SetBinContent(timeCalib_updown[icutIadp][icutFit]->FindBin(mod2), calibConstant);
        timeCalib_updown[icutIadp][icutFit] -> SetBinContent(timeCalib_updown[icutIadp][icutFit]->FindBin(mod3), calibConstant);
        // Set bin errors
        timeCalib_updown[icutIadp][icutFit] -> SetBinError(timeCalib_updown[icutIadp][icutFit]->FindBin(mod0), 1e-10);
        timeCalib_updown[icutIadp][icutFit] -> SetBinError(timeCalib_updown[icutIadp][icutFit]->FindBin(mod1), 1e-10);
        timeCalib_updown[icutIadp][icutFit] -> SetBinError(timeCalib_updown[icutIadp][icutFit]->FindBin(mod2), errCalibConstant);
        timeCalib_updown[icutIadp][icutFit] -> SetBinError(timeCalib_updown[icutIadp][icutFit]->FindBin(mod3), errCalibConstant);


      }
    }
  }

  #if 1 
  // Calculate timing calibration constant - Between reference channel of each modules - T3
  std::array<int, 2> refModNum = {0, 4};
  std::array<int, 6> adjustModNum{1, 2, 3, 5, 6, 7};
  
  /// Fill calib factor for reference modules : 0 
  for (const auto& mod : refModNum) {
    for (auto ch : std::array{4*mod, 4*mod+1, 4*mod+2, 4*mod+3}) {
      for (int icutIadp=0; icutIadp<SYSTEMATIC::SystIadp.size(); icutIadp++) {
        for (int icutFit=0; icutFit<SYSTEMATIC::SystFitRangeTunePar.size(); icutFit++) {
          int bin = timeCalib_betweenMods[icutIadp][icutFit] -> FindBin(ch);
          timeCalib_betweenMods[icutIadp][icutFit] -> SetBinContent(bin, 0.0);
          timeCalib_betweenMods[icutIadp][icutFit] -> SetBinError(bin, 1e-10);
        }// Syst: FitRangeTune 
      }// Syst: IadpSelection
    }// Set calibration constante of each feference channels of each modules
  }// module loop

  for (const auto& mod : adjustModNum) {
    int refmod = 4*(mod/4);
    //float calibconstant = adjointTimeCalib[refmod][0].reference -> GetParameter(1) - adjointTimeCalib[mod][0].reference -> GetParameter(1);

    for (auto ch : std::array{4*mod, 4*mod+1, 4*mod+2, 4*mod+3}) {
      for (int icutIadp=0; icutIadp<SYSTEMATIC::SystIadp.size(); icutIadp++) {
        for (int icutFit=0; icutFit<SYSTEMATIC::SystFitRangeTunePar.size(); icutFit++) {
          int bin = timeCalib_betweenMods[icutIadp][icutFit] -> FindBin(ch);
          float calibconstant = adjointTimeCalib[refmod][0].reference[icutIadp][icutFit]->GetParameter(1) - adjointTimeCalib[mod][0].reference[icutIadp][icutFit]->GetParameter(1);
          float errCalibConstant = std::sqrt(std::pow(adjointTimeCalib[refmod][0].reference[icutIadp][icutFit]->GetParError(1), 2) + std::pow(adjointTimeCalib[mod][0].reference[icutIadp][icutFit]->GetParError(1), 2));
          timeCalib_betweenMods[icutIadp][icutFit] -> SetBinContent(bin, calibconstant);
          timeCalib_betweenMods[icutIadp][icutFit] -> SetBinError(bin, errCalibConstant);
        }// Syst: FitRagneTune
      }// Syst: IadpSelection
    }// channel loop 
  }// module loop
  #endif

  // Calculate finial timing calibration constatnt : T_total
  for (int icut=0; icut<SYSTEMATIC::SystIadp.size(); icut++) {
    for (int ifit=0; ifit<SYSTEMATIC::SystFitRangeTunePar.size(); ifit++) {

      TString outhisname = Form("TimeCalib_total_SystIadpCutset%d_SystFitRangeTuneset%d", icut, ifit);
      
      TH1D* total = (TH1D*)timeCalib_adjoint[icut][ifit]->Clone(outhisname);
      total -> Add(timeCalib_updown[icut][ifit]);
      total -> Add(timeCalib_betweenMods[icut][ifit]);
      hRes -> Add(total);

      // save calibration constant as .txt for reference configuration
      if (saveTcTxt && icut == SYSTEMATIC::RefIADP && ifit == SYSTEMATIC::RefFitR) {

        TString outdirtxt = Form("TimeCalibConstant/Formated_Run%s", runId.c_str());
        if (!std::filesystem::exists(outdirtxt.Data())) {
          std::filesystem::create_directory(outdirtxt.Data()); 
        }

        TString outfiletxt = Form("%s/t0_DCV_%d.txt", outdirtxt.Data(), periodId);

        // create .txt and write
        ofstream file(outfiletxt.Data());
        if (file.is_open()) {
          // first line : # of channels
          file << 32 << endl;
          // Next, t0 value of each channels and their error
          for (int ich=0; ich<32; ich++) {
            int binIdx = total->FindBin(ich);
            float tc_val = total->GetBinContent(binIdx);
            float tc_err = total->GetBinError(binIdx);
            TString record = Form("%d %f %f", ich, tc_val, tc_err);
            file << record.Data() << endl;
          }
          file.close();
          DCV_LOG_INFO(">>>>> Formated Time calibration file created: " << outfiletxt);
        }
      }

    }// FitR syst
  }// IADP syst

  // Wrapup
  // ------
  ofile -> cd();
  //timeCalib_adjoint     -> Write();
  //timeCalib_updown      -> Write();
  //timeCalib_betweenMods -> Write();
  //hRes -> Write("", TObject::kOverwrite);
  hRes -> Write();
  ofile -> Close();
  infile -> Close();
}
