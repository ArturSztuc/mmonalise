#include "plotter.h"

Plotter::Plotter(TTree *treeIn){
  std::cout << "Producing plots for " << treeIn->GetName() << std::endl;

  // We are not doing comparison between different TTrees
  ttreeComparison = false;
  tree = treeIn;
};

// Simle plotter for all the variables
void Plotter::PlotBare(){
  setBranches();
  setMinMax(NULL, true);
  init();
  fillHistograms();
}
void Plotter::JennyPlots(){
  std::cout << "Setting branches!" << std::endl;
  setBranches();
  std::cout << "Filling plots!" << std::endl;
  setTimePlots();
  fillTimePlots();
}

void Plotter::JennyPlots2(std::string fout){
  setBranches();
  setTimePlots();
  timeBatchedTTree(fout);
}

void Plotter::timeBatchedTTree(std::string fout){
  TFile *fo = new TFile(fout.c_str(), "RECREATE");
  TTree *to = new TTree("muon_monitors", "Batched muon monitor variables");

  // Make the parameter holder
  std::vector< std::vector< double > > par_holder;
  for(int i = 0; i < k_nLevel; ++i){
    std::vector< double > tmp1;
    par_holder.push_back(tmp1);
  }

  Long64_t t_time;

  // Fill vector with the parameter values
  for(int i = 0; i < nEvents; ++i){
    tree->GetEntry(i);
    if(i == 0) t_time = time;
    for(int par = 0; par < k_nLevel; ++par){
      if(time_plot_b[par] == false) continue;
      par_holder[par].push_back(vals[par]);
    }
  }

  for(int par = 0; par < k_nLevel; ++par){
    // Get the mean and SD
    if(time_plot_b[par] == false) continue;
    m_vals[par] = 0;
    sd_vals[par] = 0;
    m_vals[par] = getMean(par_holder[par]);
    sd_vals[par] = getVariance(par_holder[par], m_vals[par]);
    
    // Set the TTree
    to->Branch(levelX_to_str(par).c_str(), &m_vals[par]);
    to->Branch((levelX_to_str(par) + "_SD").c_str(), &sd_vals[par]);
  }
  to->Branch("time", &t_time, "time/L");

  to->Fill();
  to->Write();
  fo->Close();
}


void Plotter::setTimePlots(){

  // Switch off all the plots by default
  for(int i = 0; i < k_nLevel; ++i)
    time_plot_b[i] = false;

  // Swithch on the MM#COR plots
  time_plot_b[k_mm1cor_cal] = true;
  time_plot_b[k_mm2cor_cal] = true;
  time_plot_b[k_mm3cor_cal] = true;


  // Set MM#COR plots
  time_plot[k_mm1cor_cal] = new TGraph(nEvents);
  time_plot[k_mm1cor_cal]->SetTitle(";Date;MM1COR_CAL/TRTGTD");
  time_plot[k_mm2cor_cal] = new TGraph(nEvents);
  time_plot[k_mm2cor_cal]->SetTitle(";Date;MM2COR_CAL/TRTGTD");
  time_plot[k_mm3cor_cal] = new TGraph(nEvents);
  time_plot[k_mm3cor_cal]->SetTitle(";Date;MM3COR_CAL/TRTGTD");

  // Set the nicer plotting style
  for(int i = 0; i < k_nLevel; ++i){
    if(time_plot_b[i] == true){
      setTGraphTimeStyle(time_plot[i]);
    }
  }
}

bool Plotter::timeBatcher(int minutes){
    time_converted.Set(time/1000);
    int minute_now = 
    (int)time_converted.GetDay()*24*60 +
    (int)time_converted.GetHour()*60 +
    (int)time_converted.GetMinute();
    //int minute_now = (int)time_converted.GetMinute();
    if( minute_now < itime_batch ){
      itime_batch = minute_now;
      return false;
    }
    else if(minutes > (minute_now - itime_batch)){
      return true;
    }
    else{
      itime_batch = minute_now;
      return false;
    }
}

void Plotter::fillDouble(std::vector< double > v_pars, double *d_pars){
  d_pars = new double(v_pars.size());
  for(int i = 0; i < (int)v_pars.size(); ++i){
    d_pars[i] = v_pars[i];
  }
}

