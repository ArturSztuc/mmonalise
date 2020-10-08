/*
 *
 *
 *
 *
 * TODO:
 *  - Need to do some checks for when parameters are missing from the ttree!
 *    Maybe with an option to ignore missing branches? At least foce the code
 *    to  send the right errors with assert or something like that
 *
 *
 *
 *
 *
 *
 *
 *
 */
#ifndef _plot_h_
#define _plot_h_

#include <iostream>
#include <vector>
#include <algorithm>
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
#include "TPaletteAxis.h"


#include "TPad.h"
#include "TLine.h"
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TDatime.h"
#include "structs.h"

#include "datareader.h"

struct Cut{
  Cut(ACNET*_par, double _mean, double _rms)
    : par(_par), 
    mean(_mean),
    rms(_rms)
  {}

  bool pass(double val){
    if( val > mean + (3*rms))
      return false;
    else if (val < mean - (3*rms))
      return false;
    else
      return true;
  }

  ACNET *par;
  double mean;
  double rms;
};

struct Hist2D{
  Hist2D(ACNET *_par, ACNET *_par2, bool _ratio = false, ACNET *_par_norm=NULL, ACNET *_par2_norm=NULL){
    par = _par;
    par2 = _par2;
    par_norm = _par_norm;
    par2_norm = _par2_norm;
    ratio = _ratio;

    std::string prefix;
    std::string title;
    std::string titlex;
    std::string titley;

    std::string titlexid;
    std::string titleyid;

    double xmin, xmax, ymin, ymax;
    xmin = -999;
    xmax = 999;
    ymin = -999;
    ymax = 999;

    if(!par_norm){
      titlex = par->name;
      titlexid = par->name;
      normx = false;
    }
    else{
      titlex = (par->name + "/" + par_norm->name).c_str();
      titlexid = (par->name + par_norm->name).c_str();
      normx = true;
    }
    if(!par2_norm){
      titley = par2->name;
      titleyid = par2->name;
      normy = false;
    }
    else{
      titley = (par2->name + "/" + par2_norm->name);
      titleyid = (par2->name + par2_norm->name).c_str();
      normy = true;
    }

    if(ratio){
      xmin = 0;
      xmax = 2;
      ymin = 0;
      ymax = 2;
      title = "Ratio from t_{0}";
      prefix = "ratio_";
    }
    else{
      title= "";
      prefix= "";
      if(!par_norm){
        xmin = par->min;
        xmax = par->max;
      }
      else{
        xmin = par->min/par_norm->min;
        xmin = par->max/par_norm->max;
      }
      if(!par2_norm){
        ymin = par2->min;
        ymax = par2->max;
      }
      else{
        ymin = par2->min/par2_norm->min;
        ymin = par2->max/par2_norm->max;
      }
    }

    plot = new TH2D(("TH2_" + prefix+ titlexid +"_"+ titleyid).c_str(), 
        ("TH2_" + prefix+ titlexid +"_"+ titleyid).c_str(), 
        100, xmin, xmax, 100, ymin, ymax);
    plot->SetTitle((title +";" + titlex + ";" + titley).c_str());
    c = new TCanvas(("TC_TH2_" + prefix+ titlexid +"_"+ titleyid).c_str(), 
        ("TC_TH2_" + prefix+ titlexid +"_"+ titleyid).c_str(), 
        800, 800);

    c->SetLeftMargin(0.15);
    c->SetBottomMargin(0.15);
    c->SetRightMargin(0.14);
    c->SetTopMargin(0.1);

    plot->SetLineColor(kBlack);
    plot->SetFillColor(13);
    plot->SetMarkerColor(kBlack);
    plot->SetMarkerStyle(6);
    plot->SetMarkerSize(0.5);
    plot->SetTitleSize(0.07);

    plot->GetXaxis()->SetTitleFont(132);
    plot->GetXaxis()->SetTitleSize(0.07);
    plot->GetXaxis()->SetTitleOffset(1);
    plot->GetXaxis()->SetLabelFont(132);
    plot->GetXaxis()->SetLabelSize(0.05);
    plot->GetXaxis()->SetNdivisions(1004, false);

    plot->GetYaxis()->SetTitleFont(132);
    plot->GetYaxis()->SetTitleSize(0.07);
    plot->GetYaxis()->SetTitleOffset(1);
    plot->GetYaxis()->SetLabelFont(132);
    plot->GetYaxis()->SetLabelSize(0.05);
    plot->GetYaxis()->SetNdivisions(1004, false);
  };

  TH2D *plot;
  ACNET *par;
  ACNET *par2;
  ACNET *par_norm;
  ACNET *par2_norm;
  TCanvas *c;
  bool normx;
  bool normy;
  bool ratio;
};

