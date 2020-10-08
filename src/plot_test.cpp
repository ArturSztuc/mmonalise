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
#include "../classes/plot.h"
#include "../classes/structs.h"

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

  // Maybe will need removing at some point?
  int mode = atoi(argv[1]);
  
  // Get the first root file
  std::string input1(argv[2]);
  TFile *fin1 = new TFile(input1.c_str(), "READ");
  TTree *tin1 = (TTree*)fin1->Get("beam_monitors");

  // List the variables we want. Can choose any variable that's implemented in Structs.h
  std::vector< ACNET* > variables;
  variables.push_back(new ACNET(k_e12_trtgtd, 0, 0, 60, 1));
  variables.push_back(new ACNET(k_hp121, 0, -1, 2, 6));
  variables.push_back(new ACNET(k_vp121, 0, -1, 2, 6));
  variables.push_back(new ACNET(k_hptgt, 0, -1, 2, 6));
  variables.push_back(new ACNET(k_vptgt, 0, -1, 2, 6));
  variables.push_back(new ACNET(k_mm1xav, 0, -2, 2, 1));
  variables.push_back(new ACNET(k_mm2xav, 0, -2, 2, 1));
  variables.push_back(new ACNET(k_mm3xav, 0, -2, 2, 1));
  variables.push_back(new ACNET(k_mm1yav, 0, -2, 2, 1));
  variables.push_back(new ACNET(k_mm2yav, 0, -2, 2, 1));
  variables.push_back(new ACNET(k_mm3yav, 0, -2, 2, 1));
  variables.push_back(new ACNET(k_mm1cor_cal, 1, 0, 100, 1));
  variables.push_back(new ACNET(k_mm2cor_cal, 1, 100, 300, 1));
  variables.push_back(new ACNET(k_mm3cor_cal, 1, 10, 25, 1));
  variables.push_back(new ACNET(k_nslina, 0, -51, -49, 1));
  variables.push_back(new ACNET(k_nslinb, 0, -51, -49, 1));
  variables.push_back(new ACNET(k_nslinc, 0, -51, -49, 1));
  variables.push_back(new ACNET(k_nslind, 0, -51, -49, 1));

  // Initialise the plotting object first (I wonder if we could change all of
  // this to more of a functional programming thing...)
  Plot tester(tin1, variables);
  tester.fillRAM();
  //std::vector< Cut* > cuts;

  //for(int i = 0; i < int(variables.size()) ; ++i){
  //  double mean, rms;
  //  tester.GetRMS(mean, rms, variables[i], NULL);
  //  cuts.push_back(new Cut(variables[i], mean, rms));
  //  std::cout << variables[i]->name << " mean = " << mean << " RMS: " << rms <<std::endl;
  //}
  //tester.LoadCuts(cuts);

  // *************************************************************************//
  // Now let's define 2D plots we want. Start with the standard display plots:
  std::vector< Hist2D* > fDisplayPlots;
  // Standard display plots first
  fDisplayPlots.push_back(new Hist2D(variables[tester.mapKPar(k_mm1xav)], variables[tester.mapKPar(k_mm1yav)]));
  fDisplayPlots.push_back(new Hist2D(variables[tester.mapKPar(k_mm2xav)], variables[tester.mapKPar(k_mm2yav)]));
  fDisplayPlots.push_back(new Hist2D(variables[tester.mapKPar(k_mm3xav)], variables[tester.mapKPar(k_mm3yav)]));
  fDisplayPlots.push_back(new Hist2D(variables[tester.mapKPar(k_mm1xav)], variables[tester.mapKPar(k_mm2xav)]));
  fDisplayPlots.push_back(new Hist2D(variables[tester.mapKPar(k_mm1xav)], variables[tester.mapKPar(k_mm3xav)]));
  fDisplayPlots.push_back(new Hist2D(variables[tester.mapKPar(k_mm2xav)], variables[tester.mapKPar(k_mm3xav)]));
  fDisplayPlots.push_back(new Hist2D(variables[tester.mapKPar(k_mm1yav)], variables[tester.mapKPar(k_mm2yav)]));
  fDisplayPlots.push_back(new Hist2D(variables[tester.mapKPar(k_mm1yav)], variables[tester.mapKPar(k_mm3yav)]));
  fDisplayPlots.push_back(new Hist2D(variables[tester.mapKPar(k_mm2yav)], variables[tester.mapKPar(k_mm3yav)]));

  // MM vs TRTGTD hirizontal/vertical positions
  fDisplayPlots.push_back(new Hist2D(variables[tester.mapKPar(k_mm1xav)], variables[tester.mapKPar(k_hptgt)]));
  fDisplayPlots.push_back(new Hist2D(variables[tester.mapKPar(k_mm2xav)], variables[tester.mapKPar(k_hptgt)]));
  fDisplayPlots.push_back(new Hist2D(variables[tester.mapKPar(k_mm3xav)], variables[tester.mapKPar(k_hptgt)]));
  fDisplayPlots.push_back(new Hist2D(variables[tester.mapKPar(k_mm1yav)], variables[tester.mapKPar(k_vptgt)]));
  fDisplayPlots.push_back(new Hist2D(variables[tester.mapKPar(k_mm2yav)], variables[tester.mapKPar(k_vptgt)]));
  fDisplayPlots.push_back(new Hist2D(variables[tester.mapKPar(k_mm3yav)], variables[tester.mapKPar(k_vptgt)]));

  // MM positions vs target intensity
  fDisplayPlots.push_back(new Hist2D(variables[tester.mapKPar(k_mm1xav)], variables[tester.mapKPar(k_e12_trtgtd)]));
  fDisplayPlots.push_back(new Hist2D(variables[tester.mapKPar(k_mm2xav)], variables[tester.mapKPar(k_e12_trtgtd)]));
  fDisplayPlots.push_back(new Hist2D(variables[tester.mapKPar(k_mm3xav)], variables[tester.mapKPar(k_e12_trtgtd)]));
  fDisplayPlots.push_back(new Hist2D(variables[tester.mapKPar(k_mm1yav)], variables[tester.mapKPar(k_e12_trtgtd)]));
  fDisplayPlots.push_back(new Hist2D(variables[tester.mapKPar(k_mm2yav)], variables[tester.mapKPar(k_e12_trtgtd)]));
  fDisplayPlots.push_back(new Hist2D(variables[tester.mapKPar(k_mm3yav)], variables[tester.mapKPar(k_e12_trtgtd)]));

  // MM positions vs MM intensity normalized by the target
  fDisplayPlots.push_back(new Hist2D(variables[tester.mapKPar(k_mm1xav)], variables[tester.mapKPar(k_mm1cor_cal)], false, NULL, variables[tester.mapKPar(k_e12_trtgtd)]));
  fDisplayPlots.push_back(new Hist2D(variables[tester.mapKPar(k_mm1yav)], variables[tester.mapKPar(k_mm1cor_cal)], false, NULL, variables[tester.mapKPar(k_e12_trtgtd)]));
  fDisplayPlots.push_back(new Hist2D(variables[tester.mapKPar(k_mm2xav)], variables[tester.mapKPar(k_mm2cor_cal)], false, NULL, variables[tester.mapKPar(k_e12_trtgtd)]));
  fDisplayPlots.push_back(new Hist2D(variables[tester.mapKPar(k_mm2yav)], variables[tester.mapKPar(k_mm2cor_cal)], false, NULL, variables[tester.mapKPar(k_e12_trtgtd)]));
  fDisplayPlots.push_back(new Hist2D(variables[tester.mapKPar(k_mm3xav)], variables[tester.mapKPar(k_mm3cor_cal)], false, NULL, variables[tester.mapKPar(k_e12_trtgtd)]));
  fDisplayPlots.push_back(new Hist2D(variables[tester.mapKPar(k_mm3yav)], variables[tester.mapKPar(k_mm3cor_cal)], false, NULL, variables[tester.mapKPar(k_e12_trtgtd)]));

  // MM positions vs MM intensity
  fDisplayPlots.push_back(new Hist2D(variables[tester.mapKPar(k_mm1xav)], variables[tester.mapKPar(k_mm1cor_cal)]));
  fDisplayPlots.push_back(new Hist2D(variables[tester.mapKPar(k_mm1yav)], variables[tester.mapKPar(k_mm1cor_cal)]));
  fDisplayPlots.push_back(new Hist2D(variables[tester.mapKPar(k_mm2xav)], variables[tester.mapKPar(k_mm2cor_cal)]));
  fDisplayPlots.push_back(new Hist2D(variables[tester.mapKPar(k_mm2yav)], variables[tester.mapKPar(k_mm2cor_cal)]));
  fDisplayPlots.push_back(new Hist2D(variables[tester.mapKPar(k_mm3xav)], variables[tester.mapKPar(k_mm3cor_cal)]));
  fDisplayPlots.push_back(new Hist2D(variables[tester.mapKPar(k_mm3yav)], variables[tester.mapKPar(k_mm3cor_cal)]));

  // Target positions vs MM intensity normalized by the target
  fDisplayPlots.push_back(new Hist2D(variables[tester.mapKPar(k_hptgt)], variables[tester.mapKPar(k_mm1cor_cal)], false, NULL, variables[tester.mapKPar(k_e12_trtgtd)]));
  fDisplayPlots.push_back(new Hist2D(variables[tester.mapKPar(k_hptgt)], variables[tester.mapKPar(k_mm2cor_cal)], false, NULL, variables[tester.mapKPar(k_e12_trtgtd)]));
  fDisplayPlots.push_back(new Hist2D(variables[tester.mapKPar(k_hptgt)], variables[tester.mapKPar(k_mm3cor_cal)], false, NULL, variables[tester.mapKPar(k_e12_trtgtd)]));
  fDisplayPlots.push_back(new Hist2D(variables[tester.mapKPar(k_vptgt)], variables[tester.mapKPar(k_mm1cor_cal)], false, NULL, variables[tester.mapKPar(k_e12_trtgtd)]));
  fDisplayPlots.push_back(new Hist2D(variables[tester.mapKPar(k_vptgt)], variables[tester.mapKPar(k_mm2cor_cal)], false, NULL, variables[tester.mapKPar(k_e12_trtgtd)]));
  fDisplayPlots.push_back(new Hist2D(variables[tester.mapKPar(k_vptgt)], variables[tester.mapKPar(k_mm3cor_cal)], false, NULL, variables[tester.mapKPar(k_e12_trtgtd)]));

  fDisplayPlots.push_back(new Hist2D(variables[tester.mapKPar(k_mm3cor_cal)], variables[tester.mapKPar(k_e12_trtgtd)]));
  fDisplayPlots.push_back(new Hist2D(variables[tester.mapKPar(k_hptgt)], variables[tester.mapKPar(k_vptgt)]));

  fDisplayPlots.push_back(new Hist2D(variables[tester.mapKPar(k_nslina)], variables[tester.mapKPar(k_mm1yav)]));
  fDisplayPlots.push_back(new Hist2D(variables[tester.mapKPar(k_nslina)], variables[tester.mapKPar(k_mm2yav)]));
  fDisplayPlots.push_back(new Hist2D(variables[tester.mapKPar(k_nslina)], variables[tester.mapKPar(k_mm3yav)]));
  fDisplayPlots.push_back(new Hist2D(variables[tester.mapKPar(k_nslinb)], variables[tester.mapKPar(k_mm1yav)]));
  fDisplayPlots.push_back(new Hist2D(variables[tester.mapKPar(k_nslinb)], variables[tester.mapKPar(k_mm2yav)]));
  fDisplayPlots.push_back(new Hist2D(variables[tester.mapKPar(k_nslinb)], variables[tester.mapKPar(k_mm3yav)]));

  fDisplayPlots.push_back(new Hist2D(variables[tester.mapKPar(k_nslina)], variables[tester.mapKPar(k_mm1xav)]));
  fDisplayPlots.push_back(new Hist2D(variables[tester.mapKPar(k_nslina)], variables[tester.mapKPar(k_mm2xav)]));
  fDisplayPlots.push_back(new Hist2D(variables[tester.mapKPar(k_nslina)], variables[tester.mapKPar(k_mm3xav)]));
  fDisplayPlots.push_back(new Hist2D(variables[tester.mapKPar(k_nslinb)], variables[tester.mapKPar(k_mm1xav)]));
  fDisplayPlots.push_back(new Hist2D(variables[tester.mapKPar(k_nslinb)], variables[tester.mapKPar(k_mm2xav)]));
  fDisplayPlots.push_back(new Hist2D(variables[tester.mapKPar(k_nslinb)], variables[tester.mapKPar(k_mm3xav)]));

  fDisplayPlots.push_back(new Hist2D(variables[tester.mapKPar(k_nslina)], variables[tester.mapKPar(k_mm1cor_cal)], false, NULL, variables[tester.mapKPar(k_e12_trtgtd)]));
  fDisplayPlots.push_back(new Hist2D(variables[tester.mapKPar(k_nslina)], variables[tester.mapKPar(k_mm2cor_cal)], false, NULL, variables[tester.mapKPar(k_e12_trtgtd)]));
  fDisplayPlots.push_back(new Hist2D(variables[tester.mapKPar(k_nslina)], variables[tester.mapKPar(k_mm3cor_cal)], false, NULL, variables[tester.mapKPar(k_e12_trtgtd)]));
  fDisplayPlots.push_back(new Hist2D(variables[tester.mapKPar(k_nslinb)], variables[tester.mapKPar(k_mm1cor_cal)], false, NULL, variables[tester.mapKPar(k_e12_trtgtd)]));
  fDisplayPlots.push_back(new Hist2D(variables[tester.mapKPar(k_nslinb)], variables[tester.mapKPar(k_mm2cor_cal)], false, NULL, variables[tester.mapKPar(k_e12_trtgtd)]));
  fDisplayPlots.push_back(new Hist2D(variables[tester.mapKPar(k_nslinb)], variables[tester.mapKPar(k_mm3cor_cal)], false, NULL, variables[tester.mapKPar(k_e12_trtgtd)]));

  // That was a lot of plots... now, the ratio plots are going to be for the
  // same parameters as the display plots, but let's initialise them
  // automatically
  std::vector< Hist2D* > fRatioPlots;
  for(int i = 0; i < int(fDisplayPlots.size()); ++i)
    fRatioPlots.push_back(new Hist2D(fDisplayPlots[i]->par, fDisplayPlots[i]->par2, true, fDisplayPlots[i]->par_norm, fDisplayPlots[i]->par2_norm));


  // Initialise the plotter
  tester.SetPlots();
  tester.LoadDisplayPlots(fDisplayPlots);
  tester.LoadRatioPlots(fRatioPlots);

  tester.FillPlots();
  tester.clearRAM();

  if(argc > 3){
      TFile *fin2;
      TTree *tin2;
    
    for(int plt = 3; plt < argc; ++plt){
      std::cout << plt << std::endl;
      std::string input2(argv[plt]);
      fin2 = new TFile(input2.c_str(), "READ");
      tin2 = (TTree*)fin2->Get("beam_monitors");

      tester.setTTree(tin2);
      tester.fillRAM();

      // Draw again
      tester.FillPlots();

      //delete fin2;
      tester.clearRAM();
      delete tin2;
      delete fin2;
    }
  }

  tester.DrawPlots();
  tester.SavePlots();


  return 0;
}
