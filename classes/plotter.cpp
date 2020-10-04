#include "plotter.h"

Plotter::Plotter(TTree *treeIn){
  std::cout << "Producing plots for " << treeIn->GetName() << std::endl;

  // Horn mode name (default none, can be horizontal, vertical, both)
  hornmode_str = "";
  // Horn mode -- default to ignore (999)
  hornmode_int = 999;
  // We are not doing comparison between different TTrees
  ttreeComparison = false;
  // TTree in
  tree = treeIn;
  // Are parameters from Struct.h or exe? Default all from Struct
  isUserPar = false;
  time_counter = 0;
}

Plotter::Plotter(TTree *treeIn, std::vector< int > _pars){
  std::cout << "Producing plots for " << treeIn->GetName() << std::endl;

  // Horn mode name (default none, can be horizontal, vertical, both)
  hornmode_str = "";
  // Horn mode -- default to ignore (999)
  hornmode_int = 999;
  // We are not doing comparison between different TTrees
  ttreeComparison = false;
  // TTree in
  tree = treeIn;
  // Are parameters from Struct.h or exe? Default all from Struct
  isUserPar = true;
  // Get the parameter list
  pars = _pars;
}

// Clears the RAM
void Plotter::clearRAM(){
  int evs = tree->GetEntries();
  int npars = pars.size();
  for(int par = 0; par < npars; ++par){
    for(int ev = 0; ev < evs; ++ev){
      delete d_vals6[par][ev];
      delete d_vals81[par][ev];
    }
    delete d_vals6[par];
    delete d_vals81[par];
    delete d_vals[par];
    delete d_times[par];
  }
  delete d_time;
}

void Plotter::setTTree(TTree *treeIn){
  //delete tree;
  tree = treeIn;
}

void Plotter::fillRAM(){
  // Get the number of events and number of parameters
  int evs = tree->GetEntries();
  int npars = pars.size();

  //Initialise the data holders
  d_vals = new double*[npars](); 
  d_vals6 = new double**[npars](); 
  d_vals81 = new double**[npars](); 
  d_times = new Long64_t*[npars](); 
  d_time = new Long64_t[evs];
  // Foe every parameter
  for(int par = 0; par < npars; ++par){
    d_vals[par] = new double[evs];
    d_times[par] = new Long64_t[evs];
    d_vals6[par] = new double*[evs]();
    d_vals81[par] = new double*[evs]();
    // And for every event, when more than 1 subevent per event
    for(int ev = 0; ev < evs; ++ev){
      d_vals6[par][ev] = new double[6];
      d_vals81[par][ev] = new double[81];
    }
  }

  // Iterate fill holders with ram
  // Set Branches
  for(int par = 0; par < npars; ++par){
    // Set the branches for both input ttrees and output ttree
    // Notice that e.g. k_vptht variable reads 6 values per bunch, but we only save one (a sum)
    if(is6(pars[par]) == true){
        tree->SetBranchAddress(levelX_to_str(pars[par]).c_str(), &vals6[par]);
    }
    else if(is81(pars[par]) == true){
        tree->SetBranchAddress(levelX_to_str(pars[par]).c_str(), &vals81[par]);
    }
    else{
        tree->SetBranchAddress(levelX_to_str(pars[par]).c_str(), &vals[par]);
    }
    // Save the times too, at least for now...
    //tree->Branch((levelX_to_str(par) + "_time").c_str(), &times[par]);
  } 
  tree->SetBranchAddress("time", &time);

  for(int event = 0; event < evs; ++event){
    // Each parameter separately
    tree->GetEntry(event);
    d_time[event] = time;
    for(int par = 0; par < npars; ++par){
      //inFileVec[par]->cd();

      d_times[par][event] = times[par];

      if(is6(pars[par]) == true){
        for(int i = 0; i < 6; ++i){
          d_vals6[par][event][i] = vals6[par][i];
        }
      } 
      else if(is81(pars[par]) == true){
        for(int i = 0; i < 81; ++i){
          d_vals81[par][event][i] = vals81[par][i];
        }
      }
      else{
        d_vals[par][event] = vals[par];
      }
    }
  }
}

void Plotter::GetRMS(){

  std::cout << "gr_mm1_time RMS Y:" << gr_mm1_time->GetRMS(2) << std::endl;
  std::cout << "gr_mm2_time RMS Y:" << gr_mm2_time->GetRMS(2) << std::endl;
  std::cout << "gr_mm3_time RMS Y:" << gr_mm3_time->GetRMS(2) << std::endl;
  std::cout << "gr_mm1trtgtd_time RMS Y:" << gr_mm1trtgtd_time->GetRMS(2) << std::endl;
  std::cout << "gr_mm2trtgtd_time RMS Y:" << gr_mm2trtgtd_time->GetRMS(2) << std::endl;
  std::cout << "gr_mm3trtgtd_time RMS Y:" << gr_mm3trtgtd_time->GetRMS(2) << std::endl;
  std::cout << "gr_trtgtd_time RMS Y:" << gr_trtgtd_time->GetRMS(2) << std::endl;
  std::cout << "gr_mm1yav_time RMS Y:" << gr_mm1yav_time->GetRMS(2) << std::endl;
  std::cout << "gr_mm2yav_time RMS Y:" << gr_mm2yav_time->GetRMS(2) << std::endl;
  std::cout << "gr_mm3yav_time RMS Y:" << gr_mm3yav_time->GetRMS(2) << std::endl;
  std::cout << "gr_mm1xav_time RMS Y:" << gr_mm1xav_time->GetRMS(2) << std::endl;
  std::cout << "gr_mm2xav_time RMS Y:" << gr_mm2xav_time->GetRMS(2) << std::endl;
  std::cout << "gr_mm3xav_time RMS Y:" << gr_mm3xav_time->GetRMS(2) << std::endl;

}

void Plotter::setRatioPlots(int col){
  c_ratio[0] = new TCanvas("cr_x1_x2", "cr_x1_x2", 800, 800);
  c_ratio[1] = new TCanvas("cr_x1_x3", "cr_x1_x3", 800, 800);
  c_ratio[2] = new TCanvas("cr_x2_x3", "cr_x2_x3", 800, 800);

  c_ratio[3] = new TCanvas("cr_y1_y2", "cr_y1_y2", 800, 800);
  c_ratio[4] = new TCanvas("cr_y1_y3", "cr_y1_y3", 800, 800);
  c_ratio[5] = new TCanvas("cr_y2_y3", "cr_y2_y3", 800, 800);

  c_ratio[6] = new TCanvas("cr_x1_y1", "cr_x1_y1", 800, 800);
  c_ratio[7] = new TCanvas("cr_x2_y2", "cr_x2_y2", 800, 800);
  c_ratio[8] = new TCanvas("cr_x3_y3", "cr_x3_y3", 800, 800);

  c_ratio[9]  = new TCanvas("cr_mm1xav_hptgt", "cr_mm1xav_hptgt", 800, 800);
  c_ratio[10] = new TCanvas("cr_mm2xav_hptgt", "cr_mm2xav_hptgt", 800, 800);
  c_ratio[11] = new TCanvas("cr_mm3xav_hptgt", "cr_mm3xav_hptgt", 800, 800);

  c_ratio[12] = new TCanvas("cr_mm1yav_hptgt", "cr_mm1yav_hptgt", 800, 800);
  c_ratio[13] = new TCanvas("cr_mm2yav_hptgt", "cr_mm2yav_hptgt", 800, 800);
  c_ratio[14] = new TCanvas("cr_mm3yav_hptgt", "cr_mm3yav_hptgt", 800, 800);

  c_ratio[15] = new TCanvas("cr_hptgt_vptgt", "cr_hptgt_vptgt", 800, 800);

  c_ratio[16] = new TCanvas("cr_mm1xav_mm1cortrtgtd", "cr_mm1xav_mm1cortrtgtd", 800, 800);
  c_ratio[17] = new TCanvas("cr_mm2xav_mm2cortrtgtd", "cr_mm2xav_mm2cortrtgtd", 800, 800);
  c_ratio[18] = new TCanvas("cr_mm3xav_mm3cortrtgtd", "cr_mm3xav_mm3cortrtgtd", 800, 800);
  c_ratio[19] = new TCanvas("cr_mm1yav_mm1cortrtgtd", "cr_mm1yav_mm1cortrtgtd", 800, 800);
  c_ratio[20] = new TCanvas("cr_mm2yav_mm2cortrtgtd", "cr_mm2yav_mm2cortrtgtd", 800, 800);
  c_ratio[21] = new TCanvas("cr_mm3yav_mm3cortrtgtd", "cr_mm3yav_mm3cortrtgtd", 800, 800);

  th_x1_x2 = new TH2D(("th_x1_x2_" + std::to_string(col)).c_str(),
      ("th_x1_x2_" + std::to_string(col)).c_str(), 100, 0, 2, 100, 0, 2);
  th_x1_x3 = new TH2D(("th_x1_x3_" + std::to_string(col)).c_str(),
      ("th_x1_x3_" + std::to_string(col)).c_str(), 100, 0, 2, 100, 0, 2);
  th_x2_x3 = new TH2D(("th_x2_x3_" + std::to_string(col)).c_str(),
      ("th_x2_x3_" + std::to_string(col)).c_str(), 100, 0, 2, 100, 0, 2);

  th_y1_y2 = new TH2D(("th_y1_y2_" + std::to_string(col)).c_str(),
      ("th_y1_y2_" + std::to_string(col)).c_str(), 100, 0, 2, 100, 0, 2);
  th_y1_y3 = new TH2D(("th_y1_y3_" + std::to_string(col)).c_str(),
      ("th_y1_y3_" + std::to_string(col)).c_str(), 100, 0, 2, 100, 0, 2);
  th_y2_y3 = new TH2D(("th_y2_y3_" + std::to_string(col)).c_str(),
      ("th_y2_y3_" + std::to_string(col)).c_str(), 100, 0, 2, 100, 0, 2);

  th_x1_y1 = new TH2D(("th_x1_y1_" + std::to_string(col)).c_str(),
      ("th_x1_y1_" + std::to_string(col)).c_str(), 100, 0, 2, 100, 0, 2);
  th_x2_y2 = new TH2D(("th_x2_y2_" + std::to_string(col)).c_str(),
      ("th_x2_y2_" + std::to_string(col)).c_str(), 100, 0, 2, 100, 0, 2);
  th_x3_y3 = new TH2D(("th_x3_y3_" + std::to_string(col)).c_str(),
      ("th_x3_y3_" + std::to_string(col)).c_str(), 100, 0, 2, 100, 0, 2);

  th_mm1xav_hptgt = new TH2D(("th_mm1xav_hptgt" + std::to_string(col)).c_str(),
      ("th_mm1xav_hptgt" + std::to_string(col)).c_str(), 100, 0, 2, 100, 0, 2);
  th_mm2xav_hptgt = new TH2D(("th_mm2xav_hptgt" + std::to_string(col)).c_str(),
      ("th_mm2xav_hptgt" + std::to_string(col)).c_str(), 100, 0, 2, 100, 0, 2);
  th_mm3xav_hptgt = new TH2D(("th_mm3xav_hptgt" + std::to_string(col)).c_str(),
      ("th_mm3xav_hptgt" + std::to_string(col)).c_str(), 100, 0, 2, 100, 0, 2);

  th_mm1yav_vptgt = new TH2D(("" + std::to_string(col)).c_str(),
      ("" + std::to_string(col)).c_str(), 100, 0, 2, 100, 0, 2);
  th_mm2yav_vptgt = new TH2D(("th_mm2yav_vptgt" + std::to_string(col)).c_str(),
      ("th_mm2yav_vptgt" + std::to_string(col)).c_str(), 100, 0, 2, 100, 0, 2);
  th_mm3yav_vptgt = new TH2D(("th_mm3yav_vptgt" + std::to_string(col)).c_str(),
      ("th_mm3yav_vptgt" + std::to_string(col)).c_str(), 100, 0, 2, 100, 0, 2);

  th_hptgt_vptgt = new TH2D(("th_hptgt_vptgt" + std::to_string(col)).c_str(),
      ("th_hptgt_vptgt" + std::to_string(col)).c_str(), 100, 0, 2, 100, 0, 2);

  th_mm1xav_mm1cortrtgtd = new TH2D(("th_mm1xav_mm1cortrtgtd" + std::to_string(col)).c_str(),
      ("th_mm1xav_mm1cortrtgtd" + std::to_string(col)).c_str(), 100, 0, 2, 100, 0, 2);
  th_mm2xav_mm2cortrtgtd = new TH2D(("th_mm2xav_mm2cortrtgtd" + std::to_string(col)).c_str(),
      ("th_mm2xav_mm2cortrtgtd" + std::to_string(col)).c_str(), 100, 0, 2, 100, 0, 2);
  th_mm3xav_mm3cortrtgtd = new TH2D(("th_mm3xav_mm3cortrtgtd" + std::to_string(col)).c_str(),
      ("th_mm3xav_mm3cortrtgtd" + std::to_string(col)).c_str(), 100, 0, 2, 100, 0, 2);

  th_mm1yav_mm1cortrtgtd = new TH2D(("th_mm1yav_mm1cortrtgtd" + std::to_string(col)).c_str(),
      ("th_mm1yav_mm1cortrtgtd" + std::to_string(col)).c_str(), 100, 0, 2, 100, 0, 2);
  th_mm2yav_mm2cortrtgtd = new TH2D(("th_mm2yav_mm2cortrtgtd" + std::to_string(col)).c_str(),
      ("th_mm2yav_mm2cortrtgtd" + std::to_string(col)).c_str(), 100, 0, 2, 100, 0, 2);
  th_mm3yav_mm3cortrtgtd = new TH2D(("th_mm3yav_mm3cortrtgtd" + std::to_string(col)).c_str(),
      ("th_mm3yav_mm3cortrtgtd" + std::to_string(col)).c_str(), 100, 0, 2, 100, 0, 2);

  th_x1_x2->SetTitle("Ratio from t_{0};MM1XAV;MM2XAV");
  th_x1_x3->SetTitle("Ratio from t_{0};MM1XAV;MM3XAV");
  th_x2_x3->SetTitle("Ratio from t_{0};MM2XAV;MM3XAV");

  th_y1_y2->SetTitle("Ratio from t_{0};MM1YAV;MM2YAV");
  th_y1_y3->SetTitle("Ratio from t_{0};MM1YAV;MM3YAV");
  th_y2_y3->SetTitle("Ratio from t_{0};MM2YAV;MM3YAV");

  th_x1_y1->SetTitle("Ratio from t_{0};MM1XAV;MM1YAV");
  th_x2_y2->SetTitle("Ratio from t_{0};MM2XAV;MM2YAV");
  th_x3_y3->SetTitle("Ratio from t_{0};MM3XAV;MM3YAV");

  th_mm1xav_hptgt->SetTitle("Ratio from t_{0};MM1XAV;HPTGT");
  th_mm2xav_hptgt->SetTitle("Ratio from t_{0};MM2XAV;HPTGT");
  th_mm3xav_hptgt->SetTitle("Ratio from t_{0};MM3XAV;HPTGT");
  th_mm1yav_vptgt->SetTitle("Ratio from t_{0};MM1YAV;VPTGT");
  th_mm2yav_vptgt->SetTitle("Ratio from t_{0};MM2YAV;VPTGT");
  th_mm3yav_vptgt->SetTitle("Ratio from t_{0};MM3YAV;VPTGT");

  th_hptgt_vptgt->SetTitle("Ratio from t_{0};HPTGT;VPTGT");

  th_mm1xav_mm1cortrtgtd->SetTitle("Ratio from t_{0};MM1XAV;MM1COR/TRTGTD");
  th_mm2xav_mm2cortrtgtd->SetTitle("Ratio from t_{0};MM2XAV;MM2COR/TRTGTD");
  th_mm3xav_mm3cortrtgtd->SetTitle("Ratio from t_{0};MM3XAV;MM3COR/TRTGTD");
  th_mm1yav_mm1cortrtgtd->SetTitle("Ratio from t_{0};MM1YAV;MM1COR/TRTGTD");
  th_mm2yav_mm2cortrtgtd->SetTitle("Ratio from t_{0};MM2YAV;MM2COR/TRTGTD");
  th_mm3yav_mm3cortrtgtd->SetTitle("Ratio from t_{0};MM3YAV;MM3COR/TRTGTD");

  // Set plot styles
  setStyle(th_x1_x2, c_ratio[0], col);
  setStyle(th_x1_x3, c_ratio[1], col);
  setStyle(th_x2_x3, c_ratio[2], col);
  setStyle(th_y1_y2, c_ratio[3], col);
  setStyle(th_y1_y3, c_ratio[4], col);
  setStyle(th_y2_y3, c_ratio[5], col);
  setStyle(th_x1_y1, c_ratio[6], col);
  setStyle(th_x2_y2, c_ratio[7], col);
  setStyle(th_x3_y3, c_ratio[8], col);

  setStyle(th_mm1xav_hptgt, c_ratio[9], col);
  setStyle(th_mm2xav_hptgt, c_ratio[10], col);
  setStyle(th_mm3xav_hptgt, c_ratio[11], col);
  setStyle(th_mm1yav_vptgt, c_ratio[12], col);
  setStyle(th_mm2yav_vptgt, c_ratio[13], col);
  setStyle(th_mm3yav_vptgt, c_ratio[14], col);

  setStyle(th_hptgt_vptgt, c_ratio[15], col);

  setStyle(th_mm1xav_mm1cortrtgtd, c_ratio[16], col);
  setStyle(th_mm2xav_mm2cortrtgtd, c_ratio[17], col);
  setStyle(th_mm3xav_mm3cortrtgtd, c_ratio[18], col);
  setStyle(th_mm1yav_mm1cortrtgtd, c_ratio[19], col);
  setStyle(th_mm2yav_mm2cortrtgtd, c_ratio[20], col);
  setStyle(th_mm3yav_mm3cortrtgtd, c_ratio[21], col);
}