struct Hist1D{
  Hist1D(ACNET *_par, ACNET *_par2 = NULL, double min = 0, double max = 0){
    par = _par;
    par2 = _par2;
    if(!par2){
      plot = new TH1D(("TH1_" + par->name).c_str(), 
          ("TH1_" + par->name).c_str(),
          100, par->min, par->max);
      norm = false;
      plot->SetTitle((";" + par->name + "").c_str());
      c = new TCanvas(("TC_TH1_" + par->name).c_str(), 
          ("TC_TH1_" + par->name).c_str(),
          800, 800);
    }
    else{
      plot = new TH1D(("TH1_" + par->name + par2->name).c_str(), 
          ("TH1_" + par->name + par2->name).c_str(),
          100, min, max);
      norm = true;
      plot->SetTitle((";" + par->name + "/" + par2->name + ";").c_str());
      c = new TCanvas(("TC_TH1_" + par->name + par2->name).c_str(), 
          ("TC_TH1_" + par->name + par2->name).c_str(),
          800, 800);
    }

    c->SetLeftMargin(0.15);
    c->SetBottomMargin(0.15);
    c->SetRightMargin(0.14);
    c->SetTopMargin(0.1);

    plot->SetLineColor(kBlack);
    plot->SetFillColor(13);
    plot->SetMarkerColor(kBlack);
    plot->SetMarkerStyle(6);
    plot->SetMarkerSize(0.5);
    plot->SetTitleSize(0.07);

    plot->GetXaxis()->SetTitleFont(132);
    plot->GetXaxis()->SetTitleSize(0.07);
    plot->GetXaxis()->SetTitleOffset(1);
    plot->GetXaxis()->SetLabelFont(132);
    plot->GetXaxis()->SetLabelSize(0.05);
    plot->GetXaxis()->SetNdivisions(1004, false);

    plot->GetYaxis()->SetTitleFont(132);
    plot->GetYaxis()->SetTitleSize(0.07);
    plot->GetYaxis()->SetTitleOffset(1);
    plot->GetYaxis()->SetLabelFont(132);
    plot->GetYaxis()->SetLabelSize(0.05);
    plot->GetYaxis()->SetNdivisions(1004, false);

  };

  TH1D *plot;
  ACNET *par;
  ACNET *par2;
  TCanvas *c;
  bool norm;
};

struct TimePlot{
  TimePlot(ACNET *_par, ACNET *_par2 = NULL, double min = 0, double max = 0){
    par = _par;
    par2 = _par2;
    plot = new TGraph(3000);
    timeCounter = 0;

    if(!par2){
      norm = false;
      plot->SetTitle((";Date;" + par->name).c_str());
      c = new TCanvas(("TC_TG_" + par->name).c_str(), 
          ("TC_TG_" + par->name).c_str(),
          1200, 800);
      plot->GetYaxis()->SetLimits(par->min, par->max);
    }
    else{
      norm = true;
      plot->SetTitle((";Date;" + par->name + "/" + par2->name).c_str());
      c = new TCanvas(("TC_TG_" + par->name + par2->name).c_str(), 
          ("TC_TG_" + par->name + par2->name).c_str(),
          1200, 800);
      plot->GetYaxis()->SetLimits(min, max);
    }

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
    plot->SetMarkerColor(kBlack);
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

  };
  TGraph *plot;
  ACNET *par;
  ACNET *par2;
  TCanvas *c;
  bool norm;
  int timeCounter;
};

class Plot{
  public:
    // Makes a comparison between variables in one TTree
    Plot(TTree *_treeIn, std::vector< ACNET* > _pars);

    void SetPlots();
    void fillRAM();
    void clearRAM();
    void FillPlots();
    void DrawPlots();
    void SavePlots();
    void setTTree(TTree *treeIn);

    void GetRMS(ACNET* par, ACNET* par_norm);

    void GetRMS(double &_mean, double &_rms, ACNET* par, ACNET* par_norm);

    void LoadDisplayPlots(std::vector< Hist2D* > _dispPlots){ dispPlots = _dispPlots; };
    void LoadRatioPlots(std::vector< Hist2D* > _ratiPlots){ ratiPlots = _ratiPlots; };
    void LoadCuts(std::vector< Cut* > _cuts){ cuts = _cuts; };

    // TODO : Maybe a base class that holds parameter values and does functions
    //        like below would be useful. Something that only handles the data,
    //        and both the plotting and pasing/reducing code would use these
    int mapKPar(int k_par);

  private:
    std::vector< ACNET* > pars;
    TTree* tree;

    void FillHist1D(Hist1D* plot);
    void FillHist2D(Hist2D* plot);
    void FillTimePlot(TimePlot* plot);

    void FillCuts();

    std::vector< Hist1D* >   histPlots;
    std::vector< TimePlot* > timePlots;
    std::vector< Hist2D* >   dispPlots;
    std::vector< Hist2D* >   ratiPlots;
    std::vector< Cut* > cuts;


    // Fills the variables
    double **d_vals;
    double ***d_vals6;
    double ***d_vals81;
    Long64_t **d_times;
    Long64_t *d_time;
    bool *d_cuts;
    int time_counter;

    // Branch value holder
    double vals81[k_nLevel][81];
    double vals6[k_nLevel][6];
    double vals[k_nLevel];
    Long64_t time;
    Long64_t times[k_nLevel];

    double GetEvent(ACNET* par, int event);

};
#endif