double * Plotter::getDouble(std::vector< int > v_pars){
  double *d = new double(v_pars.size());
  for(int i = 0; i < (int)v_pars.size(); ++i){
    d[i] = v_pars[i];
  }
  return d;
}

double * Plotter::getDouble(std::vector< double > v_pars){
  double *d = new double(v_pars.size());
  for(int i = 0; i < (int)v_pars.size(); ++i){
    d[i] = v_pars[i];
  }
  return d;
}

double Plotter::getMean(std::vector< double > pars){
  int len = pars.size();
  double sum = 0;
  for(int i = 0; i < len; ++i){
    sum += pars[i];
  }
  return sum/len;
}

double Plotter::getVariance(std::vector< double > pars, double mean){
  double var = 0;
  int len = pars.size();
  for(int i = 0; i < len; ++i){
    var += (pars[i] - mean)*(pars[i] - mean);
  }
  var /= len;
  return sqrt(var);
}

  // Check for Jenny
void Plotter::fillTimePlots(){

  // Batch timewise
  std::vector< std::vector < double > > batched_pars;
  std::vector< Long64_t > x_times;
  std::vector< std::vector< double > > y_batched_pars;
  std::vector< std::vector< double > > y_batched_pars_errors;

  // k_nLevel vectors
  for(int i = 0; i < k_nLevel; ++i){
    std::vector< double > tmp1;
    std::vector< double > tmp2;
    std::vector< double > tmp3;
    batched_pars.push_back(tmp1);
    y_batched_pars.push_back(tmp2);
    y_batched_pars_errors.push_back(tmp3);
  }

  // First time
  tree->GetEntry(0);
  time_converted.Set(time/1000);

  itime_batch = 
    (int)time_converted.GetDay()*24*60 +
    (int)time_converted.GetHour()*60 +
    (int)time_converted.GetMinute();
  x_times.push_back(time);

  double mean = 0;
  double sd = 0;
  // Iterate through the events
  for(int i = 0; i < nEvents; ++i){
    tree->GetEntry(i);
    //bool isFine = timeBatcher(30); 
    bool isFine = timeBatcher(30); 

    // Batching
    if(isFine == true){
      for(int par = 0; par < k_nLevel; ++par){
        if(time_plot_b[par] == false) continue;
        batched_pars[par].push_back(vals[par]);
      }
    }
    // Plotting
    else{
      for(int par = 0; par < k_nLevel; ++par){
        if(time_plot_b[par] == false) continue;
        // Get mean and SD
        mean = getMean(batched_pars[par]);
        sd = getVariance(batched_pars[par], mean);

        if(std::isnan(mean)){
          batched_pars[par].clear();
          continue;
        }
        // Fill batched means/sds
        x_times.push_back(time);
        y_batched_pars[par].push_back(mean);
        y_batched_pars_errors[par].push_back(sd);

        // Clear batched container
        batched_pars[par].clear();
      }
    }
  }
  // Remove the last time element (overflow!).
  x_times.pop_back();

  // Do the plotting stuff, finally
  int size = x_times.size();
  //TGraphErrors *mm1cor = new TGraphErrors(size, getDouble(x_times),
  //    getDouble(y_batched_pars[k_mm1cor_cal]), 0,
  //    getDouble(y_batched_pars_errors[k_mm1cor_cal]));
  //TGraphErrors *mm2cor = new TGraphErrors(size, getDouble(x_times),
  //    getDouble(y_batched_pars[k_mm2cor_cal]), 0,
  //    getDouble(y_batched_pars_errors[k_mm2cor_cal]));
  //TGraphErrors *mm3cor = new TGraphErrors(size, getDouble(x_times),
  //    getDouble(y_batched_pars[k_mm3cor_cal]), 0,
  //    getDouble(y_batched_pars_errors[k_mm3cor_cal]));

  TGraphErrors *mm1cor = new TGraphErrors(size);
  TGraphErrors *mm2cor = new TGraphErrors(size);
  TGraphErrors *mm3cor = new TGraphErrors(size);

  for(int i = 0; i < size; ++i){
    mm1cor->SetPoint(i, x_times[i], y_batched_pars[k_mm1cor_cal][i]);
    mm2cor->SetPoint(i, x_times[i], y_batched_pars[k_mm2cor_cal][i]);
    mm3cor->SetPoint(i, x_times[i], y_batched_pars[k_mm3cor_cal][i]);

    mm1cor->SetPointError(i, 0, y_batched_pars_errors[k_mm1cor_cal][i]);
    mm2cor->SetPointError(i, 0, y_batched_pars_errors[k_mm2cor_cal][i]);
    mm3cor->SetPointError(i, 0, y_batched_pars_errors[k_mm3cor_cal][i]);
  }
  std::cout << "We got all the way here..." << std::endl;

  setTGraphTimeStyle(mm1cor);
  setTGraphTimeStyle(mm2cor);
  setTGraphTimeStyle(mm3cor);

  TCanvas *c = new TCanvas("c", "c", 1200, 800);
  c->cd();
  mm1cor->Draw("ap");
  c->SaveAs("mm1cor.png");

  TCanvas *cc = new TCanvas("cc", "cc", 1200, 800);
  cc->cd();
  mm2cor->Draw("ap");
  cc->SaveAs("mm2cor.png");

  TCanvas *ccc = new TCanvas("ccc", "ccc", 1200, 800);
  ccc->cd();
  mm3cor->Draw("ap");
  ccc->SaveAs("mm3cor.png");
}



