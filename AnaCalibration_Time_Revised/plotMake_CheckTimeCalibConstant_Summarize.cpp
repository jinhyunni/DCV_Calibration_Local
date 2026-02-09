#include "analysisHeader.h"
#include "SystematicSources.h"

void plotMake_CheckTimeCalibConstant_Summarize
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
      
      TString infilename = Form("TimeCalibConstant/Run%s_Period%02d_TimeCalibration%s_Syst.root", id.c_str(), iperiod, CalConfig->infoTrigCtr.Data());
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
  std::vector<TH1D*> h1TimeCalib;

  for (int ifile=0; ifile<inputFiles.size(); ifile++) {
    // Reading in input fits
    TString readinSet = Form("SystIadpCutset%d_SystFitRangeTuneset%d", SYSTEMATIC::RefIADP, SYSTEMATIC::RefFitR);
    h1TimeCalib.push_back((TH1D*)inputFiles[ifile]->Get(Form("TimeCalib_total_%s", readinSet.Data())));
  }
  
  // Make histograms
  std::vector<TH1D*> ichs;
  for (int ich=0; ich<32; ich++) {

    TH1D* ihis = new TH1D(Form("ich_%02d", ich), "", nFiles, 0, nFiles);

    for (int ifile=0; ifile<nFiles; ifile++) {

      const auto& currentTimeCalib = h1TimeCalib[ifile];
      
      int fillbin = ihis->FindBin(ifile);
      ihis -> GetXaxis() -> SetBinLabel(fillbin, runInfo[ifile]);
    
      int chBin = currentTimeCalib -> FindBin(ich);
      float tc_val= currentTimeCalib -> GetBinContent(chBin);
      float tc_err= currentTimeCalib -> GetBinError(chBin);

      ihis -> SetBinContent(fillbin, tc_val);
      ihis -> SetBinError(fillbin, tc_err);
    }
    ichs.push_back(ihis);
    DCV_LOG_INFO("push_back");
  }

  // Draw plot
  int textstyle = 43;
  int textsize = 20;

  auto DrawPlot = [&](TCanvas*& fcanvas, std::vector<int>& chList, bool fSaveAsPdf) {
    
    int canvasIdx = chList[0]/4;

    TCanvas* c1 = new TCanvas(Form("c%d", canvasIdx), Form("c%d", canvasIdx), 1.2*1000, 1000);
    c1 -> cd();
    
    // Mean from fit
    TPad* p1 = new TPad(Form("p1_%d", canvasIdx), Form("p1_%d", canvasIdx), 0.0, 0.0, 1.0, 1.0);
    p1 -> Draw();
    p1 -> cd();
    p1 -> SetBottomMargin(0.15);
    p1 -> SetLeftMargin(0.15);
    p1 -> SetTopMargin(0.02);
    p1 -> SetRightMargin(0.15);

    gPad -> SetTicks();
    gStyle -> SetOptStat(0);

    TH1F* htmp = (TH1F*)ichs[0] -> Clone("htmp");
    htmp -> GetYaxis() -> SetTitleFont(textstyle);
    htmp -> GetYaxis() -> SetTitleSize(textsize);
    htmp -> GetYaxis() -> SetLabelFont(textstyle);
    htmp -> GetYaxis() -> SetLabelSize(textsize);
    htmp -> GetYaxis() -> SetRangeUser(-2.0, 2.0);
    htmp -> GetYaxis() -> SetTitle("Time Calibration Constant(Time sample)");

    htmp -> GetXaxis() -> SetTitleFont(textstyle);
    htmp -> GetXaxis() -> SetTitleSize(textsize);
    htmp -> GetXaxis() -> SetLabelFont(textstyle);
    htmp -> GetXaxis() -> SetLabelSize(10);

    htmp -> Draw("same axis");

    std::vector<int> markerStyles{24, 25, 26, 28}; // -> different channels
    std::vector<int> markerColors{kRed, kPink, kMagenta, kViolet, kBlue, kAzure+3, kTeal+3, kGreen+2}; // -> Different modules

    TLegend* l1 = new TLegend(0.15, 0.7, 0.8, 0.9);
    l1 -> SetTextFont(textstyle);
    l1 -> SetTextSize(textsize);
    l1 -> SetFillStyle(0);
    l1 -> SetBorderSize(0);
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

    // save as pdf if required
    if (fSaveAsPdf) {
      TString saveDir = "pdf_TimeCalibSyst";
      if (!std::filesystem::exists(saveDir.Data())) {
        std::filesystem::create_directory(saveDir.Data());
      }

      TString saveFileName = Form("%s/DCVTime_TimeCalibConstant_Mod%d.pdf", saveDir.Data(), canvasIdx);
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
