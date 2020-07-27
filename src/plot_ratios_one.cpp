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
  if (argc != 2){
    std::cout << "USAGE: ./plot TTree_1 TTree_2" << std::endl;
  }
  std::string input1(argv[1]);
  TFile *fin1 = new TFile(input1.c_str(), "READ");
  TTree *tin1 = (TTree*)fin1->Get("beam_monitors");

  // Make plotting object
  Plotter plot1(tin1);
  plot1.fillRAM();
  plot1.setRatioPlots();
  plot1.drawRatioPlots(kRed);

  plot1.setTimePlots();
  plot1.drawTimePlots(kRed, 0);

  plot1.saveRatioPlots();
  plot1.saveTimePlots();

  return 0;
}
