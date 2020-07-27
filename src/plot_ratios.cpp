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
  if (argc != 3){
    std::cout << "USAGE: ./plot TTree_1 TTree_2" << std::endl;
  }
  std::string input1(argv[1]);
  TFile *fin1 = new TFile(input1.c_str(), "READ");
  TTree *tin1 = (TTree*)fin1->Get("beam_monitors");

  std::string input2(argv[2]);
  TFile *fin2 = new TFile(input2.c_str(), "READ");
  TTree *tin2 = (TTree*)fin2->Get("beam_monitors");

  // Make plotting object
  Plotter plot1(tin1);
  plot1.fillRAM();

  // Set all the plots
  plot1.setRatioPlots();
  plot1.setTimePlots();
  plot1.setDisplayPlots();

  // Draw all the plots
  plot1.drawRatioPlots(kRed);
  plot1.drawTimePlots(kRed);
  plot1.drawDisplayPlots(kRed);

  plot1.clearRAM();
  plot1.setTTree(tin2);
  plot1.fillRAM();

  // Draw again
  plot1.drawRatioPlots(kBlue, 1);
  plot1.drawTimePlots(kBlue, 1);
  plot1.drawDisplayPlots(kBlue, 1);

  // Save them all
  plot1.saveRatioPlots();
  plot1.saveTimePlots();
  plot1.saveDisplayPlots();

  return 0;
}
