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
  if (argc != 3){
    std::cout << "USAGE: ./parse /folder/to/parse  /folder/outout" << std::endl;
  }
  std::string output(argv[2]);

  Datum day1(argv[1]);
  day1.ReduceBatchTTree();
  day1.printer();
  day1.saveData((output +"/"+ day1.getFileNameBase() + "MM_batched.root").c_str());

  return 0;
}