void Plotter::setTimePlots(int col){
  int evs = tree->GetEntries();

  int counter = 0;
  for(int event = 0; event < evs; ++event){
    TDatime time_cor;
    time_cor.Set(d_time[event]/1000);
    TDatime *ttme;
    //ttme->Set(d_time[event]/1000);
    ttme = &time_cor;
    if(timecut(ttme)==false)
      continue;
    counter++;
  }
  // MM1 MM2 MM3
  c_time[0] = new TCanvas("ct_mm1corcal", "ct_mm1corcal", 1200, 800);
  c_time[1] = new TCanvas("ct_mm2corcal", "ct_mm2corcal", 1200, 800);
  c_time[2] = new TCanvas("ct_mm3corcal", "ct_mm3corcal", 1200, 800);

  // MM /trtgtd
  c_time[3] = new TCanvas("ct_mm1corcaltrtgtd", "ct_mm1corcaltrtgtd", 1200, 800);
  c_time[4] = new TCanvas("ct_mm2corcaltrtgtd", "ct_mm2corcaltrtgtd", 1200, 800);
  c_time[5] = new TCanvas("ct_mm3corcaltrtgtd", "ct_mm3corcaltrtgtd", 1200, 800);

  //TRTGTD
  c_time[6] = new TCanvas("ct_trtgtd", "ct_trtgtd", 1200, 800);

  c_time[7]  = new TCanvas("ct_mm1yav", "ct_mm1yav", 1200, 800);
  c_time[8]  = new TCanvas("ct_mm2yav", "ct_mm2yav", 1200, 800);
  c_time[9]  = new TCanvas("ct_mm3yav", "ct_mm3yav", 1200, 800);
  c_time[10] = new TCanvas("ct_mm1xav", "ct_mm1xav", 1200, 800);
  c_time[11] = new TCanvas("ct_mm2xav", "ct_mm2xav", 1200, 800);
  c_time[12] = new TCanvas("ct_mm3xav", "ct_mm3xav", 1200, 800);

  c_time[13] = new TCanvas("ct_nslina", "ct_nslina", 1200, 800);
  c_time[14] = new TCanvas("ct_nslinb", "ct_nslinb", 1200, 800);
  c_time[15] = new TCanvas("ct_nslinc", "ct_nslinc", 1200, 800);
  c_time[16] = new TCanvas("ct_nslind", "ct_nslind", 1200, 800);

  leg = new TLegend(0.1, 0.6, 0.9, 0.9);
  leg->SetNColumns(5);

  gr_mm1_time = new TGraph(counter);
  gr_mm2_time = new TGraph(counter);
  gr_mm3_time = new TGraph(counter);

  gr_mm1trtgtd_time = new TGraph(counter);
  gr_mm2trtgtd_time = new TGraph(counter);
  gr_mm3trtgtd_time = new TGraph(counter);

  gr_trtgtd_time = new TGraph(counter);

  gr_mm1yav_time = new TGraph(counter);
  gr_mm2yav_time = new TGraph(counter);
  gr_mm3yav_time = new TGraph(counter);
  gr_mm1xav_time = new TGraph(counter);
  gr_mm2xav_time = new TGraph(counter);
  gr_mm3xav_time = new TGraph(counter);

  gr_nslina_time = new TGraph(counter);
  gr_nslinb_time = new TGraph(counter);
  gr_nslinc_time = new TGraph(counter);
  gr_nslind_time = new TGraph(counter);

  //std::string title = "Parameter variation from t_{0}";
  std::string title = "Parameter variation";
  //std::string x_title = "Time from t_{0}";
  std::string x_title = "Date";

  gr_mm1_time->SetTitle((title + ";" + x_title + ";MM1COR").c_str());
  gr_mm2_time->SetTitle((title + ";" + x_title + ";MM2COR").c_str());
  gr_mm3_time->SetTitle((title + ";" + x_title + ";MM3COR").c_str());

  gr_mm1trtgtd_time->SetTitle((title + ";" + x_title + ";MM1COR/E12_TRTGTD").c_str());
  gr_mm2trtgtd_time->SetTitle((title + ";" + x_title + ";MM2COR/E12_TRTGTD").c_str());
  gr_mm3trtgtd_time->SetTitle((title + ";" + x_title + ";MM3COR/E12_TRTGTD").c_str());

  gr_trtgtd_time->SetTitle((title + ";" + x_title + ";E12_TRTGTD").c_str());

  gr_mm1yav_time->SetTitle((title + ";" + x_title + ";MM1YAV").c_str());
  gr_mm2yav_time->SetTitle((title + ";" + x_title + ";MM2YAV").c_str());
  gr_mm3yav_time->SetTitle((title + ";" + x_title + ";MM3YAV").c_str());

  gr_mm1xav_time->SetTitle((title + ";" + x_title + ";MM1XAV").c_str());
  gr_mm2xav_time->SetTitle((title + ";" + x_title + ";MM2XAV").c_str());
  gr_mm3xav_time->SetTitle((title + ";" + x_title + ";MM3XAV").c_str());

  gr_nslina_time->SetTitle((title + ";" + x_title + ";NSLINA").c_str());
  gr_nslinb_time->SetTitle((title + ";" + x_title + ";NSLINB").c_str());
  gr_nslinc_time->SetTitle((title + ";" + x_title + ";NSLINC").c_str());
  gr_nslind_time->SetTitle((title + ";" + x_title + ";NSLIND").c_str());

  gr_mm1_time->GetYaxis()->SetRangeUser(0, 100);
  gr_mm2_time->GetYaxis()->SetRangeUser(200, 300);
  gr_mm3_time->GetYaxis()->SetRangeUser(15, 25);
  gr_mm1trtgtd_time->GetYaxis()->SetRangeUser(0, 2);
  gr_mm2trtgtd_time->GetYaxis()->SetRangeUser(0, 10);
  gr_mm3trtgtd_time->GetYaxis()->SetRangeUser(0.3, 1);
  gr_trtgtd_time->GetYaxis()->SetRangeUser(0, 60);

  gr_mm1yav_time->GetYaxis()->SetRangeUser(-2,2);
  gr_mm2yav_time->GetYaxis()->SetRangeUser(-2,2);
  gr_mm3yav_time->GetYaxis()->SetRangeUser(-2,2);
  gr_mm1xav_time->GetYaxis()->SetRangeUser(-2,2);
  gr_mm2xav_time->GetYaxis()->SetRangeUser(-2,2);
  gr_mm3xav_time->GetYaxis()->SetRangeUser(-2,2);

  gr_nslina_time->GetYaxis()->SetRangeUser(-51, -49);
  gr_nslinb_time->GetYaxis()->SetRangeUser(-51, -49);
  gr_nslinc_time->GetYaxis()->SetRangeUser(-51, -49);
  gr_nslind_time->GetYaxis()->SetRangeUser(-51, -49);

  // Set plot styles
  setTGraphStyle(gr_mm1_time, c_time[0], col);
  setTGraphStyle(gr_mm2_time, c_time[1], col);
  setTGraphStyle(gr_mm3_time, c_time[2], col);
  setTGraphStyle(gr_mm1trtgtd_time, c_time[3], col);
  setTGraphStyle(gr_mm2trtgtd_time, c_time[4], col);
  setTGraphStyle(gr_mm3trtgtd_time, c_time[5], col);
  setTGraphStyle(gr_trtgtd_time, c_time[6], col);

  setTGraphStyle(gr_mm1yav_time, c_time[7], col);
  setTGraphStyle(gr_mm2yav_time, c_time[8], col);
  setTGraphStyle(gr_mm3yav_time, c_time[9], col);
  setTGraphStyle(gr_mm1xav_time, c_time[10], col);
  setTGraphStyle(gr_mm2xav_time, c_time[11], col);
  setTGraphStyle(gr_mm3xav_time, c_time[12], col);
  setTGraphStyle(gr_nslina_time, c_time[13], col);
  setTGraphStyle(gr_nslinb_time, c_time[14], col);
  setTGraphStyle(gr_nslinc_time, c_time[15], col);
  setTGraphStyle(gr_nslind_time, c_time[16], col);

  gr_mm1trtgtd_time->GetHistogram()->SetMinimum(0);
  gr_mm2trtgtd_time->GetHistogram()->SetMinimum(0);
  gr_mm3trtgtd_time->GetHistogram()->SetMinimum(0.3);
  gr_mm1trtgtd_time->GetHistogram()->SetMaximum(2);
  gr_mm2trtgtd_time->GetHistogram()->SetMaximum(10);
  gr_mm3trtgtd_time->GetHistogram()->SetMaximum(1);
  gr_mm1_time->GetHistogram()->SetMinimum(0);
  gr_mm2_time->GetHistogram()->SetMinimum(200);
  gr_mm3_time->GetHistogram()->SetMinimum(15);
  gr_mm1_time->GetHistogram()->SetMaximum(100);
  gr_mm2_time->GetHistogram()->SetMaximum(300);
  gr_mm3_time->GetHistogram()->SetMaximum(25);
  gr_mm1yav_time->GetHistogram()->SetMinimum(-2);
  gr_mm2yav_time->GetHistogram()->SetMinimum(-2);
  gr_mm3yav_time->GetHistogram()->SetMinimum(-2);
  gr_mm1xav_time->GetHistogram()->SetMinimum(-2);
  gr_mm2xav_time->GetHistogram()->SetMinimum(-2);
  gr_mm3xav_time->GetHistogram()->SetMinimum(-2);
  gr_mm1yav_time->GetHistogram()->SetMaximum(2);
  gr_mm2yav_time->GetHistogram()->SetMaximum(2);
  gr_mm3yav_time->GetHistogram()->SetMaximum(2);
  gr_mm1xav_time->GetHistogram()->SetMaximum(2);
  gr_mm2xav_time->GetHistogram()->SetMaximum(2);
  gr_mm3xav_time->GetHistogram()->SetMaximum(2);

  gr_nslina_time->GetHistogram()->SetMinimum(-51);
  gr_nslinb_time->GetHistogram()->SetMinimum(-51);
  gr_nslinc_time->GetHistogram()->SetMinimum(-51);
  gr_nslind_time->GetHistogram()->SetMinimum(-51);
  gr_nslina_time->GetHistogram()->SetMaximum(-49);
  gr_nslinb_time->GetHistogram()->SetMaximum(-49);
  gr_nslinc_time->GetHistogram()->SetMaximum(-49);
  gr_nslind_time->GetHistogram()->SetMaximum(-49);

}

