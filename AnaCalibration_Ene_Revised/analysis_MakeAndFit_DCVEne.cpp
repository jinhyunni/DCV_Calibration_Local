#include "analysisHeader.h"
#include "LanGausFitPar.h" // <-- Parameters for langaus fit

// Helper function: Reading in file & Saving objects
// --------------------------------------------------
template <typename T>
void ReadFileAndObject(TFile*& file, T*& object, TString& filename, TString& objname) {
  
  if (!std::filesystem::exists(filename.Data())) {
    DCV_LOG_ABORT("Required input file not found: " << filename);
  } else {
    file = new TFile(filename, "read");
    object = (T*)file->Get(objname);
    
    DCV_LOG_INFO(">>>>>READ IN: " << filename);

    if (object == nullptr) {
      DCV_LOG_ABORT("File is ready but ttree not found");
    } else {
      DCV_LOG_INFO(">>>>>READ IN: " << objname);
    }
  }
}

// Helper function: Make required histogram
// ----------------------------------------
void PrepareOutputHistogram(int objOption, TString& info_constraint, std::vector<std::vector<TH1F*>>& storage, TObjArray*& list) {

  TString outputObj;
  if (objOption > DCVMODULEENE) {
    DCV_LOG_ABORT("Wrong option given at calibrationOption(0~2): " << objOption);
  } else if (objOption == DCVPEAK) {
    outputObj="DCVPeak";
  } else if (objOption == DCVPAIRENE) {
    outputObj="DCVPairEne";
  } else if (objOption == DCVMODULEENE) {
    outputObj="DCVModuleEne";
  }

  auto it = TrackModuleMPPC.begin();
  const auto itBegin = TrackModuleMPPC.begin();
  const auto itEnd = TrackModuleMPPC.end();

  const int nBins[nOBJS] = {160, 160, 160};
  const int minBins[nOBJS] = {-100, -1, -1};
  const int maxBins[nOBJS] = {1500, 15, 15};

  for (;it != itEnd; it++) {
  
    int trackId = std::distance(itBegin, it);
    const auto& objs = *it;

    for (auto objIt = objs.begin(); objIt != objs.end(); objIt++) {

      const auto& currentObj = *objIt;
      int histIdx = std::distance(objs.begin(), objIt);
      TString outHisNameBase0;
      TString outHisNameBase1;

      int mainMod0 = currentObj.upstream.mainMod;
      int mainMod1 = currentObj.downstream.mainMod;
      int subMod0 = currentObj.upstream.subMod;
      int subMod1 = currentObj.downstream.subMod;

      if (objOption == DCVPEAK) {
        outHisNameBase0 = Form("%s_%s_trk%02d_Mod%02d", outputObj.Data(), info_constraint.Data(), trackId, mainMod0);
        outHisNameBase1 = Form("%s_%s_trk%02d_Mod%02d", outputObj.Data(), info_constraint.Data(), trackId, mainMod1);
      } else if (objOption == DCVPAIRENE) {
        outHisNameBase0 = Form("%s_%s_trk%02d_MainMod%02d_SubMod%02d", outputObj.Data(), info_constraint.Data(), trackId, mainMod0, subMod0);
        outHisNameBase1 = Form("%s_%s_trk%02d_MainMod%02d_SubMod%02d", outputObj.Data(), info_constraint.Data(), trackId, mainMod1, subMod1);
      } else if (objOption == DCVMODULEENE) {
        outHisNameBase0 = Form("%s_%s_trk%02d_Upstream_Main%02d_Sub%02d_Downstream_Main%02d_Sub%02d", outputObj.Data(), info_constraint.Data(), trackId, mainMod0, subMod0, mainMod1, subMod1);
      }

      TH1F* h1out0 = nullptr;
      TH1F* h1out1 = nullptr;

      h1out0 = new TH1F(outHisNameBase0, outHisNameBase0, nBins[objOption], minBins[objOption], maxBins[objOption]);
      h1out0 -> Sumw2();
      storage[trackId].push_back(h1out0);
      DCV_LOG_INFO(">>>>> CHECK HISTOGRAM CREATED: " << h1out0 -> GetName());
      if (objOption != DCVMODULEENE) {
        h1out1 = new TH1F(outHisNameBase1, outHisNameBase1, nBins[objOption], minBins[objOption], maxBins[objOption]);
        h1out1 -> Sumw2();
        storage[trackId].push_back(h1out1);
        DCV_LOG_INFO(">>>>> CHECK HISTOGRAM CREATED: " << h1out1 -> GetName());
      }
      
      if (storage[trackId][histIdx] == NULL) {
        DCV_LOG_ABORT("Expected histogram not filled");
      } else {
        list -> Add(h1out0);
        if (objOption != DCVMODULEENE) {
          list -> Add(h1out1);
        } 
      }
    }
  }// Iteration over cosmic track ids
}

