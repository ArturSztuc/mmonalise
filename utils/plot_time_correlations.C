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

  c->cd();
  plot->Draw();
}

void setStylePlot(TGraph *plot, TCanvas *c){
  // TCanvas stylystics
  c->SetLeftMargin(0.15);
  c->SetBottomMargin(0.15);
  c->SetRightMargin(0.1);
  c->SetTopMargin(0.1);

  //plot->GetXaxis()->SetTimeDisplay(1);
  //plot->GetXaxis()->SetTimeFormat("%d/%m/%y");
  //plot->GetXaxis()->SetTimeOffset(0,"cst");
  //plot->GetXaxis()->SetNdivisions(-505);

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

  c->cd();
  plot->Draw("ap");
}

void plot_time_correlations(std::string sin){

  // Get the TTree
  TFile *fin = new TFile(sin.c_str(), "READ");
  TTree *tin = (TTree*)fin->Get("muon_monitors");

  // Set up the time holders
  Long64_t m1_t, m2_t, m3_t, gtd_t, nslina_t;

  //Global time (the same as mm1_t, so maybe scrap it)
  Long64_t time;

  // Setup times
  tin->SetBranchAddress("time", &time);
  tin->SetBranchAddress("MM1COR_CAL_time", &m1_t);
  tin->SetBranchAddress("MM2COR_CAL_time", &m2_t);
  tin->SetBranchAddress("MM3COR_CAL_time", &m3_t);
  tin->SetBranchAddress("NSLINA_time", &nslina_t);
  tin->SetBranchAddress("E12_TRTGTD_time", &gtd_t);

  // We will only plot errors smaller than 1...
  int nevents = tin->GetEntries();

  // Set the TGraphs
  TGraph *m1gtd_m2gtd = new TGraph(nevents);
  TGraph *m1gtd_m3gtd = new TGraph(nevents);
  TGraph *m1gtd_nslinagtd = new TGraph(nevents);

  int min_m1    = 99999;
  int min_m2    = 99999;
  int min_m3    = 99999;
  int min_nslina= 99999;

  int max_m1    = -99999;
  int max_m2    = -99999;
  int max_m3    = -99999;
  int max_nslina= -99999;

  // Load and set the points
  for(int i = 0; i < nevents; ++i){
    tin->GetEntry(i);

    m1gtd_m2gtd->SetPoint(i, m1_t - gtd_t, m2_t - gtd_t);
    m1gtd_m3gtd->SetPoint(i, m1_t - gtd_t, m3_t - gtd_t);
    m1gtd_nslinagtd->SetPoint(i, m1_t - gtd_t, nslina_t - gtd_t);

    // Find min
    if(min_m1 > m1_t-gtd_t)
      min_m1 = m1_t-gtd_t;
    if(min_m2 > m2_t-gtd_t)
      min_m2 = m2_t-gtd_t;
    if(min_m3 > m3_t-gtd_t)
      min_m3 = m3_t-gtd_t;
    if(min_nslina > nslina_t-gtd_t)
      min_nslina= nslina_t-gtd_t;

    // Find max
    if(max_m1 < m1_t-gtd_t)
      max_m1 = m1_t-gtd_t;
    if(max_m2 < m2_t-gtd_t)
      max_m2 = m2_t-gtd_t;
    if(max_m3 < m3_t-gtd_t)
      max_m3 = m3_t-gtd_t;
    if(max_nslina < nslina_t-gtd_t)
      max_nslina= nslina_t-gtd_t;
  }

  TH1D *mm1gtd = new TH1D("mm1gtd", "mm1gtd", 100, min_m1, max_m1);
  TH1D *mm2gtd = new TH1D("mm2gtd", "mm2gtd", 100, min_m2, max_m2);
  TH1D *mm3gtd = new TH1D("mm3gtd", "mm3gtd", 100, min_m3, max_m3);
  TH1D *nslinagtd = new TH1D("nslina", "nslina", 100, min_nslina, max_nslina);

  for(int i = 0; i < nevents; ++i){
    tin->GetEntry(i);

    mm1gtd->Fill(m1_t - gtd_t); 
    mm2gtd->Fill(m2_t - gtd_t); 
    mm3gtd->Fill(m3_t - gtd_t); 
    nslinagtd->Fill(nslina_t - gtd_t); 
  }

  //Style->SetOptStat(0);
  gStyle->SetPalette(51,0);


  // TCanvases for the TGraphs
  TCanvas *c_m1gtd_m2gtd    = new TCanvas("c_m1gtd_m2gtd", "c_m1gtd_m2gtd", 800, 800);
  TCanvas *c_m1gtd_m3gtd    = new TCanvas("c_m1gtd_m3gtd", "c_m1gtd_m3gtd", 800, 800);
  TCanvas *c_m1gtd_nslinagtd= new TCanvas("c_m1gtd_nslinagtd", "c_m1gtd_nslinagtd", 800, 800);

  // TCanvases for the TH1Ds
  TCanvas *c_mm1gtd    = new TCanvas("c_m1gtd", "c_m1gtd", 800, 800);
  TCanvas *c_mm2gtd    = new TCanvas("c_m2gtd", "c_m2gtd", 800, 800);
  TCanvas *c_mm3gtd    = new TCanvas("c_m3gtd", "c_m3gtd", 800, 800);
  TCanvas *c_nslinagtd= new TCanvas("c_nslinagtd", "c_nslinagtd", 800, 800);

  // TGraph titles
  m1gtd_m2gtd     ->SetTitle(";MMCOR1_t - TRTGTD_t;MMCOR2_t - TRTGTD_t");
  m1gtd_m3gtd     ->SetTitle(";MMCOR1_t - TRTGTD_t;MMCOR3_t - TRTGTD_t");
  m1gtd_nslinagtd ->SetTitle(";MMCOR1_t - TRTGTD_t;NSLINA - TRTGTD_t");

  // TH1D titles
  mm1gtd          ->SetTitle(";MMCOR1_t - TRTGTD_t;N Spills");
  mm2gtd          ->SetTitle(";MMCOR2_t - TRTGTD_t;N Spills");
  mm3gtd          ->SetTitle(";MMCOR3_t - TRTGTD_t;N Spills");
  nslinagtd       ->SetTitle(";NSLINA_t - TRTGTD_t;N Spills");

  // Set the plot stylistics
  setStylePlot(m1gtd_m2gtd, c_m1gtd_m2gtd);
  setStylePlot(m1gtd_m3gtd, c_m1gtd_m3gtd);
  setStylePlot(m1gtd_nslinagtd, c_m1gtd_nslinagtd);
  setStyleTH1(mm1gtd, c_mm1gtd);
  setStyleTH1(mm2gtd, c_mm2gtd);
  setStyleTH1(mm3gtd, c_mm3gtd);
  setStyleTH1(nslinagtd, c_nslinagtd);

  // Save all the plots
  savePlot(c_m1gtd_m2gtd,     "time_MM1CORTRTGTD_MM2CORTRTGTD");
  savePlot(c_m1gtd_m3gtd,     "time_MM1CORTRTGTD_MM3CORTRTGTD");
  savePlot(c_m1gtd_nslinagtd, "time_MM1CORTRTGTD_NSLINATRTGTD");
  savePlot(c_mm1gtd,    "time_th1_MM1COR_TRTGTD");
  savePlot(c_mm2gtd,    "time_th1_MM2COR_TRTGTD");
  savePlot(c_mm3gtd,    "time_th1_MM3COR_TRTGTD");
  savePlot(c_nslinagtd, "time_th1_NSLINA_TRTGTD");
}