void Plotter::PlotThermoC(){

  // Set the branches first
  double t_vals6[t_thermo][6];
  double t_vals[t_thermo];

  // Set all the branches!
  for (int param = 0; param < t_thermo; param++) {
    if(is6(param,1) == true){
      tree->SetBranchAddress(getString(param,1).c_str(), &t_vals6[param]);
    }
    else{
      tree->SetBranchAddress(getString(param,1).c_str(), &t_vals[param]);
    }
  }
  tree->SetBranchAddress("time", &time);

  TH2D *vertical2D = new TH2D("vertical_thermocouple", "Vertical Thermocouple;#Delta T_{m}/#Delta T_{b};#Delta T_{m}/#Delta T_{t};", 100, 1, 2, 100, 1, 2);
  TH2D *horizontal2D = new TH2D("horizontal_thermocouple", "Horizontal Thermocouple;#Delta T_{c}/#Delta T_{l};#Delta T_{c}/#Delta T_{r};", 100, 1, 2, 100, 1, 2);

  nEvents = tree->GetEntries();
  for(int i = 0; i < nEvents; ++i){
    tree->GetEntry(i);

    double delta_tb, delta_tm, delta_tt, delta_tl, delta_tc, delta_tr;
    delta_tb = t_vals[t_tgtt3] - t_vals[t_tgtt4];
    delta_tm = t_vals[t_tgtt1] - t_vals[t_tgtt4];
    delta_tt = t_vals[t_tgtt2] - t_vals[t_tgtt4];

    delta_tl = t_vals[t_thptbw] - t_vals[t_thpths];
    delta_tc = t_vals[t_thptcw] - t_vals[t_thpths];
    delta_tr = t_vals[t_thpttw] - t_vals[t_thpths];

    vertical2D->Fill(delta_tm/delta_tb, delta_tm/delta_tt);
    horizontal2D->Fill(delta_tc/delta_tl, delta_tc/delta_tr);
  }
  TCanvas *c1 = new TCanvas("c1", "", 1200, 1200);
  vertical2D->Draw("p");
  c1->SaveAs("vertical.png");
  c1->SaveAs("vertical.C");
  c1->SaveAs("vertical.root");
  TCanvas *c2 = new TCanvas("c2", "", 1200, 1200);
  horizontal2D->Draw("p");
  c2->SaveAs("horizontal.png");
  c1->SaveAs("horizontal.C");
  c1->SaveAs("horizontal.root");
}

