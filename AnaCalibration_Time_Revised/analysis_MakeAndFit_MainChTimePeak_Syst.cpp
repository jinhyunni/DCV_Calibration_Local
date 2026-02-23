#include "analysisHeader.h"
#include "SystematicSources.h" // -> Systematic information of IADP

//! This code makes time peak distribution & Fit gaussian function
void analysis_MakeAndFit_MainChTimePeak_Syst
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
  int RunConfigIdx = ReturnRunIndex(runId);

  if (periodId >= ParForCal[RunConfigIdx].size()) {
    DCV_LOG_ABORT("No parameters prepared for this period");
  }

  auto CalConfig = ParForCal[RunConfigIdx][periodId];
  if (changeConfigs) {
    CalConfig -> setAttPathL(changeAttPathL);
    CalConfig -> setN2SubQC(changeN2SubQC);
    CalConfig -> setModEneUpdated(changeModEneUpdated);
    CalConfig -> setTrigCtr(changeTrigCtr);
  }

  TString infilename= Form("BaseHists/Run%s_Period%02d_TimePeakCorr%s.root", runId.c_str(), periodId, CalConfig->infoTrigCtr.Data());
  TFile* infile= new TFile(infilename, "read");

  if (infile->IsZombie() ) {
    DCV_LOG_ABORT("Required input file not found: " << infilename.Data());
  } else {
    DCV_LOG_INFO(">>>>>>> READ IN: " << infile->GetName());
  }

  TString infilename_iadp = Form("%s/Run%s_Period%02d_DCVSignalMain%s.root", std::getenv("DIR_CHECK_DCV_SIG"), runId.c_str(), periodId, CalConfig->infoTrigCtr.Data());
  TFile* infileIadp = new TFile(infilename_iadp, "read");
  if (infileIadp->IsZombie()) {
    DCV_LOG_ABORT("Required input file not found: " << infilename_iadp.Data());
  } else {
    DCV_LOG_INFO(">>>>>>> READ IN: " << infileIadp -> GetName());
  }
 
  // Making outputs
  // ==============
  TString outdir = "GaussFitRes";
  if (!std::filesystem::exists(outdir.Data())) {
    std::filesystem::create_directory(outdir.Data());
  }

  TString outputname = Form("%s/Run%s_Period%02d_GaussFitToMainTime%s_SystIadp.root", outdir.Data(), runId.c_str(), periodId, CalConfig->infoTrigCtr.Data());
  TFile* output = new TFile(outputname, "recreate");
  DCV_LOG_INFO(">>>>>>> Output: " << output->GetName());

  output -> mkdir("MB");
  TObjArray* MB         = new TObjArray(0);
  TObjArray* MB_Summary = new TObjArray(0);

  const int vecsize = SYSTEMATIC::SystIadp.size();

  std::vector<std::vector<TH1D*>> chi2NDF_MB(vecsize);
  std::vector<std::vector<TH1D*>> timePeakMean_MB(vecsize);
  std::vector<std::vector<TH1D*>> timePeakRMS_MB(vecsize);

  for (int icut=0; icut<SYSTEMATIC::SystIadp.size(); icut++) {
    for (int icutfit=0; icutfit<SYSTEMATIC::SystFitRangeTunePar.size(); icutfit++) {
      TString hisname0 = Form("Chi2NDF_SystIadpCutset%d_SystFitRangeTuneset%d_MB", icut, icutfit);
      TString hisname1 = Form("TimePeakMean_SystIadpCutset%d_SystFitRangeTuneset%d_MB", icut, icutfit);
      TString hisname2 = Form("TimePeakRMS_SystIadpCutset%d_SystFitRangeTuneset%d_MB", icut, icutfit);

      chi2NDF_MB[icut].push_back(new TH1D(hisname0, "x: ch, y:Chi2NDF", 32, 0, 32));
      timePeakMean_MB[icut].push_back(new TH1D(hisname1, "x: ch, y:Mean from fit", 32, 0, 32));
      timePeakRMS_MB[icut].push_back(new TH1D(hisname2, "x: ch, y:RMS from fit", 32, 0, 32));
      
      MB_Summary -> Add(chi2NDF_MB[icut][icutfit]);
      MB_Summary -> Add(timePeakMean_MB[icut][icutfit]);
      MB_Summary -> Add(timePeakRMS_MB[icut][icutfit]);
    }
  }

  // Taking in histograms
  // 1. h2MainTimeAdc_withSubAdcCut
  // 2. h2MainTimeWithGoodSub_withSubAdcCut
  // => By selecting good signal on sub channel, the number of entries will be same for two histogram
  // also make projection at the same time

  for (auto itTrack = TrackPairMPPC.begin(); itTrack != TrackPairMPPC.end(); itTrack++) {

    int trackId = std::distance(TrackPairMPPC.begin(), itTrack);
    const auto& pairs = *itTrack;

    for (auto itPair = pairs.begin(); itPair != pairs.end(); itPair++) {
      int histIndex = std::distance(pairs.begin(), itPair);
      const auto& currentPair = *itPair;

      int mainMod = currentPair.mainMod;
      int subMod = currentPair.subMod;

      // Histogram input
      TString inhisname0 = Form("DCVMainSig_trk%02d_mainMod%02d_time_vs_iadp_MB", trackId, mainMod);
      TString inhisname1 = Form("DCVMainSig_trk%02d_mainMod%02d_time_vs_iadp_vs_peak", trackId, mainMod);

      TH2D* h2MainTimeIadp_MB = (TH2D*)infile -> Get(inhisname0);

      TH3D* h3MainTimeIadpPeak = (TH3D*)infileIadp -> Get(inhisname1);
      if (h3MainTimeIadpPeak == NULL) {
        cout << " Histogram not stroed. Abort!" << endl;
        abort();
      }
      TH1D* h1MainIadp = (TH1D*)h3MainTimeIadpPeak -> ProjectionY(inhisname1+"_ProjY");
      h1MainIadp -> Rebin(2);

      float xPeakIadp = h1MainIadp -> GetBinCenter(h1MainIadp->GetMaximumBin());
      float iadpRms = h1MainIadp -> GetRMS();

      // Just for check,
      cout << "Track:" << trackId << " MainChannel:" << mainMod << " xPeak of IADP:" << xPeakIadp << " RMS:" << iadpRms << endl; 

      // Histogram output
      std::vector<TH1D*> h1MainTimeMB;
      for (int icut=0; icut<SYSTEMATIC::SystIadp.size(); icut++) {
        TString outhisname = Form("Trk%02d_mainMod%02d_SystIadpCutset%dMB", trackId, mainMod, icut);
        
        // Find bin # for cut
        int binMin=0;
        int binMax=-1;
        if (icut != 0) {
          binMin = h2MainTimeIadp_MB -> GetYaxis() -> FindBin(xPeakIadp-SYSTEMATIC::SystIadp[icut]*iadpRms);
          binMax = h2MainTimeIadp_MB -> GetYaxis() -> FindBin(xPeakIadp+SYSTEMATIC::SystIadp[icut]*iadpRms);
        }
        
        // Do the projetion
        TH1D* outhis = (TH1D*)h2MainTimeIadp_MB -> ProjectionX(outhisname, binMin, binMax);
        h1MainTimeMB.push_back(outhis);
      }
     
      // Helper function to perform fit
      auto PerformFit = [&](TH1D* dataHist, int fitTuneSet, TH1D* fchi2NDF, TH1D* ftimePeakMean, TH1D* ftimePeakRMS, TObjArray* store) {
  
        //~~0. Store data
        store -> Add(dataHist);

        //~~1. Find peak position / Find Norm value
        float yMainPeak = dataHist -> GetMaximum();
        float xMainPeak = dataHist -> GetBinCenter(dataHist->GetMaximumBin());

        dataHist -> SetMaximum(1.15*yMainPeak);

        float rangeFitWindow = 1;

        dataHist -> GetXaxis() -> SetRangeUser(xMainPeak - rangeFitWindow, xMainPeak + rangeFitWindow);
        float sigmaMainPeak = dataHist -> GetRMS();
        float aMainPeak = dataHist -> Integral();
        dataHist -> GetXaxis() -> SetRangeUser(0, 100);

        //~~2. Define fitting function
        TString outhisname = dataHist -> GetName();
        float tune = SYSTEMATIC::SystFitRangeTunePar[fitTuneSet];
        TF1* ffit = new TF1(Form("GaussFit_%s_SystFitRangeTuneset%d", outhisname.Data(), fitTuneSet), "[0]*TMath::Gaus(x, [1], [2])", xMainPeak-tune*sigmaMainPeak, xMainPeak+tune*sigmaMainPeak);
        ffit -> SetParameters(yMainPeak, xMainPeak, sigmaMainPeak);

        ffit -> SetParLimits(0, 0.5*yMainPeak, 1.5*yMainPeak);
        ffit -> SetParLimits(1, xMainPeak-tune*sigmaMainPeak, xMainPeak+tune*sigmaMainPeak);
        ffit -> SetParLimits(2, 0.0, 2.0);

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
      
      // Perform fit & saving results 
      for (int icut=0; icut<SYSTEMATIC::SystIadp.size(); icut++) {
        for (auto fitpar=0; fitpar < SYSTEMATIC::SystFitRangeTunePar.size(); fitpar++) {
          PerformFit(h1MainTimeMB[icut], fitpar, chi2NDF_MB[icut][fitpar], timePeakMean_MB[icut][fitpar], timePeakRMS_MB[icut][fitpar], MB);
        }
      }
    }
  }

  // Write output
  // ============
  MB_Summary -> Write();
  output -> cd("MB");
  MB -> Write("", TObject::kOverwrite);
  output -> Close();
  infile -> Close();

}
