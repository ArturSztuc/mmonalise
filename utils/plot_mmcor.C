void savePlot(TCanvas *c, std::string fout){
  c->SaveAs((fout + ".png").c_str());
  c->SaveAs((fout + ".pdf").c_str());
  c->SaveAs((fout + ".root").c_str());
}

void setStyleTH1(TH1D *plot, TCanvas *c){
  // TCanvas stylystics
  c->SetLeftMargin(0.15);
  c->SetBottomMargin(0.15);
  c->SetRightMargin(0.1);
  c->SetTopMargin(0.1);

  // Line/dot colors
  plot->SetLineColor(kBlack);
  plot->SetFillColor(13);
  plot->SetMarkerColor(kRed);
  plot->SetMarkerStyle(20);
  plot->SetMarkerSize(0.5);

  plot->SetTitleFont(132);
  plot->SetTitleSize(0.07);
  plot->SetTitleOffset(1);

  // X axis
  plot->GetXaxis()->SetTitleFont(132);
  plot->GetXaxis()->SetTitleSize(0.07);
  plot->GetXaxis()->SetTitleOffset(1);
  plot->GetXaxis()->SetLabelFont(132);
  plot->GetXaxis()->SetLabelSize(0.05);
  plot->GetXaxis()->SetNdivisions(-505);
  TGaxis::SetMaxDigits(3) ;

  // Y axis
  plot->GetYaxis()->SetTitleFont(132);
  plot->GetYaxis()->SetTitleSize(0.07);
  plot->GetYaxis()->SetTitleOffset(1);
  plot->GetYaxis()->SetLabelFont(132);
  plot->GetYaxis()->SetLabelSize(0.05);

  // Redraw the frame
  TLine l;
  //l.DrawLine(gPad->GetUxmin(), gPad->GetUymax(), gPad->GetUxmax(), gPad->GetUymax());
  //l.DrawLine(gPad->GetUxmax(), gPad->GetUymin(), gPad->GetUxmax(), gPad->GetUymax());

  c->cd();
  plot->Draw();
}

void setStylePlotErr(TGraphErrors *plot, TCanvas *c){
  // TCanvas stylystics
  c->SetLeftMargin(0.15);
  c->SetBottomMargin(0.15);
  c->SetRightMargin(0.1);
  c->SetTopMargin(0.1);

  // Set time/Date
  plot->GetXaxis()->SetTimeDisplay(1);
  plot->GetXaxis()->SetTimeFormat("%d/%m/%y");
  plot->GetXaxis()->SetTimeOffset(0,"cst");
  plot->GetXaxis()->SetNdivisions(-505);

  // Line/dot colors
  plot->SetLineColor(kBlack);
  plot->SetFillColor(13);
  plot->SetMarkerColor(kRed);
  plot->SetMarkerStyle(20);
  plot->SetMarkerSize(0.5);

  // X axis
  plot->GetXaxis()->SetTitleFont(132);
  plot->GetXaxis()->SetTitleSize(0.07);
  plot->GetXaxis()->SetTitleOffset(1);
  plot->GetXaxis()->SetLabelFont(132);
  plot->GetXaxis()->SetLabelSize(0.05);

  // Y axis
  plot->GetYaxis()->SetTitleFont(132);
  plot->GetYaxis()->SetTitleSize(0.07);
  plot->GetYaxis()->SetTitleOffset(1);
  plot->GetYaxis()->SetLabelFont(132);
  plot->GetYaxis()->SetLabelSize(0.05);

  // Redraw the frame
  TLine l;
  l.DrawLine(gPad->GetUxmin(), gPad->GetUymax(), gPad->GetUxmax(), gPad->GetUymax());
  l.DrawLine(gPad->GetUxmax(), gPad->GetUymin(), gPad->GetUxmax(), gPad->GetUymax());

  c->cd();
  plot->Draw("ap");
}