void Plotter::setDisplayPlots(int col){
  // MM1 MM2 MM3
  c_display[0] = new TCanvas("cd_mm1xmm1y", "cd_mm1xmm1y", 800, 800);
  c_display[1] = new TCanvas("cd_mm2xmm2y", "cd_mm2xmm2y", 800, 800);
  c_display[2] = new TCanvas("cd_mm3xmm3y", "cd_mm3xmm3y", 800, 800);

  c_display[3] = new TCanvas("cd_mmxhptgt", "cd_mmxhptgt", 1600, 800);
  c_display[4] = new TCanvas("cd_mmyvptgt", "cd_mmyvptgt", 1600, 800);

  c_display[5] = new TCanvas("cd_mm1hptgt", "cd_mm1hptgt", 800, 800);
  c_display[6] = new TCanvas("cd_mm2hptgt", "cd_mm2hptgt", 800, 800);
  c_display[7] = new TCanvas("cd_mm3hptgt", "cd_mm3hptgt", 800, 800);

  c_display[8] = new TCanvas("cd_mm1vptgt", "cd_mm1vptgt", 800, 800);
  c_display[9] = new TCanvas("cd_mm2vptgt", "cd_mm2vptgt", 800, 800);
  c_display[10] = new TCanvas("cd_mm3vptgt", "cd_mm3vptgt", 800, 800);

  c_display[11] = new TCanvas("cd_trtgtdhptgt", "cd_trtgtdhptgt", 800, 800);
  c_display[12] = new TCanvas("cd_trtgtdvptgt", "cd_trtgtdvptgt", 800, 800);

  c_display[13] = new TCanvas("cd_mm1xavhptgt", "cd_mm1xavhptgt", 800, 800);
  c_display[14] = new TCanvas("cd_mm2xavhptgt", "cd_mm2xavhptgt", 800, 800);
  c_display[15] = new TCanvas("cd_mm3xavhptgt", "cd_mm3xavhptgt", 800, 800);

  c_display[16] = new TCanvas("cd_mm1yavvptgt", "cd_mm1yavvptgt", 800, 800);
  c_display[17] = new TCanvas("cd_mm2yavvptgt", "cd_mm2yavvptgt", 800, 800);
  c_display[18] = new TCanvas("cd_mm3yavvptgt", "cd_mm3yavvptgt", 800, 800);
  c_display[19] = new TCanvas("cd_hptgtvptgt", "cd_hptgtvptgt", 800, 800);

  c_display[20] = new TCanvas("cd_mm1xavtrtgtd", "cd_mm1xavtrtgtd", 800, 800);
  c_display[21] = new TCanvas("cd_mm2xavtrtgtd", "cd_mm2xavtrtgtd", 800, 800);
  c_display[22] = new TCanvas("cd_mm3xavtrtgtd", "cd_mm3xavtrtgtd", 800, 800);
  c_display[23] = new TCanvas("cd_mm1yavtrtgtd", "cd_mm1yavtrtgtd", 800, 800);
  c_display[24] = new TCanvas("cd_mm2yavtrtgtd", "cd_mm2yavtrtgtd", 800, 800);
  c_display[25] = new TCanvas("cd_mm3yavtrtgtd", "cd_mm3yavtrtgtd", 800, 800);

  c_display[26] = new TCanvas("cd_mm1xavmm1cortrtgtd", "cd_mm1xavmm1cortrtgtd", 800, 800);
  c_display[27] = new TCanvas("cd_mm2xavmm2cortrtgtd", "cd_mm2xavmm2cortrtgtd", 800, 800);
  c_display[28] = new TCanvas("cd_mm3xavmm3cortrtgtd", "cd_mm3xavmm3cortrtgtd", 800, 800);
  c_display[29] = new TCanvas("cd_mm1yavmm1cortrtgtd", "cd_mm1yavmm1cortrtgtd", 800, 800);
  c_display[30] = new TCanvas("cd_mm2yavmm2cortrtgtd", "cd_mm2yavmm2cortrtgtd", 800, 800);
  c_display[31] = new TCanvas("cd_mm3yavmm3cortrtgtd", "cd_mm3yavmm3cortrtgtd", 800, 800);

  c_display[32] = new TCanvas("cd_mm1xavmm1cor", "cd_mm1xavmm1cor", 800, 800);
  c_display[33] = new TCanvas("cd_mm2xavmm2cor", "cd_mm2xavmm2cor", 800, 800);
  c_display[34] = new TCanvas("cd_mm3xavmm3cor", "cd_mm3xavmm3cor", 800, 800);
  c_display[35] = new TCanvas("cd_mm1yavmm1cor", "cd_mm1yavmm1cor", 800, 800);
  c_display[36] = new TCanvas("cd_mm2yavmm2cor", "cd_mm2yavmm2cor", 800, 800);
  c_display[37] = new TCanvas("cd_mm3yavmm3cor", "cd_mm3yavmm3cor", 800, 800);

  c_display[38] = new TCanvas("cd_nslinamm1yav", "cd_nslinamm1yav", 800, 800);
  c_display[39] = new TCanvas("cd_nslinbmm1yav", "cd_nslinbmm1yav", 800, 800);

  th_disp_x1_y1 = new TH2D(("th_disp_x1_y1" + std::to_string(col)).c_str(),
      ("th_disp_x1_y1" + std::to_string(col)).c_str(),
      100, -2, 2, 100, -2, 2);
  th_disp_x2_y2 = new TH2D(("th_disp_x2_y2" + std::to_string(col)).c_str(),
      ("th_disp_x2_y2" + std::to_string(col)).c_str(),
      100, -2, 2, 100, -2, 2);
  th_disp_x3_y3 = new TH2D(("th_disp_x3_y3" + std::to_string(col)).c_str(),
      ("th_disp_x3_y3" + std::to_string(col)).c_str(),
      100, -2, 2, 100, -2, 2);

  // Muon Monitors vs TRTGTD
  th_disp_x1_trtx = new TH2D(("th_disp_x1_trtx" + std::to_string(col)).c_str(),
      ("th_disp_x1_trtx" + std::to_string(col)).c_str(),
      100, -1.5, 0.5, 100, 0, 51);
  th_disp_x2_trtx = new TH2D(("th_disp_x2_trtx" + std::to_string(col)).c_str(),
      ("th_disp_x2_trtx" + std::to_string(col)).c_str(),
      100, -1.5, 0.5, 100, 0, 51);
  th_disp_x3_trtx = new TH2D(("th_disp_x3_trtx" + std::to_string(col)).c_str(),
      ("th_disp_x3_trtx" + std::to_string(col)).c_str(),
      100, -1.5, 0.5, 100, 0, 51);

  th_disp_y1_trty = new TH2D(("th_disp_y1_trty" + std::to_string(col)).c_str(),
      ("th_disp_x1_trty" + std::to_string(col)).c_str(),
      100, -1.5, 0.5, 100, -40, 40);
  th_disp_y2_trty = new TH2D(("th_disp_y2_trty" + std::to_string(col)).c_str(),
      ("th_disp_y2_trty" + std::to_string(col)).c_str(),
      100, -1.5, 0.5, 100, -40, 40);
  th_disp_y3_trty = new TH2D(("th_disp_y3_trty" + std::to_string(col)).c_str(),
      ("th_disp_y3_trty" + std::to_string(col)).c_str(),
      100, -1.5, 0.5, 100, -40, 40);

  // Muon Monitors vs TARGET
  th_disp_mm1_hptgt= new TH2D(("th_disp_mm1_hptgt" + std::to_string(col)).c_str(),
      ("th_disp_mm1_hptgt" + std::to_string(col)).c_str(),
      100, -1.5, 0.5, 100, 40, 90);
  th_disp_mm2_hptgt= new TH2D(("th_disp_mm2_hptgt" + std::to_string(col)).c_str(),
      ("th_disp_mm2_hptgt" + std::to_string(col)).c_str(),
      100, -1.5, 0.5, 100, 110, 280);
  th_disp_mm3_hptgt= new TH2D(("th_disp_mm3_hptgt" + std::to_string(col)).c_str(),
      ("th_disp_mm3_hptgt" + std::to_string(col)).c_str(),
      100, -1.5, 0.5, 100, 10, 25);

  th_disp_mm1_vptgt= new TH2D(("th_disp_mm1_vptgt" + std::to_string(col)).c_str(),
      ("th_disp_mm1_vptgt" + std::to_string(col)).c_str(),
      100, -1.5, 0.5, 100, 40, 90);
  th_disp_mm2_vptgt= new TH2D(("th_disp_mm2_vptgt" + std::to_string(col)).c_str(),
      ("th_disp_mm2_vptgt" + std::to_string(col)).c_str(),
      100, -1.5, 0.5, 100, 110, 280);
  th_disp_mm3_vptgt= new TH2D(("th_disp_mm3_vptgt" + std::to_string(col)).c_str(),
      ("th_disp_mm3_vptgt" + std::to_string(col)).c_str(),
      100, -1.5, 0.5, 100, 10, 25);

  // TRTGTD VS TARGET
  th_disp_trtgtd_hptgt= new TH2D(("th_disp_trtgtd_hptgt" + std::to_string(col)).c_str(),
      ("th_disp_trtgtd_hptgt" + std::to_string(col)).c_str(),
      100, -1.5, 0.5, 100, 20, 55);
  th_disp_trtgtd_vptgt= new TH2D(("th_disp_trtgtd_vptgt" + std::to_string(col)).c_str(),
      ("th_disp_trtgtd_vptgt" + std::to_string(col)).c_str(),
      100, -1.5, 0.5, 100, 20, 55);

  // Muon Monitors vs TARGET
  th_disp_mm1xav_hptgt= new TH2D(("th_disp_mm1xav_hptgt" + std::to_string(col)).c_str(),
      ("th_disp_mm1xav_hptgt" + std::to_string(col)).c_str(),
      100, -2.0, 2.0, 100, -1.5, 0.5);
  th_disp_mm2xav_hptgt= new TH2D(("th_disp_mm2xav_hptgt" + std::to_string(col)).c_str(),
      ("th_disp_mm2xav_hptgt" + std::to_string(col)).c_str(),
      100, -2.0, 2.0, 100, -1.5, 0.5);
  th_disp_mm3xav_hptgt= new TH2D(("th_disp_mm3xav_hptgt" + std::to_string(col)).c_str(),
      ("th_disp_mm3xav_hptgt" + std::to_string(col)).c_str(),
      100, -2.0, 2.0, 100, -1.5, 0.5);

  th_disp_mm1yav_vptgt= new TH2D(("th_disp_mm1yav_vptgt" + std::to_string(col)).c_str(),
      ("th_disp_mm1yav_vptgt" + std::to_string(col)).c_str(),
      100, -2.0, 2.0, 100, -1.5, 0.5);
  th_disp_mm2yav_vptgt= new TH2D(("th_disp_mm2yav_vptgt" + std::to_string(col)).c_str(),
      ("th_disp_mm2yav_vptgt" + std::to_string(col)).c_str(),
      100, -2.0, 2.0, 100, -1.5, 0.5);
  th_disp_mm3yav_vptgt= new TH2D(("th_disp_mm3yav_vptgt" + std::to_string(col)).c_str(),
      ("th_disp_mm3yav_vptgt" + std::to_string(col)).c_str(),
      100, -2.0, 2.0, 100, -1.5, 0.5);
  th_disp_hptgt_vptgt= new TH2D(("th_disp_hptgt_vptgt" + std::to_string(col)).c_str(),
      ("th_disp_hptgt_vptgt" + std::to_string(col)).c_str(),
      100, -1.5, 0.5, 100, -1.5, 0.5);

  th_disp_mm1xav_trtgtd = new TH2D(("th_disp_mm1xav_trtgtd" + std::to_string(col)).c_str(),
      ("th_disp_mm1xav_trtgtd" + std::to_string(col)).c_str(),
      100, -2.0, 2.0, 100, 10, 55);
  th_disp_mm2xav_trtgtd = new TH2D(("th_disp_mm2xav_trtgtd" + std::to_string(col)).c_str(),
      ("th_disp_mm2xav_trtgtd" + std::to_string(col)).c_str(),
      100, -2.0, 2.0, 100, 10, 55);
  th_disp_mm3xav_trtgtd = new TH2D(("th_disp_mm3xav_trtgtd" + std::to_string(col)).c_str(),
      ("th_disp_mm3xav_trtgtd" + std::to_string(col)).c_str(),
      100, -2.0, 2.0, 100, 10, 55);

  th_disp_mm1yav_trtgtd = new TH2D(("th_disp_mm1yav_trtgtd" + std::to_string(col)).c_str(),
      ("th_disp_mm1yav_trtgtd" + std::to_string(col)).c_str(),
      100, -2.0, 2.0, 100, 10, 55);
  th_disp_mm2yav_trtgtd = new TH2D(("th_disp_mm2yav_trtgtd" + std::to_string(col)).c_str(),
      ("th_disp_mm2yav_trtgtd" + std::to_string(col)).c_str(),
      100, -2.0, 2.0, 100, 10, 55);
  th_disp_mm3yav_trtgtd = new TH2D(("th_disp_mm3yav_trtgtd" + std::to_string(col)).c_str(),
      ("th_disp_mm3yav_trtgtd" + std::to_string(col)).c_str(),
      100, -2.0, 2.0, 100, 10, 55);

  th_disp_mm1xav_mm1cortrtgtd = new TH2D(("th_disp_mm1xav_mm1cortrtgtd" + std::to_string(col)).c_str(),
      ("th_disp_mm1xav_mm1cortrtgtd" + std::to_string(col)).c_str(),
      100, -2.0, 2.0, 100, 0, 2);
  th_disp_mm2xav_mm2cortrtgtd = new TH2D(("th_disp_mm2xav_mm2cortrtgtd" + std::to_string(col)).c_str(),
      ("th_disp_mm2xav_mm2cortrtgtd" + std::to_string(col)).c_str(),
      100, -2.0, 2.0, 100, 0, 10);
  th_disp_mm3xav_mm3cortrtgtd = new TH2D(("th_disp_mm3xav_mm3cortrtgtd" + std::to_string(col)).c_str(),
      ("th_disp_mm3xav_mm3cortrtgtd" + std::to_string(col)).c_str(),
      100, -2.0, 2.0, 100, 0.2, 0.9);

  th_disp_mm1yav_mm1cortrtgtd = new TH2D(("th_disp_mm1yav_mm1cortrtgtd" + std::to_string(col)).c_str(),
      ("th_disp_mm1yav_mm1cortrtgtd" + std::to_string(col)).c_str(),
      100, -2.0, 2.0, 100, 0, 2);
  th_disp_mm2yav_mm2cortrtgtd = new TH2D(("th_disp_mm2yav_mm2cortrtgtd" + std::to_string(col)).c_str(),
      ("th_disp_mm2yav_mm2cortrtgtd" + std::to_string(col)).c_str(),
      100, -2.0, 2.0, 100, 0, 10);
  th_disp_mm3yav_mm3cortrtgtd = new TH2D(("th_disp_mm3yav_mm3cortrtgtd" + std::to_string(col)).c_str(),
      ("th_disp_mm3yav_mm3cortrtgtd" + std::to_string(col)).c_str(),
      100, -2.0, 2.0, 100, 0.2, 0.9);

  th_disp_mm1xav_mm1cor = new TH2D(("th_disp_mm1xav_mm1cor" + std::to_string(col)).c_str(),
      ("th_disp_mm1xav_mm1cor" + std::to_string(col)).c_str(),
      100, -2.0, 2.0, 100, 25, 90);
  th_disp_mm2xav_mm2cor= new TH2D(("th_disp_mm2xav_mm2cor" + std::to_string(col)).c_str(),
      ("th_disp_mm2xav_mm2cor" + std::to_string(col)).c_str(),
      100, -2.0, 2.0, 100, 80, 280);
  th_disp_mm3xav_mm3cor= new TH2D(("th_disp_mm3xav_mm3cor" + std::to_string(col)).c_str(),
      ("th_disp_mm3xav_mm3cor" + std::to_string(col)).c_str(),
      100, -2.0, 2.0, 100, 5, 25);

  th_disp_mm1yav_mm1cor= new TH2D(("th_disp_mm1yav_mm1cor" + std::to_string(col)).c_str(),
      ("th_disp_mm1yav_mm1cor" + std::to_string(col)).c_str(),
      100, -2.0, 2.0, 100, 25, 90);
  th_disp_mm2yav_mm2cor= new TH2D(("th_disp_mm2yav_mm2cor" + std::to_string(col)).c_str(),
      ("th_disp_mm2yav_mm2cor" + std::to_string(col)).c_str(),
      100, -2.0, 2.0, 100, 80, 280);
  th_disp_mm3yav_mm3cor= new TH2D(("th_disp_mm3yav_mm3cor" + std::to_string(col)).c_str(),
      ("th_disp_mm3yav_mm3cor" + std::to_string(col)).c_str(),
      100, -2.0, 2.0, 100, 5, 25);

  th_disp_nslina_mm1yav= new TH2D(("th_disp_nslina_mm1yav" + std::to_string(col)).c_str(),
      ("th_disp_nslina_mm1yav" + std::to_string(col)).c_str(),
      100, -51, -49, 100, -2, 2);
  th_disp_nslinb_mm1yav= new TH2D(("th_disp_nslinb_mm1yav" + std::to_string(col)).c_str(),
      ("th_disp_nslinb_mm1yav" + std::to_string(col)).c_str(),
      100, -51, -49, 100, -2, 2);

  th_disp_x1_y1->SetTitle("MM1XAV vs MM1YAV (in inch);MM1XAV;MM1YAV");
  th_disp_x2_y2->SetTitle("MM2XAV vs MM2YAV (in inch);MM2XAV;MM2YAV");
  th_disp_x3_y3->SetTitle("MM3XAV vs MM3YAV (in inch);MM3XAV;MM3YAV");

  th_disp_x1_trtx->SetTitle("MMXAV vs HPTGT (in mm);HPTGT;MMXAV");
  th_disp_x2_trtx->SetTitle("MMXAV vs HPTGT (in mm);HPTGT;MMXAV");
  th_disp_x3_trtx->SetTitle("MMXAV vs HPTGT (in mm);HPTGT;MMXAV");

  th_disp_y1_trty->SetTitle("MMYAV vs VPTGT (in mm);VPTGT;MMYAV");
  th_disp_y2_trty->SetTitle("MMYAV vs VPTGT (in mm);VPTGT;MMYAV");
  th_disp_y3_trty->SetTitle("MMYAV vs VPTGT (in mm);VPTGT;MMYAV");

  th_disp_mm1_hptgt->SetTitle("MM1COR vs HPTGT;HPTGT;MM1COR");
  th_disp_mm2_hptgt->SetTitle("MM2COR vs HPTGT;HPTGT;MM2COR");
  th_disp_mm3_hptgt->SetTitle("MM3COR vs HPTGT;HPTGT;MM3COR");

  th_disp_mm1_vptgt->SetTitle("MM1COR vs VPTGT;VPTGT;MM1COR");
  th_disp_mm2_vptgt->SetTitle("MM2COR vs VPTGT;VPTGT;MM2COR");
  th_disp_mm3_vptgt->SetTitle("MM3COR vs VPTGT;VPTGT;MM3COR");

  th_disp_trtgtd_hptgt->SetTitle("TRTGTD vs HPTGT;HPTGT;TRTGTD");
  th_disp_trtgtd_vptgt->SetTitle("TRTGTD vs VPTGT;VPTGT;TRTGTD");

  th_disp_mm1xav_hptgt->SetTitle("MM1XAV vs HPTGT;MM1XAV;HPTGT");
  th_disp_mm2xav_hptgt->SetTitle("MM2XAV vs HPTGT;MM2XAV;HPTGT");
  th_disp_mm3xav_hptgt->SetTitle("MM3XAV vs HPTGT;MM3XAV;HPTGT");

  th_disp_mm1yav_vptgt->SetTitle("MM1YAV vs VPTGT;MM1YAV;VPTGT");
  th_disp_mm2yav_vptgt->SetTitle("MM2YAV vs VPTGT;MM2YAV;VPTGT");
  th_disp_mm3yav_vptgt->SetTitle("MM3YAV vs VPTGT;MM3YAV;VPTGT");
  th_disp_hptgt_vptgt->SetTitle("HPTGT vs VPTGT;HPTGT;VPTGT");

  th_disp_mm1xav_trtgtd->SetTitle("MM1XAV vs TRTGTD;MM1XAV;TRTGTD");
  th_disp_mm2xav_trtgtd->SetTitle("MM2XAV vs TRTGTD;MM2XAV;TRTGTD");
  th_disp_mm3xav_trtgtd->SetTitle("MM3XAV vs TRTGTD;MM3XAV;TRTGTD");

  th_disp_mm1yav_trtgtd->SetTitle("MM1YAV vs TRTGTD;MM1YAV;TRTGTD");
  th_disp_mm2yav_trtgtd->SetTitle("MM2YAV vs TRTGTD;MM2YAV;TRTGTD");
  th_disp_mm3yav_trtgtd->SetTitle("MM3YAV vs TRTGTD;MM3YAV;TRTGTD");

  th_disp_mm1xav_mm1cortrtgtd->SetTitle("MM1XAV vs MM1COR/TRTGTD;MM1XAV;MM1COR/TRTGTD");
  th_disp_mm2xav_mm2cortrtgtd->SetTitle("MM2XAV vs MM2COR/TRTGTD;MM2XAV;MM2COR/TRTGTD");
  th_disp_mm3xav_mm3cortrtgtd->SetTitle("MM3XAV vs MM3COR/TRTGTD;MM3XAV;MM3COR/TRTGTD");

  th_disp_mm1yav_mm1cortrtgtd->SetTitle("MM1YAV vs MM1COR/TRTGTD;MM1YAV;MM1COR/TRTGTD");
  th_disp_mm2yav_mm2cortrtgtd->SetTitle("MM2YAV vs MM2COR/TRTGTD;MM2YAV;MM2COR/TRTGTD");
  th_disp_mm3yav_mm3cortrtgtd->SetTitle("MM3YAV vs MM3COR/TRTGTD;MM3YAV;MM3COR/TRTGTD");

  th_disp_mm1xav_mm1cor->SetTitle("MM1XAV vs MM1COR;MM1XAV;MM1COR");
  th_disp_mm2xav_mm2cor->SetTitle("MM2XAV vs MM2COR;MM2XAV;MM2COR");
  th_disp_mm3xav_mm3cor->SetTitle("MM3XAV vs MM3COR;MM3XAV;MM3COR");

  th_disp_mm1yav_mm1cor->SetTitle("MM1YAV vs MM1COR;MM1YAV;MM1COR");
  th_disp_mm2yav_mm2cor->SetTitle("MM2YAV vs MM2COR;MM2YAV;MM2COR");
  th_disp_mm3yav_mm3cor->SetTitle("MM3YAV vs MM3COR;MM3YAV;MM3COR");

  th_disp_nslina_mm1yav->SetTitle("NSLINA vs MM1YAV;NSLINA;MM1YAV");
  th_disp_nslinb_mm1yav->SetTitle("NSLINB vs MM1YAV;NSLINB;MM1YAV");

  setStyle(th_disp_x1_y1, c_display[0], col);
  setStyle(th_disp_x2_y2, c_display[1], col);
  setStyle(th_disp_x3_y3, c_display[2], col);

  setStyle(th_disp_x1_trtx, c_display[3], col);
  setStyle(th_disp_x2_trtx, c_display[3], col+1);
  setStyle(th_disp_x3_trtx, c_display[3], col+2);
  setStyle(th_disp_y1_trty, c_display[4], col);
  setStyle(th_disp_y2_trty, c_display[4], col+1);
  setStyle(th_disp_y3_trty, c_display[4], col+2);

  setStyle(th_disp_mm1_hptgt, c_display[5], col);
  setStyle(th_disp_mm2_hptgt, c_display[6], col);
  setStyle(th_disp_mm3_hptgt, c_display[7], col);
  setStyle(th_disp_mm1_vptgt, c_display[8], col);
  setStyle(th_disp_mm2_vptgt, c_display[9], col);
  setStyle(th_disp_mm3_vptgt, c_display[10], col);

  setStyle(th_disp_trtgtd_hptgt, c_display[11], col);
  setStyle(th_disp_trtgtd_vptgt, c_display[12], col);

  setStyle(th_disp_mm1xav_hptgt, c_display[13], col);
  setStyle(th_disp_mm2xav_hptgt, c_display[14], col);
  setStyle(th_disp_mm3xav_hptgt, c_display[15], col);
  setStyle(th_disp_mm1yav_vptgt, c_display[16], col);
  setStyle(th_disp_mm2yav_vptgt, c_display[17], col);
  setStyle(th_disp_mm3yav_vptgt, c_display[18], col);
  setStyle(th_disp_hptgt_vptgt, c_display[19], col);

  setStyle(th_disp_mm1xav_trtgtd, c_display[20], col);
  setStyle(th_disp_mm2xav_trtgtd, c_display[21], col);
  setStyle(th_disp_mm3xav_trtgtd, c_display[22], col);

  setStyle(th_disp_mm1yav_trtgtd, c_display[23], col);
  setStyle(th_disp_mm2yav_trtgtd, c_display[24], col);
  setStyle(th_disp_mm3yav_trtgtd, c_display[25], col);

  setStyle(th_disp_mm1xav_mm1cortrtgtd, c_display[26], col);
  setStyle(th_disp_mm2xav_mm2cortrtgtd, c_display[27], col);
  setStyle(th_disp_mm3xav_mm3cortrtgtd, c_display[28], col);
  setStyle(th_disp_mm1yav_mm1cortrtgtd, c_display[29], col);
  setStyle(th_disp_mm2yav_mm2cortrtgtd, c_display[30], col);
  setStyle(th_disp_mm3yav_mm3cortrtgtd, c_display[31], col);

  setStyle(th_disp_mm1xav_mm1cor, c_display[32], col);
  setStyle(th_disp_mm2xav_mm2cor, c_display[33], col);
  setStyle(th_disp_mm3xav_mm3cor, c_display[34], col);
  setStyle(th_disp_mm1yav_mm1cor, c_display[35], col);
  setStyle(th_disp_mm2yav_mm2cor, c_display[36], col);
  setStyle(th_disp_mm3yav_mm3cor, c_display[37], col);

  setStyle(th_disp_nslina_mm1yav, c_display[38], col);
  setStyle(th_disp_nslinb_mm1yav, c_display[39], col);
}

