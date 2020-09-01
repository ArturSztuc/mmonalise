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

int main(int argc, char *argv[])
{
  // Ensure the Input is ok
  if (argc != 3){
    std::cout << "USAGE: ./parse /folder/to/parse  /folder/outout" << std::endl;
  }
  std::string output(argv[2]);

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

  int npars = pars.size();
  int k_time = 0;
  for(int i = 0; i < npars; ++i)
    if(pars[i] == k_mm1cor_cal) k_time = i;
  // Constructor(input, parameters to save, time reference)
  Datum day1(argv[1], pars, k_time);

  // Fill the TTree holder
  day1.PreFill();
  // Print info (if ok/not)
  day1.printer();

  // Save the TTree into the output
  day1.saveData((output +"/"+ day1.getFileNameBase() + "MM_reduced.root").c_str());

  return 0;
}