void Plotter::PlotBeamPositionAtTarget(){
  int nEntries = tree->GetEntries();

  int mappy[9][9]= {
    {33,42,51,60,69,78,6,15,24},
    {34,43,52,61,70,79,7,16,25},
    {35,44,53,62,71,80,8,17,26},
    {36,45,54,63,72,0,9,18,27},
    {37,46,55,64,73,1,10,19,28},
    {38,47,56,65,74,2,11,20,29},
    {39,48,57,66,75,3,12,21,30},
    {40,49,58,67,76,4,13,22,31},
    {41,50,59,68,77,5,14,23,32}
  };

  // Define plots
  TH2D *intensity1 = new TH2D("Intensity1", "", 9, 1, 10, 9, 1, 10);
  TH2D *intensity2 = new TH2D("Intensity2", "", 9, 1, 10, 9, 1, 10);
  TH2D *intensity3 = new TH2D("Intensity3", "", 9, 1, 10, 9, 1, 10);

  TH2D *int1_temp = new TH2D("Intensity1tm", "", 9, 1, 10, 9, 1, 10);
  TH2D *int2_temp = new TH2D("Intensity2tm", "", 9, 1, 10, 9, 1, 10);
  TH2D *int3_temp = new TH2D("Intensity3tm", "", 9, 1, 10, 9, 1, 10);

  TGraph *tg_centr_x1 = new TGraph(nEntries);
  TGraph *tg_centr_x2 = new TGraph(nEntries);
  TGraph *tg_centr_x3 = new TGraph(nEntries);
  //tg_centr_x1->SetTitle(";Date;Horizontal centroid 1");
  //tg_centr_x2->SetTitle(";Date;Horizontal centroid 2");
  //tg_centr_x3->SetTitle(";Date;Horizontal centroid 3");
  tg_centr_x1->SetTitle(";Date;MM1sig_integral/TRTGTD");
  tg_centr_x2->SetTitle(";Date;MM2sig_integral/TRTGTD");
  tg_centr_x3->SetTitle(";Date;MM3sig_integral/TRTGTD");

  TGraph *tg_centr_y1 = new TGraph(nEntries);
  TGraph *tg_centr_y2 = new TGraph(nEntries);
  TGraph *tg_centr_y3 = new TGraph(nEntries);
  tg_centr_y1->SetTitle(";Date;Vertical centroid 1");
  tg_centr_y2->SetTitle(";Date;Vertical centroid 2");
  tg_centr_y3->SetTitle(";Date;Vertical centroid 3");

  TH1D *cr1d = new TH1D("c11", "", 100, 1.7, 1.8);
  TH1D *cr2d = new TH1D("c22", "", 100, 5.4, 5.6);
  TH1D *cr3d = new TH1D("c33", "", 100, 0.38, 0.5);

  //double frs_centroid_x1, frs_centroid_x2, frs_centroid_x3, frs_centroid_y1, frs_centroid_y2, frs_centroid_y3;
  //Long64_t ttime;
  for(int i = 0; i < nEntries; ++i){
    tree->GetEntry(i);

    // Clear integral histograms
    int1_temp->Reset();
    int2_temp->Reset();
    int3_temp->Reset();

    for(int j = 0; j < 9; ++j){
      for(int k = 0; k < 9; ++k){

        double bin = intensity1->GetBinContent(j+1, k+1);
        bin += vals81[k_mm1_sig_calib][mappy[j][k]]; 
        intensity1->SetBinContent(j+1, k+1, bin);
        int1_temp->SetBinContent(j+1, k+1, vals81[k_mm1_sig_calib][mappy[j][k]]);

        bin = intensity2->GetBinContent(j+1, k+1);
        bin += vals81[k_mm2_sig_calib][mappy[j][k]]; 
        intensity2->SetBinContent(j+1, k+1, bin);
        int2_temp->SetBinContent(j+1, k+1, vals81[k_mm2_sig_calib][mappy[j][k]]);

        bin = intensity3->GetBinContent(j+1, k+1);
        bin += vals81[k_mm3_sig_calib][mappy[j][k]]; 
        intensity3->SetBinContent(j+1, k+1, bin);
        int3_temp->SetBinContent(j+1, k+1, vals81[k_mm3_sig_calib][mappy[j][k]]);
      }
    }
    double centroid_x1, centroid_x2, centroid_x3, centroid_y1, centroid_y2, centroid_y3;

    centroid_x1 = (4.5 - int1_temp->GetMean(1))*11.5;
    centroid_x2 = (4.5 - int2_temp->GetMean(1))*11.5;
    centroid_x3 = (4.5 - int3_temp->GetMean(1))*11.5;

    //centroid_x1 = (4.5 - int1_temp->GetMean())*11.5;
    //centroid_x2 = (4.5 - int2_temp->GetMean())*11.5;
    //centroid_x3 = (4.5 - int3_temp->GetMean())*11.5;

    centroid_x1 = int1_temp->Integral();
    centroid_x2 = int2_temp->Integral();
    centroid_x3 = int3_temp->Integral();

    centroid_y1 = (4.5 - int1_temp->GetMean(2))*11.5;
    centroid_y2 = (4.5 - int2_temp->GetMean(2))*11.5;
    centroid_y3 = (4.5 - int3_temp->GetMean(2))*11.5;

    //centroid_x1 = vals[k_mm1cor_cal];
    //centroid_x2 = vals[k_mm2cor_cal];
    //centroid_x3 = vals[k_mm3cor_cal];

    if(i == 0){
      //frs_centroid_x1 = centroid_x1;
      //frs_centroid_x2 = centroid_x2;
      //frs_centroid_x3 = centroid_x3;

      //frs_centroid_y1 = centroid_y1;
      //frs_centroid_y2 = centroid_y2;
      //frs_centroid_y3 = centroid_y3;
    }

    //centroid_x1 = centroid_x1/frs_centroid_x1;
    //centroid_x2 = centroid_x2/frs_centroid_x2;
    //centroid_x3 = centroid_x3/frs_centroid_x3;

    //centroid_y1 = centroid_y1/frs_centroid_y1;
    //centroid_y2 = centroid_y2/frs_centroid_y2;
    //centroid_y3 = centroid_y3/frs_centroid_y3;

    TDatime time_cor;
    time_cor.Set(time/1000);

    time = time_cor.Convert();
    tg_centr_x1->SetPoint(i, time, centroid_x1/(vals[k_e12_trtgtd]));
    tg_centr_x2->SetPoint(i, time, centroid_x2/(vals[k_e12_trtgtd]));
    tg_centr_x3->SetPoint(i, time, centroid_x3/(vals[k_e12_trtgtd]));
    tg_centr_y1->SetPoint(i, time, centroid_y1/(vals[k_e12_trtgtd]));
    tg_centr_y2->SetPoint(i, time, centroid_y2/(vals[k_e12_trtgtd]));
    tg_centr_y3->SetPoint(i, time, centroid_y3/(vals[k_e12_trtgtd]));



    cr1d->Fill(centroid_x1/(vals[k_e12_trtgtd]));
    cr2d->Fill(centroid_x2/(vals[k_e12_trtgtd]));
    cr3d->Fill(centroid_x3/(vals[k_e12_trtgtd]));

    //tg_centr_x1->SetPoint(i, time, centroid_x1);
    //tg_centr_x2->SetPoint(i, time, centroid_x2);
    //tg_centr_x3->SetPoint(i, time, centroid_x3);
    //tg_centr_y1->SetPoint(i, time, centroid_y1);
    //tg_centr_y2->SetPoint(i, time, centroid_y2);
    //tg_centr_y3->SetPoint(i, time, centroid_y3);

  }

  TCanvas *c11= new TCanvas("c11", "c11", 1200, 1200);
  cr1d->SetTitle(";MM1COR/TRTGTD;");
  cr1d->Draw();
  c11->SaveAs("c11.png");
  TCanvas *c22= new TCanvas("c22", "c22", 1200, 1200);
  cr2d->SetTitle(";MM2COR/TRTGTD;");
  cr2d->Draw();
  c22->SaveAs("c22.png");
  TCanvas *c33= new TCanvas("c33", "c33", 1200, 1200);
  cr3d->SetTitle(";MM3COR/TRTGTD;");
  cr3d->Draw();
  c33->SaveAs("c33.png");

  TCanvas *c1_int = new TCanvas("c1", "c1", 1200, 1200);
  TCanvas *c2_int = new TCanvas("c2", "c2", 1200, 1200);
  TCanvas *c3_int = new TCanvas("c3", "c3", 1200, 1200);


  intensity1->GetXaxis()->CenterLabels();
  intensity1->GetYaxis()->CenterLabels();
  intensity2->GetXaxis()->CenterLabels();
  intensity2->GetYaxis()->CenterLabels();
  intensity3->GetXaxis()->CenterLabels();
  intensity3->GetYaxis()->CenterLabels();

  c1_int->cd();
  intensity1->Draw("col");
  TColor::InvertPalette();
  c1_int->SaveAs("int1.png");

  c2_int->cd();
  intensity2->Draw("col");
  c2_int->SaveAs("int2.png");

  c3_int->cd();
  intensity3->Draw("col");
  c3_int->SaveAs("int3.png");


  TCanvas *canv_centroid_x1 = new TCanvas("centr_x1", "centr_x1", 1200, 800);
  canv_centroid_x1->cd();
  setTGraphTimeStyle(tg_centr_x1);
  tg_centr_x1->Draw("ap");
  canv_centroid_x1->SaveAs("canv_centroid_x1.png");

  TCanvas *canv_centroid_x2 = new TCanvas("centr_x2", "centr_x2", 1200, 800);
  canv_centroid_x2->cd();
  setTGraphTimeStyle(tg_centr_x2);
  tg_centr_x2->Draw("ap");
  canv_centroid_x2->SaveAs("canv_centroid_x2.png");

  TCanvas *canv_centroid_x3 = new TCanvas("centr_x3", "centr_x3", 1200, 800);
  canv_centroid_x3->cd();
  setTGraphTimeStyle(tg_centr_x3);
  tg_centr_x3->Draw("ap");
  canv_centroid_x3->SaveAs("canv_centroid_x3.png");

  //TCanvas *canv_centroid_y1 = new TCanvas("centr_y1", "centr_y1", 1200, 800);
  //canv_centroid_y1->cd();
  //setTGraphTimeStyle(tg_centr_y1);
  //tg_centr_y1->Draw("ap");
  //canv_centroid_y1->SaveAs("canv_centroid_y1.png");

  //TCanvas *canv_centroid_y2 = new TCanvas("centr_y2", "centr_y2", 1200, 800);
  //canv_centroid_y2->cd();
  //setTGraphTimeStyle(tg_centr_y2);
  //tg_centr_y2->Draw("ap");
  //canv_centroid_y2->SaveAs("canv_centroid_y2.png");

  //TCanvas *canv_centroid_y3 = new TCanvas("centr_y3", "centr_y3", 1200, 800);
  //canv_centroid_y3->cd();
  //setTGraphTimeStyle(tg_centr_y3);
  //tg_centr_y3->Draw("ap");
  //canv_centroid_y3->SaveAs("canv_centroid_y3.png");

}

