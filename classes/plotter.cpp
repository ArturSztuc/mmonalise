#include "plotter.h"

Plotter::Plotter(TTree *treeIn){
  std::cout << "Producing plots for " << treeIn->GetName() << std::endl;

  // We are not doing comparison between different TTrees
  ttreeComparison = false;

  tree = treeIn;

  setBranches();
  setMinMax(NULL, true);
  init();
  fillHistograms();
};

// Sets the branch addresses
void Plotter::setBranches(TTree* tree_set){

  // Use the default TTree if not parsed through
  if(tree_set == NULL)
    tree_set = tree;

  // Set all the branches!
  for (int param = 0; param < k_nLevel0; param++) {
    if(is81(param) == true){
        tree_set->SetBranchAddress(level0_to_str(param).c_str(), &vals81[param]);
    }
    else{
        tree_set->SetBranchAddress(level0_to_str(param).c_str(), &vals[param]);
    }
    //if(param == k_vptgt || param == k_hptgt || param == k_vp121 || param == k_hp121){
    //    tree_set->SetBranchAddress(level0_to_str(param).c_str(), &vals6[param]);
    //}
    //else if(param == k_mma1ds || param == k_mma2ds || param == k_mma3ds){
    //    tree_set->SetBranchAddress(level0_to_str(param).c_str(), &vals81[param]);
    //}
    //else{
    //    tree_set->SetBranchAddress(level0_to_str(param).c_str(), &vals[param]);
    //}
  }
  tree_set->SetBranchAddress("time", &time);
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
    for (int par = 0; par < k_nLevel0; par++) {
      vals_minmax[par][0] = 10e100;
      vals_minmax[par][1] = -10e100;
      vals[par] = -10e100;
      for (int par6 = 0; par6 < 6; par6++) {
        vals6_minmax[par][par6][0] = 10e100;
        vals6_minmax[par][par6][1] = -10e100;
        vals6[par][par6] = -10e100;
      }
      for (int par81 = 0; par81 < 81; par81++) {
        vals81_minmax[par][par81][0] = 10e100;
        vals81[par][par81] = -10e100;
      }
    }
  }

  // Iterate through every entry and find the biggest/smallest value for each parameter!
  int nEntries = tree->GetEntries();
  for (int entry = 0; entry < nEntries; entry++) {
    tree->GetEntry(entry);
    if(time < time_minmax[0]) time_minmax[0] = time;
    if(time > time_minmax[1]) time_minmax[1] = time;
    for (int par = 0; par < k_nLevel0; par++) {
      if(vals[par] < vals_minmax[par][0]) vals_minmax[par][0] = vals[par];
      if(vals[par] > vals_minmax[par][1]) vals_minmax[par][1] = vals[par];
      for (int par6 = 0; par6 < 6; par6++) {
        if(vals6[par][par6] < vals6_minmax[par][par6][0]) vals6_minmax[par][par6][0] = vals6[par][par6];
        if(vals6[par][par6] > vals6_minmax[par][par6][1]) vals6_minmax[par][par6][1] = vals6[par][par6];
      }
      for (int par81 = 0; par81 < 81; par81++) {
        if(vals81[par][par81] < vals81_minmax[par][par81][0]) vals81_minmax[par][par81][0] = vals81[par][par81];
        if(vals81[par][par81] > vals81_minmax[par][par81][1]) vals81_minmax[par][par81][1] = vals81[par][par81];
      }
    }
  }

  // Printer
  std::cout << "time_minmax[0]" << ": " << time_minmax[0] << std::endl;
  std::cout << "time_minmax[1]" << ": " << time_minmax[1] << std::endl;
  for (int par = 0; par < k_nLevel0; par++) {
    std::cout << "vals_minmax[" << level0_to_str(par) << "][0]" << ": " << vals_minmax[par][0] << std::endl;
    std::cout << "vals_minmax[" << level0_to_str(par) << "][1]" << ": " << vals_minmax[par][1] << std::endl;
  //  for (int par6 = 0; par6 < 6; par6++) {
  //    std::cout << "vals6_minmax[par][par6][0]" << ": " << vals6_minmax[par][par6][0] << std::endl;
  //    std::cout << "vals6_minmax[par][par6][1]" << ": " << vals6_minmax[par][par6][1] << std::endl;
  //  }
  //  for (int par81 = 0; par81 < 81; par81++) {
  //    std::cout << "vals81_minmax[par][par81][0]" << ": " << vals81_minmax[par][par81][0] << std::endl;
  //    std::cout << "vals81_minmax[par][par81][1]" << ": " << vals81_minmax[par][par81][1] << std::endl;
  //  }
  }
  std::cout << "SET MIN: " << 10E10 << "  SET MAX: " << -10E10 << std::endl;
}