void Plotter::fillDisplayPlots(){
  int evs = tree->GetEntries();
  // Muon Monitor Display

  for(int event = 0; event < evs; ++event){
    TDatime time_cor;
    time_cor.Set(d_time[event]/1000);
    TDatime *ttme;
    //ttme->Set(d_time[event]/1000);
    ttme = &time_cor;
    //if(timecut(time_cor.GetHour(), time_cor.GetMinute()) == false)
    if(timecut(ttme)==false)
      continue;

    th_disp_x1_y1->Fill(d_vals[mapKPar(k_mm1xav)][event], d_vals[mapKPar(k_mm1yav)][event]);
    th_disp_x2_y2->Fill(d_vals[mapKPar(k_mm2xav)][event], d_vals[mapKPar(k_mm2yav)][event]);
    th_disp_x3_y3->Fill(d_vals[mapKPar(k_mm3xav)][event], d_vals[mapKPar(k_mm3yav)][event]);

    th_disp_x1_trtx->Fill(sum6(mapKPar(k_hptgt), event)/6, 25.4*d_vals[mapKPar(k_mm1xav)][event]);
    th_disp_x2_trtx->Fill(sum6(mapKPar(k_hptgt), event)/6, 25.4*d_vals[mapKPar(k_mm2xav)][event]);
    th_disp_x3_trtx->Fill(sum6(mapKPar(k_hptgt), event)/6, 25.4*d_vals[mapKPar(k_mm3xav)][event]);

    th_disp_y1_trty->Fill(sum6(mapKPar(k_vptgt), event)/6, 25.4*d_vals[mapKPar(k_mm1yav)][event]);
    th_disp_y2_trty->Fill(sum6(mapKPar(k_vptgt), event)/6, 25.4*d_vals[mapKPar(k_mm2yav)][event]);
    th_disp_y3_trty->Fill(sum6(mapKPar(k_vptgt), event)/6, 25.4*d_vals[mapKPar(k_mm3yav)][event]);

    th_disp_mm1_hptgt->Fill(sum6(mapKPar(k_hptgt), event)/6, d_vals[mapKPar(k_mm1cor_cal)][event]);
    th_disp_mm2_hptgt->Fill(sum6(mapKPar(k_hptgt), event)/6, d_vals[mapKPar(k_mm2cor_cal)][event]);
    th_disp_mm3_hptgt->Fill(sum6(mapKPar(k_hptgt), event)/6, d_vals[mapKPar(k_mm3cor_cal)][event]);

    th_disp_mm1_vptgt->Fill(sum6(mapKPar(k_vptgt), event)/6, d_vals[mapKPar(k_mm1cor_cal)][event]);
    th_disp_mm2_vptgt->Fill(sum6(mapKPar(k_vptgt), event)/6, d_vals[mapKPar(k_mm2cor_cal)][event]);
    th_disp_mm3_vptgt->Fill(sum6(mapKPar(k_vptgt), event)/6, d_vals[mapKPar(k_mm3cor_cal)][event]);

    //th_disp_mm1_hptgt->Fill(sum6(k_hptgt, event)/6, d_vals[k_mm1cor_cal][event]/d_vals[k_e12_trtgtd][event]);
    //th_disp_mm2_hptgt->Fill(sum6(k_hptgt, event)/6, d_vals[k_mm2cor_cal][event]/d_vals[k_e12_trtgtd][event]);
    //th_disp_mm3_hptgt->Fill(sum6(k_hptgt, event)/6, d_vals[k_mm3cor_cal][event]/d_vals[k_e12_trtgtd][event]);

    //th_disp_mm1_vptgt->Fill(sum6(k_vptgt, event)/6, d_vals[k_mm1cor_cal][event]/d_vals[k_e12_trtgtd][event]);
    //th_disp_mm2_vptgt->Fill(sum6(k_vptgt, event)/6, d_vals[k_mm2cor_cal][event]/d_vals[k_e12_trtgtd][event]);
    //th_disp_mm3_vptgt->Fill(sum6(k_vptgt, event)/6, d_vals[k_mm3cor_cal][event]/d_vals[k_e12_trtgtd][event]);
    th_disp_trtgtd_hptgt->Fill(sum6(mapKPar(k_hptgt), event)/6, d_vals[mapKPar(k_e12_trtgtd)][event]);
    th_disp_trtgtd_vptgt->Fill(sum6(mapKPar(k_vptgt), event)/6, d_vals[mapKPar(k_e12_trtgtd)][event]);  

    th_disp_mm1xav_hptgt->Fill(d_vals[mapKPar(k_mm1xav)][event], sum6(mapKPar(k_hptgt), event)/6);
    th_disp_mm2xav_hptgt->Fill(d_vals[mapKPar(k_mm2xav)][event], sum6(mapKPar(k_hptgt), event)/6);
    th_disp_mm3xav_hptgt->Fill(d_vals[mapKPar(k_mm3xav)][event], sum6(mapKPar(k_hptgt), event)/6);
    th_disp_mm1yav_vptgt->Fill(d_vals[mapKPar(k_mm1yav)][event], sum6(mapKPar(k_vptgt), event)/6);
    th_disp_mm2yav_vptgt->Fill(d_vals[mapKPar(k_mm2yav)][event], sum6(mapKPar(k_vptgt), event)/6);
    th_disp_mm3yav_vptgt->Fill(d_vals[mapKPar(k_mm3yav)][event], sum6(mapKPar(k_vptgt), event)/6);
    th_disp_hptgt_vptgt->Fill(sum6(mapKPar(k_hptgt), event)/6, sum6(mapKPar(k_vptgt), event)/6);

  th_disp_mm1xav_trtgtd->Fill(d_vals[mapKPar(k_mm1xav)][event], d_vals[mapKPar(k_e12_trtgtd)][event]);
  th_disp_mm2xav_trtgtd->Fill(d_vals[mapKPar(k_mm2xav)][event], d_vals[mapKPar(k_e12_trtgtd)][event]);
  th_disp_mm3xav_trtgtd->Fill(d_vals[mapKPar(k_mm3xav)][event], d_vals[mapKPar(k_e12_trtgtd)][event]);
  th_disp_mm1yav_trtgtd->Fill(d_vals[mapKPar(k_mm1yav)][event], d_vals[mapKPar(k_e12_trtgtd)][event]);
  th_disp_mm2yav_trtgtd->Fill(d_vals[mapKPar(k_mm2yav)][event], d_vals[mapKPar(k_e12_trtgtd)][event]);
  th_disp_mm3yav_trtgtd->Fill(d_vals[mapKPar(k_mm3yav)][event], d_vals[mapKPar(k_e12_trtgtd)][event]);

  th_disp_mm1xav_mm1cortrtgtd->Fill(d_vals[mapKPar(k_mm1xav)][event], d_vals[mapKPar(k_mm1cor_cal)][event]/d_vals[mapKPar(k_e12_trtgtd)][event]);
  th_disp_mm2xav_mm2cortrtgtd->Fill(d_vals[mapKPar(k_mm2xav)][event], d_vals[mapKPar(k_mm2cor_cal)][event]/d_vals[mapKPar(k_e12_trtgtd)][event]);
  th_disp_mm3xav_mm3cortrtgtd->Fill(d_vals[mapKPar(k_mm3xav)][event], d_vals[mapKPar(k_mm3cor_cal)][event]/d_vals[mapKPar(k_e12_trtgtd)][event]);
  th_disp_mm1yav_mm1cortrtgtd->Fill(d_vals[mapKPar(k_mm1yav)][event], d_vals[mapKPar(k_mm1cor_cal)][event]/d_vals[mapKPar(k_e12_trtgtd)][event]);
  th_disp_mm2yav_mm2cortrtgtd->Fill(d_vals[mapKPar(k_mm2yav)][event], d_vals[mapKPar(k_mm2cor_cal)][event]/d_vals[mapKPar(k_e12_trtgtd)][event]);
  th_disp_mm3yav_mm3cortrtgtd->Fill(d_vals[mapKPar(k_mm3yav)][event], d_vals[mapKPar(k_mm3cor_cal)][event]/d_vals[mapKPar(k_e12_trtgtd)][event]);


  th_disp_mm1xav_mm1cor->Fill(d_vals[mapKPar(k_mm1xav)][event], d_vals[mapKPar(k_mm1cor_cal)][event]);
  th_disp_mm2xav_mm2cor->Fill(d_vals[mapKPar(k_mm2xav)][event], d_vals[mapKPar(k_mm2cor_cal)][event]);
  th_disp_mm3xav_mm3cor->Fill(d_vals[mapKPar(k_mm3xav)][event], d_vals[mapKPar(k_mm3cor_cal)][event]);
  th_disp_mm1yav_mm1cor->Fill(d_vals[mapKPar(k_mm1yav)][event], d_vals[mapKPar(k_mm1cor_cal)][event]);
  th_disp_mm2yav_mm2cor->Fill(d_vals[mapKPar(k_mm2yav)][event], d_vals[mapKPar(k_mm2cor_cal)][event]);
  th_disp_mm3yav_mm3cor->Fill(d_vals[mapKPar(k_mm3yav)][event], d_vals[mapKPar(k_mm3cor_cal)][event]);

  th_disp_nslina_mm1yav->Fill(d_vals[mapKPar(k_nslina)][event], d_vals[mapKPar(k_mm1yav)][event]);
  th_disp_nslinb_mm1yav->Fill(d_vals[mapKPar(k_nslinb)][event], d_vals[mapKPar(k_mm1yav)][event]);
  }

}

