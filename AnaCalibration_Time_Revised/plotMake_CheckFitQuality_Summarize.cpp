#include "analysisHeader.h"
#include "SystematicSources.h"

void plotMake_CheckFitQuality_Summarize
(
  std::vector<std::string> runIds = {"91", "92", "92b", "93"},
  int periodId                    = 0,

  bool saveAsPdf                  = true,

  bool changeConfigs              = true,
  bool changeAttPathL             = true,
  bool changeN2SubQC              = false,
  bool changeModEneUpdated        = true,
  bool changeTrigCtr              = true
)
{

  // Making inputs
  // -------------
  std::vector<TFile*> inputFiles;
  std::vector<TString> runInfo;
  
  for (const auto& id : runIds) {
    int runIdx = ReturnRunIndex(id);
    for (int iperiod=0; iperiod<ParForCal[runIdx].size(); iperiod++) {

      auto CalConfig = ParForCal[runIdx][periodId];
      if (changeConfigs) {
        CalConfig -> setAttPathL(changeAttPathL);
        CalConfig -> setN2SubQC(changeN2SubQC);
        CalConfig -> setModEneUpdated(changeModEneUpdated);
        CalConfig -> setTrigCtr(changeTrigCtr);
      }
      
      TString infilename = Form("GaussFitRes/Run%s_Period%02d_GaussFitToMainTime%s_SystIadp.root", id.c_str(), iperiod, CalConfig->infoTrigCtr.Data());
      if (!std::filesystem::exists(infilename.Data())) {
        DCV_LOG_INFO(">>>>> Required input file not found, continue: " << infilename);
        continue;
      } else {
        DCV_LOG_INFO(">>>>> READ IN: " << infilename);
        inputFiles.push_back(new TFile(infilename, "read"));
        runInfo.push_back(Form("Run%s Period%02d", id.c_str(), iperiod));
      }
    }
  }
  
  // Filling in histograms
  const int nFiles = inputFiles.size();
  std::vector<std::vector<TF1*>> h1GaussFit(nFiles);

  for (int ifile=0; ifile<inputFiles.size(); ifile++) {
    // Reading in input fits
    for (auto tracksIt = TrackPairMPPC.begin(); tracksIt != TrackPairMPPC.end(); tracksIt++) {
    
      int trackId = std::distance(TrackPairMPPC.begin(), tracksIt);
      auto const& pairs = TrackPairMPPC[trackId];

      for (auto pairsIt = pairs.begin(); pairsIt != pairs.end(); pairsIt++) {
      
        auto const& currentPair = *pairsIt;
        auto hisIndex = std::distance(pairs.begin(), pairsIt);
          
        DCV_LOG_INFO(">>>>> CHECK: " << inputFiles.size());

        TString inputFuncKey = Form("MB/GaussFit_Trk%02d_mainMod%02d_SystIadpCutset%dMB_SystFitRangeTuneset%d", trackId, currentPair.mainMod, SYSTEMATIC::RefIADP, SYSTEMATIC::RefFitR);
        h1GaussFit[ifile].push_back((TF1*)inputFiles[ifile]->Get(inputFuncKey));
        DCV_LOG_INFO(">>>>> Read in fit func: " << inputFuncKey);
      }
    }
  }
  
  // Helper structs to re-arange histogramsa
  std::map<int, int> indexMap{
    {0, 12}, {1, 14}, {2, 13}, {3, 15}, {4, 4}, {5, 0}, {6, 5}, {7, 1}, {8, 10}, {9, 8}, {10, 11}, {11, 9}, {12, 2}, {13, 6}, {14, 3}, {15, 7}, 
    {16, 28}, {17, 30}, {18, 29}, {19, 31}, {20, 20}, {21, 16}, {22, 21}, {23, 17}, {24, 26}, {25, 24}, {26, 27}, {27, 25}, {28, 18}, {29, 22}, {30, 19}, {31, 23}}; 
  
  // Make histograms
  std::vector<TH1D*> ichs;
  for (int ich=0; ich<32; ich++) {

    int chIdx = indexMap[ich];
    cout << chIdx << endl;

    TH1D* ihis = new TH1D(Form("ich_%02d", ich), "", nFiles, 0, nFiles);

    for (int ifile=0; ifile<nFiles; ifile++) {

      int fillbin = ihis->FindBin(ifile);
      const auto& currentFit = h1GaussFit[ifile][chIdx];

      ihis -> GetXaxis() -> SetBinLabel(fillbin, runInfo[ifile]);

      float mean_val = currentFit -> GetParameter(1);
      float mean_err = currentFit -> GetParError(1);

      DCV_LOG_INFO(mean_val << " : " << mean_err << " : " << ifile);

      ihis -> SetBinContent(fillbin, mean_val);
      ihis -> SetBinError(fillbin, mean_err);
    }
    ichs.push_back(ihis);
    DCV_LOG_INFO("push_back");
  }

  // Chi2 NDF
  std::vector<TH1D*> chi2Ndfs;
  for (int ich=0; ich<32; ich++) {

    int chIdx = indexMap[ich];
    cout << chIdx << endl;

    TH1D* ihis = new TH1D(Form("ich_%02d_chi2ndf", ich), "", nFiles, 0, nFiles);

    for (int ifile=0; ifile<nFiles; ifile++) {

      int fillbin = ihis->FindBin(ifile);
      const auto& currentFit = h1GaussFit[ifile][chIdx];

      ihis -> GetXaxis() -> SetBinLabel(fillbin, runInfo[ifile]);

      float chi2 = currentFit -> GetChisquare();
      float ndf = currentFit -> GetNDF();

      ihis -> SetBinContent(fillbin, chi2/ndf);
      ihis -> SetBinError(fillbin, 0);
    }
    chi2Ndfs.push_back(ihis);
  }

  // Draw plot
  auto DrawPlot = [&](TCanvas*& fcanvas, std::vector<int>& chList, bool fSaveAsPdf) {
    
    int canvasIdx = chList[0]/4;

    TCanvas* c1 = new TCanvas(Form("c%d", canvasIdx), Form("c%d", canvasIdx), 1.2*1000, 1000);
    c1 -> cd();
    
    // Mean from fit
    TPad* p1 = new TPad(Form("p1_%d", canvasIdx), Form("p1_%d", canvasIdx), 0.0, 0.4, 1.0, 1.0);
    p1 -> Draw();
    p1 -> cd();
    p1 -> SetBottomMargin(0.005);
    p1 -> SetLeftMargin(0.15);
    p1 -> SetTopMargin(0.02);
    p1 -> SetRightMargin(0.15);

    gPad -> SetTicks();
    gStyle -> SetOptStat(0);

    TH1F* htmp = (TH1F*)gPad -> DrawFrame(0.0, 18.5, nFiles, 32.0);
    htmp -> GetYaxis() -> SetTitle("DCVTime mean from fit(Time sample)");
    htmp -> Draw("axis");
    
    std::vector<int> markerStyles{24, 25, 26, 28}; // -> different channels
    std::vector<int> markerColors{kRed, kPink, kMagenta, kViolet, kBlue, kAzure+3, kTeal+3, kGreen+2}; // -> Different modules

    TLegend* l1 = new TLegend(0.15, 0.7, 0.8, 0.9);
    l1 -> SetTextSize(0.03);
    l1 -> SetTextFont(62);
    l1 -> SetFillStyle(0);
    l1 -> SetBorderSize(0);
    l1 -> SetMargin(0.1);
    l1 -> SetNColumns(4);

    for (const auto& ich : chList) {
      int style = markerStyles[ich%4];
      int color = markerColors[ich/4];

      ichs[ich] -> SetMarkerColor(color);
      ichs[ich] -> SetLineColor(color);
      ichs[ich] -> SetMarkerStyle(style);
      ichs[ich] -> SetMarkerSize(0.8);
      ichs[ich] -> Draw("same");

      l1 -> AddEntry(ichs[ich], Form("Ch%02d", ich), "p");
    }

    l1 -> Draw("same");

    // Chi2/NDF of fit
    c1 -> cd();
    TPad* p2 = new TPad(Form("p2_%d", canvasIdx), Form("p2_%d", canvasIdx), 0.0, 0.0, 1.0, 0.4);

    p2 -> Draw();
    p2 -> cd();
    p2 -> SetBottomMargin(0.15);
    p2 -> SetLeftMargin(0.15);
    p2 -> SetTopMargin(0.005);
    p2 -> SetRightMargin(0.15);

    gPad -> SetTicks();

    TH1F* htmp1 = (TH1F*)ichs[0] -> Clone("htmp");
    htmp1 -> GetYaxis() -> SetRangeUser(0.0, 4.7);
    htmp1 -> GetYaxis() -> SetTitle("Chi2/NDF");
    htmp1 -> Draw("same axis");

    TLine* line1 = new TLine(0.0, 1.0, nFiles, 1.0);
    line1 -> SetLineColor(kBlack);
    line1 -> SetLineStyle(2);
    line1 -> Draw("same");

    for (const auto& ich : chList) {
      int style = markerStyles[ich%4];
      int color = markerColors[ich/4];

      chi2Ndfs[ich] -> SetMarkerColor(color);
      chi2Ndfs[ich] -> SetLineColor(color);
      chi2Ndfs[ich] -> SetMarkerStyle(style);
      chi2Ndfs[ich] -> SetMarkerSize(0.8);
      chi2Ndfs[ich] -> Draw("same p");
    }

    // save as pdf if required
    if (fSaveAsPdf) {
      TString saveDir = "pdf_TimeCalibSyst";
      if (!std::filesystem::exists(saveDir.Data())) {
        std::filesystem::create_directory(saveDir.Data());
      }

      TString saveFileName = Form("%s/DCVTime_Fit_Mod%d.pdf", saveDir.Data(), canvasIdx);
      c1 -> SaveAs(saveFileName);
    }
  };
  
  // Draw
  // ----
  for (const auto imod : std::vector<int>{0, 1, 2, 3, 4, 5, 6, 7}) {
    
    TCanvas* c1 = NULL;
    std::vector<int> passList{imod*4, imod*4+1, imod*4+2, imod*4+3};
    DrawPlot(c1, passList, saveAsPdf);
  }

}
