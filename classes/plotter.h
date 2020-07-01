#ifndef _plotter_h_
#define _plotter_h_

#include <iostream>
#include <vector>
#include "structs.h"
#include <string.h>
#include <cmath>

#include "TStyle.h"
#include "TH2F.h"
#include "TH2D.h"
#include "TH1D.h"
#include "TGraph.h"
#include "TGraphErrors.h"


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

    // Simple plotter for all variables
    void PlotBare();

    void PlotBeamPositionAtTarget();

    void PlotThermoC();

    void SavePlots();

    // Sets the TTree branches
    void setBranches(TTree *tree_set = NULL);

    void JennyPlots();
    void JennyPlots2(std::string fout);

  private:

    void setMinMax(TTree *tree_set = NULL, bool clear = false);

    void setStyle(TH2D *plot, TCanvas *c);

    void fillHistograms(TTree *tree_set = NULL, bool first = true);

    void fillTimePlots();

    void setTGraphTimeStyle(TGraph *gr);

    void setTimePlots();

    void timeBatchedTTree(std::string fout);

    void init();

    bool is81(int i);

    bool is6(int i, int mode = 0);
  
    bool isLvl0(int lev){ return lev < k_nLevel0; };

    double getMean(std::vector< double > pars);

    double getVariance(std::vector< double > pars, double mean);

    void fillDouble(std::vector< double > v_pars, double *d_pars);

    double *getDouble(std::vector< double > v_pars);

    double *getDouble(std::vector< int > v_pars);


    bool timeBatcher(int minutes);

    std::string levelX_to_str(int lev);


    std::string getString(int i, int mode);

    int getNPars(int mode);

    // TTree inputs
    std::vector< TTree* > treeVec;
    TTree* tree;

    // Branch value holder
    double vals81[k_nLevel][81];
    double vals6[k_nLevel][6];
    double vals[k_nLevel];
    Long64_t time;

    double m_vals81[k_nLevel][81];
    double m_vals6[k_nLevel][6];
    double m_vals[k_nLevel];

    double sd_vals81[k_nLevel][81];
    double sd_vals6[k_nLevel][6];
    double sd_vals[k_nLevel];

    int nEvents;


    // Event time batcher
    int itime_batch;
    TDatime time_converted;

    // This will hold min and max values, for more automated plots...
    double vals81_minmax[k_nLevel][81][2];
    double vals6_minmax[k_nLevel][6][2];
    double vals_minmax[k_nLevel][2];
    Long64_t time_minmax[2];

    // Are we doing comparison between TTrees?
    bool ttreeComparison;

    // Normal plot
    bool normal_plot_b[k_nLevel][k_nLevel];
    TH2D* normal_plot[k_nLevel][k_nLevel];
    TCanvas* normal_canv[k_nLevel][k_nLevel];

    // Percentage plot
    bool percentage_plot_b[k_nLevel][k_nLevel];
    TH2D* percentage_plot[k_nLevel][k_nLevel];
    TCanvas* percentage_canv[k_nLevel][k_nLevel];

    // time plot
    bool time_plot_b[k_nLevel];
    TGraph* time_plot[k_nLevel];
    TCanvas* time_canv[k_nLevel];


};

#endif /* ifndef _plotter_h_ */