void Plotter::drawDisplayPlots(int col, int opt){
  drawTH2D(th_disp_x1_y1, c_display[0], opt);
  drawTH2D(th_disp_x2_y2, c_display[1], opt);
  drawTH2D(th_disp_x3_y3, c_display[2], opt);

  //drawTH2D(th_disp_x1_trtx, c_display[3], opt);
  //drawTH2D(th_disp_x2_trtx, c_display[4], opt);
  //drawTH2D(th_disp_x3_trtx, c_display[5], opt);
  //drawTH2D(th_disp_y1_trty, c_display[6], opt);
  //drawTH2D(th_disp_y2_trty, c_display[7], opt);
  //drawTH2D(th_disp_y3_trty, c_display[8], opt);

  drawTH2D(th_disp_x1_trtx, c_display[3], opt, "");
  drawTH2D(th_disp_x2_trtx, c_display[3], 1, "");
  drawTH2D(th_disp_x3_trtx, c_display[3], 1, "");
  drawTH2D(th_disp_y1_trty, c_display[4], opt, "");
  drawTH2D(th_disp_y2_trty, c_display[4], 1, "");
  drawTH2D(th_disp_y3_trty, c_display[4], 1, "");

  drawTH2D(th_disp_mm1_hptgt, c_display[5],   opt);
  drawTH2D(th_disp_mm2_hptgt, c_display[6],   opt);
  drawTH2D(th_disp_mm3_hptgt, c_display[7],   opt);
  drawTH2D(th_disp_mm1_vptgt, c_display[8],   opt);
  drawTH2D(th_disp_mm2_vptgt, c_display[9],   opt);
  drawTH2D(th_disp_mm3_vptgt, c_display[10],  opt);

  drawTH2D(th_disp_trtgtd_hptgt, c_display[11], opt);
  drawTH2D(th_disp_trtgtd_vptgt, c_display[12], opt);

  drawTH2D(th_disp_mm1xav_hptgt, c_display[13], opt);
  drawTH2D(th_disp_mm2xav_hptgt, c_display[14], opt);
  drawTH2D(th_disp_mm3xav_hptgt, c_display[15], opt);
  drawTH2D(th_disp_mm1yav_vptgt, c_display[16], opt);
  drawTH2D(th_disp_mm2yav_vptgt, c_display[17], opt);
  drawTH2D(th_disp_mm3yav_vptgt, c_display[18], opt);
  drawTH2D(th_disp_hptgt_vptgt, c_display[19], opt);

  drawTH2D(th_disp_mm1xav_trtgtd, c_display[20], opt);
  drawTH2D(th_disp_mm2xav_trtgtd, c_display[21], opt);
  drawTH2D(th_disp_mm3xav_trtgtd, c_display[22], opt);
  drawTH2D(th_disp_mm1yav_trtgtd, c_display[23], opt);
  drawTH2D(th_disp_mm2yav_trtgtd, c_display[24], opt);
  drawTH2D(th_disp_mm3yav_trtgtd, c_display[25], opt);

  drawTH2D(th_disp_mm1xav_mm1cortrtgtd, c_display[26], opt);
  drawTH2D(th_disp_mm2xav_mm2cortrtgtd, c_display[27], opt);
  drawTH2D(th_disp_mm3xav_mm3cortrtgtd, c_display[28], opt);
  drawTH2D(th_disp_mm1yav_mm1cortrtgtd, c_display[29], opt);
  drawTH2D(th_disp_mm2yav_mm2cortrtgtd, c_display[30], opt);
  drawTH2D(th_disp_mm3yav_mm3cortrtgtd, c_display[31], opt);

  drawTH2D(th_disp_mm1xav_mm1cor, c_display[32], opt);
  drawTH2D(th_disp_mm2xav_mm2cor, c_display[33], opt);
  drawTH2D(th_disp_mm3xav_mm3cor, c_display[34], opt);
  drawTH2D(th_disp_mm1yav_mm1cor, c_display[35], opt);
  drawTH2D(th_disp_mm2yav_mm2cor, c_display[36], opt);
  drawTH2D(th_disp_mm3yav_mm3cor, c_display[37], opt);

  drawTH2D(th_disp_nslina_mm1yav, c_display[38], opt);
  drawTH2D(th_disp_nslinb_mm1yav, c_display[39], opt);
}

void Plotter::SetHornMode(int mode){
  hornmode_int = mode;
  if(mode == 0)
    hornmode_str = "_both";
  else if(mode == 1)
    hornmode_str = "_horizontal";
  else if(mode == 2)
    hornmode_str = "_vertical";
  else 
    hornmode_str = "";
}

