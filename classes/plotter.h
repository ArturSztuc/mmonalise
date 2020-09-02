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
#include "TGaxis.h"
#include "TLegend.h"


#include "TPad.h"
#include "TLine.h"
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TDatime.h"
#include "structs.h"

#include "datareader.h"

class Plotter{
  public:
    // Makes a comparison between variables in one TTree
    Plotter(TTree *treeIn);

    // Makes a comparison between user-defined variables in one TTree
    Plotter(TTree *treeIn, std::vector< int > _pars);

    // Simple plotter for all variables
    void PlotBare();

    void PlotBeamPositionAtTarget();

    void PlotThermoC();

    void SavePlots();

    // Sets the TTree branches
    void setBranches(TTree *tree_set = NULL);

    void drawTH2D(TH2D* th, TCanvas* c, int opt = 0);
    void drawTGraph(TGraph* th, TCanvas* c, int opt = 0);

    void JennyPlots();
    void JennyPlots2(std::string fout);

    void fillRAM();
    void clearRAM();
    void setTTree(TTree *treeIn);

    void setRatioPlots(int col = 1);
    void fillRatioPlots();
    void drawRatioPlots(int col = 1, int opt = 0);
    void saveRatioPlots();

    void setTimePlots(int col = 1);
    void fillTimePlots();
    void drawTimePlots(int col = 1, int opt = 0);
    void saveTimePlots();

    void setDisplayPlots(int col = 1);
    void fillDisplayPlots();
    void drawDisplayPlots(int col = 1, int opt = 0);
    void saveDisplayPlots();

    void SetHornMode(int mode);

//    double GetCovariance(int par1, int par2);
//    double GetCorrelation(int par1, int par3);
    void GetMeansSDCorrelationCovariance(int par1, int par2, double &m1,
        double &m2, double &sd1, double &sd2, double &cor, double &cov);

    Long64_t GetTime(int event){ return d_time[event]; };

  private:
    bool isUserPar;
    std::vector< int > pars;

    void setMinMax(TTree *tree_set = NULL, bool clear = false);

    void setStyle(TH2D *plot, TCanvas *c, int col = 1);
    void setTGraphStyle(TGraph *plot, TCanvas *c, int col = 1);

    void fillHistograms(TTree *tree_set = NULL, bool first = true);

    void saveTCanvas(TCanvas *c, std::string name);

    void setTGraphTimeStyle(TGraph *gr);


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

    double sum6(int type, int event );


    std::string getString(int i, int mode);

    int getNPars(int mode);

    TCanvas* c_ratio[16];

    TCanvas* c_time[13];

    TCanvas* c_display[20];

    std::string hornmode_str;
    int hornmode_int;

    //bool timecut(int hour, int minute);
    bool timecut(TDatime *tme);

    //TGraph *gr_mm1_time[10];
    //TGraph *gr_mm2_time[10];
    //TGraph *gr_mm3_time[10];

    //TGraph *gr_mm1trtgtd_time[10];
    //TGraph *gr_mm2trtgtd_time[10];
    //TGraph *gr_mm3trtgtd_time[10];

    //TGraph *gr_trtgtd_time[10];

    // TTree inputs
    std::vector< TTree* > treeVec;
    TTree* tree;

    // Fills the variables onto ram
    //double *d_vals[k_nLevel];
    //double *d_vals6[k_nLevel][6];
    //double *d_vals81[k_nLevel][81];
    //Long64_t *d_times[k_nLevel];
    double **d_vals;
    double ***d_vals6;
    double ***d_vals81;
    Long64_t **d_times;
    Long64_t *d_time;

    // maps k_parameter to pars -> iterator
    int mapKPar(int k_par);

    // Branch value holder
    double vals81[k_nLevel][81];
    double vals6[k_nLevel][6];
    double vals[k_nLevel];
    Long64_t time;
    Long64_t times[k_nLevel];

    double m_vals81[k_nLevel][81];
    double m_vals6[k_nLevel][6];
    double m_vals[k_nLevel];

    double sd_vals81[k_nLevel][81];
    double sd_vals6[k_nLevel][6];
    double sd_vals[k_nLevel];

    double *d_covmat[k_nLevel][k_nLevel];

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


    // Legend for the time plots
    TLegend *leg;





    // AAAAAL THE PLOTS!
  TH2D *th_disp_x1_y1;
  TH2D *th_disp_x2_y2;
  TH2D *th_disp_x3_y3;

  // Muon Monitors vs TRTGTD
  TH2D *th_disp_x1_trtx;
  TH2D *th_disp_x2_trtx;
  TH2D *th_disp_x3_trtx;

  TH2D *th_disp_y1_trty;
  TH2D *th_disp_y2_trty;
  TH2D *th_disp_y3_trty;

  // Muon Monitors vs TARGET
  TH2D *th_disp_mm1_hptgt;
  TH2D *th_disp_mm2_hptgt;
  TH2D *th_disp_mm3_hptgt;

  TH2D *th_disp_mm1_vptgt;
  TH2D *th_disp_mm2_vptgt;
  TH2D *th_disp_mm3_vptgt;

  // TRTGTD VS TARGET
  TH2D *th_disp_trtgtd_hptgt;
  TH2D *th_disp_trtgtd_vptgt;

  // Muon Monitors vs TARGET
  TH2D *th_disp_mm1xav_hptgt;
  TH2D *th_disp_mm2xav_hptgt;
  TH2D *th_disp_mm3xav_hptgt;

  TH2D *th_disp_mm1yav_vptgt;
  TH2D *th_disp_mm2yav_vptgt;
  TH2D *th_disp_mm3yav_vptgt;
  TH2D *th_disp_hptgt_vptgt;


  TGraph *gr_mm1_time;
  TGraph *gr_mm2_time;
  TGraph *gr_mm3_time;

  TGraph *gr_mm1trtgtd_time;
  TGraph *gr_mm2trtgtd_time;
  TGraph *gr_mm3trtgtd_time;

  TGraph *gr_trtgtd_time;

  TGraph *gr_mm1yav_time;
  TGraph *gr_mm2yav_time;
  TGraph *gr_mm3yav_time;
  TGraph *gr_mm1xav_time;
  TGraph *gr_mm2xav_time;
  TGraph *gr_mm3xav_time;

  TH2D *th_x1_x2;
  TH2D *th_x1_x3;
  TH2D *th_x2_x3;

  TH2D *th_y1_y2;
  TH2D *th_y1_y3;
  TH2D *th_y2_y3;

  TH2D *th_x1_y1;
  TH2D *th_x2_y2;
  TH2D *th_x3_y3;

  TH2D *th_mm1xav_hptgt;
  TH2D *th_mm2xav_hptgt;
  TH2D *th_mm3xav_hptgt;

  TH2D *th_mm1yav_vptgt;
  TH2D *th_mm2yav_vptgt;
  TH2D *th_mm3yav_vptgt;

  TH2D *th_hptgt_vptgt;
};

#endif /* ifndef _plotter_h_ */
