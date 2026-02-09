void plotMake_Compare_gain
(
  std::vector<std::string> runArray  = {"92"}
)
{
  // Making inputs
  // -------------
  TString dir_updated = "/Users/jinhyunpark/npl/Analysis/KOTO/DCV/calibration_for_distribution/AnaCalibration_Ene_Revised";
  TString dir_default = "/Users/jinhyunpark/npl/Analysis/KOTO/DCV/calibration/LocalTest/AnaCalibration_Ene/5.CalibrationFactor";

  std::vector<TH1D*> gain_upd, gain_def, gain_ratio;
  std::vector<TString> objInfo;
  for (const auto& run : runArray) {

    for (int iperiod=0; iperiod<10; iperiod++) {
      TString inputfilename0 = Form("%s/Run%s_Period%02d_Nx/Run%s_Period%02d_GainFactor_UT.root", dir_updated.Data(), run.c_str(), iperiod, run.c_str(), iperiod);
      TString inputfilename1 = Form("%s/Run%s_Period%02d_GainFactor_UT.root", dir_default.Data(), run.c_str(), iperiod);

      if (!std::filesystem::exists(inputfilename0.Data()) || !std::filesystem::exists(inputfilename1.Data())) {
        cout << "One of the required inputfile not found. Continue;" << endl;
        continue;
      } else {
        cout << "Processing..." << endl;
      }

      TFile* file_up = new TFile(inputfilename0, "read");
      TFile* file_de = new TFile(inputfilename1, "read");

      TH1D* h1Gain_up = (TH1D*)file_up->Get("gain");
      TH1D* h1Gain_de = (TH1D*)file_de->Get("gain");
    
      if (h1Gain_up == NULL || h1Gain_de == NULL) {
        cout << "One of input histo is NULL. Abort!" << endl;
        abort();
      } else {
        gain_upd.push_back(h1Gain_up);
        gain_def.push_back(h1Gain_de);
      
        TString output_name(Form("Run%s_Period%02d", run.c_str(), iperiod));
        TH1D* gain_ratio_tmp = (TH1D*)h1Gain_up -> Clone(output_name);
        gain_ratio_tmp -> Divide(h1Gain_de);
        gain_ratio.push_back(gain_ratio_tmp);
        objInfo.push_back(output_name);
      }

      // After saving all the histograms, delete pointer to inputfile
      //file_up -> Close();
      //file_de -> Close();
    }
  };

  // Drawing plots
  // -------------
  std::vector<int> markerStyles{24, 25, 26, 27, 28, 32, 46};
  std::vector<int> markerColors{kRed, kPink, kMagenta, kViolet, kBlue, kAzure, kCyan, kTeal, kGreen, kSpring};


  TCanvas* c1 = new TCanvas("c1", "c1", 1.2*1500, 1500);
  c1 -> cd();

  TPad* p1 = new TPad("p1", "p1", 0.0, 0.0, 1.0, 1.0);
  p1 -> Draw();
  p1 -> cd();
  p1 -> SetBottomMargin(0.1);
  p1 -> SetLeftMargin(0.1);
  p1 -> SetTopMargin(0.05);
  p1 -> SetRightMargin(0.1);

  p1 -> SetTicks();
  gStyle -> SetOptStat(0);

  TH1F* htmp = (TH1F*)gPad -> DrawFrame(0.0, 0.0, 32.0, 2.5);
  htmp -> GetYaxis() -> SetTitle("Ratio of gainFactor(updated/default)");
  htmp -> GetXaxis() -> SetTitle("Ch");
  htmp -> Draw("same");

  TLegend* l1 = new TLegend(0.15, 0.60, 0.90, 0.90);
  l1 -> SetFillStyle(0);
  l1 -> SetBorderSize(0);
  l1 -> SetMargin(0.04);
  l1 -> SetTextFont(43);
  l1 -> SetTextSize(20);
  l1 -> SetNColumns(4);

  for (int iobj=0; iobj<gain_ratio.size(); iobj++) {
    int styleIdx = iobj/markerStyles.size();
    int colorIdx = iobj%markerColors.size();
    
    int style = markerStyles[styleIdx];
    int color = markerStyles[colorIdx];

    gain_ratio[iobj] -> SetMarkerStyle(style);
    gain_ratio[iobj] -> SetMarkerColor(color);
    gain_ratio[iobj] -> SetLineColor(color);
    gain_ratio[iobj] -> SetMarkerSize(0.8);
    gain_ratio[iobj] -> Draw("p same");

    l1 -> AddEntry(gain_ratio[iobj], objInfo[iobj], "p");
  }

  l1 -> Draw("same");

  // Draw reference line
  TLine* line1 = new TLine(0.0, 1.0, 32.0, 1.0);
  line1 -> SetLineStyle(2);
  line1 -> SetLineColor(kBlack);
  line1 -> Draw("same");

}