void Plotter::fillTimePlots(){
  int evs = tree->GetEntries();

  //int counter2 = -1;
  TDatime time_cor0;
  time_cor0.Set(d_time[0]/1000);
  for(int event = 0; event < evs; ++event){
    TDatime time_cor;
    Long64_t time_;
    time_cor.Set(d_time[event]/1000);
    //time_ = time_cor.Convert() - time_cor0.Convert();
    time_ = time_cor.Convert();
    TDatime *ttme;
    ttme = &time_cor;
    //ttme->Set(d_time[event]/1000);
    //if(timecut(time_cor.GetHour(), time_cor.GetMinute()) == false)
    if(timecut(ttme)==false)
      continue;
    //counter2++;

    if(gr_mm1_time->GetN() <= time_counter -1){
      gr_mm1_time->Expand(gr_mm1_time->GetN() + 10000);
      gr_mm2_time->Expand(gr_mm2_time->GetN() + 10000);
      gr_mm3_time->Expand(gr_mm3_time->GetN() + 10000);
      gr_mm1trtgtd_time->Expand(gr_mm1trtgtd_time->GetN() + 10000);
      gr_mm2trtgtd_time->Expand(gr_mm2trtgtd_time->GetN() + 10000);
      gr_mm3trtgtd_time->Expand(gr_mm3trtgtd_time->GetN() + 10000);
      gr_trtgtd_time->Expand(gr_trtgtd_time->GetN() + 10000);
      gr_mm1yav_time->Expand(gr_mm1yav_time->GetN() + 10000);
      gr_mm2yav_time->Expand(gr_mm2yav_time->GetN() + 10000);
      gr_mm3yav_time->Expand(gr_mm3yav_time->GetN() + 10000);
      gr_mm1xav_time->Expand(gr_mm1xav_time->GetN() + 10000);
      gr_mm2xav_time->Expand(gr_mm2xav_time->GetN() + 10000);
      gr_mm3xav_time->Expand(gr_mm3xav_time->GetN() + 10000);

      gr_nslina_time->Expand(gr_nslina_time->GetN() + 10000);
      gr_nslinb_time->Expand(gr_nslinb_time->GetN() + 10000);
      gr_nslinc_time->Expand(gr_nslinc_time->GetN() + 10000);
      gr_nslind_time->Expand(gr_nslind_time->GetN() + 10000);
    }

    gr_mm1_time->SetPoint(time_counter, time_, d_vals[mapKPar(k_mm1cor_cal)][event]);
    gr_mm2_time->SetPoint(time_counter, time_, d_vals[mapKPar(k_mm2cor_cal)][event]);
    gr_mm3_time->SetPoint(time_counter, time_, d_vals[mapKPar(k_mm3cor_cal)][event]);

    gr_mm1trtgtd_time->SetPoint(time_counter, time_, d_vals[mapKPar(k_mm1cor_cal)][event]/d_vals[mapKPar(k_e12_trtgtd)][event]);
    gr_mm2trtgtd_time->SetPoint(time_counter, time_, d_vals[mapKPar(k_mm2cor_cal)][event]/d_vals[mapKPar(k_e12_trtgtd)][event]);
    gr_mm3trtgtd_time->SetPoint(time_counter, time_, d_vals[mapKPar(k_mm3cor_cal)][event]/d_vals[mapKPar(k_e12_trtgtd)][event]);

    gr_trtgtd_time->SetPoint(time_counter, time_, d_vals[mapKPar(k_e12_trtgtd)][event]);

    gr_mm1yav_time->SetPoint(time_counter, time_, d_vals[mapKPar(k_mm1yav)][event]);
    gr_mm2yav_time->SetPoint(time_counter, time_, d_vals[mapKPar(k_mm2yav)][event]);
    gr_mm3yav_time->SetPoint(time_counter, time_, d_vals[mapKPar(k_mm3yav)][event]);
    gr_mm1xav_time->SetPoint(time_counter, time_, d_vals[mapKPar(k_mm1xav)][event]);
    gr_mm2xav_time->SetPoint(time_counter, time_, d_vals[mapKPar(k_mm2xav)][event]);
    gr_mm3xav_time->SetPoint(time_counter, time_, d_vals[mapKPar(k_mm3xav)][event]);

    gr_nslina_time->SetPoint(time_counter, time_, d_vals[mapKPar(k_nslina)][event]);
    gr_nslinb_time->SetPoint(time_counter, time_, d_vals[mapKPar(k_nslinb)][event]);
    gr_nslinc_time->SetPoint(time_counter, time_, d_vals[mapKPar(k_nslinc)][event]);
    gr_nslind_time->SetPoint(time_counter, time_, d_vals[mapKPar(k_nslind)][event]);
    time_counter++;
  }
}
void Plotter::drawTimePlots(int col, int opt){
  drawTGraph(gr_mm1_time, c_time[0], opt);
  drawTGraph(gr_mm2_time, c_time[1], opt);
  drawTGraph(gr_mm3_time, c_time[2], opt);
  drawTGraph(gr_mm1trtgtd_time, c_time[3], opt);
  drawTGraph(gr_mm2trtgtd_time, c_time[4], opt);
  drawTGraph(gr_mm3trtgtd_time, c_time[5], opt);
  drawTGraph(gr_trtgtd_time, c_time[6], opt);

  drawTGraph(gr_mm1yav_time, c_time[7],  opt);
  drawTGraph(gr_mm2yav_time, c_time[8],  opt);
  drawTGraph(gr_mm3yav_time, c_time[9],  opt);
  drawTGraph(gr_mm1xav_time, c_time[10], opt);
  drawTGraph(gr_mm2xav_time, c_time[11], opt);
  drawTGraph(gr_mm3xav_time, c_time[12], opt);

  drawTGraph(gr_nslina_time, c_time[13], opt);
  drawTGraph(gr_nslinb_time, c_time[14], opt);
  drawTGraph(gr_nslinc_time, c_time[15], opt);
  drawTGraph(gr_nslind_time, c_time[16], opt);

  gr_mm1trtgtd_time->GetYaxis()->SetRangeUser(0, 2);
  gr_mm2trtgtd_time->GetYaxis()->SetRangeUser(0, 10);
  gr_mm3trtgtd_time->GetYaxis()->SetRangeUser(0.3, 1);
  gr_mm1trtgtd_time->GetYaxis()->SetLimits(0, 2);
  gr_mm2trtgtd_time->GetYaxis()->SetLimits(0, 10);
  gr_mm3trtgtd_time->GetYaxis()->SetLimits(0.3, 1);

  gr_mm1_time->GetYaxis()->SetRangeUser(0, 100);
  gr_mm2_time->GetYaxis()->SetRangeUser(0, 300);
  gr_mm3_time->GetYaxis()->SetRangeUser(0, 25);
  gr_mm1_time->GetYaxis()->SetLimits(0, 100);
  gr_mm2_time->GetYaxis()->SetLimits(0, 300);
  gr_mm3_time->GetYaxis()->SetLimits(0, 25);

  gr_mm1yav_time->GetYaxis()->SetRangeUser(-2, 2);
  gr_mm2yav_time->GetYaxis()->SetRangeUser(-2, 2);
  gr_mm3yav_time->GetYaxis()->SetRangeUser(-2, 2);
  gr_mm1xav_time->GetYaxis()->SetRangeUser(-2, 2);
  gr_mm2xav_time->GetYaxis()->SetRangeUser(-2, 2);
  gr_mm3xav_time->GetYaxis()->SetRangeUser(-2, 2);

  gr_nslina_time->GetYaxis()->SetRangeUser(-51, -49);
  gr_nslinb_time->GetYaxis()->SetRangeUser(-51, -49);
  gr_nslinc_time->GetYaxis()->SetRangeUser(-51, -49);
  gr_nslind_time->GetYaxis()->SetRangeUser(-51, -49);

  gr_mm1yav_time->GetYaxis()->SetLimits(-2, 2);
  gr_mm2yav_time->GetYaxis()->SetLimits(-2, 2);
  gr_mm3yav_time->GetYaxis()->SetLimits(-2, 2);
  gr_mm1xav_time->GetYaxis()->SetLimits(-2, 2);
  gr_mm2xav_time->GetYaxis()->SetLimits(-2, 2);
  gr_mm3xav_time->GetYaxis()->SetLimits(-2, 2);

  gr_nslina_time->GetYaxis()->SetLimits(-51, -49);
  gr_nslinb_time->GetYaxis()->SetLimits(-51, -49);
  gr_nslinc_time->GetYaxis()->SetLimits(-51, -49);
  gr_nslind_time->GetYaxis()->SetLimits(-51, -49);
}


void Plotter::fillRatioPlots(){

  int evs = tree->GetEntries();

  for(int event = 0; event < evs; ++event){
    TDatime time_cor;
    time_cor.Set(d_time[event]/1000);
    TDatime *ttme;
    ttme = &time_cor;
    if(timecut(ttme)==false)
      continue;

    th_x1_x2->Fill(d_vals[mapKPar(k_mm1xav)][event]/d_vals[mapKPar(k_mm1xav)][0], d_vals[mapKPar(k_mm2xav)][event]/d_vals[mapKPar(k_mm2xav)][0]);
    th_x1_x3->Fill(d_vals[mapKPar(k_mm1xav)][event]/d_vals[mapKPar(k_mm1xav)][0], d_vals[mapKPar(k_mm3xav)][event]/d_vals[mapKPar(k_mm3xav)][0]);
    th_x2_x3->Fill(d_vals[mapKPar(k_mm2xav)][event]/d_vals[mapKPar(k_mm2xav)][0], d_vals[mapKPar(k_mm3xav)][event]/d_vals[mapKPar(k_mm3xav)][0]);
    th_y1_y2->Fill(d_vals[mapKPar(k_mm1yav)][event]/d_vals[mapKPar(k_mm1yav)][0], d_vals[mapKPar(k_mm2yav)][event]/d_vals[mapKPar(k_mm2yav)][0]);
    th_y1_y3->Fill(d_vals[mapKPar(k_mm1yav)][event]/d_vals[mapKPar(k_mm1yav)][0], d_vals[mapKPar(k_mm3yav)][event]/d_vals[mapKPar(k_mm3yav)][0]);
    th_y2_y3->Fill(d_vals[mapKPar(k_mm2yav)][event]/d_vals[mapKPar(k_mm2yav)][0], d_vals[mapKPar(k_mm3yav)][event]/d_vals[mapKPar(k_mm3yav)][0]);
    th_x1_y1->Fill(d_vals[mapKPar(k_mm1xav)][event]/d_vals[mapKPar(k_mm1xav)][0], d_vals[mapKPar(k_mm1yav)][event]/d_vals[mapKPar(k_mm1yav)][0]);
    th_x2_y2->Fill(d_vals[mapKPar(k_mm2xav)][event]/d_vals[mapKPar(k_mm2xav)][0], d_vals[mapKPar(k_mm2yav)][event]/d_vals[mapKPar(k_mm2yav)][0]);
    th_x3_y3->Fill(d_vals[mapKPar(k_mm3xav)][event]/d_vals[mapKPar(k_mm3xav)][0], d_vals[mapKPar(k_mm3yav)][event]/d_vals[mapKPar(k_mm3yav)][0]);

    th_mm1xav_hptgt->Fill(d_vals[mapKPar(k_mm1xav)][event]/d_vals[mapKPar(k_mm1xav)][0], (sum6(mapKPar(k_hptgt), event)/6)/(sum6(mapKPar(k_hptgt), 0)/6));
    th_mm2xav_hptgt->Fill(d_vals[mapKPar(k_mm2xav)][event]/d_vals[mapKPar(k_mm2xav)][0], (sum6(mapKPar(k_hptgt), event)/6)/(sum6(mapKPar(k_hptgt), 0)/6));
    th_mm3xav_hptgt->Fill(d_vals[mapKPar(k_mm3xav)][event]/d_vals[mapKPar(k_mm3xav)][0], (sum6(mapKPar(k_hptgt), event)/6)/(sum6(mapKPar(k_hptgt), 0)/6));
    th_mm1yav_vptgt->Fill(d_vals[mapKPar(k_mm1yav)][event]/d_vals[mapKPar(k_mm1yav)][0], (sum6(mapKPar(k_vptgt), event)/6)/(sum6(mapKPar(k_vptgt), 0)/6));
    th_mm2yav_vptgt->Fill(d_vals[mapKPar(k_mm2yav)][event]/d_vals[mapKPar(k_mm2yav)][0], (sum6(mapKPar(k_vptgt), event)/6)/(sum6(mapKPar(k_vptgt), 0)/6));
    th_mm3yav_vptgt->Fill(d_vals[mapKPar(k_mm3yav)][event]/d_vals[mapKPar(k_mm3yav)][0], (sum6(mapKPar(k_vptgt), event)/6)/(sum6(mapKPar(k_vptgt), 0)/6));

    th_hptgt_vptgt->Fill((sum6(mapKPar(k_hptgt), event)/6)/(sum6(mapKPar(k_hptgt), 0)/6), (sum6(mapKPar(k_vptgt), event)/6)/(sum6(mapKPar(k_vptgt), 0)/6));


    th_mm1xav_mm1cortrtgtd->Fill(d_vals[mapKPar(k_mm1xav)][event]/d_vals[mapKPar(k_mm1xav)][0], (d_vals[mapKPar(k_mm1cor_cal)][event]/d_vals[mapKPar(k_e12_trtgtd)][event])/(d_vals[mapKPar(k_mm1cor_cal)][0]/d_vals[mapKPar(k_e12_trtgtd)][0]));
    th_mm2xav_mm2cortrtgtd->Fill(d_vals[mapKPar(k_mm2xav)][event]/d_vals[mapKPar(k_mm2xav)][0], (d_vals[mapKPar(k_mm2cor_cal)][event]/d_vals[mapKPar(k_e12_trtgtd)][event])/(d_vals[mapKPar(k_mm2cor_cal)][0]/d_vals[mapKPar(k_e12_trtgtd)][0]));
    th_mm3xav_mm3cortrtgtd->Fill(d_vals[mapKPar(k_mm3xav)][event]/d_vals[mapKPar(k_mm3xav)][0], (d_vals[mapKPar(k_mm3cor_cal)][event]/d_vals[mapKPar(k_e12_trtgtd)][event])/(d_vals[mapKPar(k_mm3cor_cal)][0]/d_vals[mapKPar(k_e12_trtgtd)][0]));
    th_mm1yav_mm1cortrtgtd->Fill(d_vals[mapKPar(k_mm1yav)][event]/d_vals[mapKPar(k_mm1yav)][0], (d_vals[mapKPar(k_mm1cor_cal)][event]/d_vals[mapKPar(k_e12_trtgtd)][event])/(d_vals[mapKPar(k_mm1cor_cal)][0]/d_vals[mapKPar(k_e12_trtgtd)][0]));
    th_mm2yav_mm2cortrtgtd->Fill(d_vals[mapKPar(k_mm2yav)][event]/d_vals[mapKPar(k_mm2yav)][0], (d_vals[mapKPar(k_mm2cor_cal)][event]/d_vals[mapKPar(k_e12_trtgtd)][event])/(d_vals[mapKPar(k_mm2cor_cal)][0]/d_vals[mapKPar(k_e12_trtgtd)][0]));
    th_mm3yav_mm3cortrtgtd->Fill(d_vals[mapKPar(k_mm3yav)][event]/d_vals[mapKPar(k_mm3yav)][0], (d_vals[mapKPar(k_mm3cor_cal)][event]/d_vals[mapKPar(k_e12_trtgtd)][event])/(d_vals[mapKPar(k_mm3cor_cal)][0]/d_vals[mapKPar(k_e12_trtgtd)][0]));

  }
}