// TODO: 
// * Need to split the bool setup to be done independently of TCanvas/TH2D
//   setup. The TC/TH setup should be "cleared" each time, since we might be
//   otherwise overwriting things when we are doing comparison between TTrees
void Plotter::init(){
  // Compare between mm's
  vals_minmax[k_mm1xav][0] = 0; vals_minmax[k_mm1xav][1] = 1;
  vals_minmax[k_mm2xav][0] = 0; vals_minmax[k_mm2xav][1] = 1;
  vals_minmax[k_mm3xav][0] = 1; vals_minmax[k_mm3xav][1] = 2;

  vals_minmax[k_mm1yav][0] = -2; vals_minmax[k_mm1yav][1] = 0;
  vals_minmax[k_mm2yav][0] = 0; vals_minmax[k_mm2yav][1] = 1;
  vals_minmax[k_mm3yav][0] = 0; vals_minmax[k_mm3yav][1] = 2;

  normal_plot_b[k_mm1xav][k_mm2xav] = true;
  normal_plot_b[k_mm1xav][k_mm3xav] = true;
  normal_plot_b[k_mm1yav][k_mm2yav] = true;
  normal_plot_b[k_mm1yav][k_mm3yav] = true;

  // Compare between mm xy
  normal_plot_b[k_mm1xav][k_mm1yav] = true;
  normal_plot_b[k_mm2xav][k_mm2yav] = true;

  for (int i = 0; i < k_nLevel0; ++i) {
    for (int j = 0; j < k_nLevel0; ++j) {
      if(normal_plot_b[i][j] == true){
        std::string axi = level0_to_str(i);
        std::string axj = level0_to_str(j);
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
    for (int i = 0; i < k_nLevel0; ++i) {
      for (int j = 0; j < k_nLevel0; ++j) {
        if(normal_plot_b[i][j] == true){
          normal_plot[i][j]->Fill(vals[i], vals[j]);
        }
      }
    }
  }

  std::cout << "Plottin!" << std::endl;
  // And draw the histograms!
  for (int i = 0; i < k_nLevel0; ++i) {
    for (int j = 0; j < k_nLevel0; ++j) {
      if(normal_plot_b[i][j] == true){
        std::cout << "Eureka!" << std::endl;
        normal_canv[i][j]->cd();
        if(first){
          setStyle(normal_plot[i][j], normal_canv[i][j]);
          normal_plot[i][j]->Draw("p");
        }
        else
          normal_plot[i][j]->Draw("p same");
        // FIXME XXX TODO:
        //  * This should be in a separate PUBLIC function!!
        normal_canv[i][j]->SaveAs((level0_to_str(i) + "_" + level0_to_str(j) + ".png").c_str());
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
    plot->SetMarkerSize(1);
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

bool Plotter::is81(int i){

    if(i == k_mma1ds || i == k_mma2ds || i == k_mma3ds 
        || i == k_mma1pd || i == k_mma2pd || i == k_mma3pd 
        || i == k_mm1_sig_calib || i == k_mm2_sig_calib || i == k_mm3_sig_calib){
      return true;
    }
    else
      return false;
}
