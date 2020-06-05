#ifndef _plotter_h_
#define _plotter_h_

#include <iostream>
#include <vector>
#include "structs.h"
#include <string.h>

#include "TStyle.h"
#include "TH2F.h"
#include "TH2D.h"
#include "TH1D.h"
#include "TGraph.h"


#include "TPad.h"
#include "TLine.h"
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TDatime.h"
#include "structs.h"

class Plotter{
  public:
    // Makes a comparison between variables in one TTree
    Plotter(TTree *treeIn);

    // Makes a comparison between different TTrees
    Plotter(std::vector< TTree* > treeVecIn);
    void SavePlots();

  private:
    // Sets the TTree branches
    void setBranches(TTree *tree_set = NULL);

    void setMinMax(TTree *tree_set = NULL, bool clear = false);

    void setStyle(TH2D *plot, TCanvas *c);

    void fillHistograms(TTree *tree_set = NULL, bool first = true);

    void init();

    // TTree inputs
    std::vector< TTree* > treeVec;
    TTree* tree;

    // Branch value holder
    double vals81[k_nLevel0][81];
    double vals6[k_nLevel0][6];
    double vals[k_nLevel0];
    Long64_t time;

    // This will hold min and max values, for more automated plots...
    double vals81_minmax[k_nLevel0][81][2];
    double vals6_minmax[k_nLevel0][6][2];
    double vals_minmax[k_nLevel0][2];
    Long64_t time_minmax[2];


    // Are we doing comparison between TTrees?
    bool ttreeComparison;

    // Normal plot
    bool normal_plot_b[k_nLevel0][k_nLevel0];
    TH2D* normal_plot[k_nLevel0][k_nLevel0];
    TCanvas* normal_canv[k_nLevel0][k_nLevel0];

    // Percentage plot
    bool percentage_plot_b[k_nLevel0][k_nLevel0];
    TH2D* percentage_plot[k_nLevel0][k_nLevel0];
    TCanvas* percentage_canv[k_nLevel0][k_nLevel0];

    // time plot
    bool time_plot_b[k_nLevel0][k_nLevel0];
    TGraph* time_plot[k_nLevel0][k_nLevel0];
    TCanvas* time_canv[k_nLevel0][k_nLevel0];

};

#endif /* ifndef _plotter_h_ */