// Helper function: Fit required histogram
// ---------------------------------------
void FitGivenHistogram(TH1F*& inputHistogram, int objOption, int nRebin, float fitRangeTuneMin, float fitRangeTuneMax, double (&output_val)[4], double (&output_err)[4], double& SNRPeak, double& SNRFWHM, double& returnChiSqr, int& returnNdf, TObjArray*& saveList) {

  inputHistogram -> Rebin(nRebin);

  // Get initial values for fitting
  int iniMin, iniMax, min, max;
  if (objOption == DCVPEAK) {
    iniMin = 100;
    iniMax = 1500;
    min = -100;
    max = 1500;
  } else if (objOption == DCVPAIRENE) {
    iniMin = 0;
    iniMax = 15;
    min = -1;
    max = 15;
  } else if (objOption == DCVMODULEENE) {
    iniMin = 0;
    iniMax = 15;
    min = -1;
    max = 15;
  }

  inputHistogram -> SetAxisRange(iniMin, iniMax);
  double xPeak = inputHistogram -> GetBinCenter(inputHistogram->GetMaximumBin());
  double iniArea = inputHistogram -> Integral();
  inputHistogram -> SetAxisRange(min, max);

  // Decide fit range
  double fitRange[2];
  double h1Mean = inputHistogram -> GetMean();
  fitRange[0] = h1Mean*fitRangeTuneMin;
  fitRange[1] = h1Mean*fitRangeTuneMax;

  // Give initial value
  double startValue[4];
  double h1RMS = inputHistogram -> GetRMS();
  double iniWidthLangaus;
  if (objOption == DCVPEAK) {
    iniWidthLangaus = 50.;
  } else if (objOption == DCVPAIRENE)  {
    iniWidthLangaus = 0.2;
  } else if (objOption == DCVMODULEENE) {
    iniWidthLangaus = 0.2;
  }

  startValue[0] = h1RMS*0.5;
  startValue[1] = xPeak;
  startValue[2] = iniArea;
  startValue[3] = iniWidthLangaus;
  
  // Fit with langaus function
  TF1* ffit = langaufit(inputHistogram,
                        fitRange,
                        startValue,
                        parLimitLow[objOption],
                        parLimitHigh[objOption],
                        output_val,
                        output_err,
                        &returnChiSqr,
                        &returnNdf);
  // Find maximum x of fitted function
  langaupro(output_val, SNRPeak, SNRFWHM);

  // Print out fitted results
  DCV_LOG_INFO(Form("%s Entries: %.0f FitRange: %4.2f~%4.2f MPV: %.2f RelativeErrorMPV: %.2f FWHM: %.2f Chi2/NDF: %.2f Rebin: %d",
                    inputHistogram->GetName(),
                    inputHistogram->GetEntries(),
                    fitRange[0], fitRange[1],
                    SNRPeak,
                    output_err[1]/output_val[1]*100,
                    SNRFWHM,
                    ffit->GetChisquare()/ffit->GetNDF(),
                    nRebin));

   // Print out errors if relative errors exceeds 5%
   if (output_err[1]/output_val[1]*100 > 5.0) {
     DCV_LOG_ERROR(Form("%s Entries: %.0f FitRange: %4.2f~%4.2f MPV: %.2f RelativeErrorMPV: %.2f FWHM: %.2f Chi2/NDF: %.2f Rebin: %d",
                        inputHistogram->GetName(),
                        inputHistogram->GetEntries(),
                        fitRange[0], fitRange[1],
                        SNRPeak,
                        output_err[1]/output_val[1]*100,
                        SNRFWHM,
                        ffit->GetChisquare()/ffit->GetNDF(),
                        nRebin));
   }

   // Save result to list
   ffit -> SetName(Form("LangausFit_%s", inputHistogram->GetName()));
   saveList -> Add(ffit);
}