void Plotter::setTGraphTimeStyle(TGraph *gr){
  //gr->SetMarkerStyle(20);
  //gr->GetXaxis()->SetTimeDisplay(1);
  gr->GetXaxis()->SetTimeDisplay(1);
  gr->GetXaxis()->SetTimeFormat("%d/%m/%y");
  gr->GetXaxis()->SetTimeOffset(0,"cst");
  gr->GetXaxis()->SetNdivisions(-505);
}

// Sets the branch addresses
void Plotter::setBranches(TTree* tree_set){

  // Use the default TTree if not parsed through
  if(tree_set == NULL)
    tree_set = tree;

  // Set all the branches!
  for (int param = 0; param < k_nLevel; param++) {
    if(is81(param) == true){
      tree_set->SetBranchAddress(levelX_to_str(param).c_str(), &vals81[param]);
    }
    else if(is6(param) == true){
      tree_set->SetBranchAddress(levelX_to_str(param).c_str(), &vals6[param]);
    }
    else{
      tree_set->SetBranchAddress(levelX_to_str(param).c_str(), &vals[param]);
    }
  }
  tree_set->SetBranchAddress("time", &time);
  nEvents = tree_set->GetEntries();
}

// This will find the min and max of each variable, to be used later for nicer plotting
void Plotter::setMinMax(TTree* tree_set, bool clear){

  // Use default TTree if not defined
  if(tree_set == NULL)
    tree_set = tree;

  // Are we clrearing the previous previous entries?
  if(clear){
    time_minmax[0] = 10E10;
    time_minmax[1] = -10E10;
    time = -10E10;
    for (int par = 0; par < k_nLevel; par++) {
      vals_minmax[par][0] = 10e100;
      vals_minmax[par][1] = -10e100;
      //vals[par] = -10e100;
      for (int par6 = 0; par6 < 6; par6++) {
        vals6_minmax[par][par6][0] = 10e100;
        vals6_minmax[par][par6][1] = -10e100;
        //vals6[par][par6] = -10e100;
      }
      for (int par81 = 0; par81 < 81; par81++) {
        vals81_minmax[par][par81][0] = 10e100;
        vals81_minmax[par][par81][1] = -10e100;
        //vals81[par][par81] = -10e100;
      }
    }
  }

  // Iterate through every entry and find the biggest/smallest value for each parameter!
  int nEntries = tree->GetEntries();
  for (int entry = 0; entry < nEntries; entry++) {
    tree->GetEntry(entry);
    if(time < time_minmax[0]) time_minmax[0] = time;
    if(time > time_minmax[1]) time_minmax[1] = time;
    for (int par = 0; par < k_nLevel; par++) {
      if(vals[par] < vals_minmax[par][0]) vals_minmax[par][0] = vals[par];
      if(vals[par] > vals_minmax[par][1]) vals_minmax[par][1] = vals[par];
      for (int par6 = 0; par6 < 6; par6++) {
        if(vals6[par][par6] < vals6_minmax[par][par6][0]) vals6_minmax[par][par6][0] = vals6[par][par6];
        if(vals6[par][par6] > vals6_minmax[par][par6][1]) vals6_minmax[par][par6][1] = vals6[par][par6];
      }
      for (int par81 = 0; par81 < 81; par81++) {
        if(is81(par) == true){
            if(vals81[par][par81] < vals_minmax[par][0]) vals_minmax[par][0] = vals81[par][par81];
            if(vals81[par][par81] > vals_minmax[par][1]) vals_minmax[par][1] = vals81[par][par81];
        }
      }
    }
  }

#if OUTPUT
  // Printer
  std::cout << "time_minmax[0]" << ": " << time_minmax[0] << std::endl;
  std::cout << "time_minmax[1]" << ": " << time_minmax[1] << std::endl;
  for (int par = 0; par < k_nLevel; par++) {
    std::cout << "vals_minmax[" << levelX_to_str(par) << "][0]" << ": " << vals_minmax[par][0] << std::endl;
    std::cout << "vals_minmax[" << levelX_to_str(par) << "][1]" << ": " << vals_minmax[par][1] << std::endl;
  }
  std::cout << "SET MIN: " << 10E10 << "  SET MAX: " << -10E10 << std::endl;
#endif
}


