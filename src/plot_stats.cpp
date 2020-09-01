#include <iostream>
#include <TChain.h>
#include <TFile.h>
#include <TLine.h>
#include <TGraph.h>
#include <TH2D.h>
#include <TCanvas.h>
#include <TRandom3.h>
#include <TLegend.h>
#include <TMath.h>
#include <TROOT.h>
#include <stdlib.h>
#include <TStyle.h>
#include "../classes/datareader.h"
#include "../classes/plotter.h"
#include "../classes/structs.h"


bool isLvl0(int lev){ return lev < k_nLevel0; }

std::string levelX_to_str(int lev){
  if(isLvl0(lev) == true)
    return level0_to_str(lev);
  else
    return level1_to_str(lev);
}


void setStyle(TH1D *plot, TCanvas *c, int col){

    c->SetLeftMargin(0.15);
    c->SetBottomMargin(0.15);
    c->SetRightMargin(0.1);
    c->SetTopMargin(0.1);

    plot->SetLineColor(col);
    plot->SetFillColor(13);
    plot->SetMarkerColor(col);
    plot->SetMarkerStyle(7);
    //plot->SetMarkerSize(0.5);
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
    //TLine l;
    //l.DrawLine(gPad->GetUxmin(), gPad->GetUymax(), gPad->GetUxmax(), gPad->GetUymax());
    //l.DrawLine(gPad->GetUxmax(), gPad->GetUymin(), gPad->GetUxmax(), gPad->GetUymax());
}

int main(int argc, char *argv[])
{
  // Input
  if (argc < 2){
    std::cout << "USAGE: ./plot TTree_1 [TTree_2, TTree_3 .... TTree_N]" << std::endl;
  }
  for(int arg = 0; arg < argc; ++arg){
    std::cout << argv[arg] << std::endl;
  }

  // Set plotting

  std::vector< int > pars;
  pars.push_back(k_e12_trtgtd);
  pars.push_back(k_mm1yav);
  pars.push_back(k_mm2yav);
  pars.push_back(k_mm3yav);
  pars.push_back(k_mm1xav);
  pars.push_back(k_mm2xav);
  pars.push_back(k_mm3xav);
  pars.push_back(k_mm1cor_cal);
  pars.push_back(k_mm2cor_cal);
  pars.push_back(k_mm3cor_cal);
  pars.push_back(k_hp121);
  pars.push_back(k_vp121);
  pars.push_back(k_hptgt);
  pars.push_back(k_vptgt);

  TH2D *correlation_matrix = new TH2D("correlation_matrix", "correlation_matrix", pars.size(), 0, pars.size(), pars.size(), 0, pars.size());

  int ncomb = pars.size()*pars.size();

  TCanvas **canv;
  TH1D **th1;
  canv = new TCanvas*[ncomb];
  th1 = new TH1D*[ncomb];

  for(int i = 0; i < pars.size(); ++i){
    std::string name1 = levelX_to_str(pars[i]);
    for(int j = 0; j < pars.size(); ++j){
      std::string name2 = levelX_to_str(pars[j]);
      int itr = (j + (pars.size())*i);
      canv[itr] = new TCanvas(("canv_" + name1 + "_" + name2).c_str(), ("canv_"
            + name1 + "_" + name2).c_str(), 1200, 1200);
      th1[itr] = new TH1D(("th1_" + name1 + "_" + name2).c_str(), ("th1_" +
            name1 + "_" + name2).c_str(), 100, -1.02, 1.02);
      th1[itr]->SetTitle((name1 + " vs " + name2 + " correlation;Correlation;").c_str());
    }
  }
  std::string input1(argv[2]);
  TFile *fin1 = new TFile(input1.c_str(), "READ");
  TTree *tin1 = (TTree*)fin1->Get("beam_monitors");

  // Make plotting object
  Plotter plot(tin1);
  plot.fillRAM();

  //std::vector< double > means;
  //std::vector< double > sd;
  double mean1 = 0;
  double mean2 = 0;
  double sd1 = 0;
  double sd2 = 0;
  double cov = 0;
  double cor = 0;

  for(int i = 0; i < pars.size(); ++i){
    for(int j = 0; j < pars.size(); ++j){
      int itr = (j + (pars.size())*i);
      plot.GetMeansSDCorrelationCovariance(pars[i], pars[j], mean1, mean2, sd1, sd2, cor, cov);
      th1[itr]->Fill(cor);
    }
  }


  if(argc > 2){
      TFile *fin2;
      TTree *tin2;
    
    for(int plt = 2; plt < argc; ++plt){
      std::cout << plt << std::endl;
      std::string input2(argv[plt]);
      fin2 = new TFile(input2.c_str(), "READ");
      tin2 = (TTree*)fin2->Get("beam_monitors");

      plot.setTTree(tin2);
      plot.fillRAM();

      for(int i = 0; i < pars.size(); ++i){
        for(int j = 0; j < pars.size(); ++j){
          int itr = (j + (pars.size())*i);
          plot.GetMeansSDCorrelationCovariance(pars[i], pars[j], mean1, mean2, sd1, sd2, cor, cov);
          th1[itr]->Fill(cor);
        }
      }

      //delete fin2;
      plot.clearRAM();
    }
  }


  for(int i = 0; i < pars.size(); ++i){
    for(int j = 0; j < pars.size(); ++j){
      int itr = (j + (pars.size())*i);
      double mean = 0;
      mean = th1[itr]->GetMean();
      
      correlation_matrix->Fill(i, j, mean);
    }
  }

  correlation_matrix->SetTitle("");
  for (int i=0;i<pars.size();i++){
    correlation_matrix->GetXaxis()->SetBinLabel(i+1,"");
    correlation_matrix->GetYaxis()->SetBinLabel(i+1,"");
  }

  for (int i=0;i<pars.size();i++){
    correlation_matrix->GetXaxis()->SetBinLabel(i+1,levelX_to_str(pars[i]).c_str());
    correlation_matrix->GetYaxis()->SetBinLabel(i+1,levelX_to_str(pars[i]).c_str());
  }




  for(int k = 0; k < ncomb; ++k){
    int i = k/pars.size();
    int j = k%pars.size();
    setStyle(th1[k], canv[k], kBlack);
    canv[k]->cd();
    th1[k]->Draw();
    canv[k]->SaveAs(("cor_" + levelX_to_str(pars[i]) + "_" + levelX_to_str(pars[j]) + ".png").c_str());
    canv[k]->SaveAs(("cor_" + levelX_to_str(pars[i]) + "_" + levelX_to_str(pars[j]) + ".root").c_str());
  }

  gStyle->SetOptStat(0);
  TCanvas *canv_corr = new TCanvas("canv_cor", "canv_cor", 1200, 1200);
  canv_corr->SetLeftMargin(0.21);
  canv_corr->SetBottomMargin(0.21);
  canv_corr->SetRightMargin(0.19);
  correlation_matrix->LabelsOption("v", "X");
  canv_corr->cd();
  correlation_matrix->GetZaxis()->SetRangeUser(-1,1);
  gStyle->SetPalette(kTemperatureMap);
  correlation_matrix->Draw("COLZ");
  canv_corr->SaveAs("correlation_matrix.png");
  canv_corr->SaveAs("correlation_matrix.root");
  canv_corr->SaveAs("correlation_matrix.C");


    return 0;
  }
