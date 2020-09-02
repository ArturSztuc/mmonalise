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

int main(int argc, char *argv[])
{
  // Input
  if (argc < 2){
    std::cout << "USAGE: ./plot mode TTree_1 [TTree_2, TTree_3 .... TTree_N]" << std::endl;
  }
  for(int arg = 0; arg < argc; ++arg){
    std::cout << argv[arg] <<  "      ";
  }
  std::cout << std::endl;

  int mode = atoi(argv[1]);
  
  std::string input1(argv[2]);
  TFile *fin1 = new TFile(input1.c_str(), "READ");
  TTree *tin1 = (TTree*)fin1->Get("beam_monitors");

  // Select parameters we and to process
  std::vector< int > pars;
  // Beam intensity
  pars.push_back(k_e12_trtgtd);
  // Beam monitors before the target
  pars.push_back(k_hp121);
  pars.push_back(k_vp121);
  pars.push_back(k_hptgt);
  pars.push_back(k_vptgt);
  // MM# X
  pars.push_back(k_mm1xav);
  pars.push_back(k_mm2xav);
  pars.push_back(k_mm3xav);
  // MM# Y
  pars.push_back(k_mm1yav);
  pars.push_back(k_mm2yav);
  pars.push_back(k_mm3yav);
  // Intensity (corrected)
  pars.push_back(k_mm1cor_cal);
  pars.push_back(k_mm2cor_cal);
  pars.push_back(k_mm3cor_cal);
  // Signal (81 matrix)
  pars.push_back(k_mm1_sig_calib);
  pars.push_back(k_mm2_sig_calib);
  pars.push_back(k_mm3_sig_calib);
  // Horn current variables
  pars.push_back(k_nslina);
  pars.push_back(k_nslinb);
  pars.push_back(k_nslinc);
  pars.push_back(k_nslind);

  // Make plotting object
  Plotter plot1(tin1, pars);
  plot1.fillRAM();

  // Set all the plots
  plot1.setRatioPlots();
  plot1.setTimePlots();
  plot1.setDisplayPlots();

  plot1.SetHornMode(mode);
  // Draw all the plots
  plot1.fillRatioPlots();
  plot1.fillTimePlots();
  plot1.fillDisplayPlots();

  plot1.clearRAM();



  if(argc > 3){
      TFile *fin2;
      TTree *tin2;
    
    for(int plt = 3; plt < argc; ++plt){
      std::cout << plt << std::endl;
      std::string input2(argv[plt]);
      fin2 = new TFile(input2.c_str(), "READ");
      tin2 = (TTree*)fin2->Get("beam_monitors");

      plot1.setTTree(tin2);
      plot1.fillRAM();

      // Draw again
      plot1.fillRatioPlots();
      plot1.fillTimePlots();
      plot1.fillDisplayPlots();

      //delete fin2;
      plot1.clearRAM();
    }
  }

  plot1.drawRatioPlots(kBlack, 0);
  plot1.drawTimePlots(kBlack, 0);
  plot1.drawDisplayPlots(kBlack, 0);

  // Save them all
  plot1.saveRatioPlots();
  plot1.saveTimePlots();
  plot1.saveDisplayPlots();

  return 0;
}