void Plotter::drawRatioPlots(int col, int opt){

  drawTH2D(th_x1_x2, c_ratio[0], opt);
  drawTH2D(th_x1_x3, c_ratio[1], opt);
  drawTH2D(th_x2_x3, c_ratio[2], opt);
  drawTH2D(th_y1_y2, c_ratio[3], opt);
  drawTH2D(th_y1_y3, c_ratio[4], opt);
  drawTH2D(th_y2_y3, c_ratio[5], opt);
  drawTH2D(th_x1_y1, c_ratio[6], opt);
  drawTH2D(th_x2_y2, c_ratio[7], opt);
  drawTH2D(th_x3_y3, c_ratio[8], opt);

  drawTH2D(th_mm1xav_hptgt, c_ratio[9],  opt);
  drawTH2D(th_mm2xav_hptgt, c_ratio[10], opt);
  drawTH2D(th_mm3xav_hptgt, c_ratio[11], opt);
  drawTH2D(th_mm1yav_vptgt, c_ratio[12], opt);
  drawTH2D(th_mm2yav_vptgt, c_ratio[13], opt);
  drawTH2D(th_mm3yav_vptgt, c_ratio[14], opt);

  drawTH2D(th_hptgt_vptgt, c_ratio[15], opt);

  drawTH2D(th_mm1xav_mm1cortrtgtd, c_ratio[16], opt);
  drawTH2D(th_mm2xav_mm2cortrtgtd, c_ratio[17], opt);
  drawTH2D(th_mm3xav_mm3cortrtgtd, c_ratio[18], opt);
  drawTH2D(th_mm1yav_mm1cortrtgtd, c_ratio[19], opt);
  drawTH2D(th_mm2yav_mm2cortrtgtd, c_ratio[20], opt);
  drawTH2D(th_mm3yav_mm3cortrtgtd, c_ratio[21], opt);
}

void Plotter::saveRatioPlots(){
  // Save plots
  saveTCanvas(c_ratio[0], "ratios_x1_x2");
  saveTCanvas(c_ratio[1], "ratios_x1_x3");
  saveTCanvas(c_ratio[2], "ratios_x2_x3");
  saveTCanvas(c_ratio[3], "ratios_y1_y2");
  saveTCanvas(c_ratio[4], "ratios_y1_y3");
  saveTCanvas(c_ratio[5], "ratios_y2_y3");
  saveTCanvas(c_ratio[6], "ratios_x1_y1");
  saveTCanvas(c_ratio[7], "ratios_x2_y2");
  saveTCanvas(c_ratio[8], "ratios_x3_y3");

  saveTCanvas(c_ratio[9], "ratios_mm1xav_hptgt");
  saveTCanvas(c_ratio[10], "ratios_mm2xav_hptgt");
  saveTCanvas(c_ratio[11], "ratios_mm3xav_hptgt");
  saveTCanvas(c_ratio[12], "ratios_mm1yav_vptgt");
  saveTCanvas(c_ratio[13], "ratios_mm2yav_vptgt");
  saveTCanvas(c_ratio[14], "ratios_mm3yav_vptgt");
  saveTCanvas(c_ratio[15], "ratios_vptgt_hptgt");

  saveTCanvas(c_ratio[16], "ratios_mm1xav_mm1cortrtgtd");
  saveTCanvas(c_ratio[17], "ratios_mm2xav_mm2cortrtgtd");
  saveTCanvas(c_ratio[18], "ratios_mm3xav_mm3cortrtgtd");
  saveTCanvas(c_ratio[19], "ratios_mm1yav_mm1cortrtgtd");
  saveTCanvas(c_ratio[20], "ratios_mm2yav_mm2cortrtgtd");
  saveTCanvas(c_ratio[21], "ratios_mm3yav_mm3cortrtgtd");
}

void Plotter::saveTimePlots(){
  // Save plots
  saveTCanvas(c_time[0], "times_mm1corcal");
  saveTCanvas(c_time[1], "times_mm2corcal");
  saveTCanvas(c_time[2], "times_mm3corcal");
  //c_time[3]->cd();
  //leg->Draw("SAME");
  saveTCanvas(c_time[3], "times_mm1corcaltrtgtd");
  saveTCanvas(c_time[4], "times_mm2corcaltrtgtd");
  saveTCanvas(c_time[5], "times_mm3corcaltrtgtd");
  saveTCanvas(c_time[6], "times_trtgtd");

  saveTCanvas(c_time[7],  "times_mm1yav");
  saveTCanvas(c_time[8],  "times_mm2yav");
  saveTCanvas(c_time[9],  "times_mm3yav");
  saveTCanvas(c_time[10], "times_mm1xav");
  saveTCanvas(c_time[11], "times_mm2xav");
  saveTCanvas(c_time[12], "times_mm3xav");

  saveTCanvas(c_time[13], "times_nslina");
  saveTCanvas(c_time[14], "times_nslinb");
  saveTCanvas(c_time[15], "times_nslinc");
  saveTCanvas(c_time[16], "times_nslind");
}

void Plotter::saveDisplayPlots(){
  // Save plots
  saveTCanvas(c_display[0], "display_mm1xav_mm1yav");
  saveTCanvas(c_display[1], "display_mm2xav_mm2yav");
  saveTCanvas(c_display[2], "display_mm3xav_mm3yav");

  saveTCanvas(c_display[3], "display_mmxav_hptgt");
  saveTCanvas(c_display[4], "display_mmyav_vptgt");

  //saveTCanvas(c_display[3], "display_mm1xav_hptgt");
  //saveTCanvas(c_display[4], "display_mm2xav_hptgt");
  //saveTCanvas(c_display[5], "display_mm3xav_hptgt");

  //saveTCanvas(c_display[6], "display_mm1yav_vptgt");
  //saveTCanvas(c_display[7], "display_mm2yav_vptgt");
  //saveTCanvas(c_display[8], "display_mm3yav_vptgt");

  saveTCanvas(c_display[5], "display_mm1_hptgt");
  saveTCanvas(c_display[6], "display_mm2_hptgt");
  saveTCanvas(c_display[7], "display_mm3_hptgt");

  saveTCanvas(c_display[8], "display_mm1_vptgt");
  saveTCanvas(c_display[9], "display_mm2_vptgt");
  saveTCanvas(c_display[10], "display_mm3_vptgt");

  saveTCanvas(c_display[11], "display_trtgtd_hptgt");
  saveTCanvas(c_display[12], "display_trtgtd_vptgt");

  saveTCanvas(c_display[13], "display_mm1xav_hptgt");
  saveTCanvas(c_display[14], "display_mm2xav_hptgt");
  saveTCanvas(c_display[15], "display_mm3xav_hptgt");
  saveTCanvas(c_display[16], "display_mm1yav_vptgt");
  saveTCanvas(c_display[17], "display_mm2yav_vptgt");
  saveTCanvas(c_display[18], "display_mm3yav_vptgt");

  saveTCanvas(c_display[19], "display_vptgt_hptgt");

  saveTCanvas(c_display[20], "display_mm1xav_trtgtd");
  saveTCanvas(c_display[21], "display_mm2xav_trtgtd");
  saveTCanvas(c_display[22], "display_mm3xav_trtgtd");
  saveTCanvas(c_display[23], "display_mm1yav_trtgtd");
  saveTCanvas(c_display[24], "display_mm2yav_trtgtd");
  saveTCanvas(c_display[25], "display_mm3yav_trtgtd");

  saveTCanvas(c_display[26], "display_mm1xav_mm1cortrtgtd");
  saveTCanvas(c_display[27], "display_mm2xav_mm2cortrtgtd");
  saveTCanvas(c_display[28], "display_mm3xav_mm3cortrtgtd");
  saveTCanvas(c_display[29], "display_mm1yav_mm1cortrtgtd");
  saveTCanvas(c_display[30], "display_mm2yav_mm2cortrtgtd");
  saveTCanvas(c_display[31], "display_mm3yav_mm3cortrtgtd");

  saveTCanvas(c_display[32], "display_mm1xav_mm1cor");
  saveTCanvas(c_display[33], "display_mm2xav_mm2cor");
  saveTCanvas(c_display[34], "display_mm3xav_mm3cor");
  saveTCanvas(c_display[35], "display_mm1yav_mm1cor");
  saveTCanvas(c_display[36], "display_mm2yav_mm2cor");
  saveTCanvas(c_display[37], "display_mm3yav_mm3cor");

  saveTCanvas(c_display[38], "display_nslina_mm1yav");
  saveTCanvas(c_display[39], "display_nslinb_mm1yav");
}

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

//double Plotter::GetCovariance(int par1, int par2){
//  int n = ttree->GetEvents();
//  double mean1 = 0;
//  double mean2 = 0;
//  double cov = 0;
//  for(int event = 0; i < n; ++event){
//    mean1 += d_vals[par1][event]
//    mean2 += d_vals[par2][event]
//  }
//  mean1 /= n;
//  mean2 /= n;
//
//  for(int event = 0; i < n; ++event){
//    cov += (d_vals[par1][event] - mean1)*(d_vals[par2][event] - mean2);
//  }
//  cov /= (n -1);
//  return cov;
//}
//
//double Plotter::GetCorrelation(int par1, int par2){
//  int n = ttree->GetEvents();
//  double mean1 = 0;
//  double mean2 = 0;
//  double var1 = 0;
//  double var2 = 0;
//  double cov = 0;
//
//  // Calculate means
//  for(int event = 0; i < n; ++event){
//    mean1 += d_vals[par1][event]
//    mean2 += d_vals[par2][event]
//  }
//  mean1 /= n;
//  mean2 /= n;
//
//  // Calculate variances & covariance
//  for(int event = 0; i < n; ++event){
//  // Calculate variances
//    var1 += (d_vals[par1][event] - mean1)*(d_vals[par1][event] - mean1);
//    var2 += (d_vals[par2][event] - mean2)*(d_vals[par2][event] - mean2);
//    cov += (d_vals[par1][event] - mean1)*(d_vals[par2][event] - mean2);
//  }
//  var1 /= (n -1);
//  var2 /= (n -1);
//  cov /= (n -1);
//
//  return cov/std::sqrt(var1*var2);
//}