void setStylePlot(TGraph *plot, TCanvas *c){
  // TCanvas stylystics
  c->SetLeftMargin(0.15);
  c->SetBottomMargin(0.15);
  c->SetRightMargin(0.1);
  c->SetTopMargin(0.1);

  plot->GetXaxis()->SetTimeDisplay(1);
  plot->GetXaxis()->SetTimeFormat("%d/%m/%y");
  plot->GetXaxis()->SetTimeOffset(0,"cst");
  plot->GetXaxis()->SetNdivisions(-505);

  plot->SetLineColor(kBlack);
  plot->SetFillColor(13);
  plot->SetMarkerColor(kRed);
  plot->SetMarkerStyle(20);
  plot->SetMarkerSize(0.5);

  plot->GetXaxis()->SetTitleFont(132);
  plot->GetXaxis()->SetTitleSize(0.07);
  plot->GetXaxis()->SetTitleOffset(1);
  plot->GetXaxis()->SetLabelFont(132);
  plot->GetXaxis()->SetLabelSize(0.05);

  plot->GetYaxis()->SetTitleFont(132);
  plot->GetYaxis()->SetTitleSize(0.07);
  plot->GetYaxis()->SetTitleOffset(1);
  plot->GetYaxis()->SetLabelFont(132);
  plot->GetYaxis()->SetLabelSize(0.05);
  TLine l;
  l.DrawLine(gPad->GetUxmin(), gPad->GetUymax(), gPad->GetUxmax(), gPad->GetUymax());
  l.DrawLine(gPad->GetUxmax(), gPad->GetUymin(), gPad->GetUxmax(), gPad->GetUymax());

  c->cd();
  plot->Draw("ap");
}

