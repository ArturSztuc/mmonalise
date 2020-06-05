// mergettree: you provide a folder with the beam monitor files in, and it will
// merge the root files with different variables into one. To be used as a
// library in your executible or in different class. 
#ifndef _mergettree_h_
#define _mergettree_h_

#include <iostream>
#include <vector>
#include "structs.h"
#include <string.h>

#include "TStyle.h"
#include "TH2F.h"
#include "TH2D.h"
#include "TH1D.h"


#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TDatime.h"
#include "structs.h"

class MergeT{
  public:
    // Constructor
    MergeT(std::string i_folder);

    /*  Getters  */
    // Returns the processed TTree
    TTree* getTTree(){ return dataHolder; };

    // Returns the "reconstructed" file basename
    std::string getFileNameBase(){ return fileNameBase; };

    // Saves the merged/parsed data into a .root file.
    // Will save it as fileNameBase_parsed.root if empty
    void saveData(std::string fout_name = "" );

    // Printer
    void printer();

  private:
    // Parses the input folder string to "reconstruct" the expected input root
    // filenames. The expected variables-ttres are defined in structs.h
    void parse();

    // Opens the root files (with names generated in parse()) and fills an
    // output root file
    void fillDataHolder();

    std::string     folder;       // Folder containing the root files
    std::string     fileNameBase; // Basename of the root files
    std::vector< std::string >  rootFilesIn;  // An array with all the root file names
    TTree*          dataHolder;   // TTree output holding all the converted data
    TFile*          dataOut;      // TFile containing all the data, if we actually want it
    double vals6_sum( double vals[]);
};

#endif /* ifndef __mergettree_h__ */

