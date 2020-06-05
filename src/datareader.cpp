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

  Datum day1("/home/artur/WORK/CHIPS/NUMI/2020-01-20/08_00_00");
  day1.printer();
  day1.saveData( ("/home/artur/WORK/CHIPS/NUMI/output/TEST/" + day1.getFileNameBase() + "parsed.root").c_str());

  //Plotter plot(day1.getTTree());
  return 0;
}