// Helper function: Make plots and save them
// -----------------------------------------
void SavePlotAsPdf(int calibrationOption, TH1F*& fittedHistogram, TObjArray*& f1List, TString& savedir) {

  const auto& currentHist = fittedHistogram;
  const auto& currentHistName = currentHist -> GetName();
  const auto& currentFit  = (TF1*)f1List->FindObject(Form("LangausFit_%s", currentHistName));

  // Settings for Marker styles and colors
  int usedTextFont = 43;
  int usedTextSize = 20;

  // Prepare canvas
  TCanvas* c1 = new TCanvas("c1", "c1", 1.2*1000, 1000);
  c1 -> cd();

  TPad* p1 = new TPad("p1", "p1", 0.0, 0.0, 1.0, 1.0);
  p1 -> Draw();
  p1 -> cd();
  p1 -> SetTicks();
  p1 -> SetBottomMargin(0.05);
  p1 -> SetLeftMargin(0.05);
  p1 -> SetTopMargin(0.02);
  p1 -> SetRightMargin(0.05);

  float ymax = currentHist->GetMaximum();
  float xmin, xmax;
  if (calibrationOption == DCVPEAK) {
    xmin = -100.0;
    xmax = 1500.0;
  } else {
    xmin = -1.0;
    xmax = 15.0;
  }
  TH1F* htmp = (TH1F*)gPad -> DrawFrame(xmin, 0.0, xmax, ymax*1.5);
  
  TLegend* l1 = new TLegend(0.1, 0.8, 0.9, 0.9);
  l1 -> SetFillStyle(0);
  l1 -> SetBorderSize(0);
  l1 -> SetMargin(0.02);
  l1 -> SetTextFont(usedTextFont);
  l1 -> SetTextSize(usedTextSize);

  TLegend* l2 = new TLegend(0.50, 0.7, 0.8, 0.8);
  l2 -> SetFillStyle(0);
  l2 -> SetBorderSize(0);
  l2 -> SetMargin(0.02);
  l2 -> SetTextFont(usedTextFont);
  l2 -> SetTextSize(usedTextSize);

  // Draw histogram & Fit function
  currentHist -> SetMarkerSize(0.7);
  currentHist -> SetMarkerColor(kBlack);
  currentHist -> SetLineColor(kBlack);
  currentHist -> SetMarkerStyle(25);
  currentHist -> Draw("same");

  currentFit -> SetLineStyle(2);
  currentFit -> SetLineColor(kRed);
  currentFit -> Draw("same");

  // Legend settings
  double returnVals[4];
  double returnErrs[4];
  double snrpeak, snrfwhm;
  currentFit -> GetParameters(returnVals);
  for (int i=0; i<4; i++) {
    returnErrs[i] = currentFit->GetParError(i);
  }
  langaupro(returnVals, snrpeak, snrfwhm);

  double mpv_val = snrpeak;
  double mpv_err = returnErrs[0]/returnVals[0]*mpv_val;
  double chi2 = currentFit->GetChisquare();
  int ndf = currentFit->GetNDF();

  l1 -> AddEntry(currentHist, currentHist->GetName(), "p");
  l2 -> AddEntry("", Form("Entries: %.0f", currentHist->GetEntries()), "h");
  l2 -> AddEntry("", Form("MPV: %.2f#pm%.3f", mpv_val, mpv_err), "h");
  l2 -> AddEntry("", Form("Chi2/Ndf: %.2f/%d(%.2f)", chi2, ndf, chi2/(double)ndf), "h");
  l1 -> Draw("same");
  l2 -> Draw("same");

  // save the result as pdf
  TString savedir_pdf = savedir+"/pdf";
  if (!std::filesystem::exists(savedir_pdf.Data())) {
    std::filesystem::create_directory(savedir_pdf.Data());
  }
  
  TString savefilename = Form("%s/%s.pdf", savedir_pdf.Data(), currentHistName);
  c1 -> SaveAs(savefilename);
}