// TODO: 
// * Need to split the bool setup to be done independently of TCanvas/TH2D
//   setup. The TC/TH setup should be "cleared" each time, since we might be
//   otherwise overwriting things when we are doing comparison between TTrees
void Plotter::init(){
  for (int i = 0; i < k_nLevel; ++i) {
    for (int j = 0; j < k_nLevel; ++j) {
      normal_plot_b[i][j] = false;
    }
  }
  // Compare between mm's
  //vals_minmax[k_mm1xav][0] = 0.55; vals_minmax[k_mm1xav][1] = 0.65;
  //vals_minmax[k_mm2xav][0] = 0; vals_minmax[k_mm2xav][1] = 1;
  //vals_minmax[k_mm3xav][0] = 1; vals_minmax[k_mm3xav][1] = 2;

  //vals_minmax[k_mm1yav][0] = -2; vals_minmax[k_mm1yav][1] = 0;
  //vals_minmax[k_mm2yav][0] = 0; vals_minmax[k_mm2yav][1] = 1;
  //vals_minmax[k_mm3yav][0] = 0; vals_minmax[k_mm3yav][1] = 2;

  //vals_minmax[k_mm1cor][0] = 84; vals_minmax[k_mm1cor][1] = 89;

  //normal_plot_b[k_mm1xav][k_mm2xav] = true;
  //normal_plot_b[k_mm1xav][k_mm3xav] = true;
  //normal_plot_b[k_mm1yav][k_mm2yav] = true;
  //normal_plot_b[k_mm1yav][k_mm3yav] = true;
  //normal_plot_b[k_mm1yav][k_mm3yav] = true;

//  normal_plot_b[k_mm1xav][k_mm1cor] = true;

  // Compare between mm xy
  //normal_plot_b[k_mm1xav][k_mm1yav] = true;
  //normal_plot_b[k_mm2xav][k_mm2yav] = true;

  for (int i = 0; i < k_nLevel; ++i) {
    for (int j = 0; j < k_nLevel; ++j) {
      if(normal_plot_b[i][j] == true){
        std::string axi = levelX_to_str(i);
        std::string axj = levelX_to_str(j);
        std::string plotname = (axi + "vs" + axj).c_str();
        normal_canv[i][j] = new TCanvas(plotname.c_str(), plotname.c_str(), 800, 800);
        normal_plot[i][j] = new TH2D(plotname.c_str(), 
            (axi + " vs " + axj + ";" + axi + ";" + axj).c_str(),
            100, vals_minmax[i][0], vals_minmax[i][1],
            100, vals_minmax[j][0], vals_minmax[j][1]);
      }
    }
  }
}

