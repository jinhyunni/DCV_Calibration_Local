#include "analysisHeader.h"
#include "SystematicSources.h"

void plotMake_CheckTimeCalibConstant_Syst
(
  std::string runId         = "92",
  int periodId              = 0,

  bool drawRef              = true,

  bool drawT1               = false,
  bool drawT2               = false,
  bool drawT3               = false,
  bool drawTTotal           = true,

  bool changeConfigs        = true,
  bool changeAttPathL       = true,
  bool changeN2SubQC        = false,
  bool changeModEneUpdated  = true,
  bool changeTrigCtr        = true,
  bool saveAsPdf            = true
)
{
  // Making inputs
  // -------------
  int runIdx = ReturnRunIndex(runId);

  if (ParForCal[runIdx].size() <=  periodId) {
    DCV_LOG_ABORT("Required input period does not exists");
  }

  auto CalConfig = ParForCal[runIdx][periodId];
  if (changeConfigs) {
    CalConfig -> setAttPathL(changeAttPathL);
    CalConfig -> setN2SubQC(changeN2SubQC);
    CalConfig -> setModEneUpdated(changeModEneUpdated);
    CalConfig -> setTrigCtr(changeTrigCtr);
  }
  
  TString infilename = Form("TimeCalibConstant/Run%s_Period%02d_TimeCalibration%s_Syst.root", runId.c_str(), periodId, CalConfig->infoTrigCtr.Data());
  TFile* infile = new TFile(infilename, "read");
  
  using Vec2dTH1 = std::vector<std::vector<TH1D*>>;

  Vec2dTH1 timeCalibAdjoint(SYSTEMATIC::SystIadp.size());
  Vec2dTH1 timeCalibUpdown(SYSTEMATIC::SystIadp.size());
  Vec2dTH1 timeCalibModDiff(SYSTEMATIC::SystIadp.size());
  Vec2dTH1 timeCalibTotal(SYSTEMATIC::SystIadp.size());

  for (int icut=0; icut<SYSTEMATIC::SystIadp.size(); icut++) {
    for (int ifit=0; ifit<SYSTEMATIC::SystFitRangeTunePar.size(); ifit++) {
      TString setinfo = Form("SystIadpCutset%d_SystFitRangeTuneset%d", icut, ifit);

      TString inhisname0 = Form("TimeCalib_adjoint_%s", setinfo.Data());
      TString inhisname1 = Form("TimeCalib_updown_%s", setinfo.Data());
      TString inhisname2 = Form("TimeCalib_betweenMods_%s", setinfo.Data());
      TString inhisname3 = Form("TimeCalib_total_%s", setinfo.Data());

      timeCalibAdjoint[icut].push_back((TH1D*)infile->Get(inhisname0));
      timeCalibUpdown[icut].push_back((TH1D*)infile->Get(inhisname1));
      timeCalibModDiff[icut].push_back((TH1D*)infile->Get(inhisname2));
      timeCalibTotal[icut].push_back((TH1D*)infile->Get(inhisname3));

      if (timeCalibAdjoint[icut][ifit] == NULL || timeCalibUpdown[icut][ifit] == NULL || timeCalibModDiff[icut][ifit] == NULL || timeCalibTotal[icut][ifit] == NULL) {
        cout << "Histogram is not safely stored. Abort!" << endl;
        if (timeCalibAdjoint[icut][ifit] == NULL) cout << "T1 not saved" << endl;
        if (timeCalibUpdown[icut][ifit] == NULL) cout << "T2 not saved" << endl;
        if (timeCalibModDiff[icut][ifit] == NULL) cout << "T3 not saved" << endl;
        if (timeCalibTotal[icut][ifit] == NULL) cout << "T_Total not saved" << endl;
        abort();
      }
    }
  }

  // PlotMake
  // --------
  // x axis : Channel # 
  // y axis : 
  //        Peak mean from fit
  //        Peak rms from fit
  //        Chi2NDF of fit

   struct SPECS {
    TString object;
    float ymin;
    float ymax;
    TString outputname;
  };

  SPECS specsAdjoint{"Adjoint calib factor", -1.5, 2.5, "T1"};
  SPECS specsUpdown{"Updown calib factor", -1.5, 2.5, "T2"};
  SPECS specsModDiff{"Mod diff calib factor", -1.5, 2.5, "T3"};
  SPECS specsTotal{"Total calibration factor", -1.5, 2.5, "T_Total"};

  TCanvas* c1;
  TCanvas* c2;
  TCanvas* c3;
  TCanvas* c4;

  auto DrawPlot = [&](TCanvas*& fCanvas, const std::vector<std::vector<TH1D*>>& vecHis, SPECS fspecs) {
    
    fCanvas = new TCanvas(fspecs.object, fspecs.object, 1.5*1500, 1500);
    fCanvas -> cd();
    
    TPad* p1 = new TPad(fspecs.object, fspecs.object, 0.005, 0.005, 0.995, 0.995);
    p1 -> Draw();
    p1 -> cd();
    p1 -> SetBottomMargin(0.1);
    p1 -> SetLeftMargin(0.1);
    p1 -> SetTopMargin(0.01);
    p1 -> SetRightMargin(0.1);
    p1 -> SetTicks();
    p1 -> SetGrid();

    gStyle -> SetOptStat(0);

    TH1D* htmp = (TH1D*)gPad -> DrawFrame(0.0, fspecs.ymin, 32.0, fspecs.ymax);
    htmp -> GetXaxis() -> SetTitleSize(0.02);
    htmp -> GetXaxis() -> SetTitleOffset(2);
    htmp -> GetXaxis() -> SetTitle("MPPC Channel");
    htmp -> GetYaxis() -> SetTitleSize(0.04);
    htmp -> GetYaxis() -> SetTitle(fspecs.outputname);

    TLegend* l1 = new TLegend(0.15, 0.50, 0.9, 0.95);
    l1 -> SetTextSize(0.03);
    l1 -> SetTextFont(62);
    l1 -> SetBorderSize(0);
    l1 -> SetFillStyle(0);
    l1 -> SetMargin(0.1);
    //l1 -> SetEntrySeparation(10.0);
    l1 -> SetNColumns(SYSTEMATIC::SystFitRangeTunePar.size());
    l1 -> AddEntry("", Form("Run%s Period%02d", runId.c_str(), periodId), "h");
    //l1 -> AddEntry("", fspecs.object, "h");

    std::vector<int> markerStyles{24, 25, 26, 27};    // -> Different markerstyle represents different fit range
    std::vector<int> markerColors{1, 2, 3, 4, 6, 8};  // -> Different marker clors represents different iadp cut sets
                                                      
    for (int icut=0; icut<SYSTEMATIC::SystIadp.size(); icut++) {
      if (drawRef && icut != SYSTEMATIC::RefIADP) continue;
      for (int ifit=0; ifit<SYSTEMATIC::SystFitRangeTunePar.size(); ifit++) {
        if (drawRef && ifit!= SYSTEMATIC::RefFitR) continue;
        vecHis[icut][ifit] -> SetMarkerSize(2.5);
        vecHis[icut][ifit] -> SetMarkerStyle(markerStyles[ifit]);
        vecHis[icut][ifit] -> SetMarkerColor(markerColors[icut]);
        vecHis[icut][ifit] -> SetLineColor(markerColors[icut]);
        vecHis[icut][ifit] -> Draw("p same");
      
        // Make legend
        if (icut == 0) {
          l1 -> AddEntry(vecHis[icut][ifit], Form("#splitline{FitRange : #pm %.0f RMS}{IADP : No selection}", SYSTEMATIC::SystFitRangeTunePar[ifit]), "p");
        } else {
          l1 -> AddEntry(vecHis[icut][ifit], Form("#splitline{FitRange : #pm %.0f RMS}{IADP : #pm %.0f RMS}", SYSTEMATIC::SystFitRangeTunePar[ifit], SYSTEMATIC::SystIadp[icut]), "p");
        }
      }
    }

    l1 -> Draw("same");

    if (saveAsPdf) {
      TString savedir = Form("pdf_TimeCalibSyst");
      if (!std::filesystem::exists(savedir.Data())) {
        std::filesystem::create_directory(savedir.Data());
      }

      //TString filename = Form("%s/Run%s_Period%02d_Syst_%s%s.pdf", savedir.Data(), runId.c_str(), periodId, fspecs.outputname.Data(), CalConfig->infoTrigCtr.Data());
      TString infoDrawRef = (drawRef) ? "_onlyRef" : "";
      TString filename = Form("%s/Syst_%s%s_Run%s_Period%02d%s.pdf", savedir.Data(), fspecs.outputname.Data(), infoDrawRef.Data(), runId.c_str(), periodId, CalConfig->infoTrigCtr.Data());
      fCanvas -> SaveAs(filename);
    }
  };

  // Draw
  // ----
  if (drawT1) DrawPlot(c1, timeCalibAdjoint, specsAdjoint);
  if (drawT2) DrawPlot(c2, timeCalibUpdown, specsUpdown);
  if (drawT3) DrawPlot(c3, timeCalibModDiff, specsModDiff);
  if (drawTTotal) DrawPlot(c4, timeCalibTotal, specsTotal);

}
