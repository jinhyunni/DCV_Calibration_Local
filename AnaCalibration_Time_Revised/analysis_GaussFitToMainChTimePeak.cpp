#include "analysisHeader.h"

void analysis_GaussFitToMainChTimePeak
(
  std::string runId         = "92",
  int periodId              = 0,
  bool changeConfigs        = true,
  bool changeAttPathL       = true,
  bool changeN2SubQC        = false,
  bool changeModEneUpdated  = true,
  bool changeTrigCtr        = true,

  bool QA                   = true
)
{
  // Making inputs
  // =============
  int runIdx = ReturnRunIndex(runId);
  if (periodId >= ParForCal[runIdx].size()) {
    DCV_LOG_ABORT("This period doesn't have parameters for calibration");
  }

  auto CalConfig = ParForCal[runIdx][periodId];
  if (changeConfigs) {
    CalConfig -> setAttPathL(changeAttPathL);
    CalConfig -> setN2SubQC(changeN2SubQC);
    CalConfig -> setModEneUpdated(changeModEneUpdated);
    CalConfig -> setTrigCtr(changeTrigCtr);
  }

  TFile* infile = NULL;
  TString infilename= Form("BaseHists/Run%s_Period%02d_TimePeakCorr%s.root", runId.c_str(), periodId, CalConfig->infoTrigCtr.Data());
 
  if (!std::filesystem::exists(infilename.Data())) {
    DCV_LOG_ABORT("Requred inputfile not found: " << infilename);
  } else {
    infile= new TFile(infilename, "read");
  }

  // Making outputs
  // ==============
  TString outdir = "GaussFitRes";
  if (!std::filesystem::exists(outdir.Data())) {
    std::filesystem::create_directory(outdir.Data());
  }

  TString outputname = Form("%s/Run%s_Period%02d_GaussFitToMainTime%s.root", outdir.Data(), runId.c_str(), periodId, CalConfig->infoTrigCtr.Data());
  TFile* output = new TFile(outputname, "recreate");

  output -> mkdir("MB");
  output -> mkdir("IADP");
  output -> mkdir("GoodSubSig");

  TObjArray* MB           = new TObjArray(0);
  TObjArray* GoodIadpMain = new TObjArray(0);
  TObjArray* GoodSubSig   = new TObjArray(0);

  TH1D* chi2NDF_MB       = new TH1D("Chi2NDF_MB", "x: ch, y: Chi2NDF", 32, 0, 32);
  TH1D* timePeakMean_MB  = new TH1D("timePeakMean_MB", "x: ch, y: Mean from Fit", 32, 0, 32);
  TH1D* timePeakRMS_MB   = new TH1D("timePeakRMS_MB", "x: ch, y: RMS from Fit", 32, 0, 32);
  MB -> Add(chi2NDF_MB);
  MB -> Add(timePeakMean_MB);
  MB -> Add(timePeakRMS_MB);

  TH1D* chi2NDF_GoodIadpMain       = new TH1D("Chi2NDF_GoodIadpMain", "x: ch, y: Chi2NDF", 32, 0, 32);
  TH1D* timePeakMean_GoodIadpMain  = new TH1D("timePeakMean_GoodIadpMain", "x: ch, y: Mean from Fit", 32, 0, 32);
  TH1D* timePeakRMS_GoodIadpMain   = new TH1D("timePeakRMS_GoodIadpMain", "x: ch, y: RMS from Fit", 32, 0, 32);
  GoodIadpMain -> Add(chi2NDF_GoodIadpMain);
  GoodIadpMain -> Add(timePeakMean_GoodIadpMain);
  GoodIadpMain -> Add(timePeakRMS_GoodIadpMain);

  TH1D* chi2NDF_GoodSubSig       = new TH1D("Chi2NDF_GoodSubSig", "x: ch, y: Chi2NDF", 32, 0, 32);
  TH1D* timePeakMean_GoodSubSig  = new TH1D("timePeakMean_SubAdcCut", "x: ch, y: Mean from Fit", 32, 0, 32);
  TH1D* timePeakRMS_GoodSubSig   = new TH1D("timePeakRMS_SubAdcCut", "x: ch, y: RMS from Fit", 32, 0, 32);
  GoodSubSig -> Add(chi2NDF_GoodSubSig);
  GoodSubSig -> Add(timePeakMean_GoodSubSig);
  GoodSubSig -> Add(timePeakRMS_GoodSubSig);

  // Taking in histograms
  // 1. h2MainTimeAdc_withSubAdcCut
  // 2. h2MainTimeWithGoodSub_withSubAdcCut
  // => By selecting good signal on sub channel, the number of entries will be same for two histogram
  // also make projection at the same time

  float rangeFitWindow = 0.5;
  float fitRangeTunePar = 5;

  for (auto itTrack = TrackPairMPPC.begin(); itTrack != TrackPairMPPC.end(); itTrack++) {

    int trackId = std::distance(TrackPairMPPC.begin(), itTrack);
    const auto& pairs = *itTrack;

    for (auto itPair = pairs.begin(); itPair != pairs.end(); itPair++) {
      int histIndex = std::distance(pairs.begin(), itPair);
      const auto& currentPair = *itPair;

      int mainMod = currentPair.mainMod;
      int subMod = currentPair.subMod;

      TString inhisname0 = Form("DCVMainSig_trk%02d_mainMod%02d_time_vs_adc_MB", trackId, mainMod);
      //TString inhisname1 = Form("DCVMainSig_trk%02d_mainMod%02d_time_vs_adc_GoodIadpMain", trackId, mainMod);
      //TString inhisname2 = Form("DCVMainSig_trk%02d_mainMod%02d_time_vs_adc_WithSubAdcCut", trackId, mainMod);

      TString outhisname0 = Form("Trk%02d_mainMod%02d_MB", trackId, mainMod);
      //TString outhisname1 = Form("Trk%02d_mainMod%02d_GoodIadpMain", trackId, mainMod);
      //TString outhisname2 = Form("Trk%02d_mainMod%02d_WithSubAdcCut", trackId, mainMod);
      
      TH2D* h2MainTimeAdc_MB            = (TH2D*)infile -> Get(inhisname0);
      //TH2D* h2MainTimeAdc_GoodIadpMain  = (TH2D*)infile -> Get(inhisname1);
      //TH2D* h2MainTimeAdc_GoodSubSignal = (TH2D*)infile -> Get(inhisname2);

      TH1D* h1MainTimeMB          = (TH1D*)h2MainTimeAdc_MB             -> ProjectionX(outhisname0);
      //TH1D* h1MainTimeIadpMain    = (TH1D*)h2MainTimeAdc_GoodIadpMain   -> ProjectionX(outhisname1);
      //TH1D* h1MainTimeGoodSubSig  = (TH1D*)h2MainTimeAdc_GoodSubSignal  -> ProjectionX(outhisname2);
    
      auto PerformFit = [&](TH1D* dataHist, TH1D* fchi2NDF, TH1D* ftimePeakMean, TH1D* ftimePeakRMS, TObjArray* store) {
  
        TString outhisname = dataHist -> GetName();

        //~~0. Store data
        store -> Add(dataHist);

        //~~1. Find peak position / Find Norm value
        float yMainPeak = dataHist -> GetMaximum();
        float xMainPeak = dataHist -> GetBinCenter(dataHist->GetMaximumBin());

        dataHist -> SetMaximum(1.15*yMainPeak);
          
        dataHist -> GetXaxis() -> SetRangeUser(xMainPeak - rangeFitWindow, xMainPeak + rangeFitWindow);
        float sigmaMainPeak = dataHist -> GetRMS();
        float aMainPeak = dataHist -> Integral();
        dataHist -> GetXaxis() -> SetRangeUser(0, 100);

        //~~2. Define fitting function
        TF1* ffit = new TF1("GaussFit_"+outhisname, "[0]*TMath::Gaus(x, [1], [2])", xMainPeak-fitRangeTunePar*sigmaMainPeak, xMainPeak+fitRangeTunePar*sigmaMainPeak);
        ffit -> SetParameters(yMainPeak, xMainPeak, sigmaMainPeak);

        ffit -> SetParLimits(0, 0.5*yMainPeak, 1.5*yMainPeak);
        ffit -> SetParLimits(1, xMainPeak-fitRangeTunePar*sigmaMainPeak, xMainPeak+fitRangeTunePar*sigmaMainPeak);
        ffit -> SetParLimits(2, 0.0, 10.0);

        //~~3. Perform fitting
        dataHist -> Fit(ffit, "R0Q");

        //~~4. Save fitted function for QA
        store -> Add(ffit);
          
        //~~5. Save some information for QA
        float chi2ndf = ffit->GetChisquare()/(float)ffit->GetNDF();
        fchi2NDF -> SetBinContent(fchi2NDF->FindBin(mainMod), chi2ndf);
          
        ftimePeakMean -> SetBinContent(ftimePeakMean->FindBin(mainMod), ffit->GetParameter(1));
        ftimePeakMean -> SetBinError(ftimePeakMean->FindBin(mainMod), ffit->GetParError(1));
     
        ftimePeakRMS -> SetBinContent(ftimePeakRMS->FindBin(mainMod), ffit->GetParameter(2));
        ftimePeakRMS -> SetBinError(ftimePeakRMS->FindBin(mainMod), ffit->GetParError(2));
      };
      
      PerformFit(h1MainTimeMB, chi2NDF_MB, timePeakMean_MB, timePeakRMS_MB, MB);
      //PerformFit(h1MainTimeIadpMain, chi2NDF_GoodIadpMain, timePeakMean_GoodIadpMain, timePeakRMS_GoodIadpMain, GoodIadpMain);
      //PerformFit(h1MainTimeGoodSubSig, chi2NDF_GoodSubSig, timePeakMean_GoodSubSig, timePeakRMS_GoodSubSig, GoodSubSig);

    }
  }

  // Write output
  // ============
  output -> cd("MB");
  MB -> Write();
  output -> cd("IADP");
  GoodIadpMain -> Write();
  output -> cd("GoodSubSig");
  //GoodSubSig -> Write();
  output -> Close();

}
