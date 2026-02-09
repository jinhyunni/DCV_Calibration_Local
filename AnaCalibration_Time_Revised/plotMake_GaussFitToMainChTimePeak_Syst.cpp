#include "analysisHeader.h"
#include "SystematicSources.h"

void plotMake_GaussFitToMainChTimePeak_Syst
(
  std::string runId         = "92",
  int periodId              = 0,
  int drawOption            = SYSTEMATIC::RefIADP,

  bool changeConfigs        = true,
  bool changeAttPathL       = true,
  bool changeN2SubQC        = false,
  bool changeModEneUpdated  = true,
  bool changeTrigCtr        = true
)
{
  auto CalConfig = ParForCal[ReturnRunIndex(runId)][periodId];
  if (changeConfigs) {
    CalConfig -> setAttPathL(changeAttPathL);
    CalConfig -> setN2SubQC(changeN2SubQC);
    CalConfig -> setModEneUpdated(changeModEneUpdated);
    CalConfig -> setTrigCtr(changeTrigCtr);
  }

  TFile* infile = new TFile(Form("GaussFitRes/Run%s_Period%02d_GaussFitToMainTime%s_SystIadp.root", runId.c_str(), periodId, CalConfig->infoTrigCtr.Data()), "read");
 
  std::vector<std::vector<TH1D*>> h1PeakTime(12);
  std::vector<std::vector<std::vector<TF1*>>> h1GaussFit(12);

  for (auto tracksIt = TrackPairMPPC.begin(); tracksIt != TrackPairMPPC.end(); tracksIt++) {
    
    int trackId = std::distance(TrackPairMPPC.begin(), tracksIt);
    auto const& pairs = TrackPairMPPC[trackId];
    h1GaussFit[trackId].resize(pairs.size()); // -> Reserve memory for fit function storage

    for (auto pairsIt = pairs.begin(); pairsIt != pairs.end(); pairsIt++) {
      
      auto const& currentPair = *pairsIt;
      auto hisIndex = std::distance(pairs.begin(), pairsIt);

      // Bring in data hist
      TString inputHistKey = Form("MB/Trk%02d_mainMod%02d_SystIadpCutset%dMB", trackId, currentPair.mainMod, drawOption);
      h1PeakTime[trackId].push_back((TH1D*)infile->Get(inputHistKey));

      // Reserve memory for bringing in fit functions
      //h1GaussFit[trackId][hisIndex].resize(SYSTEMATIC::SystFitRangeTunePar.size()); // -> Why this should be removed???
      
      for (int iFitTune=0; iFitTune<SYSTEMATIC::SystFitRangeTunePar.size(); iFitTune++) {
        TString inputFuncKey = Form("MB/GaussFit_Trk%02d_mainMod%02d_SystIadpCutset%dMB_SystFitRangeTuneset%d", trackId, currentPair.mainMod, drawOption, iFitTune);
        h1GaussFit[trackId][hisIndex].push_back((TF1*)infile->Get(inputFuncKey));
        
        // Check fit function storage
        if (h1GaussFit[trackId][hisIndex][iFitTune] == NULL) {
          cout << "Fit function is not safely stored!" << endl;
          abort();
        } else {
          cout << "Saved : " << inputFuncKey << endl;
        }
      }
      
      // Check histogram storage
      if (h1PeakTime[trackId][hisIndex] == NULL) {
        cout << "Histograms are not safely stored. Abort!" << endl;
        abort();
      }
    }
  }
  
  #if 0
  struct ChannelsInModules {
    // Histograms
    TH1D* up0;
    TH1D* up1;
    TH1D* down0;
    TH1D* down1;
    
    // Fit functions
    TF1* fitUp0;
    TF1* fitUp1;
    TF1* fitDown0;
    TF1* fitDown1;
  };
  #endif

  struct ChannelsInModules {
    // Histograms
    TH1D* up0;
    TH1D* up1;
    TH1D* down0;
    TH1D* down1;

    // Fit functions
    std::vector<TF1*> fitsUp0;
    std::vector<TF1*> fitsUp1;
    std::vector<TF1*> fitsDown0;
    std::vector<TF1*> fitsDown1;
  };

  std::vector<ChannelsInModules> h1TimesFits;
  //DCV1
  h1TimesFits.push_back({h1PeakTime[4][0], h1PeakTime[5][0], h1PeakTime[4][1], h1PeakTime[5][1], h1GaussFit[4][0], h1GaussFit[5][0], h1GaussFit[4][1], h1GaussFit[5][1]});
  h1TimesFits.push_back({h1PeakTime[1][0], h1PeakTime[0][0], h1PeakTime[1][1], h1PeakTime[0][1], h1GaussFit[1][0], h1GaussFit[0][0], h1GaussFit[1][1], h1GaussFit[0][1]});
  h1TimesFits.push_back({h1PeakTime[3][0], h1PeakTime[2][0], h1PeakTime[3][1], h1PeakTime[2][1], h1GaussFit[3][0], h1GaussFit[2][0], h1GaussFit[3][1], h1GaussFit[2][1]});
  h1TimesFits.push_back({h1PeakTime[1][2], h1PeakTime[0][2], h1PeakTime[1][3], h1PeakTime[0][3], h1GaussFit[1][2], h1GaussFit[0][2], h1GaussFit[1][3], h1GaussFit[0][3]});
  //DCV2
  h1TimesFits.push_back({h1PeakTime[10][0], h1PeakTime[11][0], h1PeakTime[10][1], h1PeakTime[11][1], h1GaussFit[10][0], h1GaussFit[11][0], h1GaussFit[10][1], h1GaussFit[11][1]});
  h1TimesFits.push_back({h1PeakTime[7][0], h1PeakTime[6][0], h1PeakTime[7][1], h1PeakTime[6][1], h1GaussFit[7][0], h1GaussFit[6][0], h1GaussFit[7][1], h1GaussFit[6][1]});
  h1TimesFits.push_back({h1PeakTime[9][0], h1PeakTime[8][0], h1PeakTime[9][1], h1PeakTime[8][1], h1GaussFit[9][0], h1GaussFit[8][0], h1GaussFit[9][1], h1GaussFit[8][1]});
  h1TimesFits.push_back({h1PeakTime[7][2], h1PeakTime[6][2], h1PeakTime[7][3], h1PeakTime[6][3], h1GaussFit[7][2], h1GaussFit[6][2], h1GaussFit[7][3], h1GaussFit[6][3]});


  std::vector<TCanvas*> canvases;
  
  for (int imod=0; imod<8; imod++) {

    canvases.push_back(new TCanvas(Form("c1_mod%02d", imod), Form("c1_mod%02d", imod), 4*1.2*500, 4*500));
    canvases[imod] -> Divide(2, 2);

    for (int ich=0; ich<4; ich++) {
 
      TH1D* his;
      std::vector<TF1*> fit;
      if (ich == 0) {
        his = h1TimesFits[imod].up0;
        fit = h1TimesFits[imod].fitsUp0;
      } else if (ich ==1) {
        his = h1TimesFits[imod].up1;
        fit = h1TimesFits[imod].fitsUp1;
      } else if (ich ==2) {
        his = h1TimesFits[imod].down0;
        fit = h1TimesFits[imod].fitsDown0;
      } else if (ich ==3) {
        his = h1TimesFits[imod].down1;
        fit = h1TimesFits[imod].fitsDown1;
      }

      float ymax = his -> GetMaximum();

      canvases[imod] -> cd(ich+1);
      
      gStyle -> SetOptStat(0);
      gPad -> SetTicks();
      gPad -> SetMargin(1.2, 1.2, 1.2, 1.2);
     
      TH1D* htmp = (TH1D*)gPad -> DrawFrame(16.0, 0.0, 28.0, ymax*1.25);
      htmp -> GetXaxis() -> SetTitle("Time sampe(1 time sample = 8 ns)");
      htmp -> GetYaxis() -> SetTitle("Entries");

      TLegend* l1 = new TLegend(0.15, 0.7, 0.45, 0.9);
      l1 -> SetTextSize(0.03);
      l1 -> SetTextFont(62);
      l1 -> SetBorderSize(0);
      l1 -> SetFillStyle(0);
      l1 -> AddEntry("", Form("Run%s Period%02d", runId.c_str(), periodId), "h"); 
      l1 -> AddEntry("", Form("Chosen cut set: %d", drawOption), "h"); 
      l1 -> AddEntry("", his -> GetName(), "h");
      l1 -> AddEntry("", Form("#Entries: %.0f", his->GetEntries()), "h");

      TLegend* l2 = new TLegend(0.45, 0.6, 0.8, 0.8);
      l2 -> SetTextSize(0.02);
      l2 -> SetTextFont(62);
      l2 -> SetBorderSize(0);
      l2 -> SetFillStyle(0);

      his -> SetMarkerStyle(21);
      his -> SetMarkerSize(0.03);
      his -> SetMarkerColor(kBlack);
      his -> SetLineColor(kBlack);
      his -> Draw("E1 same");

      //std::vector<int> linecolors = {1, 2, 3, 4, 6};
      std::vector<int> linecolors = {kRed, kMagenta, kViolet, kBlue, kGreen};
      for (int ifit=0; ifit<SYSTEMATIC::SystFitRangeTunePar.size(); ifit++) {
        fit[ifit] -> SetLineStyle(1);
        fit[ifit] -> SetLineWidth(3);
        fit[ifit] -> SetLineColor(linecolors[ifit]);
        fit[ifit] -> Draw("same");

        float varMean = fit[ifit]->GetParameter(1);
        float errMean = fit[ifit]->GetParError(1);
        float varRms = fit[ifit]->GetParameter(2);
        float errRms = fit[ifit]->GetParError(2);
        float chi2 = fit[ifit]->GetChisquare();
        int ndf = fit[ifit]->GetNDF();
        float chi2ndf = chi2/(float)ndf;
        l2 -> AddEntry(fit[ifit], Form("Fit variation(#pm%.1f) #mu: %.1f#pm%.2f | #sigma: %.1f#pm%.2f | #chi^{2}/NDF(%.0f/%d): %.2f", SYSTEMATIC::SystFitRangeTunePar[ifit], varMean, errMean, varRms, errRms, chi2, ndf, chi2ndf), "l");
      }
      
      l1 -> Draw("same");
      l2 -> Draw("same");
    }

  }
  
}