// Main function: Perform histogram building & Langaus Fitting
// -----------------------------------------------------------
void analysis_MakeAndFit_DCVEne
(
  std::string runId         = "92",
  int periodId              = 0, 
  int calibrationOption     = DCVMODULEENE,
  int cutOption             = IADPTIME,
  
  bool saveResAsPdf         = true,

  bool changeConfigs        = true,
  bool changeAttPathL       = true,
  bool changeN2SubQC        = false,
  bool changeModEneUpdated  = true,
  bool changeTrigCtr        = true

)
{
  // Making input
  // ------------
  int runIdx = ReturnRunIndex(runId);
  if (periodId >= ParForCal[runIdx].size()) {
    DCV_LOG_ABORT("Parameter settings for this period not prepared");
  }

  auto Config = ParForCal[runIdx][periodId];
  if (changeConfigs) {
    Config->setAttPathL(changeAttPathL);
    Config->setN2SubQC(changeN2SubQC);
    Config->setModEneUpdated(changeModEneUpdated);
    Config->setTrigCtr(changeTrigCtr);
  }
 
  // Info
  TString cutinfo_suffix_file;
  TString cutinfo_suffix_hist;
  if (cutOption == MB) {
    cutinfo_suffix_file = "";
    cutinfo_suffix_hist = "NoCuts";
  } else if (cutOption == IADP) {
    cutinfo_suffix_file = Form("_%sCutApplied", Config->IADPCut.CutName.Data());
    cutinfo_suffix_hist = Form("with%sCut", Config->IADPCut.CutName.Data());
  } else if (cutOption == TIME) {
    cutinfo_suffix_file = Form("_%sCutApplied", Config->TimeCut.CutName.Data());
    cutinfo_suffix_hist = Form("with%sCut", Config->IADPCut.CutName.Data());
  } else if (cutOption == IADPTIME) {
    cutinfo_suffix_file = Form("_%s%sCutApplied", Config->IADPCut.CutName.Data(), Config->TimeCut.CutName.Data());
    cutinfo_suffix_hist = Form("with%s%sCut", Config->IADPCut.CutName.Data(), Config->TimeCut.CutName.Data());
  } else {
    DCV_LOG_ABORT("Wrong input given at cutOption(0~3): " << cutOption);
  }
 
  TString h1ObjInfo;
  if (calibrationOption == DCVPEAK) {
    h1ObjInfo = "DCVPeak";
  } else if (calibrationOption == DCVPAIRENE) {
    h1ObjInfo = "DCVPairEne";
  } else if (calibrationOption == DCVMODULEENE) {
    h1ObjInfo = "DCVModuleEne";
  }

  // Prepare cosmic tracked events
  TFile* cosmic     = nullptr;
  TTree* tin_cosmic = nullptr;

  TFile* mc         = nullptr;
  TH1D* mc_mpv      = nullptr;

  TFile* fileN1     = nullptr;
  TFile* fileN2     = nullptr;
  TFile* fileN3     = nullptr;
  TH1F* hisN1       = nullptr;
  TH1F* hisN2       = nullptr;
  TH1F* hisN3       = nullptr;

  TFile* outfile_nx = nullptr;
  TH1F* outhis_nx   = nullptr;
  
  // Prepare cosmic tracked events
  TString filename_cosmic = Form("%s/Run%s_Period%02d_trackMapping_ttree%s.root", std::getenv("DIR_COSMIC_TRACKED_FILE"), runId.c_str(), periodId, Config->infoTrigCtr.Data());
  TString tname_cosmic= "tout";
  ReadFileAndObject<TTree>(cosmic, tin_cosmic, filename_cosmic, tname_cosmic);

  const int DCVCH=32;

  int DCVNumber;
  int DCVModID[DCVCH];
  short DCVPeak[DCVCH];
  float DCVIntegratedADC[DCVCH];
  float DCVTime[DCVCH];
  int CosmicTrackNumber; // number of reconstructed cosmic events in this entry
  int CosmicTrackID[10]; // cosmic track ids of current cosmic event

  tin_cosmic -> SetBranchAddress("DCVNumber", &DCVNumber);
  tin_cosmic -> SetBranchAddress("DCVModID", DCVModID);
  tin_cosmic -> SetBranchAddress("DCVPeak", DCVPeak);
  tin_cosmic -> SetBranchAddress("DCVIntegratedADC", DCVIntegratedADC);
  tin_cosmic -> SetBranchAddress("DCVTime", DCVTime);
  tin_cosmic -> SetBranchAddress("CosmicTrackNumber", &CosmicTrackNumber);
  tin_cosmic -> SetBranchAddress("CosmicTrackID", CosmicTrackID);

  // Prepare Nx result if requested
  TString savedir = Form("Run%s_Period%02d_Nx", runId.c_str(), periodId);
  if (!std::filesystem::exists(savedir.Data())) {
    std::filesystem::create_directory(savedir.Data());
  }

  if (calibrationOption >= nOBJS) {
    DCV_LOG_ABORT("Wrong option given at calibrationOption(0~2): " << calibrationOption);
  } else if (calibrationOption == DCVPEAK) {
    TString outputfilename0 = Form("%s/Run%s_Period%02d_N1%s%s.root", savedir.Data(), runId.c_str(), periodId, cutinfo_suffix_file.Data(), Config->infoTrigCtr.Data());
    TString hisname_n1 = "hisN1";
    fileN1 = new TFile(outputfilename0, "recreate");
    hisN1 = new TH1F(hisname_n1, hisname_n1, 32, 0, 32); 

    outfile_nx = fileN1;
    outhis_nx = hisN1;
  } else if (calibrationOption == DCVPAIRENE) {
    TString inputfilename0 = Form("%s/Run%s_Period%02d_N1%s%s.root", savedir.Data(), runId.c_str(), periodId, cutinfo_suffix_file.Data(), Config->infoTrigCtr.Data());
    TString hisname_n1 = "hisN1";
    ReadFileAndObject<TH1F>(fileN1, hisN1, inputfilename0, hisname_n1); 

    TString outputfilename0 = Form("%s/Run%s_Period%02d_N2%s%s.root", savedir.Data(), runId.c_str(), periodId, cutinfo_suffix_file.Data(), Config->infoTrigCtr.Data());
    TString hisname_n2 = "hisN2";
    fileN2 = new TFile(outputfilename0, "recreate");
    hisN2 = new TH1F(hisname_n2, hisname_n2, 32, 0, 32);

    outfile_nx = fileN2;
    outhis_nx = hisN2;
  } else if (calibrationOption == DCVMODULEENE) {
    TString inputfilename0 = Form("%s/Run%s_Period%02d_N1%s%s.root", savedir.Data(), runId.c_str(), periodId, cutinfo_suffix_file.Data(), Config->infoTrigCtr.Data());
    TString inputfilename1 = Form("%s/Run%s_Period%02d_N2%s%s.root", savedir.Data(), runId.c_str(), periodId, cutinfo_suffix_file.Data(), Config->infoTrigCtr.Data());
    TString hisname_n1 = "hisN1";
    TString hisname_n2 = "hisN2";
    ReadFileAndObject<TH1F>(fileN1, hisN1, inputfilename0, hisname_n1); 
    ReadFileAndObject<TH1F>(fileN2, hisN2, inputfilename1, hisname_n2); 

    TString outputfilename0 = Form("%s/Run%s_Period%02d_N3%s%s.root", savedir.Data(), runId.c_str(), periodId, cutinfo_suffix_file.Data(), Config->infoTrigCtr.Data());
    TString hisname_n3 = "hisN3";
    fileN3 = new TFile(outputfilename0, "recreate");
    hisN3 = new TH1F(hisname_n3, hisname_n3, 32, 0, 32);

    outfile_nx = fileN3;
    outhis_nx = hisN3;
  } 

  outhis_nx -> Sumw2();

  // Ext histograms to be filled at Nx outfile
  TH1F* h1_mpv_val= new TH1F("mpv_val", "mpv_val", 32, 0, 32); 
  TH1F* h1_mpv_err= new TH1F("mpv_err", "mpv_err", 32, 0, 32); 
  TH1F* h1_fwhm = new TH1F("fwhm", "fwhm", 32, 0, 32); 

  h1_mpv_val -> Sumw2();
  h1_mpv_err -> Sumw2();
  h1_fwhm -> Sumw2();

  // prepare MC result
  TString filename_mc = Form("Run91_Period16_Nx/Run91_Period16_LanGausMpvWithError%s.root", Config->infoTrigCtr.Data());
  TString mpv_mc = "LanGausMpv";
  ReadFileAndObject<TH1D>(mc, mc_mpv, filename_mc, mpv_mc);

  // Make output
  // -----------
  TFile* h1File = new TFile(Form("%s/Run%s_Period%02d_%s%s.root", savedir.Data(), runId.c_str(), periodId, h1ObjInfo.Data(), cutinfo_suffix_file.Data()), "recreate");
  TObjArray* h1List = new TObjArray(0);
  TObjArray* f1List = new TObjArray(0);

  const int totalTrack = 12;
  std::vector<std::vector<TH1F*>> h1Ene(totalTrack);
  PrepareOutputHistogram(calibrationOption, cutinfo_suffix_hist, h1Ene, h1List);

  // Analysis 1. Make histogram
  // --------------------------
  int nEntries = tin_cosmic->GetEntries();

  for (int ientry=0; ientry<nEntries; ientry++) {
    tin_cosmic -> GetEntry(ientry);

    for (int i_cos=0; i_cos<CosmicTrackNumber; i_cos++) {

      int TID = CosmicTrackID[i_cos];
      const auto& ModWithHit = TrackModuleMPPC[TID];

      for (auto it = ModWithHit.begin(); it != ModWithHit.end(); it++) {
        
        const auto& currentMod = *it;
        const int histIdx = std::distance(ModWithHit.begin(), it);

        int main0 = currentMod.upstream.mainMod;
        int main1 = currentMod.downstream.mainMod;
        int sub0= currentMod.upstream.subMod;
        int sub1= currentMod.downstream.subMod;

        // Get Properties
        short peak_main0  = DCVPeak[main0];
        short peak_main1  = DCVPeak[main1];
        short peak_sub0   = DCVPeak[sub0];
        short peak_sub1   = DCVPeak[sub1];

        float time_main0  = DCVTime[main0];
        float time_main1  = DCVTime[main1];
        float time_sub0   = DCVTime[sub0];
        float time_sub1   = DCVTime[sub1];
        
        float iadp_main0  = DCVIntegratedADC[main0]/(float)DCVPeak[main0];
        float iadp_main1  = DCVIntegratedADC[main1]/(float)DCVPeak[main1];
        float iadp_sub0   = DCVIntegratedADC[sub0]/(float)DCVPeak[sub0];
        float iadp_sub1   = DCVIntegratedADC[sub1]/(float)DCVPeak[sub1];

        // Evalute qc
        bool pass_iadp_main0 = (iadp_main0 >= Config->IADPCut.LE) && (iadp_main0 < Config->IADPCut.HE);
        bool pass_time_main0 = (time_main0 >= Config->TimeCut.LE) && (time_main0 < Config->TimeCut.HE);
        bool pass_iadp_main1 = (iadp_main1 >= Config->IADPCut.LE) && (iadp_main1 < Config->IADPCut.HE);
        bool pass_time_main1 = (time_main1 >= Config->TimeCut.LE) && (time_main1 < Config->TimeCut.HE);
        bool pass_iadptime_main0 = pass_iadp_main0 && pass_time_main0;
        bool pass_iadptime_main1 = pass_iadp_main1 && pass_time_main1;

        // Perform rejection if required
        bool resultSelection0 = true;
        bool resultSelection1 = true;

        if (cutOption != MB) {

          switch (calibrationOption) {

            case DCVPEAK:
              if (cutOption == IADP) {
                resultSelection0 = pass_iadp_main0;
                resultSelection1 = pass_iadp_main1;
              } else if (cutOption == TIME) {
                resultSelection0 = pass_time_main0;
                resultSelection1 = pass_time_main1;
              } else if (cutOption == IADPTIME) {
                resultSelection0 = pass_iadptime_main0;
                resultSelection1 = pass_iadptime_main1;
              }
              break;

            case DCVPAIRENE:
              if (cutOption == IADP) {
                resultSelection0 = pass_iadp_main0;
                resultSelection1 = pass_iadp_main1;
              } else if (cutOption == TIME) {
                resultSelection0 = pass_time_main0;
                resultSelection1 = pass_time_main1;
              } else if (cutOption == IADPTIME) {
                resultSelection0 = pass_iadptime_main0;
                resultSelection1 = pass_iadptime_main1;
              }
              break;

            case DCVMODULEENE:
              if (cutOption == IADP) {
                resultSelection0 = pass_iadp_main0 && pass_iadp_main1;
              } else if (cutOption == TIME) {
                resultSelection0 = pass_time_main0 && pass_time_main1;
              } else if (cutOption == IADPTIME) {
                resultSelection0 = pass_iadptime_main0 && pass_iadptime_main1;
              }
              break;
          }
        }
        
        // Calculate values to be filled
        float fill0, fill1;
        if (calibrationOption == DCVPEAK) {
          fill0 = peak_main0;
          fill1 = peak_main1;
        } else if (calibrationOption == DCVPAIRENE) {
          int bin_main0 = hisN1 -> FindBin(main0);
          int bin_main1 = hisN1 -> FindBin(main1);
          int bin_sub0 = hisN1 -> FindBin(sub0);
          int bin_sub1 = hisN1 -> FindBin(sub1);

          float n1_main0 = hisN1 -> GetBinContent(bin_main0);
          float n1_main1 = hisN1 -> GetBinContent(bin_main1);
          float n1_sub0 = hisN1 -> GetBinContent(bin_sub0);
          float n1_sub1 = hisN1 -> GetBinContent(bin_sub1);

          fill0 = peak_main0/n1_main0 + peak_sub0/n1_sub0;
          fill1 = peak_main1/n1_main1 + peak_sub1/n1_sub1;
        } else if (calibrationOption == DCVMODULEENE) {

          int bin_main0 = hisN1 -> FindBin(main0);
          int bin_main1 = hisN1 -> FindBin(main1);
          int bin_sub0 = hisN1 -> FindBin(sub0);
          int bin_sub1 = hisN1 -> FindBin(sub1);

          float n1_main0 = hisN1 -> GetBinContent(bin_main0);
          float n1_main1 = hisN1 -> GetBinContent(bin_main1);
          float n1_sub0 = hisN1 -> GetBinContent(bin_sub0);
          float n1_sub1 = hisN1 -> GetBinContent(bin_sub1);

          float pairene_up  = peak_main0/n1_main0 + peak_sub0/n1_sub0;
          float pairene_down= peak_main1/n1_main1 + peak_sub1/n1_sub1;

          float n2_main0 = hisN2 -> GetBinContent(bin_main0);
          float n2_main1 = hisN2 -> GetBinContent(bin_main1);

          pairene_up/=n2_main0;
          pairene_down/=n2_main1;

          if (Config->fModEneUpdated) {
            float length = (TID<6) ? fullL_DCV1 : fullL_DCV2;
            float lambda = (TID<6) ? lambda_DCV1 : lambda_DCV2;
            float attenu = exp(length/lambda);
            fill0 = 2*sqrt(pairene_up*pairene_down*attenu);
          } else {
            fill0 = pairene_up + pairene_down;
          }
        }
        
        // Fill in values
        if (calibrationOption == DCVPEAK) {
          if (resultSelection0) h1Ene[TID][2*histIdx+0] -> Fill(fill0);
          if (resultSelection1) h1Ene[TID][2*histIdx+1] -> Fill(fill1);
        } else if (calibrationOption == DCVPAIRENE) {
          if (resultSelection0) h1Ene[TID][2*histIdx+0] -> Fill(fill0);
          if (resultSelection1) h1Ene[TID][2*histIdx+1] -> Fill(fill1);
        } else if (calibrationOption == DCVMODULEENE) {
          if (resultSelection0) h1Ene[TID][histIdx] -> Fill(fill0);
        }
      }// module loop
    } // cosmic loop
  } // entry loop
  
  // Analysis 2. Fit histogram & Extract Nx value
  // --------------------------------------------
  for (int itrk=0; itrk<totalTrack; itrk++) {
    
    const auto& currentHists = h1Ene[itrk];
    const int nMods = currentHists.size();

    for (int imod=0; imod<nMods; imod++) {

      // Fit distribution
      double saveoutput_val[4];
      double saveoutput_err[4];
      double snrpeak;
      double snrfwhm;
      double returnchi2;
      int returnndf;

      float minFitR, maxFitR;
      if (calibrationOption == DCVPEAK) {
        minFitR = Config->N1FitMinFactor;
        maxFitR = Config->N1FitMaxFactor;
      } else if (calibrationOption == DCVPAIRENE) {
        minFitR = Config->N2FitMinFactor;
        maxFitR = Config->N2FitMaxFactor;
      } else if (calibrationOption == DCVMODULEENE) {
        minFitR = Config->N3FitMinFactor;
        maxFitR = Config->N3FitMaxFactor;
      }

      FitGivenHistogram(h1Ene[itrk][imod], calibrationOption, Config->fNRebin, minFitR, maxFitR, saveoutput_val, saveoutput_err, snrpeak, snrfwhm, returnchi2, returnndf, f1List);
      if (saveResAsPdf) {
        SavePlotAsPdf(calibrationOption, h1Ene[itrk][imod], f1List, savedir);
      }

      // Calculate Nx
      int ch0 = (calibrationOption != DCVMODULEENE) ? TrackMPPC[itrk][imod] : TrackModuleMPPC[itrk][imod].upstream.mainMod;
      int ch1 = (calibrationOption != DCVMODULEENE) ? -99 : TrackModuleMPPC[itrk][imod].downstream.mainMod;

      float normValue0 = 1;
      float normValue1 = 1;
      if (Config->fAttPathL) {
        if (calibrationOption == DCVPEAK) {
          normValue0 = mc_mpv->GetBinContent(mc_mpv->FindBin(ch0))*att[ch0];
        } else if (calibrationOption == DCVPAIRENE) {
          normValue0 = mc_mpv->GetBinContent(mc_mpv->FindBin(ch0))*att[ch0];
        } else if (calibrationOption == DCVMODULEENE) {
          normValue0 = mc_mpv->GetBinContent(mc_mpv->FindBin(ch0));
          normValue1 = mc_mpv->GetBinContent(mc_mpv->FindBin(ch1));
        }
      }

      float mpv = snrpeak;
      float Nx_0 = mpv / normValue0;
      float Nx_1 = mpv / normValue1;
      float mpv_err = saveoutput_err[1]/saveoutput_val[1] * mpv;
      float fwhm = snrfwhm;
      
      outhis_nx -> SetBinContent(outhis_nx->FindBin(ch0), Nx_0);
      outhis_nx -> SetBinError(outhis_nx->FindBin(ch0), 0);
      h1_mpv_val -> SetBinContent(h1_mpv_val->FindBin(ch0), mpv);
      h1_mpv_val -> SetBinError(h1_mpv_val->FindBin(ch0), mpv_err);
      h1_mpv_err -> SetBinContent(h1_mpv_err->FindBin(ch0), mpv_err);
      h1_mpv_err -> SetBinError(h1_mpv_err->FindBin(ch0), 0);
      h1_fwhm -> SetBinContent(h1_fwhm->FindBin(ch0), fwhm);
      h1_fwhm -> SetBinError(h1_fwhm->FindBin(ch0), 0);
      if (calibrationOption == DCVMODULEENE) {
        outhis_nx -> SetBinContent(outhis_nx->FindBin(ch1), Nx_1);
        outhis_nx -> SetBinError(outhis_nx->FindBin(ch1), 0);
        h1_mpv_val -> SetBinContent(h1_mpv_val->FindBin(ch1), mpv);
        h1_mpv_val -> SetBinError(h1_mpv_val->FindBin(ch1), mpv_err);
        h1_mpv_err -> SetBinContent(h1_mpv_err->FindBin(ch1), mpv_err);
        h1_mpv_err -> SetBinError(h1_mpv_err->FindBin(ch1), 0);
        h1_fwhm -> SetBinContent(h1_fwhm->FindBin(ch1), fwhm);
        h1_fwhm -> SetBinError(h1_fwhm->FindBin(ch1), 0);
      }
    }
  }

  // Wrapup -> Save results if requested
  cosmic -> Close();

  h1File -> cd();
  h1List -> Write();
  f1List -> Write();
  h1File -> Close();

  mc -> Close();

  outfile_nx -> cd();
  outhis_nx -> Write();
  h1_mpv_val -> Write();
  h1_mpv_err -> Write();
  h1_fwhm -> Write();
  outfile_nx -> Close();
}