// This will fill the histograms and draw them on their canvases
void Plotter::fillHistograms(TTree *tree_set, bool first){
  if(tree_set == NULL)
    tree_set = tree;

  int nEntries = tree_set->GetEntries();

  // Fill the histograms
  for (int entry = 0; entry < nEntries; ++entry) {
    tree_set->GetEntry(entry);
    for (int i = 0; i < k_nLevel; ++i) {
      for (int j = 0; j < k_nLevel; ++j) {
        if(normal_plot_b[i][j] == true){
          if((is81(i) == true) && (is81(j) == true))
            normal_plot[i][j]->Fill(vals81[i][0], vals81[j][0]);
          else if((is81(i) == true) && (is81(j) == false))
            normal_plot[i][j]->Fill(vals81[i][0], vals[j]);
          else if((is81(i) == false) && (is81(j) == true))
            normal_plot[i][j]->Fill(vals[i], vals81[j][0]);
          else
            normal_plot[i][j]->Fill(vals[i], vals[j]);
        }
      }
    }
  }

  // And draw the histograms!
  for (int i = 0; i < k_nLevel; ++i) {
    for (int j = 0; j < k_nLevel; ++j) {
      if(normal_plot_b[i][j] == true){
        normal_canv[i][j]->cd();
        if(first){
          setStyle(normal_plot[i][j], normal_canv[i][j]);
          normal_plot[i][j]->Draw("p");
        }
        else
          normal_plot[i][j]->Draw("p same");
        // FIXME XXX TODO:
        //  * This should be in a separate PUBLIC function!!
        normal_canv[i][j]->SaveAs((levelX_to_str(i) + "_" + levelX_to_str(j) + ".png").c_str());
      }
    }
  }
}

