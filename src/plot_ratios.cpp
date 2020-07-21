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
    std::cout << "USAGE: ./plot TTree to plot" << std::endl;
  }
  std::string input(argv[1]);
  TFile *fin = new TFile(input.c_str(), "READ");
  TTree *tin = (TTree*)fin->Get("beam_monitors");

  // Make plotting object
  Plotter plot1(tin);
  plot1.fillRAM();
  plot1.ratioPlots();

  return 0;
}