void plot_mmcor(std::string sin){
  TFile *fin = new TFile(sin.c_str(), "READ");

  TTree *tin = (TTree*)fin->Get("thermo_couple");

  // MM#COR_CAL means and standard deviations
  double m1, m2, m3;
  double sd1, sd2, sd3;

  Long64_t time;


  // Setup time
  tin->SetBranchAddress("time", &time);

  // Setup means
  tin->SetBranchAddress("MM1COR_CAL_E12_TRTGTD", &m1);
  tin->SetBranchAddress("MM2COR_CAL_E12_TRTGTD", &m2);
  tin->SetBranchAddress("MM3COR_CAL_E12_TRTGTD", &m3);

  // Setup RMSs
  tin->SetBranchAddress("MM1COR_CAL_E12_TRTGTD_SD", &sd1);
  tin->SetBranchAddress("MM2COR_CAL_E12_TRTGTD_SD", &sd2);
  tin->SetBranchAddress("MM3COR_CAL_E12_TRTGTD_SD", &sd3);

  // We will only plot errors smaller than 1...
  int nevents = tin->GetEntries();
  int nEvents_true = 0;
  std::vector< int > indices;

  double min_sd_1 = 9999999;
  double min_sd_2 = 9999999;
  double min_sd_3 = 9999999;

  double max_sd_1 = -999999;
  double max_sd_2 = -999999;
  double max_sd_3 = -999999;

  double min_1 = 9999999;
  double min_2 = 9999999;
  double min_3 = 9999999;

  double max_1 = -999999;
  double max_2 = -999999;
  double max_3 = -999999;

  for(int i = 0; i < nevents; ++i){
    tin->GetEntry(i);
    if( min_1 > m1)
      min_1 = m1;
    if(max_1 < m1)
      max_1 = m1;

    if( min_2 > m2)
      min_2 = m2;
    if(max_2 < m2)
      max_2 = m2;

    if( min_3 > m3)
      min_3 = m3;
    if(max_3 < m3)
      max_3 = m3;

    if( min_sd_1 > sd1)
      min_sd_1 = sd1;
    if(max_sd_1 < sd1)
      max_sd_1 = sd1;

    if( min_sd_2 > sd2)
      min_sd_2 = sd2;
    if(max_sd_2 < sd2)
      max_sd_2 = sd2;

    if( min_sd_3 > sd3)
      min_sd_3 = sd3;
    if(max_sd_3 < sd3)
      max_sd_3 = sd3;

    if(sd1 > 1.0)
      continue;
    if(sd2 > 1.0)
      continue;
    if(sd3 > 1.0)
      continue;


    nEvents_true++;
    indices.push_back(i);
  }

  TGraphErrors *g1_trim = new TGraphErrors(nEvents_true);
  TGraphErrors *g2_trim = new TGraphErrors(nEvents_true);
  TGraphErrors *g3_trim = new TGraphErrors(nEvents_true);

  TGraph *gg1_trim = new TGraph(nEvents_true);
  TGraph *gg2_trim = new TGraph(nEvents_true);
  TGraph *gg3_trim = new TGraph(nEvents_true);

  // Initialize graphs
  TGraphErrors *g1 = new TGraphErrors(nevents);
  TGraphErrors *g2 = new TGraphErrors(nevents);
  TGraphErrors *g3 = new TGraphErrors(nevents);

  TGraph *gg1 = new TGraph(nevents);
  TGraph *gg2 = new TGraph(nevents);
  TGraph *gg3 = new TGraph(nevents);


  TH1D *trim_h1mm1 = new TH1D("trim_h1mm1", "trim_h1mm1", 100, 1.7, 1.9 );
  TH1D *trim_h1mm2 = new TH1D("trim_h1mm2", "trim_h1mm2", 100, 5.2, 6);
  TH1D *trim_h1mm3 = new TH1D("trim_h1mm3", "trim_h1mm3", 100, 0.42, 0.5);

  TH1D *trim_h1m1 = new TH1D("trim_h1m1", "trim_h1m1", 100, 0.001, 0.005 );
  TH1D *trim_h1m2 = new TH1D("trim_h1m2", "trim_h1m2", 100, 0, 0.03);
  TH1D *trim_h1m3 = new TH1D("trim_h1m3", "trim_h1m3", 100, 0.005, 0.015);
  // Load and set the points
  for(int i = 0; i < nEvents_true; ++i){
    std::cout << "Event: " << i << std::endl;
    tin->GetEntry(indices[i]);

    g1_trim->SetPoint(i, time/1000,m1);
    gg1_trim->SetPoint(i, time/1000,m1);
    g1_trim->SetPointError(i, 0, sd1);

    g2_trim->SetPoint(i, time/1000, m2);
    gg2_trim->SetPoint(i, time/1000, m2);
    g2_trim->SetPointError(i, 0, sd2);

    g3_trim->SetPoint(i, time/1000, m3);
    gg3_trim->SetPoint(i, time/1000, m3);
    g3_trim->SetPointError(i, 0, sd3);

    trim_h1mm1->Fill(m1);
    trim_h1mm2->Fill(m2);
    trim_h1mm3->Fill(m3);

    trim_h1m1->Fill(sd1);
    trim_h1m2->Fill(sd2);
    trim_h1m3->Fill(sd3);
  }

  //for(int i = 0; i < nEvents_true; ++i){
  //  std::cout << "Event: " << i << std::endl;
  //  tin->GetEntry(indices[i]);

  //  g1->SetPoint(i, time/1000,m1);
  //  gg1->SetPoint(i, time/1000,m1);
  //  g1->SetPointError(i, 0, sd1);

  //  g2->SetPoint(i, time/1000, m2);
  //  gg2->SetPoint(i, time/1000, m2);
  //  g2->SetPointError(i, 0, sd2);

  //  g3->SetPoint(i, time/1000, m3);
  //  gg3->SetPoint(i, time/1000, m3);
  //  g3->SetPointError(i, 0, sd3);
  //}

  TH1D *h1m1 = new TH1D("h1m1", "h1m1", 100, min_sd_1, max_sd_1);
  TH1D *h1m2 = new TH1D("h1m2", "h1m2", 100, min_sd_2, max_sd_2);
  TH1D *h1m3 = new TH1D("h1m3", "h1m3", 100, min_sd_3, max_sd_3);

  TH1D *h1mm1 = new TH1D("h1mm1", "h1mm1", 100, min_1, max_1);
  TH1D *h1mm2 = new TH1D("h1mm2", "h1mm2", 100, min_2, max_2);
  TH1D *h1mm3 = new TH1D("h1mm3", "h1mm3", 100, min_3, max_3);

  for(int i = 0; i < nevents; ++i){
    std::cout << "Event: " << i << std::endl;
    tin->GetEntry(i);

    g1->SetPoint(i, time/1000,m1);
    gg1->SetPoint(i, time/1000,m1);
    g1->SetPointError(i, 0, sd1);

    g2->SetPoint(i, time/1000, m2);
    gg2->SetPoint(i, time/1000, m2);
    g2->SetPointError(i, 0, sd2);

    g3->SetPoint(i, time/1000, m3);
    gg3->SetPoint(i, time/1000, m3);
    g3->SetPointError(i, 0, sd3);

    h1m1->Fill(sd1);
    h1m2->Fill(sd2);
    h1m3->Fill(sd3);

    h1mm1->Fill(m1);
    h1mm2->Fill(m2);
    h1mm3->Fill(m3);

  }

  //Style->SetOptStat(0);
  gStyle->SetPalette(51,0);

  //gStyle->SetOptTitle(0);

  // Set titles
  g1->SetTitle(";Date;MM1COR_CAL/TRTGTD");
  g2->SetTitle(";Date;MM2COR_CAL/TRTGTD");
  g3->SetTitle(";Date;MM3COR_CAL/TRTGTD");

  gg1->SetTitle(";Date;MM1COR_CAL/TRTGTD");
  gg2->SetTitle(";Date;MM2COR_CAL/TRTGTD");
  gg3->SetTitle(";Date;MM3COR_CAL/TRTGTD");

  g1_trim->SetTitle(";Date;MM1COR_CAL/TRTGTD");
  g2_trim->SetTitle(";Date;MM2COR_CAL/TRTGTD");
  g3_trim->SetTitle(";Date;MM3COR_CAL/TRTGTD");

  gg1_trim->SetTitle(";Date;MM1COR_CAL/TRTGTD");
  gg2_trim->SetTitle(";Date;MM2COR_CAL/TRTGTD");
  gg3_trim->SetTitle(";Date;MM3COR_CAL/TRTGTD");

  TCanvas *c1_trim = new TCanvas("c1_trim", "c1_trim", 1200, 800);
  TCanvas *c2_trim = new TCanvas("c2_trim", "c2_trim", 1200, 800);
  TCanvas *c3_trim = new TCanvas("c3_trim", "c3_trim", 1200, 800);

  TCanvas *cc1_trim = new TCanvas("cc1_trim", "cc1_trim", 1200, 800);
  TCanvas *cc2_trim = new TCanvas("cc2_trim", "cc2_trim", 1200, 800);
  TCanvas *cc3_trim = new TCanvas("cc3_trim", "cc3_trim", 1200, 800);

  TCanvas *c1 = new TCanvas("c1", "c1", 1200, 800);
  TCanvas *c2 = new TCanvas("c2", "c2", 1200, 800);
  TCanvas *c3 = new TCanvas("c3", "c3", 1200, 800);

  TCanvas *cc1 = new TCanvas("cc1", "cc1", 1200, 800);
  TCanvas *cc2 = new TCanvas("cc2", "cc2", 1200, 800);
  TCanvas *cc3 = new TCanvas("cc3", "cc3", 1200, 800);

  //double ss1 = (max_1 - min_1)*0.2;
  //double ss2 = (max_2 - min_2)*0.2;
  //double ss3 = (max_3 - min_3)*0.2;
  //g1->GetYaxis()->SetRangeUser(min_1 - ss1, max_1 + ss1);
  //g2->GetYaxis()->SetRangeUser(min_2 - ss2, max_2 + ss2);
  //g3->GetYaxis()->SetRangeUser(min_3 - ss3, max_3 + ss3);
  


  g1->GetYaxis()->SetRangeUser(1.5, 2.0);
  g2->GetYaxis()->SetRangeUser(5, 6.2);
  g3->GetYaxis()->SetRangeUser(0.4,0.5);

  g1_trim->GetYaxis()->SetRangeUser(1.5, 2.0);
  g2_trim->GetYaxis()->SetRangeUser(5, 6.2);
  g3_trim->GetYaxis()->SetRangeUser(0.4,0.5);

  // Set the plot style
  setStylePlotErr(g1, c1);
  setStylePlotErr(g2, c2);
  setStylePlotErr(g3, c3);

  setStylePlotErr(g1_trim, c1_trim);
  setStylePlotErr(g2_trim, c2_trim);
  setStylePlotErr(g3_trim, c3_trim);

  setStylePlot(gg1, cc1);
  setStylePlot(gg2, cc2);
  setStylePlot(gg3, cc3);

  setStylePlot(gg1_trim, cc1_trim);
  setStylePlot(gg2_trim, cc2_trim);
  setStylePlot(gg3_trim, cc3_trim);

  // Save plots
  savePlot(c1, "err_MM1COR_CAL_TRTGTD");
  savePlot(c2, "err_MM2COR_CAL_TRTGTD");
  savePlot(c3, "err_MM3COR_CAL_TRTGTD");

  savePlot(cc1, "MM1COR_CAL_TRTGTD");
  savePlot(cc2, "MM2COR_CAL_TRTGTD");
  savePlot(cc3, "MM3COR_CAL_TRTGTD");

  savePlot(c1_trim, "trim_err_MM1COR_CAL_TRTGTD");
  savePlot(c2_trim, "trim_err_MM2COR_CAL_TRTGTD");
  savePlot(c3_trim, "trim_err_MM3COR_CAL_TRTGTD");

  savePlot(cc1_trim, "trim_MM1COR_CAL_TRTGTD");
  savePlot(cc2_trim, "trim_MM2COR_CAL_TRTGTD");
  savePlot(cc3_trim, "trim_MM3COR_CAL_TRTGTD");

  TCanvas *h1c1 = new TCanvas("h1c1", "h1c1", 800, 800);
  TCanvas *h1c2 = new TCanvas("h1c2", "h1c2", 800, 800);
  TCanvas *h1c3 = new TCanvas("h1c3", "h1c3", 800, 800);

  TCanvas *trim_h1c1 = new TCanvas("trim_h1c1", "trim_h1c1", 800, 800);
  TCanvas *trim_h1c2 = new TCanvas("trim_h1c2", "trim_h1c2", 800, 800);
  TCanvas *trim_h1c3 = new TCanvas("trim_h1c3", "trim_h1c3", 800, 800);

  TCanvas *h1mc1 = new TCanvas("h1mc1", "h1mc1", 800, 800);
  TCanvas *h1mc2 = new TCanvas("h1mc2", "h1mc2", 800, 800);
  TCanvas *h1mc3 = new TCanvas("h1mc3", "h1mc3", 800, 800);

  TCanvas *trim_h1mc1 = new TCanvas("trim_h1mc1", "trim_h1mc1", 800, 800);
  TCanvas *trim_h1mc2 = new TCanvas("trim_h1mc2", "trim_h1mc2", 800, 800);
  TCanvas *trim_h1mc3 = new TCanvas("trim_h1mc3", "trim_h1mc3", 800, 800);

  h1mm1->SetTitle("MM1COR_CAL/TRTGTD;Mean over 30 minutes;N Events");
  h1mm2->SetTitle("MM2COR_CAL/TRTGTD;Mean over 30 minutes;N Events");
  h1mm3->SetTitle("MM3COR_CAL/TRTGTD;Mean over 30 minutes;N Events");

  trim_h1mm1->SetTitle("MM1COR_CAL/TRTGTD;Mean over 30 minutes;N Events");
  trim_h1mm2->SetTitle("MM2COR_CAL/TRTGTD;Mean over 30 minutes;N Events");
  trim_h1mm3->SetTitle("MM3COR_CAL/TRTGTD;Mean over 30 minutes;N Events");

  h1m1->SetTitle("MM1COR_CAL/TRTGTD;Standard Deviation;N Events");
  h1m2->SetTitle("MM2COR_CAL/TRTGTD;Standard Deviation;N Events");
  h1m3->SetTitle("MM3COR_CAL/TRTGTD;Standard Deviation;N Events");

  trim_h1m1->SetTitle("MM1COR_CAL/TRTGTD;Standard Deviation;N Events");
  trim_h1m2->SetTitle("MM2COR_CAL/TRTGTD;Standard Deviation;N Events");
  trim_h1m3->SetTitle("MM3COR_CAL/TRTGTD;Standard Deviation;N Events");

  setStyleTH1(h1mm1, h1mc1);
  setStyleTH1(h1mm2, h1mc2);
  setStyleTH1(h1mm3, h1mc3);
  setStyleTH1(trim_h1mm1, trim_h1mc1);
  setStyleTH1(trim_h1mm2, trim_h1mc2);
  setStyleTH1(trim_h1mm3, trim_h1mc3);

  setStyleTH1(h1m1, h1c1);
  setStyleTH1(h1m2, h1c2);
  setStyleTH1(h1m3, h1c3);
  setStyleTH1(trim_h1m1, trim_h1c1);
  setStyleTH1(trim_h1m2, trim_h1c2);
  setStyleTH1(trim_h1m3, trim_h1c3);

  savePlot(h1c1, "SD_MM1COR_CAL_TRTGTD");
  savePlot(h1c2, "SD_MM2COR_CAL_TRTGTD");
  savePlot(h1c3, "SD_MM3COR_CAL_TRTGTD");

  savePlot(trim_h1c1, "SD_trim_MM1COR_CAL_TRTGTD");
  savePlot(trim_h1c2, "SD_trim_MM2COR_CAL_TRTGTD");
  savePlot(trim_h1c3, "SD_trim_MM3COR_CAL_TRTGTD");

  savePlot(h1mc1, "mean_MM1COR_CAL_TRTGTD");
  savePlot(h1mc2, "mean_MM2COR_CAL_TRTGTD");
  savePlot(h1mc3, "mean_MM3COR_CAL_TRTGTD");

  savePlot(trim_h1mc1, "mean_trim_MM1COR_CAL_TRTGTD");
  savePlot(trim_h1mc2, "mean_trim_MM2COR_CAL_TRTGTD");
  savePlot(trim_h1mc3, "mean_trim_MM3COR_CAL_TRTGTD");

}