void Plotter::GetMeansSDCorrelationCovariance(int par1, int par2, double &m1,
    double &m2, double &sd1, double &sd2, double &cor, double &cov){
  int n = tree->GetEntries();
  double mean1 = 0;
  double mean2 = 0;
  double var1 = 0;
  double var2 = 0;
  cov = 0;
  cor = 0;
  int i_par1 = mapKPar(par1);
  int i_par2 = mapKPar(par2);

  // Calculate means
  for(int event = 0; event < n; ++event){
    if(is6(par1) == true){
      double sum6 = 0;
      for(int i = 0; i < 6; ++i)
        sum6 += d_vals6[i_par1][event][i];
      sum6 /= 6;
      mean1 += sum6;
    }
    else
      mean1 += d_vals[i_par1][event];

    if(is6(par2) == true){
      double sum6 = 0;
      for(int i = 0; i < 6; ++i)
        sum6 += d_vals6[i_par2][event][i];
      sum6 /= 6;
      mean2 += sum6;
    }
    else
      mean2 += d_vals[i_par2][event];
  }
  mean1 /= n;
  mean2 /= n;

  // Save means
  m1 = mean1;
  m2 = mean2;

  // Calculate variances & covariance
  for(int event = 0; event < n; ++event){
    double av1 = 0;
    double av2 = 0;
  // Calculate variances
    if(is6(par1) == true){
      for(int i = 0; i < 6; ++i)
        av1 += d_vals6[i_par1][event][i];
      av1 /= 6;
      var1 += (av1 - mean1)*(av1 - mean1);
    }
    else
      var1 += (d_vals[i_par1][event] - mean1)*(d_vals[i_par1][event] - mean1);

    if(is6(par2) == true){
      for(int i = 0; i < 6; ++i)
        av2 += d_vals6[i_par2][event][i];
      av2 /= 6;
      var2 += (av2 - mean2)*(av2 - mean2);
    }
    else
      var2 += (d_vals[i_par2][event] - mean2)*(d_vals[i_par2][event] - mean2);

    if(is6(par1) == true && is6(par2) == true)
      cov += (av1 - mean1)*(av2 - mean2);
    else if(is6(par1) == true && is6(par2) == false)
      cov += (av1 - mean1)*(d_vals[i_par2][event] - mean2);
    else if(is6(par1) == false && is6(par2) == true)
      cov += (d_vals[i_par1][event] - mean1)*(av2 - mean2);
    else
      cov += (d_vals[i_par1][event] - mean1)*(d_vals[i_par2][event] - mean2);
  }

  var1 /= n;
  var2 /= n;

  // Save SDs
  sd1 = std::sqrt(var1);
  sd2 = std::sqrt(var2);

  // Save Covariane and Correlation
  cov /= n;
  cor = cov/std::sqrt(var1*var2);

  //if(par1 == par2){
  //  std::cout << "############## " << levelX_to_str(par1) << " VS " << levelX_to_str(par2) << "############" << std::endl;
  //  std::cout << "MEANS   : " << mean1 << " :: " << mean2 << std::endl;
  //  std::cout << "SD      : " << sd1 << " :: " << sd2 << std::endl;
  //  std::cout << "VARS    : " << var1 << " :: " << var2 << std::endl;
  //  std::cout << "COV     : " << cov << std::endl;
  //  std::cout << "COR     : " << cor << std::endl;
  //}
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
//  TH2D *intensity3 = new TH2D("Intensity3", "", 9, 1, 10, 9, 1, 10);

  TH2D *int1_temp = new TH2D("Intensity1tm", "", 9, 1, 10, 9, 1, 10);
  TH2D *int2_temp = new TH2D("Intensity2tm", "", 9, 1, 10, 9, 1, 10);
//  TH2D *int3_temp = new TH2D("Intensity3tm", "", 9, 1, 10, 9, 1, 10);

  TGraph *tg_centr_x1 = new TGraph(nEntries);
  TGraph *tg_centr_x2 = new TGraph(nEntries);
//  TGraph *tg_centr_x3 = new TGraph(nEntries);
  //tg_centr_x1->SetTitle(";Date;Horizontal centroid 1");
  //tg_centr_x2->SetTitle(";Date;Horizontal centroid 2");
  //tg_centr_x3->SetTitle(";Date;Horizontal centroid 3");
  tg_centr_x1->SetTitle(";Date;MM1sig_integral/TRTGTD");
  tg_centr_x2->SetTitle(";Date;MM2sig_integral/TRTGTD");
//  tg_centr_x3->SetTitle(";Date;MM3sig_integral/TRTGTD");

  TGraph *tg_centr_y1 = new TGraph(nEntries);
  TGraph *tg_centr_y2 = new TGraph(nEntries);
//  TGraph *tg_centr_y3 = new TGraph(nEntries);
  tg_centr_y1->SetTitle(";Date;Vertical centroid 1");
  tg_centr_y2->SetTitle(";Date;Vertical centroid 2");
//  tg_centr_y3->SetTitle(";Date;Vertical centroid 3");

  TH1D *cr1d = new TH1D("c11", "", 100, 1.7, 1.8);
  TH1D *cr2d = new TH1D("c22", "", 100, 5.4, 5.6);
//  TH1D *cr3d = new TH1D("c33", "", 100, 0.38, 0.5);

  //double frs_centroid_x1, frs_centroid_x2, frs_centroid_x3, frs_centroid_y1, frs_centroid_y2, frs_centroid_y3;
  //Long64_t ttime;
  for(int i = 0; i < nEntries; ++i){
    tree->GetEntry(i);

    // Clear integral histograms
    int1_temp->Reset();
    int2_temp->Reset();
//    int3_temp->Reset();

    for(int j = 0; j < 9; ++j){
      for(int k = 0; k < 9; ++k){

        double bin = intensity1->GetBinContent(j+1, k+1);
        bin += vals81[k_mm1_sig_calib][mappy[j][k]]; 
        //bin += vals81[k_mma1ds][mappy[j][k]]; 
        intensity1->SetBinContent(j+1, k+1, bin);
        int1_temp->SetBinContent(j+1, k+1, vals81[k_mm1_sig_calib][mappy[j][k]]);
        //int1_temp->SetBinContent(j+1, k+1, vals81[k_mma1ds][mappy[j][k]]);

        bin = intensity2->GetBinContent(j+1, k+1);
        bin += vals81[k_mm2_sig_calib][mappy[j][k]]; 
        //bin += vals81[k_mma2ds][mappy[j][k]]; 
        intensity2->SetBinContent(j+1, k+1, bin);
        int2_temp->SetBinContent(j+1, k+1, vals81[k_mm2_sig_calib][mappy[j][k]]);
        //int2_temp->SetBinContent(j+1, k+1, vals81[k_mma2ds][mappy[j][k]]);

//        bin = intensity3->GetBinContent(j+1, k+1);
//        //bin += vals81[k_mm3_sig_calib][mappy[j][k]]; 
//        bin += vals81[k_mma2ds][mappy[j][k]]; 
//        intensity3->SetBinContent(j+1, k+1, bin);
//        //int3_temp->SetBinContent(j+1, k+1, vals81[k_mm3_sig_calib][mappy[j][k]]);
//        int3_temp->SetBinContent(j+1, k+1, vals81[k_mma2ds][mappy[j][k]]);
      }
    }
    //double centroid_x1, centroid_x2, centroid_x3, centroid_y1, centroid_y2, centroid_y3;
    double centroid_x1, centroid_x2, centroid_y1, centroid_y2;

    centroid_x1 = (4.5 - int1_temp->GetMean(1))*11.5;
    centroid_x2 = (4.5 - int2_temp->GetMean(1))*11.5;
//    centroid_x3 = (4.5 - int3_temp->GetMean(1))*11.5;

    //centroid_x1 = (4.5 - int1_temp->GetMean())*11.5;
    //centroid_x2 = (4.5 - int2_temp->GetMean())*11.5;
    //centroid_x3 = (4.5 - int3_temp->GetMean())*11.5;

    centroid_x1 = int1_temp->Integral();
    centroid_x2 = int2_temp->Integral();
//    centroid_x3 = int3_temp->Integral();

    centroid_y1 = (4.5 - int1_temp->GetMean(2))*11.5;
    centroid_y2 = (4.5 - int2_temp->GetMean(2))*11.5;
//    centroid_y3 = (4.5 - int3_temp->GetMean(2))*11.5;

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
//    tg_centr_x3->SetPoint(i, time, centroid_x3/(vals[k_e12_trtgtd]));
    tg_centr_y1->SetPoint(i, time, centroid_y1/(vals[k_e12_trtgtd]));
    tg_centr_y2->SetPoint(i, time, centroid_y2/(vals[k_e12_trtgtd]));
//    tg_centr_y3->SetPoint(i, time, centroid_y3/(vals[k_e12_trtgtd]));



    cr1d->Fill(centroid_x1/(vals[k_e12_trtgtd]));
    cr2d->Fill(centroid_x2/(vals[k_e12_trtgtd]));
//    cr3d->Fill(centroid_x3/(vals[k_e12_trtgtd]));

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
//  TCanvas *c33= new TCanvas("c33", "c33", 1200, 1200);
////  cr3d->SetTitle(";MM3COR/TRTGTD;");
//  cr3d->Draw();
//  c33->SaveAs("c33.png");

  TCanvas *c1_int = new TCanvas("c1", "c1", 1200, 1200);
  TCanvas *c2_int = new TCanvas("c2", "c2", 1200, 1200);
//  TCanvas *c3_int = new TCanvas("c3", "c3", 1200, 1200);


  intensity1->GetXaxis()->CenterLabels();
  intensity1->GetYaxis()->CenterLabels();
  intensity2->GetXaxis()->CenterLabels();
  intensity2->GetYaxis()->CenterLabels();
//  intensity3->GetXaxis()->CenterLabels();
//  intensity3->GetYaxis()->CenterLabels();

  c1_int->cd();
  intensity1->Draw("col");
  TColor::InvertPalette();
  c1_int->SaveAs("int1.png");

  c2_int->cd();
  intensity2->Draw("col");
  c2_int->SaveAs("int2.png");

//  c3_int->cd();
//  intensity3->Draw("col");
//  c3_int->SaveAs("int3.png");


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

//  TCanvas *canv_centroid_x3 = new TCanvas("centr_x3", "centr_x3", 1200, 800);
//  canv_centroid_x3->cd();
//  setTGraphTimeStyle(tg_centr_x3);
//  tg_centr_x3->Draw("ap");
//  canv_centroid_x3->SaveAs("canv_centroid_x3.png");

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

  //gStyle->SetLineWidth(3);

  //gr->SetMarkerStyle(20);
  //gr->GetXaxis()->SetTimeDisplay(1);
  gr->GetXaxis()->SetTimeDisplay(1);
  gr->GetXaxis()->SetTimeFormat("%d/%m/%y");
  gr->GetXaxis()->SetTimeOffset(0,"cst");
  gr->GetXaxis()->SetNdivisions(-505);
}

// Sets the branch addresses
void Plotter::setBranches(TTree* tree_set){
  int npars = pars.size();

  // Use the default TTree if not parsed through
  if(tree_set == NULL)
    tree_set = tree;

  // Set all the branches!
  for (int pars = 0; pars < npars ; pars++) {
    if(is81(mapKPar(pars)) == true){
      tree_set->SetBranchAddress(levelX_to_str(mapKPar(pars)).c_str(), &vals81[pars]);
    }
    else if(is6(mapKPar(pars)) == true){
      tree_set->SetBranchAddress(levelX_to_str(mapKPar(pars)).c_str(), &vals6[pars]);
    }
    else{
      tree_set->SetBranchAddress(levelX_to_str(mapKPar(pars)).c_str(), &vals[pars]);
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

void Plotter::saveTCanvas(TCanvas *c, std::string name){
  c->SaveAs((name + hornmode_str + ".png").c_str());
  c->SaveAs((name + hornmode_str + ".pdf").c_str());
  c->SaveAs((name + hornmode_str + ".root").c_str());
}


void Plotter::setTGraphStyle(TGraph *plot, TCanvas *c, int col){

  //gStyle->SetLineWidth(3);
    // TCanvas stylystics
    c->SetLeftMargin(0.15);
    c->SetBottomMargin(0.15);
    c->SetRightMargin(0.1);
    c->SetTopMargin(0.1);

    plot->GetXaxis()->SetTimeDisplay(1);
    plot->GetXaxis()->SetTimeFormat("%d/%m/%y");
    //plot->GetXaxis()->SetTimeFormat("%M");
    plot->GetXaxis()->SetTimeOffset(0,"cst");
    plot->GetXaxis()->SetNdivisions(-505);

    plot->SetLineColor(kBlack);
    plot->SetFillColor(13);
    plot->SetMarkerColor(col);
    plot->SetMarkerStyle(7);
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
}





void Plotter::setStyle(TH2D *plot, TCanvas *c, int col){

  gStyle->SetOptStat(0);
  //gStyle->SetLineWidth(3);

    c->SetLeftMargin(0.15);
    c->SetBottomMargin(0.15);
    c->SetRightMargin(0.14);
    c->SetTopMargin(0.1);

    plot->SetLineColor(col);
    plot->SetFillColor(13);
    plot->SetMarkerColor(col);
    plot->SetMarkerStyle(6);
    plot->SetMarkerSize(0.5);
    plot->SetTitleSize(0.07);

    plot->GetXaxis()->SetTitleFont(132);
    plot->GetXaxis()->SetTitleSize(0.07);
    plot->GetXaxis()->SetTitleOffset(1);
    plot->GetXaxis()->SetLabelFont(132);
    plot->GetXaxis()->SetLabelSize(0.05);
    plot->GetXaxis()->SetNdivisions(1004, false);

    plot->GetZaxis()->SetMaxDigits(2);

    plot->GetYaxis()->SetTitleFont(132);
    plot->GetYaxis()->SetTitleSize(0.07);
    plot->GetYaxis()->SetTitleOffset(1);
    plot->GetYaxis()->SetLabelFont(132);
    plot->GetYaxis()->SetLabelSize(0.05);
    plot->GetYaxis()->SetNdivisions(1004, false);
    //TLine l;
    //l.DrawLine(gPad->GetUxmin(), gPad->GetUymax(), gPad->GetUxmax(), gPad->GetUymax());
    //l.DrawLine(gPad->GetUxmax(), gPad->GetUymin(), gPad->GetUxmax(), gPad->GetUymax());
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

    if(i == k_mm1_sig_calib || i == k_mm2_sig_calib || i == k_mm3_sig_calib){
      return true;
    }
    //if(i == k_mma1ds || i == k_mma2ds){ // || i == k_mma3ds ){
//  //      || i == k_mma1pd || i == k_mma2pd || i == k_mma3pd ){
    //  return true;
    //}
    //if(i == k_mma1ds || i == k_mma2ds || i == k_mma3ds 
    //    || i == k_mma1pd || i == k_mma2pd || i == k_mma3pd 
    //    || i == k_mm1_sig_calib || i == k_mm2_sig_calib || i == k_mm3_sig_calib){
    //  return true;
    //}
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

void Plotter::drawTH2D(TH2D* th, TCanvas* c, int opt, std::string drawopt){
  c->cd();
  if(opt == 0)
    th->Draw(drawopt.c_str());
  else if(opt == 1)
    th->Draw((drawopt + " SAME").c_str());

  auto pal = (TPaletteAxis*)th->GetListOfFunctions()->FindObject("palette");
  pal->GetAxis()->SetMaxDigits(3);

}

void Plotter::drawTGraph(TGraph* th, TCanvas* c, int opt){
  c->cd();
  if(opt == 0)
    th->Draw("ap");
  else if(opt == 1){
    th->Draw("SAME p");
  }
}

double Plotter::sum6(int type, int event ){
  double sum = 0.0;
  for (int i = 0; i < 6; ++i) sum += d_vals6[type][event][i];
  return sum;
}

//bool Plotter::timecut(int hour, int minute){
bool Plotter::timecut(TDatime *ttme){
  if(hornmode_int == 999)
    return true;
  TDatime tme;
  tme.Set(ttme->GetYear(), ttme->GetMonth(), ttme->GetDay(), ttme->GetHour(),
      ttme->GetMinute(), ttme->GetSecond());

  TDatime start;
  TDatime finish;

  TDatime start_cut1;
  TDatime start_cut2;
  TDatime finish_cut1;
  TDatime finish_cut2;

  start_cut1.Set(2019, 7, 3, 11, 56, 25);
  finish_cut1.Set(2019, 7, 3, 11, 57, 3);

  start_cut2.Set(2019, 7, 3, 11, 59, 5);
  finish_cut2.Set(2019, 7, 3, 11, 59, 50);

  // Both Scan
  if(hornmode_int == 0){
    start.Set(2019, 7, 3, 11, 55, 20);
    finish.Set(2019, 7, 3, 12, 11, 30);

    if(tme <= start)
      return false;
    if(tme >= finish)
      return false;

    if((tme >= start_cut1)&&(tme <= finish_cut1))
      return false;
    if((tme >= start_cut2)&&(tme <= finish_cut2))
      return false;
  }

  // Horizontal scan
  if(hornmode_int == 1){
    start.Set(2019, 7, 3, 11, 55, 20);
    finish.Set(2019, 7, 3, 12, 2, 0 );

    if(tme <= start)
      return false;
    if(tme >= finish)
      return false;

    if((tme >= start_cut1)&&(tme <= finish_cut1))
      return false;
    if((tme >= start_cut2)&&(tme <= finish_cut2))
      return false;
  }

  // Vertical scan
  if(hornmode_int == 2){
    start.Set(2019, 7, 3, 12, 3, 0);
    finish.Set(2019, 7, 3, 12, 11, 30);
    if(tme <= start)
      return false;
    if(tme >= finish)
      return false;
    //if((hour <= 12) && (minute < 3)){
    //  return false;
    //}
    //if((hour < 12)){
    //  return false;
    //}
    //if((hour >= 12) && (minute > 13)){
    //  return false;
    //}
  }

  return true;
}

// Need to pre-assign this and make a nicer switch statement intstead... or
// something else, this cannot stay
int Plotter::mapKPar(int k_par){
  int npars = pars.size();
  int parr = -999;
  for(int par = 0; par < npars; ++par){
    if(pars[par] == k_par){
      parr = par;
    }
  }
  return parr;
}