void Plotter::setStyle(TH2D *plot, TCanvas *c){
    c->SetLeftMargin(0.15);
    c->SetBottomMargin(0.15);
    c->SetRightMargin(0.1);
    c->SetTopMargin(0.1);

    plot->SetLineColor(kBlack);
    plot->SetFillColor(13);
    plot->SetMarkerColor(kRed);
    plot->SetMarkerStyle(20);
    plot->SetMarkerSize(0.5);
    plot->SetTitleSize(0.07);

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
}

bool Plotter::is6(int i, int mode){

  switch(mode){
    case 0:
      if(i == k_vptgt || i == k_hptgt || i == k_vp121 || i == k_hp121){
        return true;
      }
      else
        return false;
    case 1:
//      if(i == t_vptgt || i == t_hptgt || i == t_vp121 || i == t_hp121){
//        return true;
//      }
//      else
        return false;
    default:
      std::cout << "Mode " << mode << " not supported!" << std::endl;
      abort();
  }
}

bool Plotter::is81(int i){

    //if(i == k_mma1ds || i == k_mma2ds || i == k_mma3ds 
    //    || i == k_mma1pd || i == k_mma2pd || i == k_mma3pd 
    //    || i == k_mm1_sig_calib || i == k_mm2_sig_calib || i == k_mm3_sig_calib){
    //  return true;
    //}
    if(i == k_mm1_sig_calib || i == k_mm2_sig_calib || i == k_mm3_sig_calib){
      return true;
    }
    else
      return false;
}

std::string Plotter::levelX_to_str(int lev){
  if(isLvl0(lev) == true)
    return level0_to_str(lev);
  else
    return level1_to_str(lev);
}

std::string Plotter::getString(int i, int mode){
  switch(mode){
    case 0:
     return levelX_to_str(i);
    case 1:
     return thermoc_to_str(i);
    default:
      std::cout << "Mode " << mode << " not supported!" << std::endl;
      abort();
  }
}

int Plotter::getNPars(int mode){
  switch(mode){
    case 0:
      return k_nLevel;
    case 1:
      return t_thermo;
    default:
      std::cout << "Mode " << mode << " not supported!" << std::endl;
      abort();
  }
}
