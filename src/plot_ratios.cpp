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
    std::cout << argv[arg] << std::endl;
  }

  int mode = atoi(argv[1]);
  
  std::string input1(argv[2]);
  TFile *fin1 = new TFile(input1.c_str(), "READ");
  TTree *tin1 = (TTree*)fin1->Get("beam_monitors");

  // Make plotting object
  Plotter plot1(tin1);
  plot1.fillRAM();

  // Set all the plots
  plot1.setRatioPlots();
  plot1.setTimePlots();
  plot1.setDisplayPlots();

  plot1.SetHornMode(mode);
  // Draw all the plots
  plot1.drawRatioPlots(kBlack, 0);
  plot1.drawTimePlots(kBlack, 0);
  plot1.drawDisplayPlots(kBlack, 0);



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
      plot1.drawRatioPlots(kBlack , 1);
      plot1.drawTimePlots(kBlack, 1);
      plot1.drawDisplayPlots(kBlack, 1);

      //delete fin2;
      plot1.clearRAM();
    }
  }

  // Save them all
  plot1.saveRatioPlots();
  plot1.saveTimePlots();
  plot1.saveDisplayPlots();

  return 0;
}
