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

void plot_muon_thermo(std::string sin){
  TFile *fin = new TFile(sin.c_str(), "READ");

  TTree *tin = (TTree*)fin->Get("thermo_couple");

  // MM#COR_CAL means and standard deviations
  double m1, m2, m3, gtd;
  double m1_g, m2_g, m3_g;

  Long64_t time;


  // Setup time
  tin->SetBranchAddress("time", &time);

  // Setup means
  tin->SetBranchAddress("MM1COR_CAL", &m1);
  tin->SetBranchAddress("MM2COR_CAL", &m2);
  tin->SetBranchAddress("MM3COR_CAL", &m3);

  tin->SetBranchAddress("MM1COR_CAL_E12_TRTGTD", &m1_g);
  tin->SetBranchAddress("MM2COR_CAL_E12_TRTGTD", &m2_g);
  tin->SetBranchAddress("MM3COR_CAL_E12_TRTGTD", &m3_g);

  tin->SetBranchAddress("E12_TRTGTD", &gtd);

  // We will only plot errors smaller than 1...
  int nevents = tin->GetEntries();

  double min_1 = 9999999;
  double min_2 = 9999999;
  double min_3 = 9999999;
  double min_1g = 9999999;
  double min_2g = 9999999;
  double min_3g = 9999999;
  double min_g = 9999999;

  double max_1 = -999999;
  double max_2 = -999999;
  double max_3 = -999999;
  double max_1g = -999999;
  double max_2g = -999999;
  double max_3g = -999999;
  double max_g = -999999;

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

    if( min_g> gtd)
      min_g = gtd; 
    if(max_g <gtd)
      max_g = gtd;

    if( min_1g > m1_g)
      min_1g = m1_g;
    if(max_1g < m1_g)
      max_1g = m1_g;

    if( min_2g > m2_g)
      min_2g = m2_g;
    if(max_2g < m2_g)
      max_2g = m2_g;

    if( min_3g > m3_g)
      min_3g = m3_g;
    if(max_3g < m3_g)
      max_3g = m3_g;
  }

  //double gr_min_m1 = 0;
  //double gr_max_m1 = 100;

  //double gr_min_g = -1;
  //double gr_max_g = 55;
  //double gr_min_m1g = -100;
  //double gr_max_m1g = 10;

  TGraph *mm1 = new TGraph(nevents);
  TGraph *mm2 = new TGraph(nevents);
  TGraph *mm3 = new TGraph(nevents);

  TGraph *mm1_gtd = new TGraph(nevents);
  TGraph *mm2_gtd = new TGraph(nevents);
  TGraph *mm3_gtd = new TGraph(nevents);

  TGraph *gg = new TGraph(nevents);





  TH1D *h1m1 = new TH1D("h1m1", "h1m1", 100, min_1, max_1);
  TH1D *h1m2 = new TH1D("h1m2", "h1m2", 100, min_2, max_2);
  TH1D *h1m3 = new TH1D("h1m3", "h1m3", 100, min_3, max_3);

  TH1D *h1m1_gtd = new TH1D("h1m1_gtd", "h1m1_gtd", 100, min_1g, max_1g);
  TH1D *h1m2_gtd = new TH1D("h1m2_gtd", "h1m2_gtd", 100, min_2g, max_2g);
  TH1D *h1m3_gtd = new TH1D("h1m3_gtd", "h1m3_gtd", 100, min_3g, max_3g);

  TH1D *h1g = new TH1D("h1g", "h1g", 100, min_g, max_g);

  // Load and set the points
  for(int i = 0; i < nevents; ++i){
    tin->GetEntry(i);

    mm1->SetPoint(i, time/1000, m1);
    mm2->SetPoint(i, time/1000, m2);
    mm3->SetPoint(i, time/1000, m3);

    mm1_gtd->SetPoint(i, time/1000, m1_g);
    mm2_gtd->SetPoint(i, time/1000, m2_g);
    mm3_gtd->SetPoint(i, time/1000, m3_g);

    gg->SetPoint(i, time/1000, gtd);

    h1m1->Fill(m1);
    h1m2->Fill(m2);
    h1m3->Fill(m3);

    h1m1_gtd->Fill(m1_g);
    h1m2_gtd->Fill(m2_g);
    h1m3_gtd->Fill(m3_g);

    h1g->Fill(gtd);
  }


  //Style->SetOptStat(0);
  gStyle->SetPalette(51,0);


  // Set titles
  mm1_gtd->SetTitle(";Date;MM1COR_CAL/TRTGTD");
  mm2_gtd->SetTitle(";Date;MM2COR_CAL/TRTGTD");
  mm3_gtd->SetTitle(";Date;MM3COR_CAL/TRTGTD");

  mm1->SetTitle(";Date;MM1COR_CAL");
  mm2->SetTitle(";Date;MM2COR_CAL");
  mm3->SetTitle(";Date;MM3COR_CAL");

  gg->SetTitle(";Date;E12_TRTGTD");


  TCanvas *c_mm1 = new TCanvas("c_mm1", "c_mm1", 1200, 800);
  TCanvas *c_mm2 = new TCanvas("c_mm2", "c_mm2", 1200, 800);
  TCanvas *c_mm3 = new TCanvas("c_mm3", "c_mm3", 1200, 800);

  TCanvas *c_mm1_gtd = new TCanvas("c_mm1_gtd", "c_mm1_gtd", 1200, 800);
  TCanvas *c_mm2_gtd = new TCanvas("c_mm2_gtd", "c_mm2_gtd", 1200, 800);
  TCanvas *c_mm3_gtd = new TCanvas("c_mm3_gtd", "c_mm3_gtd", 1200, 800);

  TCanvas *c_gg = new TCanvas("c_gg", "c_gg", 1200, 800);


  TCanvas *c_h1m1 = new TCanvas("c_h1m1", "c_h1m1", 800, 800); 
  TCanvas *c_h1m2 = new TCanvas("c_h1m2", "c_h1m2", 800, 800); 
  TCanvas *c_h1m3 = new TCanvas("c_h1m3", "c_h1m3", 800, 800); 

  TCanvas *c_h1g = new TCanvas("c_h1g", "c_h1g", 800, 800); 

  TCanvas *c_h1m1_gtd = new TCanvas("c_h1m1_gtd", "c_h1m1_gtd", 800, 800); 
  TCanvas *c_h1m2_gtd = new TCanvas("c_h1m2_gtd", "c_h1m2_gtd", 800, 800); 
  TCanvas *c_h1m3_gtd = new TCanvas("c_h1m3_gtd", "c_h1m3_gtd", 800, 800); 

  //mm1->GetYaxis()->SetRangeUser(gr_min_m1, gr_max_m1);
  //mm1_gtd->GetYaxis()->SetRangeUser(gr_min_m1g, gr_max_m1g);
  //gg->GetYaxis()->SetRangeUser(gr_min_g,gr_max_g);


  setStylePlot(mm1, c_mm1);
  setStylePlot(mm2, c_mm2);
  setStylePlot(mm3, c_mm3);

  setStylePlot(mm1_gtd, c_mm1_gtd);
  setStylePlot(mm2_gtd, c_mm2_gtd);
  setStylePlot(mm3_gtd, c_mm3_gtd);

  setStylePlot(gg, c_gg);


  // Save plots
  savePlot(c_mm1, "graph_MM1COR_CAL");
  savePlot(c_mm2, "graph_MM2COR_CAL");
  savePlot(c_mm3, "graph_MM3COR_CAL");

  savePlot(c_mm1_gtd, "graph_MM1COR_CAL_TRTGTD");
  savePlot(c_mm2_gtd, "graph_MM2COR_CAL_TRTGTD");
  savePlot(c_mm3_gtd, "graph_MM3COR_CAL_TRTGTD");

  savePlot(c_gg, "graph_TRTGTD");

  h1m1->SetTitle(";MM1COR_CAL;N Events");
  h1m2->SetTitle(";MM2COR_CAL;N Events");
  h1m3->SetTitle(";MM3COR_CAL;N Events");

  h1m1_gtd->SetTitle(";MM1COR_CAL/TRTGTD;N Events");
  h1m2_gtd->SetTitle(";MM2COR_CAL/TRTGTD;N Events");
  h1m3_gtd->SetTitle(";MM3COR_CAL/TRTGTD;N Events");

  h1g->SetTitle(";TRTGTD;N Events");

  setStyleTH1(h1m1, c_h1m1);
  setStyleTH1(h1m2, c_h1m2);
  setStyleTH1(h1m3, c_h1m3);

  setStyleTH1(h1m1_gtd, c_h1m1_gtd);
  setStyleTH1(h1m2_gtd, c_h1m2_gtd);
  setStyleTH1(h1m3_gtd, c_h1m3_gtd);

  setStyleTH1(h1g, c_h1g);

  savePlot(c_h1m1, "th1d_MM1COR_CAL");
  savePlot(c_h1m2, "th1d_MM2COR_CAL");
  savePlot(c_h1m3, "th1d_MM3COR_CAL");

  savePlot(c_h1m1_gtd, "th1d_MM1COR_CAL_TRTGTD");
  savePlot(c_h1m2_gtd, "th1d_MM2COR_CAL_TRTGTD");
  savePlot(c_h1m3_gtd, "th1d_MM3COR_CAL_TRTGTD");

  savePlot(c_h1g, "th1d_TRTGTD");

}
